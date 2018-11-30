using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ThomasEngine;
using ThomasEngine.Network;
using LiteNetLib;
using System.ComponentModel;

[Newtonsoft.Json.JsonObject]
[TypeConverter(typeof(ExpandableObjectConverter))]
public class Team
{
    public int Score { get; set; }

    public List<Material> materials { get; set; } = new List<Material>();
    public TEAM_TYPE TeamType;

    private Color _Color;

    public Color Color {get
        {
            return _Color;
        }
        set
        {
            _Color = value;
            materials.ForEach((mat) => mat.SetColor("color", _Color));
        }
    }
    public string Name { get; set; }
    [Browsable(false)]
    public int PlayerCount { get { return Players.Count; } }
    [Browsable(false)]
    public TeamSpawn SpawnArea { get; private set; }
    [Browsable(false)]
    public List<NetworkPlayer> Players { get; private set; }
    public Vector3 GoalPosition;
   
    //public int Score { get { return _Score; } }

    public Team(TEAM_TYPE type, string name, Color teamColor)
    {
        Color = teamColor;
        TeamType = type;
        Score = 0;
        Name = name;
        Players = new List<NetworkPlayer>(50);
    }

    public void SetSpawnArea(TeamSpawn spawn) { SpawnArea = spawn; }
    //public void SetGoalArea(TeamGoal goal) { _GoalArea = goal; }

    public void ResetScore()
    {
        Score = 0;
    }
    public void AddScore()
    {
        Score++;
        Debug.Log(TeamType.ToString() + " Scored!");
    }

    private void ResetPlayer(NetworkPlayer player)
    {
        switch (TeamType)
        {
            case TEAM_TYPE.UNASSIGNED:
            case TEAM_TYPE.TEAM_SPECTATOR:
                player.gameObject.SetActive(false);
                if (player.isOwner)
                {
                    //MatchSystem.instance.LocalChad.DeactivateCamera();
                    //MatchSystem.instance.spectatorCamera.enabled = true;
                }
                break;
            case TEAM_TYPE.TEAM_1:
            case TEAM_TYPE.TEAM_2:
                player.gameObject.SetActive(true);
                if (player.isOwner)
                {
                    //MatchSystem.instance.spectatorCamera.enabled = false;
                    //MatchSystem.instance.LocalChad.ActivateCamera();
                }

                break;
        }
        player.Reset();
    }

    public void ResetPlayers()
    {
        Players.ForEach((System.Action<NetworkPlayer>)((player) => 
        {
            ResetPlayer(player);
        }));
        
    }

    public Vector3 GetSpawnPosition()
    {
        Vector3 spawnPoint = Vector3.Zero;
        if (SpawnArea)
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
        Players.Add(player);
        player.Team = this;
        Debug.Log(player.Name + " Joined team " + Name);
        if(MatchSystem.instance.MatchStarted)
            ResetPlayer(player);
    }

    
    public void RemovePlayer(NetworkPlayer player)
    {
        if (player.Team != this)
        {
            Debug.LogWarning("Player is not in team " + Name);
            return;
        }
        Players.Remove(player);
        player.Team = null;
        Debug.Log(player.Name + " left team " + Name);
    }
    
}
