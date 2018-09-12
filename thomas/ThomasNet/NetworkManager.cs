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
        public string test1 { get; set; } = "asd";
        public int test2 { get; set; } = 123;
        public Vector3 test3 = new Vector3(0.0F, 0.0F, 0.0F);

        static public void PrintPacket(ExamplePacket packet, NetPeer peer)
        {
            ThomasEngine.Debug.Log(System.String.Format("Received: \n {0}\n {1}\n {2}\t{3}\t{4}", packet.test1, packet.test2, packet.test3.x, packet.test3.y, packet.test3.z));
        }
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
        public bool Server { get; set; } = false;
        //public int port { get; set; } = 9050;
        public static NetworkManager instance;
        public ExamplePacket testPacket = new ExamplePacket();
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

                 SendDataOverEvent(testPacket, DeliveryMethod.ReliableOrdered);
            }

            SubscribeDataType<ExamplePacket>(ExamplePacket.PrintPacket);

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
            listener.NetworkReceiveEvent += (fromPeer, dataReader, deliveryMethod) => netPacketProcessor.ReadAllPackets(dataReader, fromPeer);
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
