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

        public  Dictionary<NetPeer, NetworkID> Players = new Dictionary<NetPeer, NetworkID>();
        //Dictionary<int, NetworkID> localObjects = new Dictionary<int, NetworkID>();
        
       public void ReadPlayerData(NetPeer peer, NetPacketReader reader)
       {
            if (Players.ContainsKey(peer))
            {
                NetworkID playerID = Players[peer];
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
            NetworkID netID = obj.GetComponent<NetworkID>();
            netID.Owner = myPlayer;
            netID.IsPlayer = true;
            Players[peer] = netID;
        }


        public void RemovePlayer(NetPeer peer)
        {
            if (Players.ContainsKey(peer))
            {
                NetworkID id = Players[peer];
                if (id != null)
                    id.gameObject.Destroy();
                Players.Remove(peer);
            }
        }

    }
}
