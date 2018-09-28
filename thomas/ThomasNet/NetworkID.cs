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
    public class NetworkID : ScriptComponent
    {

        public bool Owner { set; get; } = false;
        [Browsable(false)]
        public bool IsPlayer { get; set; } = false;
        public int ID { set; get; }

        NetDataWriter m_dataWriter = new NetDataWriter();
        float timeLeftUntilUpdate = 0;

        NetworkManager Manager
        {
            get{return NetworkManager.instance;}
        }

        List<NetworkComponent> networkComponentsCache;
        public override void OnEnable()
        {
            networkComponentsCache = gameObject.GetComponents<NetworkComponent>();
        }
        
        public override void Update()
        {
            if(Owner && Manager.InternalManager.IsRunning)
            {
                timeLeftUntilUpdate -= Time.ActualDeltaTime;
                if(timeLeftUntilUpdate <= 0)
                {
                    timeLeftUntilUpdate = Manager.InternalManager.UpdateTime/1000.0f;
                    WriteFrameData();
                }
            }
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
            m_dataWriter.Reset();
            PacketType packetType = IsPlayer ? PacketType.PLAYER_DATA : PacketType.OBJECT_DATA;
            m_dataWriter.Put((int)packetType);
            m_dataWriter.Put(initalState);
            foreach (NetworkComponent comp in networkComponentsCache)
            {
                comp.OnWrite(m_dataWriter, initalState);
            }
            Manager.InternalManager.SendToAll(m_dataWriter, DeliveryMethod.Sequenced);
        }

        public void ReadData(NetPacketReader reader, bool initialState)
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
