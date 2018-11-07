using System;
using ThomasEngine;
using ThomasEngine.Network;
using LiteNetLib;
using LiteNetLib.Utils;
using System.Collections;

public class NetworkPlayer : NetworkComponent
{
    private String _Name;
    [Newtonsoft.Json.JsonIgnore]
    public Team Team {get; private set;}
    public float BottomOfTheWorld { get; set; } = -5;
    Material mat;


    public override void Awake()
    {
        Team = MatchSystem.instance.FindTeam(TEAM_TYPE.UNASSIGNED);
    }

    public override void Start()
    {
        if (Team == null || Team.TeamType == TEAM_TYPE.TEAM_SPECTATOR || Team.TeamType == TEAM_TYPE.UNASSIGNED)
            gameObject.SetActive(false);
        mat = (gameObject.GetComponent<RenderSkinnedComponent>().material = new Material(gameObject.GetComponent<RenderSkinnedComponent>().material));
        

    }

    public override void Update()
    {
        if (transform.position.y < BottomOfTheWorld)
            Reset();
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
        if (Team != newTeam)
            JoinTeam(newTeam);
        if (initialState)
        {
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
        if (isOwner && (int)Team.TeamType > (int)TEAM_TYPE.TEAM_SPECTATOR)
        {
            gameObject.GetComponent<Rigidbody>().enabled = false;
            StartCoroutine(EnableRigidbody());
            transform.position = Team.GetSpawnPosition();
            transform.LookAt(new Vector3(0, transform.position.y, 0));
            gameObject.GetComponent<ChadControls>().Reset();
        }
    }

    IEnumerator EnableRigidbody()
    {
        yield return null;
        gameObject.GetComponent<Rigidbody>().enabled = true;
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
}
