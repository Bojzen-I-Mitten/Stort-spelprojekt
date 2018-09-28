﻿using System;
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
            NetworkIdentity networkIdentiy = obj.GetComponent<NetworkIdentity>();
            networkIdentiy.Owner = myPlayer;
            networkIdentiy.IsPlayer = true;
            Players[peer] = networkIdentiy;
        }


        public void RemovePlayer(NetPeer peer)
        {
            if (Players.ContainsKey(peer))
            {
                NetworkIdentity id = Players[peer];
                if (id != null)
                    id.gameObject.Destroy();
                Players.Remove(peer);
            }
        }

        public void ActivateAndAssignSceneObjects()
        {
            Object.GetObjectsOfType<NetworkIdentity>().ForEach((identity) =>
            {
                if(!identity.gameObject.GetActive())
                {
                    NetworkObjects.Add(nextAssignableID++, identity);
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
            foreach(var peer in ObjectOwners)
            {
                if (peer.Value.Contains(networkIdentiy))
                    return peer.Key;
            }
            return null;
        }

    }
}
