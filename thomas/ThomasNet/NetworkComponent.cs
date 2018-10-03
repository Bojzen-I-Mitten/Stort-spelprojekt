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
        private NetworkIdentity networkIdentity;


        protected int ID
        {
            get { return Identity.ID; }
        }

        protected NetworkIdentity Identity
        {
            get
            {
                if (networkIdentity == null)
                {
                    networkIdentity = gameObject.GetComponent<NetworkIdentity>();
                    if (networkIdentity == null)
                    {
                        Debug.Log("There is no networkIdentity on this object.");
                    }
                }
                return networkIdentity;
            }
        }

        protected float SendInterval
        {
            get{return 1.0f/NetworkManager.instance.TICK_RATE;}
        }

        [Browsable(false)]
        public bool isOwner
        {
            get { return Identity.Owner; } 
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
        
        protected void TakeOwnership(GameObject obj)
        {
            NetworkIdentity networkIdentity = obj.GetComponent<NetworkIdentity>();
            if(networkIdentity != null)
            {
                networkIdentity.Owner = true;
            }
        }

    }


}
