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
        public override void Start()
        {

        }
        public override void Update()
        {

        }
       
        public override void Read(NetPacketReader reader)
        {
            Vector3 pos = new Vector3(transform.position.x, transform.position.y, transform.position.z);
            pos.x += (reader.GetFloat() - transform.position.x);
            pos.y += (reader.GetFloat() - transform.position.y);
            pos.z += (reader.GetFloat() - transform.position.z);

            transform.position = pos;

            Quaternion rot = new Quaternion(transform.rotation.x, transform.rotation.y, transform.rotation.z, transform.rotation.w);
            rot.x += (reader.GetFloat() - transform.rotation.x);
            rot.y += (reader.GetFloat() - transform.rotation.y);
            rot.z += (reader.GetFloat() - transform.rotation.z);
            rot.w += (reader.GetFloat() - transform.rotation.w);

            transform.rotation = rot;

            Vector3 scale = new Vector3(transform.scale.x, transform.scale.y, transform.scale.z);
            scale.x += (reader.GetFloat() - transform.scale.x);
            scale.y += (reader.GetFloat() - transform.scale.y);
            scale.z += (reader.GetFloat() - transform.scale.z);

            transform.scale = scale;
        }

        public override void Write(NetDataWriter writer)
        {
            writer.Put(transform.position.x);
            writer.Put(transform.position.y);
            writer.Put(transform.position.z);

            writer.Put(transform.rotation.x);
            writer.Put(transform.rotation.y);
            writer.Put(transform.rotation.z);
            writer.Put(transform.rotation.w);

            writer.Put(transform.scale.x);
            writer.Put(transform.scale.y);
            writer.Put(transform.scale.z);
        }
     
    }
}
