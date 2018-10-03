using System.ComponentModel;
using LiteNetLib;
using LiteNetLib.Utils;

namespace ThomasEngine.Network
{
    [HideInInspector]
    public class NetworkComponent : ScriptComponent
    {        
        internal NetworkID networkID;

        [Browsable(false)]
        public bool isOwner
        {
            get {return networkID != null ? networkID.Owner : false; }
            set { if (networkID != null) { networkID.Owner = value; } }
        }

        [Browsable(false)]
        public bool isClient
        {
            get { return NetworkManager.instance.isClient; }
        }

        [Browsable(false)]
        public bool isServer
        {
            get { return !isClient; }
        }

        virtual public void Read(NetPacketReader reader)
        {

        }

        virtual public void Write(NetDataWriter writer)
        {

        }
    }


}
