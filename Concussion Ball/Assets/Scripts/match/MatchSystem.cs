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
    [Newtonsoft.Json.JsonIgnore]
    public Dictionary<TEAM_TYPE, Team> Teams { get; set; }

    public Team Spectator { get; set; }

    public GameObject BallPrefab { get; set; }
    public GameObject Ball;

    private bool MatchStarted = false;

    public static new MatchSystem instance
    {
        get { return NetworkManager.instance as MatchSystem; }
    }

    bool hasScored = false;

    public MatchSystem() : base()
    {
        Teams = new Dictionary<TEAM_TYPE, Team>();

        Teams[TEAM_TYPE.TEAM_SPECTATOR] = new Team(TEAM_TYPE.TEAM_SPECTATOR, "Spectators", Color.White);

        Teams[TEAM_TYPE.TEAM_1] = new Team(TEAM_TYPE.TEAM_1, "Team 1", Color.Red);
        Teams[TEAM_TYPE.TEAM_2] = new Team(TEAM_TYPE.TEAM_2, "Team 2", Color.Blue);
    }

    public override void Start()
    {
        base.Start();
        if(BallPrefab)
            SpawnablePrefabs.Add(BallPrefab);

        //StartCoroutine(ResetCoroutine(10));
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
        OnMatchStart();
    }

    void OnMatchStart()
    {
        if (MatchStarted)
            return;
        //Spawn ball or something
        if (BallPrefab)
        {
            Debug.Log("spawned ball!");
            Ball = Scene.FindNetworkObject(8008)?.gameObject;
            if(!Ball)
                Ball = NetworkInstantiate(BallPrefab, Vector3.Zero, Quaternion.Identity, ResponsiblePeer == LocalPeer, 8008);
            Ball.SetActive(false);
        }
        MatchStarted = true;
    }

    void OnMatchEnd()
    {
        //Clean up
        //MatchStarted = false;
    }


    public void OnRoundStart()
    {
        ResetPlayers();
        ResetBall();
        hasScored = false;
    }

    void ResetBall()
    {
        Ball.GetComponent<Ball>().RPCDrop();
        Ball.SetActive(false);
        Ball.SetActive(true);
        if(Ball.GetComponent<NetworkIdentity>().Owner)
            Ball.transform.position = new Vector3(0, 10, 0);
    }

    void ResetPlayers()
    {
        foreach (var team in Teams)
        {
            team.Value.ResetPlayers();
            
        }
    }

    void OnRoundEnd()
    {

    }

    public void RPCAddScore(int teamType)
    {
        TEAM_TYPE type = (TEAM_TYPE)teamType;
        FindTeam(type)?.AddScore();
        OnRoundStart();
    }

    public void OnGoal(TEAM_TYPE teamThatScored)
    {
        if (hasScored)
            return;
        hasScored = true;
        SendRPC(-2, "RPCAddScore", (int)teamThatScored);
        RPCAddScore((int)teamThatScored);
    }

    public override void Update()
    {
        base.Update();
        if (InternalManager.IsRunning)
        {
            if (Scene.Players.ContainsKey(LocalPeer))
            {
                NetworkPlayer localPlayer = Scene.Players[LocalPeer].gameObject.GetComponent<NetworkPlayer>();
                if (Input.GetKeyDown(Input.Keys.D1))
                    localPlayer.JoinTeam(TEAM_TYPE.TEAM_1);
                if (Input.GetKeyDown(Input.Keys.D2))
                    localPlayer.JoinTeam(TEAM_TYPE.TEAM_2);
                if (Input.GetKeyDown(Input.Keys.D3))
                    localPlayer.JoinTeam(TEAM_TYPE.TEAM_SPECTATOR);
                if (Input.GetKeyDown(Input.Keys.Space))
                {
                    SendRPC(-2, "OnRoundStart");
                    OnRoundStart();
                }
            }
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
        
        int team = Scene.Players.Count % 2;
        if(team == 0)
            np.JoinTeam(Teams[TEAM_TYPE.TEAM_1]);
        else
            np.JoinTeam(Teams[TEAM_TYPE.TEAM_2]);

        Scene.Players[peer].gameObject.SetActive(false);
        OnMatchStart();
        SendRPC(-2, "OnRoundStart");
        OnRoundStart();
    }

    protected override void OnPeerLeave(NetPeer peer)
    {
        return;
        NetworkPlayer np = Scene.Players[peer].gameObject.GetComponent<NetworkPlayer>();
        if (!np)
            Debug.LogError("Failed to find network player for peer:" + peer);
        else
        {
            np.JoinTeam(null);
        }
    }

    public TEAM_TYPE GetOpposingTeam(TEAM_TYPE team)
    {
        switch (team)
        {
            case TEAM_TYPE.TEAM_1:
                return TEAM_TYPE.TEAM_2;
            case TEAM_TYPE.TEAM_2:
                return TEAM_TYPE.TEAM_1;
            case TEAM_TYPE.TEAM_SPECTATOR:
                return TEAM_TYPE.TEAM_SPECTATOR;
            default:
                return TEAM_TYPE.UNASSIGNED;
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
