using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using LiteNetLib;
namespace ThomasEngine.Network
{
    public class NetworkScene
    {

        public NetworkScene()
        {
        }
        int nextAssignableID = 0;
        public Dictionary<NetPeer, NetworkIdentity> Players = new Dictionary<NetPeer, NetworkIdentity>();
        public Dictionary<int, NetworkIdentity> NetworkObjects = new Dictionary<int, NetworkIdentity>();
        public Dictionary<NetPeer, List<NetworkIdentity>> ObjectOwners = new Dictionary<NetPeer, List<NetworkIdentity>>();

        public void ReadPlayerData(NetPeer peer, NetPacketReader reader)
        {
            if (Players.ContainsKey(peer))
            {
                NetworkIdentity playerID = Players[peer];
                bool initialState = reader.GetBool();
                if(playerID.enabled)
                    playerID.ReadData(reader, initialState);
            }

        }

        public void ReadObjectData(NetPacketReader reader)
        {
            int networkID = reader.GetInt();
            bool initialState = reader.GetBool();
            NetworkIdentity identity = NetworkObjects[networkID];
            if (identity)
            {
                if (identity.gameObject.GetActive() || initialState)
                {
                    if (initialState)
                        identity.gameObject.SetActive(true);
                    identity.ReadData(reader, initialState);
                }
            }else
            {
                Debug.LogError("network ID: " + networkID + " does not exist in scene");
            }
        }

        public void SpawnPlayer(GameObject playerPrefab, NetPeer peer, bool myPlayer)
        {
            if (Players.ContainsKey(peer))
            {
                //Player was already in scene. WTF?
                return;
            }

            ObjectOwners[peer] = new List<NetworkIdentity>();

            if(playerPrefab.GetComponent<NetworkIdentity>())
            {
                GameObject obj = GameObject.Instantiate(playerPrefab);
                NetworkIdentity networkIdentiy = obj.GetComponent<NetworkIdentity>();
                networkIdentiy.IsPlayer = true;
                networkIdentiy.Owner = myPlayer;

                Players[peer] = networkIdentiy;
            }else
            {
                Debug.LogError("Failed to spawn player. Prefab does not contain a networkIdentity component");
            }
        }
        
        public void RemovePlayer(NetPeer peer)
        {

            //Transfer that players objects to someone else.

            if (Players.ContainsKey(peer))
            {
                NetworkIdentity id = Players[peer];
                if (id != null)
                    id.gameObject.Destroy();
                Players.Remove(peer);
            }
        }

        public void InititateScene()
        {
            Object.GetObjectsOfType<NetworkIdentity>().ForEach((identity) =>
            {
                NetworkObjects.Add(nextAssignableID++, identity);
                identity.gameObject.SetActive(false);

            });
        }

        public void ActivateSceneObjects()
        {
            Object.GetObjectsOfType<NetworkIdentity>().ForEach((identity) =>
            {
                if (!identity.gameObject.GetActive())
                {
                    identity.Owner = true;
                    identity.gameObject.SetActive(true);
                }

            });
        }
        
        public NetworkIdentity FindNetworkObject(int netID)
        {
            if (NetworkObjects.ContainsKey(netID))
            {
                return NetworkObjects[netID];
            }
            return null;
        }

        public NetPeer FindOwnerOf(NetworkIdentity networkIdentiy)
        {
            foreach (var peer in ObjectOwners)
            {
                if (peer.Value.Contains(networkIdentiy))
                    return peer.Key;
            }
            return null;
        }

    }
}
