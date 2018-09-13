using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using LiteNetLib;
using LiteNetLib.Utils;


namespace ThomasEngine.Network
{
    public class ExamplePacket
    {
        public int ID { get; set; } = 0;
        public Vector3 transformPos = new Vector3(0.0F, 0.0F, 0.0F);

        static public void PrintPacket(ExamplePacket packet, NetPeer peer)
        {
            
            ThomasEngine.Debug.Log(System.String.Format("Received: \n {0}\n {1}\n {2}\t{3}\t", packet.ID, packet.transformPos.x, packet.transformPos.y, packet.transformPos.z));
        }
    }

    public enum PacketType
    {
        EVENT,
        DATA
    }

    public class NetworkManager : ScriptComponent
    {
        Dictionary<int, NetworkID> networkIDObjects = new Dictionary<int, NetworkID>();
        int iD = -1;

        private readonly NetPacketProcessor netPacketProcessor = new NetPacketProcessor();

        private EventBasedNetListener listener;
        private NetManager netManager;
        private NetDataWriter writer;
        public string IP { get; set; } = "localhost";
        public bool isServer { get; set; } = false;
        //public int port { get; set; } = 9050;
        public static NetworkManager instance;
        public ExamplePacket testPacket = new ExamplePacket();


        public bool isClient
        {
            get { return !isServer; }
        }

        public override void Start()
        {
            listener = new EventBasedNetListener();
            netManager = new NetManager(listener);
            instance = this;
            writer = new NetDataWriter();


            //Here all events are defined.
            listener.ConnectionRequestEvent += Listener_ConnectionRequestEvent;
            listener.NetworkReceiveEvent += Listener_NetworkReceiveEvent;
            listener.PeerConnectedEvent += Listener_PeerConnectedEvent;
            listener.PeerDisconnectedEvent += Listener_PeerDisconnectedEvent;
            if (isClient) //client
            {
                netManager.Start();
                netManager.Connect(IP /* host ip or name */, 9050 /* port */, "SomeConnectionKey" /* text key or NetDataWriter */);
            }
            else //server
            {
                netManager.Start(9050 /* port */);

               // SendEvent(testPacket, DeliveryMethod.ReliableOrdered);
            }

            SubscribeToEvent<ExamplePacket>(ExamplePacket.PrintPacket);
        }
        private void Listener_PeerDisconnectedEvent(NetPeer peer, DisconnectInfo disconnectInfo)
        {
            ThomasEngine.Debug.Log("A client has disconnected with the IP" + peer.EndPoint.ToString());
        }
        private void Listener_PeerConnectedEvent(NetPeer peer)
        {
            ThomasEngine.Debug.Log("A client has connected with the IP" + peer.EndPoint.ToString());
        }
        private void Listener_NetworkReceiveEvent(NetPeer peer, NetPacketReader reader, DeliveryMethod deliveryMethod)
        {
            while(reader.AvailableBytes > 0)
            {
                PacketType type = (PacketType)reader.GetInt();
                switch (type)
                {
                    case PacketType.DATA:
                        {
                            foreach (NetworkID id in networkIDObjects.Values)
                                id.Read(reader);
                            
                        }
                        break;
                    case PacketType.EVENT:
                        netPacketProcessor.ReadPacket(reader);
                        break;
                    default:
                        break;
                }
            }
        }
        private void Listener_ConnectionRequestEvent(ConnectionRequest request)
        {
            if (netManager.PeersCount < 10 /* max connections */)
                request.AcceptIfKey("SomeConnectionKey");
            else
                request.Reject();
        }
        public override void Update()
        {
            netManager.PollEvents();
            if (isServer)
                WriteData(DeliveryMethod.ReliableOrdered);
        }
        public override void Destroy()
        {
            if (netManager != null)
                netManager.Stop();

            base.Destroy();
        }
        public int Register(NetworkID netID)
        {
            iD++;
            networkIDObjects.Add(iD, netID);
            return iD;
        }
        public void SubscribeToEvent<T>(Action<T, NetPeer> onReceive) where T : class, new()
        {
            netPacketProcessor.SubscribeReusable<T, NetPeer>(onReceive);
        }
        public void SendEvent<T>(NetDataWriter writer, T data) where T : class, new()
        {
            writer.Put((int)PacketType.EVENT);
            netPacketProcessor.Write<T>(writer, data);
        }
        public void SendEvent<T>(T data, DeliveryMethod method) where T : class, new()
        {
            NetDataWriter writer = new NetDataWriter();
            writer.Put((int)PacketType.EVENT);

            netPacketProcessor.Write<T>(writer, data);
            netManager.SendToAll(writer, method);
        }
        public void WriteData(NetDataWriter writer)
        {
            writer.Put((int)PacketType.DATA);
            foreach (NetworkID id in networkIDObjects.Values)
                id.Write(writer);
        }
        public void WriteData(DeliveryMethod method)
        {
            NetDataWriter writer = new NetDataWriter();

            writer.Put((int)PacketType.DATA);
            foreach (NetworkID id in networkIDObjects.Values)
                id.Write(writer);

            netManager.SendToAll(writer, method);
        }
    }
}
