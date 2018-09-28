
using System;
using System.Collections.Generic;
using System.ComponentModel;
using LiteNetLib;
using LiteNetLib.Utils;
using System.Linq;
namespace ThomasEngine.Network
{
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

        internal NetPacketProcessor NetPacketProcessor;

        private EventBasedNetListener Listener;
        private NetManager NetManager;
        private EventBasedNatPunchListener NatPunchListener;
        private NetworkEvents Events;

        public int LocalPort { get; set; } = 9050;

        public string TargetIP { get; set; } = "localhost";
        public int TargetPort { get; set; } = 9050;

        public bool UseLobby { get; set; } = false;


        public List<GameObject> SpawnablePrefabs { get; set; } = new List<GameObject>();
        public GameObject PlayerPrefab { get; set; }
        
        private float ServerTime;

        private NetPeer LocalPeer = null;

        public int TICK_RATE { get; set; } = 24;

        internal NetworkScene NetScene;

        [Browsable(false)]
        public NetManager InternalManager { get { return NetManager; } }

        public NetworkManager()
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

        }

        #region Listners

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
            
            foreach (NetPeer peer in NetManager.GetPeers(ConnectionState.Connected))
            {
                if (peer == _peer)
                    continue;
                NetworkEvents.ConnectToPeerEvent connectEvent = new NetworkEvents.ConnectToPeerEvent
                {
                    IP = peer.EndPoint.Address.ToString(),
                    Port = peer.EndPoint.Port
                };

                Events.SendEventToPeer(connectEvent, DeliveryMethod.ReliableOrdered, _peer);

            }

            if(NetScene.Players.Count == 0) // We are new player.
            {
                NetScene.SpawnPlayer(PlayerPrefab, LocalPeer, true);
                NetScene.SpawnPlayer(PlayerPrefab, _peer, false);
            }
            else //Someone is joining us.
            {
                NetScene.SpawnPlayer(PlayerPrefab, _peer, false);
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
                    NetPacketProcessor.ReadAllPackets(reader, peer);
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
            ServerTime += Time.ActualDeltaTime;
            if(NetManager.NatPunchEnabled)
                NetManager.NatPunchModule.PollEvents();
            NetManager.PollEvents(); 

            if (Input.GetKeyDown(Input.Keys.J)) //JOIN
            {
                if(UseLobby)
                    NetManager.NatPunchModule.SendNatIntroduceRequest(NetUtils.MakeEndPoint(TargetIP, TargetPort), "Domarn");
                else
                {
                    NetManager.Connect(TargetIP, TargetPort, "SomeConnectionKey");
                }
                    
            }

            if(Input.GetKeyDown(Input.Keys.H)) //HOST
            {

                NetScene.SpawnPlayer(PlayerPrefab, LocalPeer, true);

            }

            Diagnostics();
        }

        public override void OnDestroy()
        {
            if (NetManager != null)
            {
                NetManager.DisconnectAll();
                NetManager.Stop();
            }

        }

      
        

        //internal void InitServerNTP()
        //{
        //    NetDataWriter writer = new NetDataWriter();
        //    writer.Put((int)PacketType.EVENT);

        //    netPacketProcessor.Write<T>(writer, data);
        //    sendTo.Send(writer, method);
        //}


                                    

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
