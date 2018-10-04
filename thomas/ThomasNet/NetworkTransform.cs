using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using LiteNetLib;
using LiteNetLib.Utils;

namespace ThomasEngine.Network
{

    public class NetworkTransform : NetworkComponent
    {
        struct LerpState
        {
            public object state; //maybe template instead? idk
            public float timestamp;
            public float duration;

            public LerpState(object state, float timestamp)
            {
                this.state = state;
                this.timestamp = timestamp;
                duration = 0;
            }

            public void SetDuration(float leftStateTimestamp, float maxTimestamp)
            {
                float unwrappedTimeStep = timestamp;
                while (leftStateTimestamp > unwrappedTimeStep)
                    unwrappedTimeStep += maxTimestamp;

                duration = unwrappedTimeStep;
            }
        }

        private const int MaxTimestamp = 64;

        Vector3 PrevPosition;
        Quaternion PrevRotation;
        Vector3 PrevScale;
        float prevVelocity;

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

        Rigidbody attachedRigidbody;

        public enum TransformSyncMode
        {
            SyncNone = 0,
            SyncTransform = 1,
            SyncRigidbody = 2
        }

        public TransformSyncMode SyncMode { get; set; } = TransformSyncMode.SyncTransform;

        public bool SyncParent { get; set; } = true;

        public override void Awake()
        {
            PrevPosition = transform.position;
            PrevRotation = transform.rotation;
            PrevScale = transform.scale;
            TargetSyncPosition = transform.position;

            attachedRigidbody = gameObject.GetComponent<Rigidbody>();
            if (attachedRigidbody)
            {
                TargetSyncLinearVelocity = attachedRigidbody.LinearVelocity;
                TargetSyncAngularVelocity = attachedRigidbody.AngularVelocity;
            }else
            {
                Debug.LogError("No rigidbody");
            }
        }

        public override void Update()
        {
            //CurrentPositionDuration += Time.DeltaTime;

            if (isOwner)
            {
                isDirty = HasMoved();
            }

        }

        public override void FixedUpdate()
        {
            switch(SyncMode)
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
            if(!isOwner && attachedRigidbody)
            {
                Vector3 newVelocity = (TargetSyncPosition - transform.position) * InterpolateMovement / SendInterval;
                attachedRigidbody.LinearVelocity = newVelocity;

                TargetSyncPosition += (TargetSyncLinearVelocity * Time.DeltaTime * MoveAheadRatio);
            }
        }

        private bool HasMoved()
        {
            float diff = 0;

            //Check if position has changed
            diff = Vector3.Distance(transform.position, PrevPosition);

            if (diff > LocalMovementThreshold)
                return true;

            //check if rotation has changed
            diff = Quaternion.Dot(transform.rotation, PrevRotation) - 1.0f;
            if (diff < -LocalRotationThreshold)
                return true;

            //Check if scale has changed (temp)
            diff = Vector3.Distance(transform.scale, PrevScale);
            if (diff > LocalMovementThreshold)
                return true;


            if(attachedRigidbody)
            {
                diff = attachedRigidbody.LinearVelocity.LengthSquared() - prevVelocity;
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
            writer.Put(transform.position);

            writer.Put(transform.rotation);

            writer.Put(transform.scale);

            //PrevPosition = transform.position;
            //PrevRotation = transform.rotation;
            //PrevScale = transform.scale;

        }

        private void WriteRigidbody(NetDataWriter writer)
        {
            WriteTransform(writer);

            if (attachedRigidbody)
            {
                writer.Put(attachedRigidbody.LinearVelocity);
                writer.Put(attachedRigidbody.AngularVelocity);

                prevVelocity = attachedRigidbody.LinearVelocity.LengthSquared();
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
            if (isOwner)
            {
                //Read the data even though we do not use it. Otherwise the next component will get the wrong data.
                reader.GetVector3();
                reader.GetQuaternion();
                reader.GetVector3();
                return;
            }

            TargetSyncPosition = reader.GetVector3();
            CurrentPositionDuration = 0;
            transform.rotation = reader.GetQuaternion();
            transform.scale = reader.GetVector3();

            if(Vector3.Distance(TargetPosition, transform.position) > SnapThresholdDistance)
            {
                transform.position = TargetPosition;
            }
        }

        private void InterpolatePosition()
        {
            transform.position = Vector3.Lerp(transform.position, TargetPosition, Math.Min(1.0f, CurrentPositionDuration / SendInterval));
        }

        private void ReadRigidbody(NetPacketReader reader)
        {


            if (isOwner || !attachedRigidbody)
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
            transform.rotation = reader.GetQuaternion();
            transform.scale = reader.GetVector3();

            TargetSyncLinearVelocity = reader.GetVector3();
            TargetSyncAngularVelocity = reader.GetVector3();

            float dist = Vector3.Distance(transform.position, TargetSyncPosition);
            if(dist > SnapThreshhold || !attachedRigidbody.enabled)
            {
                transform.position = TargetSyncPosition;
                attachedRigidbody.LinearVelocity = TargetSyncLinearVelocity;
                attachedRigidbody.AngularVelocity = TargetSyncAngularVelocity;
            }
        }
        #endregion
    }
}
