using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using LiteNetLib;
using LiteNetLib.Utils;


namespace ThomasEngine.Network
{
    public class Client
    { 
        public EventBasedNetListener listener;
        public NetManager client;
        public int port = 9050;
        public String IP = "localhost";
        public Client()
        { 
            listener = new EventBasedNetListener();
            client = new NetManager(listener);
        }
        public void Update()
        {
            client.PollEvents();
        }
        public void Start()
        {
            client.Start();
            client.Connect(IP /* host ip or name */, port /* port */, "SomeConnectionKey" /* text key or NetDataWriter */);
         
        }
        public void Stop()
        {
            client.Stop();
        }
        public void ExecuteEvent()
        {
            listener.NetworkReceiveEvent += (fromPeer, dataReader, deliveryMethod) =>
            {
                ThomasEngine.Debug.Log("ClientSide: " + dataReader.GetString(100 /* max length of string */));
                // Console.WriteLine("We got: {0}" , dataReader.GetString(100 /* max length of string */));
                dataReader.Recycle();
            };
        }
        public void SendData(String StringData)
        {
            listener.PeerConnectedEvent += peer =>
            {
            //   ThomasEngine.Debug.Log("Client " + peer.EndPoint);// Show peer ip
                NetDataWriter writer = new NetDataWriter();                 // Create writer class
                writer.Put(StringData);                                // Put some string
                peer.Send(writer, DeliveryMethod.ReliableOrdered);             // Send with reliability
            };
        }
        
        public void SetPort(int newPort)
        {
            port = newPort;
        }
        public int GetPort()
        {
            return port;
        }
        public void SetIp(String newIP)
        {
            IP = newIP;
        }
        public string GetIp()
        {
            return IP;
        }



    }
}
