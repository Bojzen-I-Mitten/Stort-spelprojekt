
using System.Collections.Generic;
using System.ComponentModel;
using LiteNetLib;
using LiteNetLib.Utils;
using System.Linq;
using System;

namespace ThomasEngine.Network
{
    public enum PacketType
    {
        EVENT,
        OBJECT_DATA,
        PLAYER_DATA,
        RPC
    }

    public class NetworkManager : ScriptComponent
    {

        public static NetworkManager instance;
        //private Dictionary<NetPeer, List<GameObject>> ownedObjects = new Dictionary<NetPeer, List<GameObject>>();

        internal NetPacketProcessor NetPacketProcessor;

        private EventBasedNetListener Listener;
        private NetManager NetManager;
        private EventBasedNatPunchListener NatPunchListener;
        protected NetworkEvents Events;

        public int LocalPort { get; set; } = 9050;

        public string TargetIP { get; set; } = "localhost";
        public int TargetPort { get; set; } = 9050;

        public bool UseLobby { get; set; } = false;


        public List<GameObject> SpawnablePrefabs { get; set; } = new List<GameObject>();
        public GameObject PlayerPrefab { get; set; }

        public NetPeer LocalPeer = null;

        public NetPeer ResponsiblePeer = null;

        public int TICK_RATE { get; set; } = 24;


        public long ServerStartTime;
        [Browsable(false)]
        [Newtonsoft.Json.JsonIgnore]
        public float TimeSinceServerStarted
        {
            get
            {
                System.TimeSpan elapsedTimespan = new System.TimeSpan(System.DateTime.UtcNow.Ticks - ServerStartTime);
                return (float)elapsedTimespan.TotalSeconds;
            }
        }

        internal NetworkScene NetScene;

        [Browsable(false)]
        public NetManager InternalManager { get { return NetManager; } }

        [Browsable(false)]
        public NetworkScene Scene { get { return NetScene; } }


        public NetworkManager() : base()
        {
            instance = this;
        }
            
        public override void Awake()
        {
            NetScene = new NetworkScene();
            NetPacketProcessor = new NetPacketProcessor();
            Listener = new EventBasedNetListener();
            NetManager = new NetManager(Listener);
            Events = new NetworkEvents();
            NatPunchListener = new EventBasedNatPunchListener();

            NatPunchListener.NatIntroductionSuccess += NatPunchListener_NatIntroductionSuccess;

            
            Listener.ConnectionRequestEvent += Listener_ConnectionRequestEvent;
            Listener.NetworkReceiveEvent += Listener_NetworkReceiveEvent;
            Listener.PeerConnectedEvent += Listener_PeerConnectedEvent;
            Listener.PeerDisconnectedEvent += Listener_PeerDisconnectedEvent;
            Listener.NetworkErrorEvent += Listener_NetworkErrorEvent;


        }

        private void NatPunchListener_NatIntroductionSuccess(System.Net.IPEndPoint targetEndPoint, string token)
        {
            if (targetEndPoint.Address.ToString() == "::1")
                return;
            NetManager.Connect(targetEndPoint, "SomeConnectionKey");
        }

        public override void Start()
        {
            //InitServerNTP();
            if (UseLobby)
            {
                NetManager.NatPunchEnabled = true;
                NetManager.NatPunchModule.Init(NatPunchListener);
            }

            NetManager.Start(LocalPort);
            Debug.Log("NetManager started on port" + ":" + NetManager.LocalPort);
            LocalPeer = new NetPeer(NetManager, null);

            NetScene.InititateScene();

        }

        #region Listners

