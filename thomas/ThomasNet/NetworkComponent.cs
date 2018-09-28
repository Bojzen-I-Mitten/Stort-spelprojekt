﻿using System.ComponentModel;
using LiteNetLib;
using LiteNetLib.Utils;

namespace ThomasEngine.Network
{
    [HideInInspector]
    public class NetworkComponent : ScriptComponent
    {
        protected bool isDirty = false;
        private int prefabID;
        private NetworkIdentity networkIdentiy;


        protected int ID
        {
            get { return Identity.ID; }
        }

        protected NetworkIdentity Identity
        {
            get
            {
                if(networkIdentiy == null)
                {
                    networkIdentiy = gameObject.GetComponent<NetworkIdentity>();
                    if(networkIdentiy == null)
                    {
                        Debug.Log("There is no networkID on this object.");
                    }
                }
                return networkIdentiy;
            }
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
            NetworkIdentity networkIdentiy = obj.GetComponent<NetworkIdentity>();
            if(networkIdentiy != null)
            {
                networkIdentiy.Owner = true;
            }
        }

    }


}
