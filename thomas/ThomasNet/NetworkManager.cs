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
            SubscribeToEvent<Spawner>(SpawnerHandler);
            
            
            //Stäng av alla nätverksobjekt som finns i scenen.

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
                SpawnPlayerCharacter(null);
                //Starta nätverksobjekten som finns i scenen och registera dem.
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

                //Skicka över vilka Idn objekten ska ha i scenen
                //Send all other players to the new player
                SpawnExistingPlayers(peer);
                //Create and share new player
                SpawnPlayerCharacter(peer);
            }
            else
            {
                //Ta emot objekt i scenen eller nått :)
                ThomasEngine.Debug.Log("You are now connected with the server" + peer.EndPoint.ToString());
            }

        }

        private void Listener_NetworkReceiveEvent(NetPeer peer, NetPacketReader reader, DeliveryMethod deliveryMethod)
        {
            if (reader.EndOfData)
                return;
            if (isClient)
                GetPing();
            PacketType type = (PacketType)reader.GetInt();
            switch (type)
            {
                case PacketType.DATA:
                    while (reader.AvailableBytes > 0)
                    {
                        validationID = reader.GetInt();
                        if (networkIDObjects.ContainsKey(validationID) && networkIDObjects[validationID].enabled)
                            networkIDObjects[validationID].Read(reader);
                        else
                            reader.Clear();
                    }
                    break;
                case PacketType.EVENT:
                    Debug.Log("recived events!");
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

        private int Register(NetworkID netID)
        {
            iD++;
            networkIDObjects.Add(iD, netID);
            netID.ID = iD;
            return iD;
        }

        private int Register(NetworkID netID, int targetID)
        {
            iD = targetID + 1;
            networkIDObjects.Add(targetID, netID);
            netID.ID = targetID;
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

        internal void SpawnerHandler(Spawner spawner, NetPeer peer)
        {
            GameObject prefab;
            if (spawner.prefabID >= 0 && spawner.prefabID < spawnablePrefabs.Count)
            {
                prefab = spawnablePrefabs[spawner.prefabID];
            }
            else if (spawner.prefabID == -1)
            {
                prefab = playerPrefab;
            }
            else
            {
                Debug.Log("Tried spawning object not in NetworkManager prefab list");
                return;
            }
            GameObject gObj = InstantiateAndRegister(prefab, spawner.netID, spawner.position, spawner.rotation);
            gObj.Name += spawner.isOwner ? "(My player)" : "";
            gObj.GetComponent<NetworkID>().Owner = spawner.isOwner;
        }

        private void SpawnPlayerCharacter(NetPeer connected)
        {

            GameObject player = InstantiateAndRegister(playerPrefab);
            int ID = player.GetComponent<NetworkID>().ID;

            players.Add(player.GetComponent<NetworkID>(), connected);

            Spawner spawner = new Spawner
            {
                netID = ID,
                prefabID = -1,
                position = player.transform.position,
                rotation = player.transform.rotation,
                isOwner = false
            };
            if (connected != null) //Server is null
            {
                SendEventToAllBut(spawner, DeliveryMethod.ReliableOrdered, connected); //tell old clients to spawn object
                spawner.isOwner = true;
                SendEventToPeer(spawner, DeliveryMethod.ReliableOrdered, connected); //tell new client to spawn object
            }
            else
            {
                player.GetComponent<NetworkID>().Owner = true;
                player.Name += "(My player)";
            }
                

        }

        private void SpawnExistingPlayers(NetPeer newPlayer)
        {
            NetDataWriter writer = new NetDataWriter();
            writer.Put((int)PacketType.EVENT);
            foreach (var existingPlayer in players)
            {
                Spawner spawner = new Spawner
                {
                    netID = existingPlayer.Key.ID,
                    prefabID = -1,
                    position = existingPlayer.Key.transform.position,
                    rotation = existingPlayer.Key.transform.rotation,
                    isOwner = false
                };
                SendEvent<Spawner>(writer, spawner);
            }
            newPlayer.Send(writer, DeliveryMethod.ReliableOrdered);
        }

        private GameObject InstantiateAndRegister(GameObject prefab, int networkID = -1, Vector3 position = new Vector3(), Quaternion rotation = new Quaternion())
        {
            GameObject gObj = GameObject.Instantiate(playerPrefab, new Vector3(), new Quaternion());
            if(networkID >= 0)
                Register(gObj.GetComponent<NetworkID>(), networkID);
            else
                Register(gObj.GetComponent<NetworkID>());
            return gObj;
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
