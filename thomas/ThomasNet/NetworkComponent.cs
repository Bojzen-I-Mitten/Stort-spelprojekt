using System.ComponentModel;
using LiteNetLib;
using LiteNetLib.Utils;

namespace ThomasEngine.Network
{
    [HideInInspector]
    public class NetworkComponent : ScriptComponent
    {
        protected bool isDirty = false;
        private int prefabID;
        private NetworkID NetID;


        protected int ID
        {
            get { return NetworkID.ID; }
        }

        protected NetworkID NetworkID
        {
            get
            {
                if(NetID == null)
                {
                    NetID = gameObject.GetComponent<NetworkID>();
                    if(NetID == null)
                    {
                        Debug.Log("There is no networkID on this object.");
                    }
                }
                return NetID;
            }
        }

        [Browsable(false)]
        public bool isOwner
        {
            get { return NetworkID.Owner; } 
        }

        virtual public void OnRead(NetPacketReader reader, bool initialState)
        {
            if (!initialState)
                reader.GetInt();
            
        }

        virtual public bool OnWrite(NetDataWriter writer, bool initialState)
        {
            if (!initialState)
                writer.Put(0);
            return false;
        }

        virtual public void OnGotOwnership() { }
        virtual public void OnLostOwnership() { }
    }


}
