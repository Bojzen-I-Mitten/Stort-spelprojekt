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

        public  Dictionary<NetPeer, NetworkID> players = new Dictionary<NetPeer, NetworkID>();
        //Dictionary<int, NetworkID> localObjects = new Dictionary<int, NetworkID>();
        
       public void ReadPlayerData(NetPeer peer, NetPacketReader reader)
       {
            if (players.ContainsKey(peer))
            {
                NetworkID playerID = players[peer];
                bool initialState = reader.GetBool();
                playerID.ReadData(reader, initialState);
            }
            
       }



        public void SpawnPlayer(GameObject playerPrefab, NetPeer peer, bool myPlayer)
        {
            if(players.ContainsKey(peer))
            {
                //Player was already in scene. WTF?
                return;
            }
            
            GameObject obj = GameObject.Instantiate(playerPrefab);
            NetworkID netID = obj.GetComponent<NetworkID>();
            netID.Owner = myPlayer;
            netID.IsPlayer = true;
            players[peer] = netID;
        }


        public void RemovePlayer(NetPeer peer)
        {
            if (players.ContainsKey(peer))
            {
                NetworkID id = players[peer];
                if (id != null)
                    id.gameObject.Destroy();
                players.Remove(peer);
                //Remove him from everyone :>
            }
        }

    }
}