        private void Listener_PeerDisconnectedEvent(NetPeer peer, DisconnectInfo disconnectInfo)
        {
            switch (disconnectInfo.Reason)
            {
                case DisconnectReason.RemoteConnectionClose:
                case DisconnectReason.DisconnectPeerCalled:
                    NetScene.RemovePlayer(peer);
                    OnPeerLeave(peer);
                    Debug.Log("The peer you where connected to has disconnected with the IP " + peer.EndPoint.ToString());
                    break;
                case DisconnectReason.Timeout:
                    NetScene.RemovePlayer(peer);
                    OnPeerLeave(peer);
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
        
            if(NetScene.Players.Count == 0) // We are new player.
            {
                NetScene.SpawnPlayer(PlayerPrefab, LocalPeer, true);
                NetScene.SpawnPlayer(PlayerPrefab, _peer, false);
                OnPeerJoin(LocalPeer);
            }
            else //Someone is joining us.
            {
                //Send server info to this guy.
                bool responsible = ResponsiblePeer == LocalPeer;
                NetworkEvents.ServerInfoEvent serverInfoEvent = new NetworkEvents.ServerInfoEvent(ServerStartTime, NetManager.GetPeers(ConnectionState.Connected), _peer, responsible);
                Events.SendEventToPeer(serverInfoEvent, DeliveryMethod.ReliableOrdered, _peer);

                NetScene.SpawnPlayer(PlayerPrefab, _peer, false);
                TransferOwnedObjects();
                NtpRequest.Make("pool.ntp.org", 123, dateTime =>
                {
                    if (dateTime.HasValue)
                    {
                        ServerStartTime = dateTime.Value.ToUniversalTime().Ticks;
                    }
                });
            }
            OnPeerJoin(_peer);
        }

        virtual protected void OnPeerJoin(NetPeer peer)
        {

        }
        virtual protected void OnPeerLeave(NetPeer peer)
        {

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
                        NetScene.ReadObjectData(reader);
                    }
                    break;
                case PacketType.EVENT:
                    Debug.Log("recived events!");
                    NetPacketProcessor.ReadAllPackets(reader, peer);
                    break;
                case PacketType.RPC:
                    HandleRPC(reader, peer);
                    break;
                default:
                    break;
            }
            reader.Recycle();
        }

        private void Listener_ConnectionRequestEvent(ConnectionRequest request)
        {

            if (NetManager.PeersCount < 10 /* max connections */)
                request.AcceptIfKey("SomeConnectionKey");
            else
                request.Reject();
        }

        #endregion

        public override void Update()
        {
            NetManager.UpdateTime = (1000 / TICK_RATE);
            if(NetManager.NatPunchEnabled)
                NetManager.NatPunchModule.PollEvents();
            NetManager.PollEvents(); 

            if (Input.GetKeyDown(Input.Keys.J)) //JOIN
            {
                if (NetScene.Players.ContainsKey(LocalPeer))
                {
                    Debug.LogWarning("You are already connected to a match!");
                }else
                {
                    if (UseLobby)
                        NetManager.NatPunchModule.SendNatIntroduceRequest(NetUtils.MakeEndPoint(TargetIP, TargetPort), "Domarn");
                    else
                    {
                        NetManager.Connect(TargetIP, TargetPort, "SomeConnectionKey");
                    }
                }

            }

            if(Input.GetKeyDown(Input.Keys.H)) //HOST
            {
                if (NetScene.Players.ContainsKey(LocalPeer))
                {
                    Debug.LogWarning("You are already connected to a match!");
                }
                else
                {
                    ResponsiblePeer = LocalPeer;
                    NetScene.SpawnPlayer(PlayerPrefab, LocalPeer, true);
                    NetScene.ActivateSceneObjects();
                    OnPeerJoin(LocalPeer);
                }
              
            }

            Diagnostics();
        }

        protected override void OnDestroy()
        {
            if (NetManager != null)
            {
                NetManager.DisconnectAll();
                NetManager.Stop();
            }

        }

        public void TakeOwnership(NetworkIdentity networkIdentiy)
        {

            NetworkEvents.TransferOwnerEvent transferOwnerEvent = new NetworkEvents.TransferOwnerEvent{
                NetID = networkIdentiy.ID
            };
            NetScene.ObjectOwners[LocalPeer].Add(networkIdentiy);
            Events.SendEvent(transferOwnerEvent, DeliveryMethod.ReliableOrdered);
        }


        public void SendRPC(int netID, string methodName, params object[] parameters)
        {
            Debug.Log("Sending RPC: " + methodName);
            NetDataWriter writer = new NetDataWriter();

            writer.Put((int)PacketType.RPC);
            writer.Put(netID);
            writer.Put(methodName);
            RpcUtils.WriteRPCParameters(parameters, writer);
            InternalManager.SendToAll(writer, DeliveryMethod.ReliableOrdered);
        }

