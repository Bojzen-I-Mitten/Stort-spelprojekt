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

        private NetworkScene NetScene
        {
            get { return NetworkManager.instance.NetScene; }
        }

        public NetworkEvents()
        {
            SubscribeToEvent<ConnectToPeerEvent>(ConnectToPeerEventHandler);
            SubscribeToEvent<TransferOwnerEvent>(TransferOwnerEventHandler);
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
            public int Port { get; set; }

            public ConnectToPeerEvent() { IP = ""; Port = -1; }
        }

        public class TransferOwnerEvent
        {
            public int NetID;
            public NetPeer NewOwner;

            public TransferOwnerEvent() { NetID = -1; NewOwner = null; }
        }
        #endregion
        
        #region Event Handlers
        public void ConnectToPeerEventHandler(ConnectToPeerEvent connectEvent, NetPeer peer)
        {

            Manager.InternalManager.Connect(connectEvent.IP, connectEvent.Port, "SomeConnectionKey");
        }

        public void TransferOwnerEventHandler(TransferOwnerEvent transEvent, NetPeer peer)
        {
            List<NetworkID> netObjectIDs = NetScene.NetworkObjects[peer];
            foreach (var netObjectID in netObjectIDs)
            {
                if (netObjectID.ID == transEvent.NetID) //Object found
                {
                    if (transEvent.NewOwner == Manager.LocalPeer) //If I'm the owner
                        netObjectID.Owner = true;

                    netObjectIDs.Remove(netObjectID); //Remove from old player's list
                    NetScene.NetworkObjects[transEvent.NewOwner].Add(netObjectID); //Add to new player's list

                    break;
                }
            }
        }
        #endregion

        #region Writing & reading
        public void SubscribeToEvent<T>(Action<T, NetPeer> onReceive) where T : class, new()
        {
            Manager.NetPacketProcessor.SubscribeReusable<T, NetPeer>(onReceive);
        }

        public void WriteEvent<T>(NetDataWriter writer, T data) where T : class, new()
        {
            Manager.NetPacketProcessor.Write<T>(writer, data);
        }

        public void SendEvent<T>(T data, DeliveryMethod method) where T : class, new()
        {
            NetDataWriter writer = new NetDataWriter();
            writer.Put((int)PacketType.EVENT);

            Manager.NetPacketProcessor.Write<T>(writer, data);
            Manager.InternalManager.SendToAll(writer, method);
        }

        public void SendEventToAllBut<T>(T data, DeliveryMethod method, NetPeer excluded) where T : class, new()
        {
            NetDataWriter writer = new NetDataWriter();
            writer.Put((int)PacketType.EVENT);

            Manager.NetPacketProcessor.Write<T>(writer, data);
            Manager.InternalManager.SendToAll(writer, method, excluded);
        }

        public void SendEventToPeer<T>(T data, DeliveryMethod method, NetPeer sendTo) where T : class, new()
        {
            NetDataWriter writer = new NetDataWriter();
            writer.Put((int)PacketType.EVENT);

            Manager.NetPacketProcessor.Write<T>(writer, data);
            sendTo.Send(writer, method);
        }
        #endregion
    }
}
