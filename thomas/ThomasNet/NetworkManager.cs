﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using LiteNetLib;
using LiteNetLib.Utils;


namespace ThomasEngine.Network
{
    public class NetworkManager : ScriptComponent
    {
        Dictionary<int, NetworkID> networkIDObjects = new Dictionary<int, NetworkID>();
        int iD = -1;
   
      
        private EventBasedNetListener listener;
        private NetManager netManager;
        private NetDataWriter writer;
        public string IP { get; set; } = "localhost"; 
        public bool Server { get; set; } = false;
        //       public int port { get; set; } = 9050;
        public static NetworkManager instance;
        public override void Start()
        {
            listener = new EventBasedNetListener();
            netManager = new NetManager(listener);
            instance = this;
            writer = new NetDataWriter();
            if (!Server) //client
            {
                netManager.Start();
                netManager.Connect(IP /* host ip or name */, 9050 /* port */, "SomeConnectionKey" /* text key or NetDataWriter */);
            }
            else //server
            {
                netManager.Start(9050 /* port */);


                listener.ConnectionRequestEvent += request =>
                {
                    if (netManager.PeersCount < 10 /* max connections */)
                        request.AcceptIfKey("SomeConnectionKey");
                    else
                        request.Reject();
                };

                 SendDataOverEvent("You are Connected To the server", DeliveryMethod.ReliableOrdered);

              //  SendDataOverEvent(0.0f, 2.2f, 0.0f, DeliveryMethod.ReliableOrdered);
              
            }
            ExecuteEvent();

        }
        public override void Update()
        {
            netManager.PollEvents();
        }
        public override void Destroy()
        {
            netManager.Stop();
            base.Destroy();
        }

        public int Register(NetworkID netID)
        {
            iD++;
            networkIDObjects.Add(iD, netID);
            return iD;
        }

        public void ExecuteEvent()
        {
            listener.NetworkReceiveEvent += (fromPeer, dataReader, deliveryMethod) =>
            {
                //   ThomasEngine.Debug.Log(dataReader.GetString());
                // Console.WriteLine("We got: {0}" , dataReader.GetString(100 /* max length of string */));  dataReader.GetString(100 /* max length of string */)+
                foreach (NetworkID id in networkIDObjects.Values)
                    id.Read(dataReader);

                dataReader.Recycle();
            };
        }

        public void SendDataOverEvent<T>(T Data, DeliveryMethod Order) where T : class, new()
        {
            listener.PeerConnectedEvent += peer => netPacketProcessor.Send<T>(peer, Data, Order);
        }


        public void SubscribeDataType<T>(Action<T, NetPeer> onReceive) where T : class, new()
        {
            netPacketProcessor.SubscribeReusable<T, NetPeer>(onReceive);
        }

        
    }
}
