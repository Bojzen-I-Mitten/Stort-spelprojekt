﻿//#define TRANSFORM_PRINT_DEBUG


using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using LiteNetLib;
using LiteNetLib.Utils;
using ThomasEngine;


namespace ThomasEngine.Network
{
    public class NetworkTransform : NetworkComponent
    {
        Vector3 PrevPosition;
        Quaternion PrevRotation;
        Vector3 PrevScale;
        float prevVelocity;

        float CurrentPositionDuration = 0;

        const float LocalMovementThreshold = 0.00001f;
        const float LocalRotationThreshold = 0.00001f;
        const float LocalVelocityThreshold = 0.00001f;
        const float MoveAheadRatio = 0.1f;

        const float SnapThreshhold = 2.0f;

        const float InterpolateRotation = 1.0f;
        const float InterpolateMovement = 1.0f;

        Vector3 TargetSyncPosition;
        Quaternion TargetSyncRotation;
        Vector3 TargetSyncLinearVelocity;
        Vector3 TargetSyncAngularVelocity;

        Rigidbody _targetRigidbody;
        Rigidbody targetRigidbody
        {
            get
            {
                if (!_targetRigidbody)
                    _targetRigidbody = target.gameObject.GetComponent<Rigidbody>();
                return _targetRigidbody;
            }
        }

        Transform _target;
        [Browsable(false)]
        [Newtonsoft.Json.JsonIgnore]
        public Transform target {
            get {
                if (!_target)
                    _target = transform;
                return _target;
            }
            set
            {
                _target = value;
                _targetRigidbody = _target.gameObject.GetComponent<Rigidbody>();
            }
        }

        public enum TransformSyncMode
        {
            SyncNone = 0,
            SyncTransform = 1,
            SyncRigidbody = 2
        }

        public TransformSyncMode SyncMode { get; set; } = TransformSyncMode.SyncTransform;


        public void SetTarget(Transform newTarget)
        {
            _target = newTarget;
            PrevPosition = target.position;
            PrevRotation = target.rotation;
            PrevScale = target.scale;
            TargetSyncPosition = target.position;

            if (targetRigidbody)
            {
                TargetSyncLinearVelocity = targetRigidbody.LinearVelocity;
                TargetSyncAngularVelocity = targetRigidbody.AngularVelocity;
            }
        }

        public override void Start()
        {
            PrevPosition = target.position;
            PrevRotation = target.rotation;
            PrevScale = target.scale;
            TargetSyncPosition = target.position;

            if (targetRigidbody)
            {
                TargetSyncLinearVelocity = targetRigidbody.LinearVelocity;
                TargetSyncAngularVelocity = targetRigidbody.AngularVelocity;
            }
            else
            {
                Debug.LogError("No rigidbody");
            }
        }

        public override void Update()
        {
            CurrentPositionDuration += Time.DeltaTime;
            isDirty = true;

            switch (SyncMode)
            {
                case TransformSyncMode.SyncTransform:
                    InterpolatePosition();
                    break;
                default:
                    break;
            }
        }

        public override void FixedUpdate()
        {
            switch (SyncMode)
            {
                case TransformSyncMode.SyncRigidbody:
                    InterpolateRigidbody();
                    break;
                default:
                    break;
            }
        }

        void InterpolateRigidbody()
        {
            if (!isOwner && targetRigidbody && targetRigidbody.enabled)
            {
                Vector3 newVelocity = (TargetSyncPosition - targetRigidbody.Position) * InterpolateMovement / SendInterval;
                targetRigidbody.LinearVelocity = newVelocity;

                TargetSyncPosition += (TargetSyncLinearVelocity * Time.DeltaTime * MoveAheadRatio);
            }
        }

        private bool HasMoved()
        {
            float diff = 0;

            //Check if position has changed
            diff = Vector3.Distance(target.position, PrevPosition);

            if (diff > LocalMovementThreshold)
                return true;

            //check if rotation has changed
            diff = Quaternion.Dot(target.rotation, PrevRotation) - 1.0f;
            if (diff < -LocalRotationThreshold)
                return true;

            //Check if scale has changed (temp)
            diff = Vector3.Distance(target.scale, PrevScale);
            if (diff > LocalMovementThreshold)
                return true;


            if (targetRigidbody)
            {
                diff = targetRigidbody.LinearVelocity.LengthSquared() - prevVelocity;
                if (diff > LocalVelocityThreshold)
                    return true;
            }



            return false;

        }


        #region Write

        public override bool OnWrite(NetDataWriter writer, bool initialState)
        {
            if (initialState)
            {
                //Always write initial state
            }
            else if (!isDirty)
            {
                writer.Put(0);
                return false;
            }
            else
            {
                writer.Put(1);
            }
            writer.Put((int)SyncMode);
            switch (SyncMode)
            {
                case TransformSyncMode.SyncTransform:
                    WriteTransform(writer);
                    break;
                case TransformSyncMode.SyncRigidbody:
                    WriteRigidbody(writer);
                    break;
                default:
                    break;
            }
            return true;
        }

        private void WriteTransform(NetDataWriter writer)
        {
            writer.Put(target.position);

            writer.Put(target.rotation);

            writer.Put(target.scale);

            //PrevPosition = transform.position;
            //PrevRotation = transform.rotation;
            //PrevScale = transform.scale;

        }

