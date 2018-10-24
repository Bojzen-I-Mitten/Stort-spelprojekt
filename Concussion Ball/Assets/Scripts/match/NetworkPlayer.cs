using System;
using ThomasEngine;
using ThomasEngine.Network;
using LiteNetLib;
using LiteNetLib.Utils;

public class NetworkPlayer : NetworkComponent
{
    private String _Name;
    public Team Team {get; private set;}
    public float BottomOfTheWorld { get; set; } = -5;
    Material mat;
    public override void Start()
    {
        mat = (gameObject.GetComponent<RenderSkinnedComponent>().material = new Material(gameObject.GetComponent<RenderSkinnedComponent>().material));
        mat?.SetColor("color", Team.Color);

        gameObject.SetActive(false);
    }

    public override void Update()
    {
        if (transform.position.y < BottomOfTheWorld)
            Respawn();
    }

    public override bool OnWrite(NetDataWriter writer, bool initialState)
    {
        if(initialState)
        {
            if (Team != null)
                writer.Put((int)Team.TeamType);
            else
                writer.Put((int)TEAM_TYPE.UNASSIGNED);
        }


        return true;
    }

    public override void OnRead(NetPacketReader reader, bool initialState)
    {
        if (initialState)
        {
            TEAM_TYPE teamType = (TEAM_TYPE)reader.GetInt();
            Team newTeam = MatchSystem.instance.FindTeam(teamType);
            if (Team != newTeam)
                JoinTeam(newTeam);

            if (teamType == TEAM_TYPE.TEAM_1 || teamType == TEAM_TYPE.TEAM_2)
                gameObject.SetActive(true);
        }

    }
    
    public void JoinTeam(TEAM_TYPE teamType)
    {
        RPCJoinTeam((int)teamType);
        SendRPC("RPCJoinTeam", (int)teamType);
    }

    public void RPCJoinTeam(int teamType)
    {
        Team newTeam = MatchSystem.instance.FindTeam((TEAM_TYPE)teamType);
        JoinTeam(newTeam);
    }

    public void Reset()
    {
        mat?.SetColor("color", Team.Color);
    }


    public void JoinTeam(Team team)
    {
        if (this.Team != null)
            this.Team.RemovePlayer(this);
        if (team != null)
        {
            team.AddPlayer(this);
        }
            
        this.Team = team;


    }

    public void Respawn()
    {
        if (isOwner)
            transform.position = Team.GetSpawnPosition();
    }
}
