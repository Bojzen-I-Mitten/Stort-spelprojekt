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
        public int nextAssignableID = 0;
        private List<NetworkIdentity> PlayerPool = new List<NetworkIdentity>();
        public List<NetworkIdentity> AllPlayers = new List<NetworkIdentity>();
        public Dictionary<NetPeer, NetworkIdentity> Players = new Dictionary<NetPeer, NetworkIdentity>();
        public Dictionary<int, NetworkIdentity> NetworkObjects = new Dictionary<int, NetworkIdentity>();
        private List<NetworkIdentity> SceneObjectToBeActivated = new List<NetworkIdentity>();
        public Dictionary<NetPeer, List<NetworkIdentity>> ObjectOwners = new Dictionary<NetPeer, List<NetworkIdentity>>();

        public void ReadPlayerData(NetPeer peer, NetPacketReader reader)
        {
            if (Players.ContainsKey(peer))
            {
                NetworkIdentity playerID = Players[peer];
                bool initialState = reader.GetBool();
                playerID.Ping = peer.Ping;
                playerID.ReadData(reader, initialState);
            }

        }

        public void ReadObjectData(NetPacketReader reader)
        {
            int networkID = reader.GetInt();
            bool initialState = reader.GetBool();
            NetworkIdentity identity = NetworkObjects.ContainsKey(networkID) ? NetworkObjects[networkID] : null;
            if (identity)
            {
                identity.ReadData(reader, initialState);
                
            }else
            {
                Debug.LogError("network ID: " + networkID + " does not exist in scene");
            }
        }

        public void InitPlayerPool(GameObject playerPrefab, int maxPlayers)
        {
            for(int i=0; i < maxPlayers+1; i++)
            {
                GameObject player = GameObject.Instantiate(playerPrefab, new Vector3(-1000, -1000, -1000), Quaternion.Identity);
                player.activeSelf = false;
                NetworkIdentity identity = player.GetComponent<NetworkIdentity>();
                identity.IsPlayer = true;
                PlayerPool.Add(identity);
                AllPlayers.Add(identity);
            }
        }

        private NetworkIdentity GetAvailablePlayerFromPool()
        {
            if(PlayerPool.Count > 0)
            {
                NetworkIdentity player = PlayerPool[0];
                PlayerPool.RemoveAt(0);
                return player;
            }
            else
                return null;
        }
        private void RecyclePlayer(NetworkIdentity player)
        {
            player.gameObject.SetActive(false);
            PlayerPool.Add(player);
        }

        public bool PoolNotEmpty()
        {
            return PlayerPool.Count > 0;
        }

        public void SpawnPlayer(NetPeer peer, bool myPlayer)
        {
            if (Players.ContainsKey(peer))
            {
                //Player was already in scene. WTF?
                return;
            }

            ObjectOwners[peer] = new List<NetworkIdentity>();

            NetworkIdentity player = GetAvailablePlayerFromPool();
            if(player)
            {
                NetworkIdentity networkIdentity = player.GetComponent<NetworkIdentity>();
                // If spawned player is local character: Receive ownership  
                networkIdentity.ReceiveOwnershipStatus(myPlayer);
                String name = "Chad_" + networkIdentity.ID;
                if (myPlayer)
                    player.Name = name + "_(my player)";
                else
                    player.Name = name;
                Players[peer] = networkIdentity;
            }
            else
            {
                Debug.LogError("Failed to spawn player. No free slots in pool");
            }
        }
        
        public void RemovePlayer(NetPeer peer)
        {

            //Transfer that players objects to someone else.

            if (ObjectOwners.ContainsKey(peer))
            {
                foreach (NetworkIdentity identity in ObjectOwners[peer])
                    identity.Owner = true;

                ObjectOwners[peer].Clear();
                ObjectOwners.Remove(peer);
            }

            if (Players.ContainsKey(peer))
            {
                NetworkIdentity id = Players[peer];
                if (id != null)
                    RecyclePlayer(id);
                Players.Remove(peer);
            }
        }

        public void InititateScene()
        {
            IEnumerable<NetworkIdentity> enumer = ThomasWrapper.CurrentScene.getComponentsOfType<NetworkIdentity>();
            foreach ( NetworkIdentity identity in enumer)
            {
                if (identity.IsPlayer)
                    continue;
                NetworkObjects.Add(++nextAssignableID, identity);

                if (identity.gameObject.GetActive())
                {
                    SceneObjectToBeActivated.Add(identity);
                    identity.gameObject.SetActive(false);
                }

            }
        }

        public int AddObject(NetworkIdentity identity)
        {
            NetworkObjects.Add(++nextAssignableID, identity);
            return nextAssignableID;
        }

        public void RemoveObject(NetworkIdentity identity)
        {
            NetPeer previousOwner = FindOwnerOf(identity);
            NetworkObjects.Remove(identity.ID);
            if(previousOwner != null)
                ObjectOwners[previousOwner].Remove(identity);
        }


        public void AddObject(NetworkIdentity identity, int id)
        {
            NetworkObjects.Add(id, identity);
        }

        public void ActivateSceneObjects()
        {
            ThomasWrapper.CurrentScene.getComponentsOfType<NetworkIdentity>().ForEach((identity) =>
            {
                if (!identity.gameObject.GetActive())
                {
                    identity.Owner = true;
                    if(SceneObjectToBeActivated.Contains(identity))
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
