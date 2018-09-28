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
        private NetworkID m_networkID;



        protected int NetID
        {
            get { return NetworkID.ID; }
        }

        protected NetworkID NetworkID
        {
            get
            {
                if(m_networkID == null)
                {
                    m_networkID = gameObject.GetComponent<NetworkID>();
                    if(m_networkID == null)
                    {
                        Debug.Log("There is no networkID on this object.");
                    }
                }
                return m_networkID;
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
