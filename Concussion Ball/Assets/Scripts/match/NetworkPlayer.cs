using System;
using ThomasEngine;
using ThomasEngine.Network;
using LiteNetLib;
using LiteNetLib.Utils;
public class NetworkPlayer : NetworkComponent
{
    private Team _Team;
    private String _Name;
    
    public Team Team
    {
        get { return _Team; }
    }
    Material mat;
    public override void Start()
    {
        mat = gameObject.GetComponent<RenderComponent>()?.material;
        
    }

    public override bool OnWrite(NetDataWriter writer, bool initialState)
    {
        if (Team != null)
            writer.Put((int)Team.TeamType);
        else
            writer.Put((int)TEAM_TYPE.UNASSIGNED);

        return true;
    }

    public override void OnRead(NetPacketReader reader, bool initialState)
    {
        TEAM_TYPE teamType = (TEAM_TYPE)reader.GetInt();

        Team newTeam = MatchSystem.instance.FindTeam(teamType);
        if(Team != newTeam)
            JoinTeam(newTeam);
    }



    public void JoinTeam(TEAM_TYPE teamType)
    {
        Team newTeam = MatchSystem.instance.FindTeam(teamType);
        JoinTeam(newTeam);
    }

    public void JoinTeam(Team team)
    {
        if (this._Team != null)
            this._Team.RemovePlayer(this);
        if (team != null)
        {
            team.AddPlayer(this);
            mat?.SetColor("color", team.Color);
        }
            
        this._Team = team;


    }
}
