
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

        public EventBasedNetListener Listener { get; private set; }
        private NetManager NetManager;
        private EventBasedNatPunchListener NatPunchListener;
        protected NetworkEvents Events;

        public int LocalPort { get; set; } = 9050;

        public string TargetIP { get; set; } = "localhost";
        public int TargetPort { get; set; } = 9050;

        public bool UseLobby { get; set; } = false;


        public List<GameObject> SpawnablePrefabs { get; set; } = new List<GameObject>();
        public int MaxPlayers { get; set; }
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
            
        }

        public override void Awake()
        {
            instance = this;
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
            
            Scene.InitPlayerPool(PlayerPrefab, MaxPlayers);
        }

        private void NatPunchListener_NatIntroductionSuccess(System.Net.IPEndPoint targetEndPoint, string token)
        {
            if (targetEndPoint.Address.ToString() == "::1")
                return;
            NetManager.Connect(targetEndPoint, "SomeConnectionKey");
        }

        public override void Start()
        {
            NetScene.InititateScene();
            //InitServerNTP();
            if (UseLobby)
            {
                NetManager.NatPunchEnabled = true;
                NetManager.NatPunchModule.Init(NatPunchListener);
            }
        }

        public void Init()
        {
            NetManager.Start(LocalPort);
            Debug.Log("NetManager started on port" + ":" + NetManager.LocalPort);
            LocalPeer = new NetPeer(NetManager, null);

        }

        public void Host()
        {
            initServerStartTime();
            ResponsiblePeer = LocalPeer;
            NetScene.SpawnPlayer(LocalPeer, true);
            OnPeerJoin(LocalPeer);
            NetScene.ActivateSceneObjects();

        }

        public void Connect()
        {
            NetManager.Connect(TargetIP, TargetPort, "SomeConnectionKey");
        }

        public void Disconnect()
        {
            InternalManager.DisconnectAll();
        }

        #region Listners

        private void Listener_PeerDisconnectedEvent(NetPeer peer, DisconnectInfo disconnectInfo)
        {
            switch (disconnectInfo.Reason)
            {
                case DisconnectReason.RemoteConnectionClose:
                case DisconnectReason.DisconnectPeerCalled:
                    OnPeerLeave(peer);
                    NetScene.RemovePlayer(peer);
                    Debug.Log("The peer you where connected to has disconnected with the IP " + peer.EndPoint.ToString());
                    break;
                case DisconnectReason.Timeout:
                    OnPeerLeave(peer);
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

            if (NetScene.Players.Count == 0) // We are new player.
            {
                NetScene.SpawnPlayer(LocalPeer, true);
                NetScene.SpawnPlayer(_peer, false);
                OnPeerJoin(LocalPeer);
                NetScene.Players[LocalPeer].WriteInitialData();
            }
            else //Someone is joining us.
            {
                //Send server info to this guy.
                bool responsible = ResponsiblePeer == LocalPeer;
                NetworkEvents.ServerInfoEvent serverInfoEvent = new NetworkEvents.ServerInfoEvent(ServerStartTime, NetManager.GetPeers(ConnectionState.Connected), _peer, responsible, Scene.nextAssignableID);
                Events.SendEventToPeer(serverInfoEvent, DeliveryMethod.ReliableOrdered, _peer);

                NetScene.SpawnPlayer(_peer, false);
                TransferOwnedObjects();
            }
            OnPeerJoin(_peer);
        }

        void initServerStartTime()
        {
            NtpRequest.Make("pool.ntp.org", 123, dateTime =>
                {
                    if (dateTime.HasValue)
                    {
                        ServerStartTime = dateTime.Value.ToUniversalTime().Ticks;
                    }
                });
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
            try
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
            catch (Exception e)
            {
                Debug.LogException(e);
            }
        }

        private void Listener_ConnectionRequestEvent(ConnectionRequest request)
        {
            if (NetManager.PeersCount < MaxPlayers && Scene.PoolNotEmpty() /* max connections */)
                request.AcceptIfKey("SomeConnectionKey");
            else
            {
                Debug.Log("Connection was rejected!");
                request.Reject();
            }
                
        }

        #endregion

        public override void Update()
        { 
            if (NetManager.IsRunning)
            {
                EngineAutoProfiler profile = new EngineAutoProfiler("NetworkManager Update");
                NetManager.UpdateTime = (1000 / TICK_RATE);
                if (NetManager.NatPunchEnabled)
                    NetManager.NatPunchModule.PollEvents();
                NetManager.PollEvents();
                Diagnostics();
                profile.sendSample();


                //Check real owners.
                //if((int)TimeSinceServerStarted % 3 == 0)
                //{
                //    foreach (var owners in Scene.ObjectOwners)
                //    {
                //        if (owners.Key != LocalPeer)
                //        {
                //            foreach (var identity in owners.Value)
                //            {
                //                if (identity.ID >= 0)
                //                {
                //                    SendRPC(owners.Key, -2, "RPCTempOwnerStuff", identity.ID);
                //                }
                //            }
                //        }
                //    }
                //}
                
            }
        }

        public void RPCTempOwnerStuff(int ID)
        {
            Scene.FindNetworkObject(ID).Owner = true;
        }

        public override void OnDestroy()
        {
            if (NetManager != null)
            {
                NetManager.DisconnectAll();
                NetManager.Stop();
            }

        }

        public void TakeOwnership(NetworkIdentity networkIdentity)
        {

            NetworkEvents.TransferOwnerEvent transferOwnerEvent = new NetworkEvents.TransferOwnerEvent
            {
                NetID = networkIdentity.ID
            };
            if(!NetScene.ObjectOwners[LocalPeer].Contains(networkIdentity))
                NetScene.ObjectOwners[LocalPeer].Add(networkIdentity);
            Events.SendEvent(transferOwnerEvent, DeliveryMethod.ReliableOrdered);
        }

        public void SendRPC(NetPeer peer, int netID, string methodName, params object[] parameters)
        {
            NetDataWriter writer = new NetDataWriter();

            writer.Put((int)PacketType.RPC);
            writer.Put(netID);
            writer.Put(methodName);
            RpcUtils.WriteRPCParameters(parameters, writer);
            peer.Send(writer, DeliveryMethod.ReliableOrdered);
        }

        public void SendRPC(int netID, string methodName, params object[] parameters)
        {
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
            if (netID == -2) //RPC from here
            {
                string methodName = reader.GetString();
                Debug.Log("Recived RPC: " + methodName);
                Type t = this.GetType();
                System.Reflection.MethodInfo methodInfo = t.GetMethod(methodName);
                if (methodInfo == null)
                {
                    Debug.LogError("RPC: Method: " + methodName + " does not exist.");
                }
                else
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
                if (identity == null)
                {
                    Debug.LogError("RPC: Failed to find NetworkIdentity for ID: " + netID);
                }
                else
                    identity.ReadRPC(reader);
            }
        }


        private void TransferOwnedObjects()
        {
            foreach (NetworkIdentity identity in NetScene.ObjectOwners[LocalPeer])
            {
                if (identity.PrefabID == -1) //Scene object
                {
                    identity.WriteInitialData();
                    TakeOwnership(identity);
                }
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
            if (prefabID >= 0)
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
                identity.PrefabID = prefabID;
                identity.WriteInitialData();
                return gObj;
            }
            else
            {
                Debug.LogError("Prefab is not registered");
                return null;
            }
        }

        public void RemoveNetworkObject(GameObject gObj)
        {
            NetworkIdentity identity = gObj.GetComponent<NetworkIdentity>();
            NetworkEvents.DeletePrefabEvent deletePrefabEvent = new NetworkEvents.DeletePrefabEvent
            {
                netID = identity.ID
            };
            Events.SendEvent(deletePrefabEvent, DeliveryMethod.ReliableOrdered);
            Events.DeletePrefabEventHandler(deletePrefabEvent, LocalPeer);
        }

        public void CheckPacketLoss()
        {
            Debug.Log("A error has occured here are the amount of packetloss " + NetManager.Statistics.PacketLoss + "% lost " + NetManager.Statistics.PacketLossPercent);
            Debug.Log("A total of " + NetManager.Statistics.PacketsSent + "was sent and " + NetManager.Statistics.PacketsReceived + "was recieved");
        }

        public void Diagnostics()
        {
            Gizmos.ImguiStringUpdate("Packetsloss = " + NetManager.Statistics.PacketLossPercent, new Vector2(0, 10));
            Gizmos.ImguiStringUpdate("Total package sent = " + NetManager.Statistics.PacketsSent, new Vector2(0, 20));
            Gizmos.ImguiStringUpdate("Total package recieved = " + NetManager.Statistics.PacketsReceived, new Vector2(0, 30));
            PingToAllClients();
        }

        public void PingToAllClients()
        {
            int i = 0;
            foreach (var player in NetManager.GetPeers(ConnectionState.Connected))
            {
                Gizmos.ImguiStringUpdate("Ping to client " + player.EndPoint.ToString() + "  " + player.Ping, new Vector2(0, 40 + 10 * i));
                i++;
            }
        }
    }
}