        private void WriteRigidbody(NetDataWriter writer)
        {
            if (targetRigidbody)
            {
                writer.Put(targetRigidbody.enabled);
                if (!targetRigidbody.enabled)
                    WriteTransform(writer);
                else
                {
                    writer.Put(targetRigidbody.Position);
                    writer.Put(targetRigidbody.Rotation);
                    writer.Put(target.scale);
                }
                writer.Put(targetRigidbody.LinearVelocity);
                writer.Put(targetRigidbody.AngularVelocity);
                writer.Put(targetRigidbody.IsKinematic);
                writer.Put(targetRigidbody.AttachedCollider.isTrigger);
                prevVelocity = targetRigidbody.LinearVelocity.LengthSquared();
            }else
            {
                writer.Put(false);
                WriteTransform(writer);
                writer.Put(new Vector3());
                writer.Put(new Vector3());
                writer.Put(false);
                writer.Put(false);
            }
        }
        #endregion
        #region Read
        public override void OnRead(NetDataReader reader, bool initialState)
        {
            if (!initialState && reader.GetInt() == 0)
            {
                return; //No dirty bit or initial state. Lets get the fuck out of here!
            }
            SyncMode = (TransformSyncMode)reader.GetInt();
            switch (SyncMode)
            {
                case TransformSyncMode.SyncTransform:
                    ReadTransform(reader);
                    break;
                case TransformSyncMode.SyncRigidbody:
                    ReadRigidbody(reader);
                    break;
                default:
                    break;
            }

        }

        private void ReadTransform(NetDataReader reader)
        {
            CurrentPositionDuration = 0;
            if (isOwner)
            {
                //Read the data even though we do not use it. Otherwise the next component will get the wrong data.
                reader.GetVector3();
                reader.GetQuaternion();
                reader.GetVector3();
                return;
            }

            TargetSyncPosition = reader.GetVector3();
            target.rotation = reader.GetQuaternion();
            target.scale = reader.GetVector3();

#if (TRANSFORM_PRINT_DEBUG)
            String msg = "T_Target: " + targetRigidbody.Name + (targetRigidbody.enabled ? "T" : "F");
            msg += ";Pos: " + TargetSyncPosition.x + ", " + TargetSyncPosition.y + ", " + TargetSyncPosition.z;
            msg += ";Rot: " + target.rotation.x + ", " + target.rotation.y + ", " + target.rotation.z + ", " + target.rotation.w;
            ThomasEngine.Debug.Log(msg);
#endif

            if (Vector3.Distance(TargetSyncPosition, target.position) > SnapThreshhold)
            {
                target.position = TargetSyncPosition;
            }
        }

        private void InterpolatePosition()
        {
            if (!isOwner)
            {
                target.position = Vector3.Lerp(target.position, TargetSyncPosition, Math.Min(1.0f, (CurrentPositionDuration / SendInterval) * SmoothingFactor));
            }
        }

        private void ReadRigidbody(NetDataReader reader)
        {
            if (isOwner || !targetRigidbody)
            {
                //Read the data even though we do not use it. Otherwise the next component will get the wrong data.
                reader.GetBool();
                reader.GetVector3();
                reader.GetQuaternion();
                reader.GetVector3();

                reader.GetVector3();
                reader.GetVector3();
                reader.GetBool();
                reader.GetBool();
                return;
            }
            
            targetRigidbody.enabled = reader.GetBool();

            TargetSyncPosition = reader.GetVector3();
            TargetSyncRotation = reader.GetQuaternion();
            target.scale = reader.GetVector3();

#if (TRANSFORM_PRINT_DEBUG)
            String msg = "RB_Target: " + targetRigidbody.Name + (targetRigidbody.enabled ? "T" : "F");
            msg += ";Pos: " + TargetSyncPosition.x + ", " + TargetSyncPosition.y + ", " + TargetSyncPosition.z;
            msg += ";Rot: " + TargetSyncRotation.x + ", " + TargetSyncRotation.y + ", " + TargetSyncRotation.z + ", " + target.rotation.w;
            ThomasEngine.Debug.Log(msg);
#endif


            TargetSyncLinearVelocity = reader.GetVector3();
            TargetSyncAngularVelocity = reader.GetVector3();

            targetRigidbody.IsKinematic = reader.GetBool();
            targetRigidbody.AttachedCollider.isTrigger = reader.GetBool();

            float dist = Vector3.Distance(target.position, TargetSyncPosition);

            //float rotDiff = Quaternion.Dot(target.rotation, TargetSyncRotation) - 1.0f;

            

            if(!targetRigidbody.enabled)
            {
                target.position = TargetSyncPosition;
                target.rotation = TargetSyncRotation;
                return;
            }
            else
            {
                targetRigidbody.IgnoreNextTransformUpdate();
                targetRigidbody.Position = TargetSyncPosition;
                targetRigidbody.Rotation = TargetSyncRotation;
                targetRigidbody.LinearVelocity = TargetSyncLinearVelocity;
                targetRigidbody.AngularVelocity = TargetSyncAngularVelocity;

            }
            
        }
#endregion
    }
}