using System;
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
        public int ID { set; get; }
        List<NetworkComponent> networkComponents;
        public override void Start()
        {
            ID = NetworkManager.instance.Register(this);
            networkComponents = gameObject.GetComponents<NetworkComponent>();
            
        }

        public override void Read(NetPacketReader reader)
        {
            foreach (NetworkComponent comp in networkComponents)
            {
                if (comp != this)
                    comp.Read(reader);
            }
        }
        public override void Write(NetDataWriter writer)
        {
            foreach (NetworkComponent comp in networkComponents)
            {
                if (comp != this)
                    comp.Write(writer);
            }
        }
    }
}
