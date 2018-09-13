﻿using System;
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
            pos.x += transform.position.x - reader.GetFloat();
            pos.y += transform.position.y - reader.GetFloat();
            pos.z += transform.position.z - reader.GetFloat();

            transform.position = pos;

            Quaternion rot = new Quaternion(transform.rotation.x, transform.rotation.y, transform.rotation.z, transform.rotation.w);
            rot.x += transform.rotation.x - reader.GetFloat();
            rot.y += transform.rotation.y - reader.GetFloat();
            rot.z += transform.rotation.z - reader.GetFloat();
            rot.w += transform.rotation.w - reader.GetFloat();

            transform.rotation = rot;

            Vector3 scale = new Vector3(transform.scale.x, transform.scale.y, transform.scale.z);
            scale.x += transform.scale.x - reader.GetFloat();
            scale.y += transform.scale.y - reader.GetFloat();
            scale.z += transform.scale.z - reader.GetFloat();

            transform.scale = scale;
        }

        public override void Write(NetDataWriter writer)
        {
            writer.Put(transform.position.x);
            writer.Put(transform.position.y);
            writer.Put(transform.position.z);
        }
     
    }
}
