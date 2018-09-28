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

        public Dictionary<NetPeer, NetworkIdentiy> Players = new Dictionary<NetPeer, NetworkIdentiy>();
        public Dictionary<int, NetworkIdentiy> NetworkObjects = new Dictionary<int, NetworkIdentiy>();
        public Dictionary<NetPeer, List<NetworkIdentiy>> ObjectOwners = new Dictionary<NetPeer, List<NetworkIdentiy>>();

       public void ReadPlayerData(NetPeer peer, NetPacketReader reader)
       {
            if (Players.ContainsKey(peer))
            {
                NetworkIdentiy playerID = Players[peer];
                bool initialState = reader.GetBool();
                playerID.ReadData(reader, initialState);
            }
            
       }



        public void SpawnPlayer(GameObject playerPrefab, NetPeer peer, bool myPlayer)
        {
            if(Players.ContainsKey(peer))
            {
                //Player was already in scene. WTF?
                return;
            }
            
            GameObject obj = GameObject.Instantiate(playerPrefab);
            NetworkIdentiy networkIdentiy = obj.GetComponent<NetworkIdentiy>();
            networkIdentiy.Owner = myPlayer;
            networkIdentiy.IsPlayer = true;
            Players[peer] = networkIdentiy;
        }


        public void RemovePlayer(NetPeer peer)
        {
            if (Players.ContainsKey(peer))
            {
                NetworkIdentiy id = Players[peer];
                if (id != null)
                    id.gameObject.Destroy();
                Players.Remove(peer);
            }
        }


        public NetworkIdentiy FindNetworkObject(int netID)
        {
            if (NetworkObjects.ContainsKey(netID))
            {
                return NetworkObjects[netID];
            }
            return null;
        }

        public NetPeer FindOwnerOf(NetworkIdentiy networkIdentiy)
        {
            foreach(var peer in ObjectOwners)
            {
                if (peer.Value.Contains(networkIdentiy))
                    return peer.Key;
            }
            return null;
        }

    }
}
