﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using LiteNetLib;
using LiteNetLib.Utils;

namespace ThomasEngine.Network
{
    public class NetworkID : NetworkComponent
    {

        public bool Owner { get; set; } = false;
        public int ID { set; get; }
        List<NetworkComponent> networkComponents;
        public override void OnEnable()
        {
            ID = NetworkManager.instance.Register(this);
            networkComponents = gameObject.GetComponents<NetworkComponent>();
            networkComponents.ForEach((comp) => { comp.networkID = this; });
        }

        public override void Read(NetPacketReader reader)
        {
            foreach (NetworkComponent comp in networkComponents)
            {
                if (!Owner)
                {
                    if (comp != this)
                    {
                        comp.Read(reader);
                    }
                }
            }
        }
        public override void Write(NetDataWriter writer)
        {
            if (Owner)
            {
                writer.Put(ID);
                foreach (NetworkComponent comp in networkComponents)
                {
                    if (comp != this)
                        comp.Write(writer);
                }
            }

        }
    }
}
