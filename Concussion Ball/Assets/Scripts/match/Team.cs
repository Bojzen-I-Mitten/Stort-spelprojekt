using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ThomasEngine;
using ThomasEngine.Network;
using LiteNetLib;
using System.ComponentModel;

[TypeConverter(typeof(ExpandableObjectConverter))]
public class Team
{
    private List<NetworkPlayer> _Players;
    private BoxCollider _SpawnArea;
    private int _Score;

    public TEAM_TYPE TeamType;
    public Color Color { get; set; }
    public string Name { get; set; }
    [Browsable(false)]
    public int PlayerCount { get { return _Players.Count; } }
    [Browsable(false)]
    public BoxCollider SpawnArea { get { return _SpawnArea; } }
    [Browsable(false)]
    public List<NetworkPlayer> Players { get { return _Players; } }
   
    public Team(TEAM_TYPE type, string name, Color teamColor)
    {
        Color = teamColor;
        TeamType = type;
        _Score = 0;
        Name = name;
        _Players = new List<NetworkPlayer>(4);
    }

    public void Start()
    {
        GameObject teamObject = GameObject.Find(TeamType.ToString());
        _SpawnArea = teamObject?.GetComponent<BoxCollider>();
    }


    public Vector3 GetSpawnPosition()
    {
        Vector3 spawnPoint = Vector3.Zero;
        if (_SpawnArea)
        {
            spawnPoint = SpawnArea.transform.position + SpawnArea.center;
            //Random x, z point inside the box
            spawnPoint.x += Random.Range(-SpawnArea.size.x, SpawnArea.size.x);
            spawnPoint.z += Random.Range(-SpawnArea.size.z, SpawnArea.size.z);
        }
        return spawnPoint;
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
