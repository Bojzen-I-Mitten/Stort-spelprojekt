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
    public class NetworkEvents
    {

        private NetSerializer NetSerializer;

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
            NetSerializer = new NetSerializer();
            SubscribeToEvent<ServerInfoEvent>(ServerInfoEventHandler);
            SubscribeToEvent<SpawnPrefabEvent>(SpawnPrefabEventHandler);
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
            public int NetID { get; set; }
            public int PrefabID { get; set; }
            public Vector3 Position { get; set; }
            public Quaternion Rotation { get; set; }
            public bool Owner { get; set; }

            public SpawnPrefabEvent() { NetID = -1; PrefabID = -2; Position = new Vector3(); Rotation = new Quaternion(); Owner = false; }
        }

        public class DeletePrefabEvent
        {
            public int netID { get; set; }

            public DeletePrefabEvent() { netID = -1; }
        }


        public class ServerInfoEvent
        {

            public string[] PeerIPs {get;set;}
            public int[] PeerPorts { get; set; }
            public long ServerStartTime { get; set; }
            public bool IsResponsiblePeer { get; set; }

            public ServerInfoEvent() { }
            public ServerInfoEvent(long serverStartTime, NetPeer[] peers, NetPeer excluded, bool responsible)
            {
                ServerStartTime = serverStartTime;
                PeerIPs = new string[peers.Length - 1];
                PeerPorts = new int[peers.Length - 1];
                int i = 0;
                foreach(NetPeer peer in peers)
                {
                    if (peer == excluded)
                        continue;

                    PeerIPs[i] = peer.EndPoint.Address.ToString();
                    PeerPorts[i] = peer.EndPoint.Port;
                    i++;
                }
                IsResponsiblePeer = responsible;
            }
        }

        public class TransferOwnerEvent
        {
            public int NetID { get; set; }

            public TransferOwnerEvent() { NetID = -1;}
        }
        #endregion
        
        #region Event Handlers
        public void ServerInfoEventHandler(ServerInfoEvent serverInfo, NetPeer peer)
        {
            for(int i=0; i < serverInfo.PeerIPs.Length; ++i)
            {
                Manager.InternalManager.Connect(serverInfo.PeerIPs[i], serverInfo.PeerPorts[i], "SomeConnectionKey");
            }
            if (serverInfo.IsResponsiblePeer)
                Manager.ResponsiblePeer = peer;
            Manager.ServerStartTime = serverInfo.ServerStartTime;
            
        }
        
        public void SpawnPrefabEventHandler(SpawnPrefabEvent prefabEvent, NetPeer peer)
        {
            Debug.LogWarning("spawnPrefabEvent!!!!!");
            NetworkIdentity identity = null;
            if (NetScene.NetworkObjects.ContainsKey(prefabEvent.NetID))
            {
                identity = NetScene.NetworkObjects[prefabEvent.NetID];
                if (identity.PrefabID != prefabEvent.PrefabID)
                {
                    Debug.LogError("Object already exist with network ID: " + prefabEvent.NetID);
                }
            }
            else if(Manager.SpawnablePrefabs.Count > prefabEvent.PrefabID)
                {
                GameObject prefab = Manager.SpawnablePrefabs[prefabEvent.PrefabID];
                GameObject gameObject = GameObject.Instantiate(prefab, prefabEvent.Position, prefabEvent.Rotation);
                identity = gameObject.GetComponent<NetworkIdentity>();
                NetScene.AddObject(identity, prefabEvent.NetID);
                //Ownership shit

            }else
            {
                Debug.LogError("Failed to spawn prefab. It's not registered");
            } 

            if(prefabEvent.Owner && identity != null)
            {
                NetScene.ObjectOwners[peer].Add(identity);
            }
        }

        public void TransferOwnerEventHandler(TransferOwnerEvent transEvent, NetPeer newOwner)
        {

            NetworkIdentity networkIdentity = NetScene.FindNetworkObject(transEvent.NetID);
            if (networkIdentity != null)
            {
                NetPeer previousOwner = NetScene.FindOwnerOf(networkIdentity);

                if (previousOwner == newOwner)
                {
                    Debug.Log("Peer is already owner of object: " + networkIdentity.gameObject.Name);
                    return;
                }

                //Remove previous owner
                if (previousOwner != null)
                    NetScene.ObjectOwners[previousOwner].Remove(networkIdentity);

                NetScene.ObjectOwners[newOwner].Add(networkIdentity);
                //Make sure we do not own the object.
                networkIdentity.Owner = false;
                if (previousOwner == Manager.LocalPeer)
                {
                    foreach (var comp in networkIdentity.gameObject.GetComponents<NetworkComponent>())
                    {
                        comp.OnLostOwnership();
                    }
                }
            }
            else
            {
                Debug.LogError("Could not find the network object");
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
