using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using LiteNetLib;
using LiteNetLib.Utils;

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

        const float SnapThreshhold = 5.0f;

        const float InterpolateRotation = 1.0f;
        const float InterpolateMovement = 1.0f;

        Vector3 TargetSyncPosition;
        Quaternion TargetSyncRotation;
        Vector3 TargetSyncLinearVelocity;
        Vector3 TargetSyncAngularVelocity;

        Rigidbody targetRigidbody;

        Transform _target;
        [Browsable(false)]
        public Transform target {
            get {
                if (!_target)
                    _target = transform;
                return _target;
            }
            set
            {
                _target = value;
            }
        }

        public enum TransformSyncMode
        {
            SyncNone = 0,
            SyncTransform = 1,
            SyncRigidbody = 2
        }

        public TransformSyncMode SyncMode { get; set; } = TransformSyncMode.SyncTransform;

        public bool SyncParent { get; set; } = true;

        public void SetTarget(Transform newTarget)
        {
            _target = newTarget;
        }

        public override void Start()
        {
            PrevPosition = target.localPosition;
            PrevRotation = target.localRotation;
            PrevScale = target.localScale;
            TargetSyncPosition = target.localPosition;

            targetRigidbody = target.gameObject.GetComponent<Rigidbody>();
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

            if (isOwner)
            {
                isDirty = true;// HasMoved();
            }

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
            if (!isOwner && targetRigidbody)
            {
                Vector3 newVelocity = (TargetSyncPosition - target.localPosition) * InterpolateMovement / SendInterval;
                targetRigidbody.LinearVelocity = newVelocity;

                TargetSyncPosition += (TargetSyncLinearVelocity * Time.DeltaTime * MoveAheadRatio);
            }
        }

        private bool HasMoved()
        {
            float diff = 0;

            //Check if position has changed
            diff = Vector3.Distance(target.localPosition, PrevPosition);

            if (diff > LocalMovementThreshold)
                return true;

            //check if rotation has changed
            diff = Quaternion.Dot(target.localRotation, PrevRotation) - 1.0f;
            if (diff < -LocalRotationThreshold)
                return true;

            //Check if scale has changed (temp)
            diff = Vector3.Distance(target.localScale, PrevScale);
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
            writer.Put(target.localPosition);

            writer.Put(target.localRotation);

            writer.Put(target.localScale);

            //PrevPosition = transform.position;
            //PrevRotation = transform.rotation;
            //PrevScale = transform.scale;

        }

        private void WriteRigidbody(NetDataWriter writer)
        {
            WriteTransform(writer);

            if (targetRigidbody)
            {
                writer.Put(targetRigidbody.LinearVelocity);
                writer.Put(targetRigidbody.AngularVelocity);

                prevVelocity = targetRigidbody.LinearVelocity.LengthSquared();
            }
        }

        #endregion
        #region Read
        public override void OnRead(NetPacketReader reader, bool initialState)
        {
            if (!initialState && reader.GetInt() == 0)
            {
                return; //No dirty bit or initial state. Lets get the fuck out of here!
            }

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

        private void ReadTransform(NetPacketReader reader)
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
            target.localRotation = reader.GetQuaternion();
            target.localScale = reader.GetVector3();

            if (Vector3.Distance(TargetSyncPosition, target.position) > SnapThreshhold)
            {
                target.localPosition = TargetSyncPosition;
            }
        }

        private void InterpolatePosition()
        {

            if (!isOwner)
            {
                target.localPosition = TargetSyncPosition;// Vector3.Lerp(target.localPosition, TargetSyncPosition, Math.Min(1.0f, (CurrentPositionDuration / SendInterval) * SmoothingFactor));
            }
        }

        private void ReadRigidbody(NetPacketReader reader)
        {


            if (isOwner || !targetRigidbody)
            {
                //Read the data even though we do not use it. Otherwise the next component will get the wrong data.
                reader.GetVector3();
                reader.GetQuaternion();
                reader.GetVector3();

                reader.GetVector3();
                reader.GetVector3();

                return;
            }

            TargetSyncPosition = reader.GetVector3();
            target.localRotation = reader.GetQuaternion();
            target.localScale = reader.GetVector3();

            TargetSyncLinearVelocity = reader.GetVector3();
            TargetSyncAngularVelocity = reader.GetVector3();

            float dist = Vector3.Distance(target.localPosition, TargetSyncPosition);
            if (dist > SnapThreshhold || !targetRigidbody.enabled)
            {
                target.localPosition = TargetSyncPosition;
                targetRigidbody.LinearVelocity = TargetSyncLinearVelocity;
                targetRigidbody.AngularVelocity = TargetSyncAngularVelocity;
            }
        }
        #endregion
    }
}