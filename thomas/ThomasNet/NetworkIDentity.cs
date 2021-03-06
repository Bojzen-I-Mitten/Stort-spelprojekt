﻿using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using LiteNetLib;
using LiteNetLib.Utils;

namespace ThomasEngine.Network
{
    public class NetworkIdentity : ScriptComponent
    {


        public int PrefabID = -1;
        public int Ping = 0;
        private bool _Owner = false;
        public bool Owner {
            set
            {
                if (value && !IsPlayer) // IsPlayer, redundant? Players are activated through ReceiveOwnership.
                    TakeOwnership();
                else if (!value && _Owner)
                    ReceiveOwnershipStatus(false);
            }
            get { return (NetworkManager.instance && NetworkManager.instance.ReadOwnerAsNormal) ? false : _Owner; }
        }
        /* Sets Ownership Status of the identity, assumes ownership status is resolved and forcefully set.
        */
        public void ReceiveOwnershipStatus(bool ownership)
        {
            _Owner = ownership;
            if (ownership)  // Trigger enabled
            {
                foreach (NetworkComponent nc in networkComponentsCache)
                    nc.OnGotOwnership();
            }
            else        // Trigger disabled
            {
                foreach (NetworkComponent nc in networkComponentsCache)
                    nc.OnLostOwnership();
            }
        }

        public int ID {
            get {
                if (Manager?.NetScene != null)
                    return Manager.NetScene.NetworkObjects.FirstOrDefault(pair => pair.Value == this).Key;
                else
                    return 0; // One line master race.
            }
        } 

        [Browsable(false)]
        public bool IsPlayer { get; set; } = false;

        NetDataWriter DataWriter = new NetDataWriter();
        float TimeLeftUntilUpdate = 0;

        NetworkManager Manager
        {
            get{return NetworkManager.instance;}
        }

        List<NetworkComponent> _networkComponentsCache;
        List<NetworkComponent> networkComponentsCache
        {
            get
            {
                if(_networkComponentsCache == null)
                    _networkComponentsCache = gameObject.GetComponents<NetworkComponent>();
                return _networkComponentsCache;

            }
        }
        public override void OnEnable()
        {
            _networkComponentsCache = gameObject.GetComponents<NetworkComponent>();
        }
        
        public void OnDisconnect()
        {
            foreach (NetworkComponent nc in networkComponentsCache)
                nc.OnDisconnect();
        }

        public override void Update()
        {
            if(Owner && Manager.InternalManager.IsRunning)
            {
                TimeLeftUntilUpdate -= Time.ActualDeltaTime;
                if(TimeLeftUntilUpdate <= 0)
                {
                    TimeLeftUntilUpdate = Manager.InternalManager.UpdateTime/1000.0f;
                    WriteFrameData();
                }
            }
        }

        public void RefreshCache()
        {
            _networkComponentsCache = gameObject.GetComponents<NetworkComponent>();
        }

        public void WriteFrameData()
        {
            DataWriter.Reset();

            PacketType packetType = IsPlayer ? PacketType.PLAYER_DATA : PacketType.OBJECT_DATA;
            DataWriter.Put((int)packetType);
            if (packetType == PacketType.OBJECT_DATA)
                DataWriter.Put(ID);

            DataWriter.Put(false);

            WriteData(DataWriter, false);
            Manager.InternalManager.SendToAll(DataWriter, DeliveryMethod.Sequenced);
        }

        public void WriteInitialData()
        {
            DataWriter.Reset();

            PacketType packetType = IsPlayer ? PacketType.PLAYER_DATA : PacketType.OBJECT_DATA;
            DataWriter.Put((int)packetType);
            if (packetType == PacketType.OBJECT_DATA)
                DataWriter.Put(ID);

            DataWriter.Put(true);

            WriteData(DataWriter, true);
            Manager.InternalManager.SendToAll(DataWriter, DeliveryMethod.ReliableOrdered);
        }

        public void WriteData(NetDataWriter writer, bool initalState = false)
        {
            writer.Put(gameObject.GetActive());
            
            foreach (NetworkComponent comp in networkComponentsCache)
            {

                comp.OnWrite(writer, initalState);
            }
        }

        public void ReadData(NetDataReader reader, bool initialState)
        {
            gameObject.SetActive(reader.GetBool());
            

            foreach (NetworkComponent comp in networkComponentsCache)
            {
                comp.OnRead(reader, initialState);
            }
        }

        public void ReadRPC(NetDataReader reader)
        {
            NetSerializer serializer = new NetSerializer();
            string methodName = reader.GetString();
            System.Reflection.MethodInfo methodInfo = null;
            foreach (NetworkComponent comp in networkComponentsCache)
            {
                methodInfo = comp.GetType().GetMethod(methodName);
                if (methodInfo != null)
                {
                    comp.ReadRPC(methodInfo, reader);
                    return;
                }
            }


        }

        internal void SendRPC(string methodName, object[] parameters)
        {
            Manager.SendRPC(this.ID, methodName, parameters);
        }

        private void TakeOwnership()
        {
            if (Manager != null)
            {
                if (!_Owner)
                {
                    // Broadcast ownership
                    Manager.TakeOwnership(this);
                    ReceiveOwnershipStatus(true);
                }
            }       
        }
    }
}
