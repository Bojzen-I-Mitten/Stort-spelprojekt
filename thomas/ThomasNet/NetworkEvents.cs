﻿using System;
using System.Collections.Generic;
using LiteNetLib;
using LiteNetLib.Utils;
using System.Threading;

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
            SubscribeToEvent<DeletePrefabEvent>(DeletePrefabEventHandler);
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

            public string[] PeerIPs { get; set; }
            public int[] PeerPorts { get; set; }
            public long ServerStartTime { get; set; }
            public long TimeConnected { get; set; }
            public int SceneNextAssignableID { get; set; }

            public ServerInfoEvent() { }
            public ServerInfoEvent(long serverStartTime, NetPeer[] peers, NetPeer excluded, long timeConnected, int nextAssignableID)
            {
                ServerStartTime = serverStartTime;
                PeerIPs = new string[peers.Length - 1];
                PeerPorts = new int[peers.Length - 1];
                SceneNextAssignableID = nextAssignableID;
                int i = 0;
                foreach (NetPeer peer in peers)
                {
                    if (peer == excluded)
                        continue;

                    PeerIPs[i] = peer.EndPoint.Address.ToString();
                    PeerPorts[i] = peer.EndPoint.Port;
                    i++;
                }
                TimeConnected = timeConnected;
            }
        }

        public class TransferOwnerEvent
        {
            public int NetID { get; set; }

            public TransferOwnerEvent() { NetID = -1; }
        }

        #endregion

        #region Event Handlers
        public void ServerInfoEventHandler(ServerInfoEvent serverInfo, NetPeer peer)
        {
            List<NetPeer> nps = new List<NetPeer>();
            for (int i = 0; i < serverInfo.PeerIPs.Length; ++i)
            {
                nps.Add(Manager.InternalManager.Connect(serverInfo.PeerIPs[i], serverInfo.PeerPorts[i], "SomeConnectionKey"));
            }

            Thread t = new Thread(() =>
            {
                while (true)
                {
                    bool AllDone = true;
                    foreach (var np in nps)
                    {
                        if (np.ConnectionState == ConnectionState.Disconnected)
                        {
                            Manager.InternalManager.DisconnectAll();
                            return;
                        }
                        AllDone &= np.ConnectionState == ConnectionState.Connected;
                    }
                    if (AllDone)
                    {
                        break;
                    }
                }

                Manager.InternalManager.OnAllConnected();
                Manager.ServerStartTime = serverInfo.ServerStartTime;
                Manager.Scene.nextAssignableID = serverInfo.SceneNextAssignableID;
                Manager.Scene.TimePlayerJoined.Add(peer, serverInfo.TimeConnected);
            });
            t.Start();
        }

        public void SpawnPrefabEventHandler(SpawnPrefabEvent prefabEvent, NetPeer peer)
        {
            NetworkIdentity identity;

            if (NetScene.NetworkObjects.TryGetValue(prefabEvent.NetID, out identity))
            {
                if (identity.PrefabID != prefabEvent.PrefabID)
                {
                    Debug.LogError("Object already exist with network ID: " + prefabEvent.NetID);
                }
            }
            else if (Manager.SpawnablePrefabs.Count > prefabEvent.PrefabID)
            {
                GameObject prefab = Manager.SpawnablePrefabs[prefabEvent.PrefabID];
                GameObject gameObject = GameObject.Instantiate(prefab, prefabEvent.Position, prefabEvent.Rotation);
                identity = gameObject.GetComponent<NetworkIdentity>();
                identity.PrefabID = prefabEvent.PrefabID;
                NetScene.AddObject(identity, prefabEvent.NetID);
            }
            else
            {
                Debug.LogError("Failed to spawn prefab. It's not registered");
            }

            if (prefabEvent.Owner && identity != null)
            {
                NetScene.ObjectOwners[peer].Add(identity);
            }
        }

        public void DeletePrefabEventHandler(DeletePrefabEvent prefabEvent, NetPeer peer)
        {
            NetworkIdentity identity;

            if (NetScene.NetworkObjects.TryGetValue(prefabEvent.netID, out identity))
            {
                NetScene.RemoveObject(identity);
                GameObject.Destroy(identity.gameObject);
            }//else
             //Debug.LogError("Failed to find gameObject. It does not exist in scene");
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
                Debug.LogWarning("Transfered GameObject: " + networkIdentity.gameObject.Name + " to: " + newOwner.EndPoint.Address.ToString());
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
