using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ThomasEngine;
using LiteNetLib;
using LiteNetLib.Utils;

namespace ThomasEngine.Network
{
    class NetworkEvents
    {
        private NetworkManager Manager
        {
            get { return NetworkManager.instance; }
        }

        private EventBasedNetListener Listener
        {
            get { return Manager.listener; }
        }

        public NetworkEvents()
        {
            Listener.ConnectionRequestEvent += Listener_ConnectionRequestEvent;
            Listener.NetworkReceiveEvent += Listener_NetworkReceiveEvent;
            Listener.PeerConnectedEvent += Listener_PeerConnectedEvent;
            Listener.PeerDisconnectedEvent += Listener_PeerDisconnectedEvent;
            Listener.NetworkErrorEvent += Listener_NetworkErrorEvent;

            SubscribeToEvent<SpawnPrefabEvent>(SpawnPrefabEventHandler);
            SubscribeToEvent<DeletePrefabEvent>(DeletePrefabEventHandler);
            SubscribeToEvent<ConnectToPeerEvent>(ConnectToPeerEventHandler);
        }


        #region Events
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
        #endregion

        #region Listeners
        public void Listener_PeerDisconnectedEvent(NetPeer peer, DisconnectInfo disconnectInfo)
        {
            switch (disconnectInfo.Reason)
            {
                case DisconnectReason.RemoteConnectionClose:
                case DisconnectReason.DisconnectPeerCalled:
                    Manager.RemovePlayerCharacter(peer);
                    Debug.Log("The peer you where connected to has disconnected with the IP " + peer.EndPoint.ToString());
                    break;
                case DisconnectReason.Timeout:
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

        public void Listener_PeerConnectedEvent(NetPeer _peer)
        {
            ThomasEngine.Debug.Log("A peer has connected with the IP" + _peer.EndPoint.ToString());

            foreach (NetPeer peer in Manager.netPeers)
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

            Manager.netPeers.Add(_peer);
            Manager.SpawnPlayerCharacter(_peer);
        }

        public void Listener_NetworkErrorEvent(System.Net.IPEndPoint endPoint, System.Net.Sockets.SocketError socketError)
        {
            Debug.Log("A network error has occured from peer " + endPoint);
        }

        public void Listener_NetworkReceiveEvent(NetPeer peer, NetPacketReader reader, DeliveryMethod deliveryMethod)
        {
            if (reader.EndOfData)
                return;
            PacketType type = (PacketType)reader.GetInt();
            switch (type)
            {
                case PacketType.DATA:
                    while (reader.AvailableBytes > 0)
                    {
                        int validationID = reader.GetInt();
                        if (Manager.networkIDObjects.ContainsKey(validationID) && Manager.networkIDObjects[validationID].enabled) { }
                        //networkIDObjects[validationID].Read(reader);
                        else
                            reader.Clear();
                    }
                    break;
                case PacketType.EVENT:
                    Debug.Log("recived events!");
                    Manager.netPacketProcessor.ReadAllPackets(reader, peer);

                    break;
                default:
                    break;
            }
        }

        public void Listener_ConnectionRequestEvent(ConnectionRequest request)
        {
            if (Manager.netManager.PeersCount < 10 /* max connections */)
                request.AcceptIfKey("SomeConnectionKey");
            else
                request.Reject();
        }
        #endregion

        #region Event Handlers
        public void SpawnPrefabEventHandler(SpawnPrefabEvent spawnEvent, NetPeer peer)
        {
            GameObject prefab;
            if (spawnEvent.prefabID >= 0 && spawnEvent.prefabID < Manager.spawnablePrefabs.Count)
            {
                prefab = Manager.spawnablePrefabs[spawnEvent.prefabID];
            }
            else if (spawnEvent.prefabID == -1)
            {
                prefab = Manager.playerPrefab;
            }
            else
            {
                Debug.Log("Tried spawning object not in NetworkManager prefab list");
                return;
            }

            GameObject gObj = Manager.InstantiateAndRegister(prefab, spawnEvent.netID, spawnEvent.position, spawnEvent.rotation);
            gObj.Name += spawnEvent.isOwner ? "(My player)" : "";
            gObj.GetComponent<NetworkID>().Owner = spawnEvent.isOwner;
        }

        public void DeletePrefabEventHandler(DeletePrefabEvent deleteEvent, NetPeer peer)
        {
            Manager.DeleteAndUnregister(deleteEvent.netID);
        }

        public void ConnectToPeerEventHandler(ConnectToPeerEvent connectEvent, NetPeer peer)
        {
            string address = connectEvent.IP.Substring(0, connectEvent.IP.IndexOf(":"));
            string sPort = connectEvent.IP.Substring(connectEvent.IP.IndexOf(":") + 1);
            int iPort = int.Parse(sPort);

            Manager.netManager.Connect(address, iPort, "SomeConnectionKey");
        }
        #endregion

        #region Writing & reading
        public void SubscribeToEvent<T>(Action<T, NetPeer> onReceive) where T : class, new()
        {
            Manager.netPacketProcessor.SubscribeReusable<T, NetPeer>(onReceive);
        }

        public void WriteEvent<T>(NetDataWriter writer, T data) where T : class, new()
        {
            Manager.netPacketProcessor.Write<T>(writer, data);
        }

        public void SendEvent<T>(T data, DeliveryMethod method) where T : class, new()
        {
            NetDataWriter writer = new NetDataWriter();
            writer.Put((int)PacketType.EVENT);

            Manager.netPacketProcessor.Write<T>(writer, data);
            Manager.netManager.SendToAll(writer, method);
        }

        public void SendEventToAllBut<T>(T data, DeliveryMethod method, NetPeer excluded) where T : class, new()
        {
            NetDataWriter writer = new NetDataWriter();
            writer.Put((int)PacketType.EVENT);

            Manager.netPacketProcessor.Write<T>(writer, data);
            Manager.netManager.SendToAll(writer, method, excluded);
        }

        public void SendEventToPeer<T>(T data, DeliveryMethod method, NetPeer sendTo) where T : class, new()
        {
            NetDataWriter writer = new NetDataWriter();
            writer.Put((int)PacketType.EVENT);

            Manager.netPacketProcessor.Write<T>(writer, data);
            sendTo.Send(writer, method);
        }
        #endregion
    }
}
