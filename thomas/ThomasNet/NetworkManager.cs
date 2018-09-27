using System;
using System.Collections.Generic;
using System.ComponentModel;
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

    public class SpawnPrefabEvent
    {
        public int netID { get; set; }
        public int prefabID { get; set; }
        public Vector3 position { get; set; }
        public Quaternion rotation { get; set; }
        public bool isOwner { get; set; }

        public SpawnPrefabEvent() { netID = -1; prefabID = -2; position = new Vector3(); rotation = new Quaternion(); isOwner = false; }
    }

    public class DeletePrefabEvent
    {
        public int netID { get; set; }

        public DeletePrefabEvent() { netID = -1; }
    }

    public class ConnectToPeerEvent
    {
        public string IP { get; set; } //also holds port
        public int netID { get; set; }

        public ConnectToPeerEvent() { IP = ""; netID = -1; }
    }

    public class TransferOwnerEvent
    {
        public int netID { get; set; }

        public TransferOwnerEvent() { netID = -1; }
    }

    public enum PacketType
    {
        EVENT,
        DATA
    }

    public class NetworkManager : ScriptComponent
    {
        private Dictionary<int, NetworkID> networkIDObjects = new Dictionary<int, NetworkID>();
        public Dictionary<NetPeer, GameObject> players = new Dictionary<NetPeer, GameObject>();
        int iD = -1;
        int validationID = -5;
        private NetPacketProcessor netPacketProcessor;

        private EventBasedNetListener listener;
        private NetManager netManager;
        public string IP { get; set; } = "localhost";
        public int port { get; set; } = 9050;
        public bool isServer { get; set; } = false;
        public List<GameObject> spawnablePrefabs { get; set; } = new List<GameObject>();
        public GameObject playerPrefab { get; set; }
        public static int ping = 2;
        public static NetworkManager instance;
        public ExamplePacket testPacket = new ExamplePacket();
        public List<NetPeer> netPeers;

        private float serverTime;

        private NetPeer serverPeer = null;

        public int TICK_RATE { get; set; } = 24;

        [Browsable(false)]
        public bool isClient
        {
            get { return !isServer; }

        }

        public NetworkManager()
        {
            instance = this;
        }
        public override void Awake()
        {
            netPacketProcessor = new NetPacketProcessor();
            listener = new EventBasedNetListener();
            netManager = new NetManager(listener);
            netPeers = new List<NetPeer>();

            //Here all events are defined.
            listener.ConnectionRequestEvent += Listener_ConnectionRequestEvent;
            listener.NetworkReceiveEvent += Listener_NetworkReceiveEvent;
            listener.PeerConnectedEvent += Listener_PeerConnectedEvent;
            listener.PeerDisconnectedEvent += Listener_PeerDisconnectedEvent;
            listener.NetworkErrorEvent += Listener_NetworkErrorEvent;
            //SubscribeToEvent<TimeSyncEvent>(HandleTimeSyncEvent);
            SubscribeToEvent<ExamplePacket>(ExamplePacket.PrintPacket);
            SubscribeToEvent<SpawnPrefabEvent>(SpawnPrefabEventHandler);
            SubscribeToEvent<DeletePrefabEvent>(DeletePrefabEventHandler);
            SubscribeToEvent<ConnectToPeerEvent>(ConnectToPeerEventHandler);
            SubscribeToEvent<TransferOwnerEvent>(TransferOwnereventHandler);

            //Stäng av alla nätverksobjekt som finns i scenen.

        }

        public override void Start()
        {
            InitServerNTP();
            netManager.Start(port);
            serverPeer = new NetPeer(netManager, null);
            SpawnPlayerCharacter(serverPeer);
        }

        private void Listener_PeerDisconnectedEvent(NetPeer peer, DisconnectInfo disconnectInfo)
        {
            RemovePlayerCharacter(peer);
            ThomasEngine.Debug.Log("The peer you where connected to has disconnected with the IP" + peer.EndPoint.ToString());
        }

        private void Listener_PeerConnectedEvent(NetPeer _peer)
        {
            ThomasEngine.Debug.Log("A peer has connected with the IP" + _peer.EndPoint.ToString());

            foreach (NetPeer peer in netPeers)
            {
                ConnectToPeerEvent connectEvent = new ConnectToPeerEvent
                {
                    IP = peer.EndPoint.ToString()
                };
                NetDataWriter writer = new NetDataWriter();
                writer.Put((int)PacketType.EVENT);
                WriteEvent(writer, connectEvent);
                _peer.Send(writer, DeliveryMethod.ReliableOrdered);
            }

            netPeers.Add(_peer);
            SpawnPlayerCharacter(_peer);
        }

        private void Listener_NetworkErrorEvent(System.Net.IPEndPoint endPoint, System.Net.Sockets.SocketError socketError)
        {
            if (isServer)
            {
                Debug.Log("A network error has occured from client " + endPoint);
            }
            else
            {
                Debug.Log("A network error has occured from client " + endPoint);
            }

        }


        private void Listener_NetworkReceiveEvent(NetPeer peer, NetPacketReader reader, DeliveryMethod deliveryMethod)
        {
            if (reader.EndOfData)
                return;
            PacketType type = (PacketType)reader.GetInt();
            switch (type)
            {
                case PacketType.DATA:
                    while (reader.AvailableBytes > 0)
                    {
                        validationID = reader.GetInt();
                        if (networkIDObjects.ContainsKey(validationID) && networkIDObjects[validationID].enabled) { }
                            //networkIDObjects[validationID].Read(reader);
                        else
                            reader.Clear();
                    }
                    break;
                case PacketType.EVENT:
                    Debug.Log("recived events!");
                    netPacketProcessor.ReadAllPackets(reader, peer);

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
            netManager.UpdateTime = (1000 / TICK_RATE);
            serverTime += Time.ActualDeltaTime;
            netManager.PollEvents();

            //Write full world state of owned objects.
            //WriteData(DeliveryMethod.Unreliable);


            if (isClient && netManager.GetFirstPeer() != null)
                GUI.ImguiStringUpdate(netManager.GetFirstPeer().Ping.ToString(), new Vector2(0, 0));
            if (Input.GetKey(Input.Keys.P))
                Diagnostics();
            if (Input.GetKey(Input.Keys.K) && !isServer)
            {
                netManager.Connect(IP, port, "SomeConnectionKey");
            }
        }

        public override void OnDestroy()
        {
            if (netManager != null)
            {
                netManager.DisconnectAll();
                netManager.Stop();
            }

        }

        private void Unregister(int networkID)
        {
            networkIDObjects.Remove(networkID);
        }

        private int Register(NetworkID netID)
        {
            iD++;
            networkIDObjects.Add(iD, netID);
            netID.ID = iD;
            return iD;
        }

        private int Register(NetworkID netID, int targetID)
        {
            if (!networkIDObjects.ContainsKey(targetID))
            {
                iD = targetID + 1;
                networkIDObjects.Add(targetID, netID);
                netID.ID = targetID;
                
            }
            else
            {
                Debug.Log("Tried registering already existing ID");
            }
            return iD;
        }

        public void OverwriteNetID(NetworkID netID, int new_id)
        {
            networkIDObjects.Remove(netID.ID);
            networkIDObjects.Add(new_id, netID);
        }

        public void SubscribeToEvent<T>(Action<T, NetPeer> onReceive) where T : class, new()
        {
            netPacketProcessor.SubscribeReusable<T, NetPeer>(onReceive);
        }

        public void WriteEvent<T>(NetDataWriter writer, T data) where T : class, new()
        {
            netPacketProcessor.Write<T>(writer, data);
        }

        public void SendEvent<T>(T data, DeliveryMethod method) where T : class, new()
        {
            NetDataWriter writer = new NetDataWriter();
            writer.Put((int)PacketType.EVENT);

            netPacketProcessor.Write<T>(writer, data);
            netManager.SendToAll(writer, method);
        }

        public void SendEventToAllBut<T>(T data, DeliveryMethod method, NetPeer excluded) where T : class, new()
        {
            NetDataWriter writer = new NetDataWriter();
            writer.Put((int)PacketType.EVENT);

            netPacketProcessor.Write<T>(writer, data);
            netManager.SendToAll(writer, method, excluded);
        }

        public void SendEventToPeer<T>(T data, DeliveryMethod method, NetPeer sendTo) where T : class, new()
        {
            NetDataWriter writer = new NetDataWriter();
            writer.Put((int)PacketType.EVENT);

            netPacketProcessor.Write<T>(writer, data);
            sendTo.Send(writer, method);
        }

        //public void WriteData(NetDataWriter writer)
        //{
        //    writer.Put((int)PacketType.DATA);
        //    foreach (NetworkID id in networkIDObjects.Values)
        //    {
        //        id.Write(writer);
        //    }
        //}
        //public void WriteData(DeliveryMethod method)
        //{
        //    NetDataWriter writer = new NetDataWriter();
        //    writer.Put((int)PacketType.DATA);
        //    foreach (NetworkID id in networkIDObjects.Values)
        //    {
        //        id.Write(writer);
        //    }
        //    netManager.SendToAll(writer, method);
        //}

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

        internal void SpawnPrefabEventHandler(SpawnPrefabEvent spawnEvent, NetPeer peer)
        {
            GameObject prefab;
            if (spawnEvent.prefabID >= 0 && spawnEvent.prefabID < spawnablePrefabs.Count)
            {
                prefab = spawnablePrefabs[spawnEvent.prefabID];
            }
            else if (spawnEvent.prefabID == -1)
            {
                prefab = playerPrefab;
            }
            else
            {
                Debug.Log("Tried spawning object not in NetworkManager prefab list");
                return;
            }

            GameObject gObj = InstantiateAndRegister(prefab, spawnEvent.netID, spawnEvent.position, spawnEvent.rotation);
            gObj.Name += spawnEvent.isOwner ? "(My player)" : "";
            gObj.GetComponent<NetworkID>().Owner = spawnEvent.isOwner;
        }

        private void SpawnPlayerCharacter(NetPeer connected)
        {
            GameObject player = InstantiateAndRegister(playerPrefab);

            players.Add(connected, player);

            if (connected == serverPeer)
            {
                player.GetComponent<NetworkID>().Owner = true;
                player.Name += "(My player)";
            }
            else
            {
                player.GetComponent<NetworkID>().Owner = false;
                player.Name += "(" + connected.EndPoint.ToString() + ")";
            }
        }

        private void DeletePrefabEventHandler(DeletePrefabEvent deleteEvent, NetPeer peer)
        {
            DeleteAndUnregister(deleteEvent.netID);
        }

        public void ConnectToPeerEventHandler(ConnectToPeerEvent connectEvent, NetPeer peer)
        {
            string address = connectEvent.IP.Substring(0, connectEvent.IP.IndexOf(":"));
            string sPort = connectEvent.IP.Substring(connectEvent.IP.IndexOf(":") + 1);
            int iPort = int.Parse(sPort);

            netManager.Connect(address, iPort, "SomeConnectionKey");
        }

        private void RemovePlayerCharacter(NetPeer disconnectedPeer)
        {
            players[disconnectedPeer].Destroy();
            players.Remove(disconnectedPeer);
        }

        private GameObject InstantiateAndRegister(GameObject prefab, int networkID = -1, Vector3 position = new Vector3(), Quaternion rotation = new Quaternion())
        {
            GameObject gObj = GameObject.Instantiate(playerPrefab, new Vector3(), new Quaternion());
            if (networkID >= 0)
                Register(gObj.GetComponent<NetworkID>(), networkID);
            else
                Register(gObj.GetComponent<NetworkID>());
            return gObj;
        }

        private void DeleteAndUnregister(int networkID)
        {
            networkIDObjects[networkID].gameObject.Destroy();
            Unregister(networkID);
        }

        public void Checkpacketloss()
        {
            Debug.Log("A error has occured here are the amount of packetloss " + netManager.Statistics.PacketLoss + "% lost " + netManager.Statistics.PacketLossPercent);
            Debug.Log("A total of " + netManager.Statistics.PacketsSent + "was sent and " + netManager.Statistics.PacketsReceived + "was recieved");
        }

        public void Diagnostics()
        {
            GUI.ImguiStringUpdate("Packetsloss = " + netManager.Statistics.PacketLossPercent, new Vector2(0, 10));
            GUI.ImguiStringUpdate("Total package sent = " + netManager.Statistics.PacketsSent, new Vector2(0, 20));
            GUI.ImguiStringUpdate("Total package recieved = " + netManager.Statistics.PacketsReceived, new Vector2(0, 30));
            if (!isClient)
                PingToAllClients();
        }

        public void PingToAllClients()
        {
            netManager.GetPeersNonAlloc(netPeers, ConnectionState.Connected);
            for (int i = 0; i < netPeers.Count; i++)
            {
                GUI.ImguiStringUpdate("Ping to client " + netPeers[i].EndPoint.ToString() + "  " + netPeers[i].Ping, new Vector2(0, 40 + 10 * i));
            }
        }

        public void TransferOwnereventHandler(TransferOwnerEvent transEvent, NetPeer peer)
        {
            List<NetworkID> netObjectIDs = GetObjectsOfType<NetworkID>();
            foreach (var netObjectID in netObjectIDs)
            {
                if (netObjectID.ID == transEvent.netID)
                {
                    netObjectID.Owner = true;
                    netObjectID.gameObject.transform.parent = players[serverPeer].transform;
                    break;
                }
            }
        }
    }
}
