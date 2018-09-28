using System;
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

        NetDataWriter DataWriter = new NetDataWriter();
        float TimeLeftUntilUpdate = 0;

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
                TimeLeftUntilUpdate -= Time.ActualDeltaTime;
                if(TimeLeftUntilUpdate <= 0)
                {
                    TimeLeftUntilUpdate = Manager.InternalManager.UpdateTime/1000.0f;
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
            DataWriter.Reset();
            PacketType packetType = IsPlayer ? PacketType.PLAYER_DATA : PacketType.OBJECT_DATA;
            DataWriter.Put((int)packetType);
            DataWriter.Put(initalState);
            foreach (NetworkComponent comp in networkComponentsCache)
            {
                comp.OnWrite(DataWriter, initalState);
            }
            Manager.InternalManager.SendToAll(DataWriter, DeliveryMethod.Sequenced);
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
