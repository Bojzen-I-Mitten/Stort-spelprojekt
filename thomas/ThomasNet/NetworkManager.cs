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

    public class Spawner
    {
        public int netID { get; set; }
        public int prefabID { get; set; }
        public Vector3 position { get; set; }
        public Quaternion rotation { get; set; }
        public bool isOwner { get; set; }

        public Spawner() { netID = -1; prefabID = -2; position = new Vector3(); rotation = new Quaternion(); isOwner = false; }
    }

    public enum PacketType
    {
        EVENT,
        DATA
    }

    public class NetworkManager : ScriptComponent
    {
        private Dictionary<int, NetworkID> networkIDObjects = new Dictionary<int, NetworkID>();
        private Dictionary<NetworkID, NetPeer> players = new Dictionary<NetworkID, NetPeer>();
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

            //SubscribeToEvent<TimeSyncEvent>(HandleTimeSyncEvent);
            SubscribeToEvent<ExamplePacket>(ExamplePacket.PrintPacket);
            SubscribeToEvent<Spawner>(SubscribeSpawnObject);

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
            if (isServer)
            {
                ThomasEngine.Debug.Log("A client has connected with the IP" + peer.EndPoint.ToString());
                List<GameObject> spawnObjects = new List<GameObject>();
                foreach (var player in players)
                {
                    SpawnObject(player.Key.prefabID, player.Value);
                }
                SpawnPlayerCharacter(peer);
            }
            else
            {
                ThomasEngine.Debug.Log("You are now connected with the server" + peer.EndPoint.ToString());
            }

        }

        private void Listener_NetworkReceiveEvent(NetPeer peer, NetPacketReader reader, DeliveryMethod deliveryMethod)
        {
            if (isClient)
                GetPing();
            PacketType type = (PacketType)reader.GetInt();
            switch (type)
            {
                case PacketType.DATA:
                    while (reader.AvailableBytes > 0)
                    {

                        validationID = reader.GetInt();
                        if (networkIDObjects.ContainsKey(validationID))
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
            netManager.UpdateTime = (1000 / TICK_RATE);
            serverTime += Time.ActualDeltaTime;
            netManager.PollEvents();

            //Write full world state of owned objects.
            WriteData(DeliveryMethod.Unreliable);
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

        public void OverwriteNetID(NetworkID netID, int new_id)
        {
            networkIDObjects.Remove(netID.ID);
            networkIDObjects.Add(new_id, netID);
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

        public void WriteData(NetDataWriter writer)
        {
            writer.Put((int)PacketType.DATA);
            foreach (NetworkID id in networkIDObjects.Values)
            {
                id.Write(writer);
            }
        }
        public void WriteData(DeliveryMethod method)
        {
            NetDataWriter writer = new NetDataWriter();
            writer.Put((int)PacketType.DATA);
            foreach (NetworkID id in networkIDObjects.Values)
            {
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

        internal void SubscribeSpawnObject(Spawner spawner, NetPeer peer)
        {
            if (spawner.prefabID >= 0 && spawner.prefabID < spawnablePrefabs.Count)
            {
                GameObject gObj = GameObject.Instantiate(spawnablePrefabs[spawner.prefabID], spawner.position, spawner.rotation);
                gObj.GetComponent<NetworkID>().Owner = spawner.isOwner;
                gObj.GetComponent<NetworkID>().ID = spawner.netID;
            }
            else if (spawner.prefabID == -1)
            {
                GameObject gObj = GameObject.Instantiate(playerPrefab, spawner.position, spawner.rotation);
                gObj.GetComponent<NetworkID>().Owner = spawner.isOwner;
                gObj.GetComponent<NetworkID>().ID = spawner.netID;
            }
            else
            {
                Debug.Log("Tried spawning object not in NetworkManager prefab list");
            }
        }

        public void SpawnObject(int index, Vector3 position = new Vector3(), Quaternion rotation = new Quaternion())
        {
            GameObject gObj;
            if (index >= 0 && index < spawnablePrefabs.Count)
            {
                gObj = GameObject.Instantiate(spawnablePrefabs[index], position, rotation);
            }
            else if(index == -1)
            {
                gObj = GameObject.Instantiate(playerPrefab, position, rotation);
                
            }
            else
            {
                Debug.Log("Tried to spawn object not in NetworkManager list of prefabs");
                return;
            }

            int ID = Register(gObj.GetComponent<NetworkID>());
            gObj.GetComponent<NetworkID>().ID = ID;

            Spawner spawner = new Spawner
            {
                netID = ID,
                prefabID = index,
                position = gObj.transform.position,
                rotation = gObj.transform.rotation,
                isOwner = false
            };

            SendEvent(spawner, DeliveryMethod.ReliableOrdered);
        }

        public void SpawnObject(int index, NetPeer sendTo, Vector3 position = new Vector3(), Quaternion rotation = new Quaternion())
        {
            GameObject gObj;
            if (index >= 0 && index < spawnablePrefabs.Count)
            {
                gObj = GameObject.Instantiate(spawnablePrefabs[index], position, rotation);
                
            }
            else if (index == -1)
            {
                gObj = GameObject.Instantiate(playerPrefab, position, rotation);
            }
            else
            {
                Debug.Log("Tried to spawn object not in NetworkManager list of prefabs");
                return;
            }

            int ID = Register(gObj.GetComponent<NetworkID>());
            gObj.GetComponent<NetworkID>().ID = ID;

            Spawner spawner = new Spawner
            {
                netID = ID,
                prefabID = index,
                position = gObj.transform.position,
                rotation = gObj.transform.rotation,
                isOwner = false
            };

            SendEventToPeer(spawner, DeliveryMethod.ReliableOrdered, sendTo);
        }

        public void SpawnPlayerCharacter(NetPeer connected)
        {
            GameObject gObj = GameObject.Instantiate(playerPrefab, new Vector3(), new Quaternion());
            //int ID = Register(gObj.GetComponent<NetworkID>());
            int ID = gObj.GetComponent<NetworkID>().ID;

            players.Add(gObj.GetComponent<NetworkID>(), connected);

            Spawner spawner = new Spawner
            {
                netID = ID,
                prefabID = -1,
                position = gObj.transform.position,
                rotation = gObj.transform.rotation,
                isOwner = false
            };

            SendEventToAllBut(spawner, DeliveryMethod.ReliableOrdered, connected); //tell old clients to spawn object
            spawner.isOwner = true;
            SendEventToPeer(spawner, DeliveryMethod.ReliableOrdered, connected); //tell new client to spawn object
        }

        public void GetPing()
        {

            netManager.GetPeersNonAlloc(netPeers, ConnectionState.Connected);

            for (int i = 0; i < netPeers.Count; i++)
            {

                ping = netPeers[i].Ping;

            }
        }
    }
}
