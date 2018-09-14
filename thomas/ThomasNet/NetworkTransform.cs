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
        Vector3 pos;
        Vector3 scale;
        Quaternion rot;


        public override void Start()
        {
            pos = new Vector3(0.0f);
            scale = Vector3.One;
            rot = new Quaternion();
            
        }
        public override void Update()
        {
            if (!isOwner)
            {
                transform.position = Vector3.Lerp(transform.position, pos, Time.DeltaTime * 15);
                transform.scale = Vector3.Lerp(transform.scale, scale, Time.DeltaTime * 15);
                transform.rotation = Quaternion.Lerp(transform.rotation, rot, Time.DeltaTime * 30);
            }
            else
            {
                pos = transform.position;
                scale = transform.scale;
                rot = transform.rotation;
            }
            



        }

        public override void Read(NetPacketReader reader)
        {
            pos.x = reader.GetFloat();
            pos.y = reader.GetFloat();
            pos.z = reader.GetFloat();

            rot.x = reader.GetFloat();
            rot.y = reader.GetFloat();
            rot.z = reader.GetFloat();
            rot.w = reader.GetFloat();

            scale.x = reader.GetFloat();
            scale.y = reader.GetFloat();
            scale.z = reader.GetFloat();
        }

        public override void Write(NetDataWriter writer)
        {
            writer.Put(pos.x);
            writer.Put(pos.y);
            writer.Put(pos.z);

            writer.Put(rot.x);
            writer.Put(rot.y);
            writer.Put(rot.z);
            writer.Put(rot.w);

            writer.Put(scale.x);
            writer.Put(scale.y);
            writer.Put(scale.z);
        }
     
    }
}
