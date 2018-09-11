using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using LiteNetLib;
using LiteNetLib.Utils;



namespace ThomasEngine.Network
{
    public class Server
    {
        public int port{get; set;}
        private NetManager server;
        private EventBasedNetListener listener;
        public Server()
        {
            port = 9050;
             listener = new EventBasedNetListener();
             server = new NetManager(listener); 


        }
        public void Start()
        {
            server.Start(port /* port */);


            listener.ConnectionRequestEvent += request =>
            {
                if (server.PeersCount < 10 /* max connections */)
                    request.AcceptIfKey("SomeConnectionKey");
                else
                    request.Reject();
            };



        }
        public void Update()
        {
            server.PollEvents();
        }

        public void Stop()
        {
            server.Stop();
        }

        public void SendData(String StringData)
        {
            listener.PeerConnectedEvent += peer =>
            {
                ThomasEngine.Debug.Log("We got connection: {0}" + peer.EndPoint);// Show peer ip
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
    }

}
