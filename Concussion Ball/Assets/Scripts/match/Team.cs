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
    private TeamSpawn _SpawnArea;
    private int _Score;

    public TEAM_TYPE TeamType;
    public Color Color { get; set; }
    public string Name { get; set; }
    [Browsable(false)]
    public int PlayerCount { get { return _Players.Count; } }
    [Browsable(false)]
    public TeamSpawn SpawnArea { get { return _SpawnArea; } }
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

    public void SetSpawnArea(TeamSpawn spawn) { _SpawnArea = spawn; }
    //public void SetGoalArea(TeamGoal goal) { _GoalArea = goal; }

    public void ResetScore()
    {
        _Score = 0;
    }
    public void AddScore()
    {
        _Score++;
        Debug.Log(TeamType.ToString() + " Scored!");
    }

    public void ResetPlayers()
    {
        Players.ForEach((player) => 
        {
            switch (TeamType)
            {
                case TEAM_TYPE.UNASSIGNED:
                case TEAM_TYPE.TEAM_SPECTATOR:
                    player.gameObject.SetActive(false);
                    break;
                case TEAM_TYPE.TEAM_1:
                case TEAM_TYPE.TEAM_2:
                    player.gameObject.SetActive(true);
                    player.Respawn();
                    break;
            }
            player.Reset();
        });
        
    }

    public Vector3 GetSpawnPosition()
    {
        Vector3 spawnPoint = Vector3.Zero;
        if (_SpawnArea)
        {
            spawnPoint = SpawnArea.transform.position;
            //Random x, z point inside the box
            spawnPoint.x += Random.Range(-SpawnArea.transform.scale.x, SpawnArea.transform.scale.x);
            spawnPoint.z += Random.Range(-SpawnArea.transform.scale.z, SpawnArea.transform.scale.z);
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
