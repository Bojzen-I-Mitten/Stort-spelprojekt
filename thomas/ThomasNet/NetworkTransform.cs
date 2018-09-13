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

        public int interpolateAmount { get; set; } = 15;

        public override void Start()
        {
            pos = new Vector3(0.0f);
            scale = new Vector3(0.0f);
            rot = new Quaternion(0.0f, 0.0f, 0.0f, 0.0f);
            
        }
        public override void Update()
        {
            if (NetworkManager.instance.isClient)
            {
                transform.position = Vector3.Lerp(transform.position, pos, Time.DeltaTime * interpolateAmount);
            }
           
            //temp = transform.scale;
            //transform.scale = Vector3.Lerp(temp, scale, 1);

            //Quaternion temp1 = transform.rotation;
            //transform.rotation = Quaternion.Lerp(temp1, rot, 1);
        }

        public override void Read(NetPacketReader reader)
        {
            pos.x = reader.GetFloat();
            pos.y = reader.GetFloat();
            pos.z = reader.GetFloat();

            //rot.x = reader.GetFloat();
            //rot.y = reader.GetFloat();
            //rot.z = reader.GetFloat();
            //rot.w = reader.GetFloat();

            //scale.x = reader.GetFloat();
            //scale.y = reader.GetFloat();
            //scale.z = reader.GetFloat();
        }

        public override void Write(NetDataWriter writer)
        {
            writer.Put(transform.position.x);
            writer.Put(transform.position.y);
            writer.Put(transform.position.z);

            //writer.Put(transform.rotation.x);
            //writer.Put(transform.rotation.y);
            //writer.Put(transform.rotation.z);
            //writer.Put(transform.rotation.w);

            //writer.Put(transform.scale.x);
            //writer.Put(transform.scale.y);
            //writer.Put(transform.scale.z);
        }
     
    }
}
