
using System;
using System.Collections.Generic;
using System.ComponentModel;
using LiteNetLib;
using LiteNetLib.Utils;
using System.Linq;
namespace ThomasEngine.Network
{

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
        public string IP { get; set; } 
        public int Port { get; set; }

        public ConnectToPeerEvent() { IP = ""; Port = -1; ; }
    }

    public enum PacketType
    {
        EVENT,
        OBJECT_DATA,
        PLAYER_DATA,
    }

    public class NetworkManager : ScriptComponent
    {

        public static NetworkManager instance;
        
        
        //private Dictionary<NetPeer, List<GameObject>> ownedObjects = new Dictionary<NetPeer, List<GameObject>>();

        int iD = -1;
        int validationID = -5;

        private NetPacketProcessor netPacketProcessor;
        private EventBasedNetListener listener;
        private NetManager netManager;
        private bool conectee = false;
        private EventBasedNatPunchListener natPunchListener;

        public int LocalPort { get; set; } = 9050;

        public string TargetIP { get; set; } = "localhost";
        public int TargetPort { get; set; } = 9050;

        public bool UseLobby { get; set; } = false;


        public List<GameObject> spawnablePrefabs { get; set; } = new List<GameObject>();
        public GameObject playerPrefab { get; set; }
        
        private float serverTime;

        private NetPeer localPeer = null;

        public int TICK_RATE { get; set; } = 24;

        NetworkScene NetScene;

        [Browsable(false)]
        public NetManager InternalManager { get { return netManager; } }

        public NetworkManager()
        {
            instance = this;
        }
        public override void Awake()
        {
            NetScene = new NetworkScene();
            netPacketProcessor = new NetPacketProcessor();
            listener = new EventBasedNetListener();
            netManager = new NetManager(listener);

            natPunchListener = new EventBasedNatPunchListener();

            //Here all events are defined.
            natPunchListener.NatIntroductionSuccess += NatPunchListener_NatIntroductionSuccess;

            listener.ConnectionRequestEvent += Listener_ConnectionRequestEvent;
            listener.NetworkReceiveEvent += Listener_NetworkReceiveEvent;
            listener.PeerConnectedEvent += Listener_PeerConnectedEvent;
            listener.PeerDisconnectedEvent += Listener_PeerDisconnectedEvent;
            listener.NetworkErrorEvent += Listener_NetworkErrorEvent;
            //SubscribeToEvent<TimeSyncEvent>(HandleTimeSyncEvent);

            SubscribeToEvent<ConnectToPeerEvent>(ConnectToPeerEventHandler);

            //Stäng av alla nätverksobjekt som finns i scenen.
                                              

        }

        private void NatPunchListener_NatIntroductionSuccess(System.Net.IPEndPoint targetEndPoint, string token)
        {
            if (targetEndPoint.Address.ToString() == "::1")
                return;
            netManager.Connect(targetEndPoint, "SomeConnectionKey");
        }

        public override void Start()
        {

            InitServerNTP();
            if (UseLobby)
            {
                netManager.NatPunchEnabled = true;
                netManager.NatPunchModule.Init(natPunchListener);
            }
            
            netManager.Start(LocalPort);
            Debug.Log("NetManager started on port" + ":" + netManager.LocalPort);
            localPeer = new NetPeer(netManager, null);

        }

        private void Listener_PeerDisconnectedEvent(NetPeer peer, DisconnectInfo disconnectInfo)
        {
            switch (disconnectInfo.Reason)
            {
                case DisconnectReason.RemoteConnectionClose:
                case DisconnectReason.DisconnectPeerCalled:
                    NetScene.RemovePlayer(peer);
                    Debug.Log("The peer you where connected to has disconnected with the IP " + peer.EndPoint.ToString());
                    break;
                case DisconnectReason.Timeout:
                    NetScene.RemovePlayer(peer);
                    Debug.Log("Connection to peer " + peer.EndPoint.ToString() + " timed out");
                    break;
                case DisconnectReason.ConnectionRejected:
                    Debug.Log("Connection to peer " + peer.EndPoint.ToString() + " rejected");
                    break;
                case DisconnectReason.ConnectionFailed:
                    Debug.Log("Connection to peer " + peer.EndPoint.ToString() + " failed");
                    break;
                case DisconnectReason.SocketReceiveError:
                    Debug.Log("Connection to peer " + peer.EndPoint.ToString() + " failed, peer socket closed"); //Could be the other way around
                    break;
                case DisconnectReason.SocketSendError:
                    Debug.Log("Connection to peer " + peer.EndPoint.ToString() + " failed, lcoal socket closed"); //^^^^^^^^^^^^^^^^^^^^^^^^^^^^
                    break;
            }
        }

        private void Listener_PeerConnectedEvent(NetPeer _peer)
        {
            ThomasEngine.Debug.Log("A peer has connected with the IP" + _peer.EndPoint.ToString());
            
            foreach (NetPeer peer in netManager.GetPeers(ConnectionState.Connected))
            {
                if (peer == _peer)
                    continue;
                ConnectToPeerEvent connectEvent = new ConnectToPeerEvent
                {
                    IP = peer.EndPoint.Address.ToString(),
                    Port = peer.EndPoint.Port
                };


                NetDataWriter writer = new NetDataWriter();
                writer.Put((int)PacketType.EVENT);
                WriteEvent(writer, connectEvent);
                _peer.Send(writer, DeliveryMethod.ReliableOrdered);
            }

            if(NetScene.players.Count == 0) // We are new player.
            {
                NetScene.SpawnPlayer(playerPrefab, localPeer, true);
                NetScene.SpawnPlayer(playerPrefab, _peer, false);
            }
            else //Someone is joining us.
            {
                NetScene.SpawnPlayer(playerPrefab, _peer, false);
            }
        }

        private void Listener_NetworkErrorEvent(System.Net.IPEndPoint endPoint, System.Net.Sockets.SocketError socketError)
        {
            NetUtils.DebugWriteError("Network error has occured on: {0} error: {1}", endPoint.Address, socketError.ToString());
        }


        private void Listener_NetworkReceiveEvent(NetPeer peer, NetPacketReader reader, DeliveryMethod deliveryMethod)
        {
            if (reader.EndOfData)
                return;
            PacketType type = (PacketType)reader.GetInt();
            switch (type)
            {
                case PacketType.PLAYER_DATA:
                    {
                        NetScene.ReadPlayerData(peer, reader);
                    }
                    break;
                case PacketType.OBJECT_DATA:
                    {
                        //int networkID = reader.GetInt();
                        //if (networkIDObjects.ContainsKey(networkID) && networkIDObjects[networkID].enabled)
                        //    networkIDObjects[validationID].OnUpdateVars(reader, false);
                    }
                    break;
                case PacketType.EVENT:
                    Debug.Log("recived events!");
                    netPacketProcessor.ReadAllPackets(reader, peer);
                    break;
                default:
                    break;
            }
            reader.Recycle();
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
            if(netManager.NatPunchEnabled)
                netManager.NatPunchModule.PollEvents();
            netManager.PollEvents(); 

            if (Input.GetKeyDown(Input.Keys.J)) //JOIN
            {
                conectee = true;
                if(UseLobby)
                    netManager.NatPunchModule.SendNatIntroduceRequest(NetUtils.MakeEndPoint(TargetIP, TargetPort), "Domarn");
                else
                {
                    netManager.Connect(TargetIP, TargetPort, "SomeConnectionKey");
                }
                    
            }

            if(Input.GetKeyDown(Input.Keys.H)) //HOST
            {

                NetScene.SpawnPlayer(playerPrefab, localPeer, true);

            }

            Diagnostics();
        }

        public override void OnDestroy()
        {
            if (netManager != null)
            {
                netManager.DisconnectAll();
                netManager.Stop();
            }

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




        public void ConnectToPeerEventHandler(ConnectToPeerEvent connectEvent, NetPeer peer)
        {
                        
            netManager.Connect(connectEvent.IP, connectEvent.Port, "SomeConnectionKey");
        }

        //private void RemovePlayerCharacter(NetPeer disconnectedPeer)
        //{
        //    foreach(GameObject ownedObject in ownedObjects[disconnectedPeer])
        //    {
        //        ownedObject.Destroy();
        //    }
        //    ownedObjects.Remove(disconnectedPeer);
        //}

        public void CheckPacketLoss()
        {
            Debug.Log("A error has occured here are the amount of packetloss " + netManager.Statistics.PacketLoss + "% lost " + netManager.Statistics.PacketLossPercent);
            Debug.Log("A total of " + netManager.Statistics.PacketsSent + "was sent and " + netManager.Statistics.PacketsReceived + "was recieved");
        }

        public void Diagnostics()
        {
            GUI.ImguiStringUpdate("Packetsloss = " + netManager.Statistics.PacketLossPercent, new Vector2(0, 10));
            GUI.ImguiStringUpdate("Total package sent = " + netManager.Statistics.PacketsSent, new Vector2(0, 20));
            GUI.ImguiStringUpdate("Total package recieved = " + netManager.Statistics.PacketsReceived, new Vector2(0, 30));
            PingToAllClients();
        }

        public void PingToAllClients()
        {
            int i = 0;
            foreach(var player in netManager.GetPeers(ConnectionState.Connected))
            {
                GUI.ImguiStringUpdate("Ping to client " + player.EndPoint.ToString() + "  " + player.Ping, new Vector2(0, 40 + 10 * i));
                i++;
            }
        }
    }
}