        private void HandleRPC(NetPacketReader reader, NetPeer peer)
        {
            int netID = reader.GetInt();
            if(netID == -2) //RPC from here
            {
                string methodName = reader.GetString();
                Type t = this.GetType();
                System.Reflection.MethodInfo methodInfo = t.GetMethod(methodName);
                if(methodInfo == null)
                {
                    Debug.LogError("RPC: Method: " + methodName + " does not exist.");
                }else
                {
                    object[] parameters = RpcUtils.ReadRPCParameters(methodInfo, reader);
                    methodInfo.Invoke(this, parameters);
                }
            }
            else
            {
                NetworkIdentity identity;
                if (netID > 0) //RPC from object
                {
                    identity = NetScene.FindNetworkObject(netID);
                }
                else //RPC from player
                {
                    identity = NetScene.Players[peer];
                }
                if(identity == null)
                {
                    Debug.LogError("RPC: Failed to find NetworkIdentity for ID: " + netID);
                }else
                    identity.ReadRPC(reader);
            }
        }

        
        private void TransferOwnedObjects()
        {
            foreach(NetworkIdentity identity in NetScene.ObjectOwners[LocalPeer])
            {
                if(identity.PrefabID == -1) //Scene object
                    identity.WriteInitialData();
                else
                {
                    TransferOwnedPrefab(identity);
                }
            }
            NetScene.Players[LocalPeer].WriteInitialData();
        }

        private void TransferOwnedPrefab(NetworkIdentity identity)
        {
            Debug.Log("Transfering prefab: " + identity.Name);
            NetworkEvents.SpawnPrefabEvent spawnPrefabEvent = new NetworkEvents.SpawnPrefabEvent
            {
                NetID = identity.ID,
                Position = identity.transform.position,
                Rotation = identity.transform.rotation,
                PrefabID = identity.PrefabID,
                Owner = true
            };
            Events.SendEvent(spawnPrefabEvent, DeliveryMethod.ReliableOrdered);
            identity.WriteInitialData();
        }

        public GameObject NetworkInstantiate(GameObject prefab, Vector3 position, Quaternion rotation, bool owner = false, int targetID = -1)
        {
            
            int prefabID = SpawnablePrefabs.IndexOf(prefab);
            if(prefabID >= 0)
            {
                GameObject gObj = GameObject.Instantiate(prefab, position, rotation);
                NetworkIdentity identity = gObj.GetComponent<NetworkIdentity>();
                int netID = targetID;
                if (targetID >= 0)
                    NetScene.AddObject(identity, targetID);
                else
                    netID = NetScene.AddObject(identity);
                NetworkEvents.SpawnPrefabEvent spawnPrefabEvent = new NetworkEvents.SpawnPrefabEvent
                {
                    NetID = netID,
                    Position = position,
                    Rotation = rotation,
                    PrefabID = prefabID,
                    Owner = owner
                };
                Events.SendEvent(spawnPrefabEvent, DeliveryMethod.ReliableOrdered);
                identity.Owner = owner;
                identity.WriteInitialData();
                return gObj;
            }else
            {
                Debug.LogError("Prefab is not registered");
                return null;
            }
        }

        public void CheckPacketLoss()
        {
            Debug.Log("A error has occured here are the amount of packetloss " + NetManager.Statistics.PacketLoss + "% lost " + NetManager.Statistics.PacketLossPercent);
            Debug.Log("A total of " + NetManager.Statistics.PacketsSent + "was sent and " + NetManager.Statistics.PacketsReceived + "was recieved");
        }

        public void Diagnostics()
        {
            GUI.ImguiStringUpdate("Packetsloss = " + NetManager.Statistics.PacketLossPercent, new Vector2(0, 10));
            GUI.ImguiStringUpdate("Total package sent = " + NetManager.Statistics.PacketsSent, new Vector2(0, 20));
            GUI.ImguiStringUpdate("Total package recieved = " + NetManager.Statistics.PacketsReceived, new Vector2(0, 30));
            PingToAllClients();
        }

        public void PingToAllClients()
        {
            int i = 0;
            foreach(var player in NetManager.GetPeers(ConnectionState.Connected))
            {
                GUI.ImguiStringUpdate("Ping to client " + player.EndPoint.ToString() + "  " + player.Ping, new Vector2(0, 40 + 10 * i));
                i++;
            }
        }
    }
}
