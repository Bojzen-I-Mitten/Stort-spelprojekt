using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using LiteNetLib;
using LiteNetLib.Utils;

namespace ThomasEngine.Network
{
    public class NetworkID : ScriptComponent
    {

        public bool Owner { set; get; } = false;
        public int ID { set; get; }
        List<NetworkComponent> networkComponentsCache;
        public override void OnEnable()
        {
            //ID = NetworkManager.instance.Register(this);
            networkComponentsCache = gameObject.GetComponents<NetworkComponent>();
        }

        public void WriteAllVars(NetDataWriter writer)
        {
            foreach (NetworkComponent comp in networkComponentsCache)
            {
                comp.OnWrite(writer, true);
            }
        }

        public void NetworkUpdate(NetDataWriter writer)
        {
            foreach (NetworkComponent comp in networkComponentsCache)
            {
                comp.OnWrite(writer, false);
            }
        }

        public void OnUpdateVars(NetPacketReader reader, bool initialState)
        {
            if(initialState && networkComponentsCache == null)
            {
                networkComponentsCache = gameObject.GetComponents<NetworkComponent>();
            }
            foreach (NetworkComponent comp in networkComponentsCache)
            {
                comp.OnRead(reader, initialState);
            }
        }
    }
}
