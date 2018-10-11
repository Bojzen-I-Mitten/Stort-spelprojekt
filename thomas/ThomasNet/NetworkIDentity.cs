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
    public class NetworkIdentity : ScriptComponent
    {
        private bool _Owner = false;
        public bool Owner {
            set {if(value == true && IsPlayer == false) TakeOwnership();  _Owner = value; }
            get { return _Owner; }
        }

        public int ID {
            get {
                return Manager ? Manager.NetScene.NetworkObjects.FirstOrDefault(pair => pair.Value == this).Key : -1; // One line master race.
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
            if (packetType == PacketType.OBJECT_DATA)
                DataWriter.Put(ID);

            DataWriter.Put(initalState);
            if (networkComponentsCache == null)
            {
                networkComponentsCache = gameObject.GetComponents<NetworkComponent>();
            }
            foreach (NetworkComponent comp in networkComponentsCache)
            {
                comp.OnWrite(DataWriter, initalState);
            }
            Manager.InternalManager.SendToAll(DataWriter, DeliveryMethod.Sequenced);
        }

        public void ReadData(NetPacketReader reader, bool initialState)
        {
            if(networkComponentsCache == null)
            {
                networkComponentsCache = gameObject.GetComponents<NetworkComponent>();
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
            if(Manager != null)
                if(!_Owner)
                    Manager.TakeOwnership(this);
        }
    }
}
