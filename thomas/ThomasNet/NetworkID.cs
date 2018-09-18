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

        public bool Owner { set; get; } = false;
        public int ID { set; get; }
        List<NetworkComponent> networkComponents;
        public override void OnEnable()
        {
            //ID = NetworkManager.instance.Register(this);
            networkComponents = gameObject.GetComponents<NetworkComponent>();
            networkComponents.ForEach((comp) => { comp.networkID = this; });
            networkComponents.Remove(this);
        }

        public override void Read(NetPacketReader reader)
        {
            foreach (NetworkComponent comp in networkComponents)
            {
                //if (!Owner)
                //{
                    comp.Read(reader);
                //}
            }
        }
        public override void Write(NetDataWriter writer)
        {
            if ((Owner || isServer) && enabled)
            {
                writer.Put(ID);
                foreach (NetworkComponent comp in networkComponents)
                {
                    comp.Write(writer);
                }
            }

        }
    }
}
