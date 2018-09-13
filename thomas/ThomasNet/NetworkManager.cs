using System;
using System.Collections.Generic;
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

    public class TimeSyncEvent
    {
        public float serverTime { get; set; }
        public TimeSyncEvent() { }
        public TimeSyncEvent(float time) { serverTime = time; }
    }

    public enum PacketType
    {
        EVENT,
        DATA
    }

    public class NetworkManager : ScriptComponent
    {
        Dictionary<int, NetworkID> networkIDObjects;
        int iD = -1;
        int validationID = -5;
        private NetPacketProcessor netPacketProcessor;

        private EventBasedNetListener listener;
        private NetManager netManager;
        private NetDataWriter writer;
        public string IP { get; set; } = "localhost";
        public int port { get; set; } = 9050;
        public bool isServer { get; set; } = false;
        
        public static NetworkManager instance;
        public ExamplePacket testPacket = new ExamplePacket();


        private float serverTime;

        public int TICK_RATE { get; set; } = 24;

        [HideInInspector]
        public bool isClient
        {
            get { return !isServer; }
        }

        public override void Awake()
        {
            networkIDObjects = new Dictionary<int, NetworkID>();
            netPacketProcessor = new NetPacketProcessor();
            listener = new EventBasedNetListener();
            netManager = new NetManager(listener);
            instance = this;
            writer = new NetDataWriter();


            //Here all events are defined.
            listener.ConnectionRequestEvent += Listener_ConnectionRequestEvent;
            listener.NetworkReceiveEvent += Listener_NetworkReceiveEvent;
            listener.PeerConnectedEvent += Listener_PeerConnectedEvent;
            listener.PeerDisconnectedEvent += Listener_PeerDisconnectedEvent;

            //SubscribeToEvent<TimeSyncEvent>(HandleTimeSyncEvent);
            SubscribeToEvent<ExamplePacket>(ExamplePacket.PrintPacket);
        }


        public override void Start()
        {
            if (isClient) //client
            {
                netManager.Start();
                netManager.Connect(IP /* host ip or name */, 9050 /* port */, "SomeConnectionKey" /* text key or NetDataWriter */);
            }
            else //server
            {
                InitServerNTP();
                netManager.Start(9050 /* port */);
               // SendEvent(testPacket, DeliveryMethod.ReliableOrdered);
            }

            
        }
        private void Listener_PeerDisconnectedEvent(NetPeer peer, DisconnectInfo disconnectInfo)
        {
            if (isServer)
                ThomasEngine.Debug.Log("A client has disconnected with the IP" + peer.EndPoint.ToString());
            else
                ThomasEngine.Debug.Log("The server you where connected to has disconnected with the IP" + peer.EndPoint.ToString());
        }
        private void Listener_PeerConnectedEvent(NetPeer peer)
        {
            if(isServer)
                ThomasEngine.Debug.Log("A client has connected with the IP" + peer.EndPoint.ToString());
            else
                ThomasEngine.Debug.Log("You are now connected with the server" + peer.EndPoint.ToString());
        }
        private void Listener_NetworkReceiveEvent(NetPeer peer, NetPacketReader reader, DeliveryMethod deliveryMethod)
        {
            
                PacketType type = (PacketType)reader.GetInt();
                switch (type)
                {
                    case PacketType.DATA:
                        while (reader.AvailableBytes > 0)
                        {

                            validationID = reader.GetInt();
                            if (validationID < networkIDObjects.Count)
                                networkIDObjects[validationID].Read(reader);
                            else
                                reader.Clear();
                        }
                        break;
                    case PacketType.EVENT:
                        netPacketProcessor.ReadAllPackets(reader);
                        break;
                    default:
                        break;
                
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
            netManager.UpdateTime = (1000/ TICK_RATE);
            serverTime += Time.ActualDeltaTime;
            netManager.PollEvents();

            if (isServer)
            {
                WriteData(DeliveryMethod.ReliableOrdered);
            }
                
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
            {

                writer.Put(id.ID);
                id.Write(writer);
            }
        }
        public void WriteData(DeliveryMethod method)
        {
            NetDataWriter writer = new NetDataWriter();
            writer.Put((int)PacketType.DATA);
            foreach (NetworkID id in networkIDObjects.Values)
            {
                writer.Put(id.ID);
                id.Write(writer);
            }
            netManager.SendToAll(writer, method);
        }

        private void InitServerNTP()
        {
            NtpRequest.Make("pool.ntp.org", 123, dateTime =>
            {
                if (dateTime.HasValue)
                {
                    serverTime = dateTime.Value.Millisecond * 0.001f;
                }
            });
        }
    }
}
