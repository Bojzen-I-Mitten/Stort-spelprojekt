using System;
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
        protected float SmoothingFactor = 1.0f / 3;


        public int ID
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
            get { return Identity ? Identity.Owner : false; } 
        }

        virtual public void OnRead(NetDataReader reader, bool initialState)
        {
        }

        virtual public bool OnWrite(NetDataWriter writer, bool initialState)
        {
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

        protected void SendRPC(string methodName, params object[] parameters)
        {
            Identity.SendRPC(methodName, parameters);
        }

        internal void ReadRPC(System.Reflection.MethodInfo methodInfo, NetDataReader reader)
        {
            object[] parameters = RpcUtils.ReadRPCParameters(methodInfo, reader);
            methodInfo.Invoke(this, parameters);
        }
    }


}
