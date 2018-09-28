using System;
using System.Collections.Generic;
using System.ComponentModel;
using LiteNetLib;
using LiteNetLib.Utils;


namespace ThomasEngine.Network
{
    public enum PacketType
    {
        EVENT,
        DATA
    }

    public class NetworkManager : ScriptComponent
    {
        internal Dictionary<int, NetworkID> networkIDObjects;
        private Dictionary<NetPeer, GameObject> players;
        int iD = -1;
        int validationID = -5;
        internal NetPacketProcessor netPacketProcessor;

        internal EventBasedNetListener listener { get; set; }
        internal NetManager netManager;
        public string IP { get; set; } = "localhost";
        public int port { get; set; } = 9050;
        public bool isServer { get; set; } = false;
        public List<GameObject> spawnablePrefabs { get; set; }
        public GameObject playerPrefab { get; set; }
        public static int ping = 2;
        public static NetworkManager instance;
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
            networkIDObjects = new Dictionary<int, NetworkID>();
            players = new Dictionary<NetPeer, GameObject>();
            spawnablePrefabs = new List<GameObject>();
            NetworkEvents events = new NetworkEvents();

            //Here all events are defined.

            //Stäng av alla nätverksobjekt som finns i scenen.

        }

        public override void Start()
        {
            InitServerNTP();
            netManager.Start(port);
            serverPeer = new NetPeer(netManager, null);
            SpawnPlayerCharacter(serverPeer);
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

        internal void InitServerNTP()
        {
            NtpRequest.Make("pool.ntp.org", 123, dateTime =>
            {
                if (dateTime.HasValue)
                {
                    serverTime = dateTime.Value.Millisecond * 0.001f;
                }
            });
        }

        internal void SpawnPlayerCharacter(NetPeer connected)
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

        internal void RemovePlayerCharacter(NetPeer disconnectedPeer)
        {
            players[disconnectedPeer].Destroy();
            players.Remove(disconnectedPeer);
        }

        internal GameObject InstantiateAndRegister(GameObject prefab, int networkID = -1, Vector3 position = new Vector3(), Quaternion rotation = new Quaternion())
        {
            GameObject gObj = GameObject.Instantiate(playerPrefab, new Vector3(), new Quaternion());
            if (networkID >= 0)
                Register(gObj.GetComponent<NetworkID>(), networkID);
            else
                Register(gObj.GetComponent<NetworkID>());
            return gObj;
        }

        internal void DeleteAndUnregister(int networkID)
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
    }
}
