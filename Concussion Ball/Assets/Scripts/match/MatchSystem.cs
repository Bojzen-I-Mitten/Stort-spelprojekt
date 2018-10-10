using System;
using System.Collections.Generic;
using ThomasEngine;
using ThomasEngine.Network;
using LiteNetLib;
using System.ComponentModel;

public enum TEAM_TYPE
{
    UNASSIGNED,
    TEAM_SPECTATOR,
    TEAM_1,
    TEAM_2
}

public class MatchSystem : NetworkManager
{

    [Category("test")]
    public int Test { get; set; }

    private Dictionary<TEAM_TYPE, Team> Teams;

    public static new MatchSystem instance
    {
        get { return NetworkManager.instance as MatchSystem; }
    }

    public override void Start()
    {
        base.Start();
        Teams = new Dictionary<TEAM_TYPE, Team>();

        Teams[TEAM_TYPE.TEAM_SPECTATOR] = new Team(TEAM_TYPE.TEAM_SPECTATOR, "Spectators", null);

        Teams[TEAM_TYPE.TEAM_1] = new Team(TEAM_TYPE.TEAM_1, "Team 1", null);
        Teams[TEAM_TYPE.TEAM_2] = new Team(TEAM_TYPE.TEAM_2, "Team 2", null);
        
    }

    void OnMatchStart()
    {
        //Spawn ball or something
    }

    void OnMatchEnd()
    {
        //Clean up
    }


    void OnRoundStart()
    {
        foreach(var team in Teams)
        {
            switch(team.Key)
            {
                case TEAM_TYPE.UNASSIGNED:
                case TEAM_TYPE.TEAM_SPECTATOR:
                    team.Value.Players.ForEach((player) =>
                    {
                        player.gameObject.SetActive(false);
                    });
                    break;
                case TEAM_TYPE.TEAM_1:
                case TEAM_TYPE.TEAM_2:
                    team.Value.Players.ForEach((player) =>
                    {
                        player.gameObject.SetActive(true);
                    });
                    break;
            }
        }
    }

    void OnRoundEnd()
    {

    }

    public override void Update()
    {
        base.Update();
        OnRoundStart();
        if (Scene.Players.ContainsKey(LocalPeer))
        {
            NetworkPlayer localPlayer = Scene.Players[LocalPeer].gameObject.GetComponent<NetworkPlayer>();
            if (Input.GetKeyDown(Input.Keys.D1))
                localPlayer.JoinTeam(TEAM_TYPE.TEAM_1);
            if (Input.GetKeyDown(Input.Keys.D2))
                localPlayer.JoinTeam(TEAM_TYPE.TEAM_2);
            if (Input.GetKeyDown(Input.Keys.S))
                localPlayer.JoinTeam(TEAM_TYPE.TEAM_SPECTATOR);
        }

    }

    #region Team Manager

    public Team FindTeam(TEAM_TYPE type)
    {
        Team team = null;
        Teams.TryGetValue(type, out team);
        return team;
    }

    protected override void OnPeerJoin(NetPeer peer)
    {
        //Disable the players gameObject and place in him team Spectator.
        //Give him a NetworkPlayer object.
        Debug.Log("peer joined!");
        NetworkPlayer np = Scene.Players[peer].gameObject.AddComponent<NetworkPlayer>();
        np.JoinTeam(Teams[TEAM_TYPE.TEAM_SPECTATOR]);

        Scene.Players[peer].gameObject.SetActive(false);
        
    }

    protected override void OnPeerLeave(NetPeer peer)
    {
        NetworkPlayer np = Scene.Players[peer].gameObject.GetComponent<NetworkPlayer>();
        if (!np)
            Debug.LogError("Failed to find network player for peer:" + peer);
        else
        {
            np.JoinTeam(null);
        }
    }

    private void AutoJoin(NetworkPlayer player)
    {
        //if (Team1.PlayerCount >= Team2.PlayerCount)
        //    Team1.AddPlayer(player);
        //else
        //    Team2.AddPlayer(player);
    }
    #endregion


}
