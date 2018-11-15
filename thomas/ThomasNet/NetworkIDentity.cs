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

        private bool _Owner = false;
        public bool Owner {
            set
            {
                if (value && IsPlayer) // IsPlayer, redundant? Players are activated through ReceiveOwnership.
                    TakeOwnership();
                else if (!value && _Owner)
                    ReceiveOwnershipStatus(false);
            }
            get { return _Owner; }
        }
        /* Sets Ownership Status of the identity, assumes ownership status is resolved and forcefully set.
        */
        public void ReceiveOwnershipStatus(bool ownership)
        {
            _Owner = ownership;
            if (ownership)  // Trigger enabled
            {
                foreach (NetworkComponent nc in _networkComponentsCache)
                    nc.OnGotOwnership();
            }
            else        // Trigger disabled
            {
                foreach (NetworkComponent nc in _networkComponentsCache)
                    nc.OnLostOwnership();
            }
        }

        public int ID {
            get {
                return Manager?.NetScene != null ? Manager.NetScene.NetworkObjects.FirstOrDefault(pair => pair.Value == this).Key : 0; // One line master race.
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
            WriteData(false);
        }

        public void WriteInitialData()
        {
            WriteData(true);
        }

        private void WriteData(bool initalState=false)
        {
            DataWriter.Reset();
            PacketType packetType = IsPlayer ? PacketType.PLAYER_DATA : PacketType.OBJECT_DATA;
            DataWriter.Put((int)packetType);
            if (packetType == PacketType.OBJECT_DATA)
                DataWriter.Put(ID);

            DataWriter.Put(initalState);
            if (initalState)
            {
                DataWriter.Put(gameObject.activeSelf);
            }
            foreach (NetworkComponent comp in networkComponentsCache)
            {
                
                comp.OnWrite(DataWriter, initalState);
            }
            Manager.InternalManager.SendToAll(DataWriter, initalState ? DeliveryMethod.ReliableOrdered : DeliveryMethod.Sequenced);
        }

        public void ReadData(NetPacketReader reader, bool initialState)
        {
            if (initialState)
            {
                bool active = reader.GetBool();
                gameObject.activeSelf = active;
            }

            foreach (NetworkComponent comp in networkComponentsCache)
            {
                comp.OnRead(reader, initialState);
            }
        }

        public void ReadRPC(NetPacketReader reader)
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
