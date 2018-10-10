using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ThomasEngine;
using ThomasEngine.Network;
using LiteNetLib;
public class Team
{
    private string _Name;
    private List<NetworkPlayer> _Players;
    private BoxCollider _SpawnArea;
    private int _Score;

    public readonly TEAM_TYPE TeamType;

    public string Name { get { return _Name; } }
    public int PlayerCount { get { return _Players.Count; } }

    public List<NetworkPlayer> Players { get { return _Players; } }
   
    public Team(TEAM_TYPE type, string name, BoxCollider spawnArea)
    {
        TeamType = type;
        _Score = 0;
        _Name = name;
        _Players = new List<NetworkPlayer>(4);
        _SpawnArea = spawnArea;
    }

    public void AddPlayer(NetworkPlayer player)
    {
        if (player.Team == this)
        {
            Debug.LogWarning("Player is already in team " + Name);
            return;
        }
        _Players.Add(player);
        Debug.Log(player.Name + " Joined team " + Name);
    }

    
    public void RemovePlayer(NetworkPlayer player)
    {
        if (player.Team != this)
        {
            Debug.LogWarning("Player is not in team " + Name);
            return;
        }
        _Players.Remove(player);
        Debug.Log(player.Name + " left team " + Name);
    }
    
}