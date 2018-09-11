using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using LiteNetLib;
using LiteNetLib.Utils;

namespace ThomasEngine.Network
{

    class NetworkComponentTransform : NetworkComponent
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
        }

        public override void Write(NetDataWriter writer)
        {
            writer.Put(transform.position.x);
            writer.Put(transform.position.y);
            writer.Put(transform.position.z);
        }
     
    }
}
