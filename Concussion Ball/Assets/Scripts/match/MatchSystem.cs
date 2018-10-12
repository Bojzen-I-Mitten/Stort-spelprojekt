using System;
using System.Collections.Generic;
using ThomasEngine;
using ThomasEngine.Network;
using LiteNetLib;
using System.ComponentModel;
using System.Collections;

public enum TEAM_TYPE
{
    UNASSIGNED,
    TEAM_SPECTATOR,
    TEAM_1,
    TEAM_2
}

public class MatchSystem : NetworkManager
{
    public Dictionary<TEAM_TYPE, Team> Teams { get; set; }

    public Team Spectator { get; set; }

    public GameObject BallPrefab;
    private GameObject Ball;

    public static new MatchSystem instance
    {
        get { return NetworkManager.instance as MatchSystem; }
    }

    MatchSystem() : base()
    {
        Teams = new Dictionary<TEAM_TYPE, Team>();

        Teams[TEAM_TYPE.TEAM_SPECTATOR] = new Team(TEAM_TYPE.TEAM_SPECTATOR, "Spectators", Color.White);

        Teams[TEAM_TYPE.TEAM_1] = new Team(TEAM_TYPE.TEAM_1, "Team 1", Color.Red);
        Teams[TEAM_TYPE.TEAM_2] = new Team(TEAM_TYPE.TEAM_2, "Team 2", Color.Blue);
    }

    public override void Start()
    {
        base.Start();
        foreach (var team in Teams)
            team.Value.Start();


        StartCoroutine(ResetCoroutine(10));
    }

    IEnumerator ResetCoroutine(int seconds)
    {
        int secondsRemaining = seconds;
        while(secondsRemaining > 0)
        {
            yield return new WaitForSecondsRealtime(1.0f);
            secondsRemaining -= 1;
            Debug.Log(secondsRemaining);
        }
        OnRoundStart();
    }

    void OnMatchStart()
    {
        //Spawn ball or something

        OnRoundStart();
    }

    void OnMatchEnd()
    {
        //Clean up
    }


    void OnRoundStart()
    {
        ResetPlayers();
        ResetBall();
    }

    void ResetBall()
    {
        Ball.SetActive(true);
        Ball.transform.position = new Vector3(0, 10, 0);
    }

    void ResetPlayers()
    {
        foreach (var team in Teams)
        {
            switch (team.Key)
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
                        player.gameObject.transform.position = team.Value.GetSpawnPosition();
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
