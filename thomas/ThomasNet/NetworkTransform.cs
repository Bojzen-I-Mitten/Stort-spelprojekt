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



        Vector3 PrevPosition;
        Quaternion PrevRotation;
        Vector3 PrevScale;

        float LocalMovementThreshold = 0.00001f;
        float LocalRotationThreshold = 0.00001f;

        public enum TransformSyncMode
        {
            SyncNone = 0,
            SyncTransform = 1,
            SyncRigidbody_TODO = 2
        }

        public TransformSyncMode SyncMode { get; set; } = TransformSyncMode.SyncTransform;

        public override void Awake()
        {
            PrevPosition = transform.position;
            PrevRotation = transform.rotation;
            PrevScale = transform.scale;
        }
        public override void Update()
        {
            if (isOwner)
            {
                isDirty = HasMoved();
            }

        }

        private bool HasMoved()
        {
            float diff = 0;


            //Check if position has changed
            diff = (transform.position - PrevPosition).Length();

            if (diff > LocalMovementThreshold)
                return true;

            //check if rotation has changed
            diff = Quaternion.Dot(transform.rotation, PrevRotation) - 1.0f;
            if (diff < -LocalRotationThreshold)
                return true;

            //Check if scale has changed (temp)
            diff = (transform.scale - PrevScale).Length();
            if (diff > LocalMovementThreshold)
                return true;

            return false;

        }


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

            switch(SyncMode)
            {
                case TransformSyncMode.SyncTransform:
                    WriteTransform(writer);
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

            PrevPosition = transform.position;
            PrevRotation = transform.rotation;
            PrevScale = transform.scale;

        }
        
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

            transform.position = reader.GetVector3();
            transform.rotation = reader.GetQuaternion();
            transform.scale = reader.GetVector3();
        }

    }
}
