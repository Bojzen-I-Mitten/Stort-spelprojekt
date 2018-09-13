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
            Vector3 position = new Vector3();
            position.x = reader.GetFloat();
            position.y = reader.GetFloat();
            position.z = reader.GetFloat();
            transform.position = position;

            Quaternion rotation = new Quaternion();
            rotation.w = reader.GetFloat();
            rotation.x = reader.GetFloat();
            rotation.y = reader.GetFloat();
            rotation.z = reader.GetFloat();
            transform.rotation = rotation;

            Vector3 scale = new Vector3();
            scale.x = reader.GetFloat();
            scale.y = reader.GetFloat();
            scale.z = reader.GetFloat();
            transform.scale = scale;
        }

        public override void Write(NetDataWriter writer)
        {
            writer.Put(transform.position.x);
            writer.Put(transform.position.y);
            writer.Put(transform.position.z);

            writer.Put(transform.rotation.w);
            writer.Put(transform.rotation.x);
            writer.Put(transform.rotation.y);
            writer.Put(transform.rotation.z);
            
            writer.Put(transform.scale.x);
            writer.Put(transform.scale.y);
            writer.Put(transform.scale.z);
        }
     
    }
}
