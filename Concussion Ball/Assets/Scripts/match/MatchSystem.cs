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

    public GameObject BallPrefab { get; set; }
    public GameObject Ball;

    public ChadControls LocalChad;

    public int MatchLength = 10 * 60; // Match time in seconds

    public float ElapsedMatchTime
    {
        get
        {
            if (!MatchStarted)
                return 0.0f;

            return TimeSinceServerStarted - MatchStartTime;
        }
    }

    public int MatchTimeLeft
    {
        get
        {
            return MatchLength - (int)ElapsedMatchTime;
        }
    }

    public static new MatchSystem instance
    {
        get { return NetworkManager.instance as MatchSystem; }
    }

    public bool hasScored { get; private set; } = false;

    private float MatchStartTime;
    private bool MatchStarted = false;
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
                if (Input.GetKeyDown(Input.Keys.Enter))
                {
                    OnMatchStart();
                }
                if(Input.GetKeyDown(Input.Keys.R))
                {
                    SendRPC(-2, "OnRoundStart");
                    OnRoundStart();
                }
            }
        }

        if(MatchTimeLeft <= 0 && MatchStarted)
        {
            OnMatchEnd();
        }
    }


    #region Utility
    void SpawnBall()
    {
        if (BallPrefab)
        {
            Ball = Scene.FindNetworkObject(8008)?.gameObject;
            if (!Ball)
            {
                Ball = NetworkInstantiate(BallPrefab, Vector3.Zero, Quaternion.Identity, ResponsiblePeer == LocalPeer, 8008);
            }
            Ball.SetActive(false);
        }
    }

    void ResetPlayers()
    {
        foreach (var team in Teams)
        {
            team.Value.ResetPlayers();
        }
    }
    #endregion


    #region Coroutines
    #endregion

    #region RPC

    public void RPCMatchInfo(float startTime)
    {
        Debug.Log("matchInfo!");
        if (!MatchStarted)
        {
            MatchStarted = true;
            MatchStartTime = startTime;
            Debug.Log(startTime);
        }
    }

    public void RPCStartMatch()
    {
        MatchStarted = true;
        MatchStartTime = TimeSinceServerStarted;
    }
    public void RPCEndMatch()
    {
        if (Teams[TEAM_TYPE.TEAM_1].Score == Teams[TEAM_TYPE.TEAM_2].Score)
        {
            //Golden goal
        }else
        {
            Team winningTeam = Teams[TEAM_TYPE.TEAM_1].Score > Teams[TEAM_TYPE.TEAM_2].Score ? Teams[TEAM_TYPE.TEAM_1] : Teams[TEAM_TYPE.TEAM_2];
            ChadHud.Instance.OnMatchEnd(winningTeam, 10.0f);
            MatchStarted = false;
        }
    }

    public void RPCOnGoal(int teamType)
    {
        TEAM_TYPE type = (TEAM_TYPE)teamType;
        Team team = FindTeam(type);
        team?.AddScore();
        StartCoroutine(OnGoalCoroutine(team));
    }

    #endregion
    IEnumerator RoundStartCountdown(float duration)
    {
        ResetPlayers();
        Ball.GetComponent<Ball>().Reset();

        LocalChad.Locked = true;
        ChadHud.Instance.StartCountdown(duration);
        yield return new WaitForSecondsRealtime(duration);
        LocalChad.Locked = false;

    }

    IEnumerator OnGoalCoroutine(Team teamThatScored)
    {
        ChadHud.Instance.OnGoal(teamThatScored, 7.0f);
        Time.TimeScale = 0.5f;

        Time.TimeScale = 0.5f;
        yield return new WaitForSecondsRealtime(7.0f);
        Time.TimeScale = 1.0f;
        OnRoundEnd();
        OnRoundStart();
    }

    #region Match functions

    void OnMatchEnd()
    {
        if (MatchStarted)
        {
            RPCEndMatch();
            SendRPC(-2, "RPCEndMatch");
        }
        //Clean up
        //MatchStarted = false;
    }


    void OnMatchStart()
    {
        if (MatchStarted)
            return;
        RPCStartMatch();
        SendRPC(-2, "RPCStartMatch");

        //OnRoundStart();
        //SendRPC(-2, "OnRoundStart");
    }


    public void OnRoundStart()
    {
        if (!MatchStarted)
            return;

        StartCoroutine(RoundStartCountdown(3.0f));
    }

    void OnRoundEnd()
    {
        hasScored = false;
    }

    public void OnGoal(TEAM_TYPE teamThatScored)
    {
        if (hasScored)
            return;
        hasScored = true;
        SendRPC(-2, "RPCOnGoal", (int)teamThatScored);
        RPCOnGoal((int)teamThatScored);
    }

    #endregion

    #region peer connection

    protected override void OnPeerJoin(NetPeer peer)
    {
        //Disable the players gameObject and place in him team Spectator.
        //Give him a NetworkPlayer object.
        Debug.Log("peer joined!");
        NetworkPlayer np = Scene.Players[peer].gameObject.AddComponent<NetworkPlayer>();

        int team = Scene.Players.Count % 2;
        if (team == 0)
            np.JoinTeam(Teams[TEAM_TYPE.TEAM_1]);
        else
            np.JoinTeam(Teams[TEAM_TYPE.TEAM_2]);

        SpawnBall();

        if (peer != LocalPeer)
        {
            Debug.Log(MatchStarted);
            if (MatchStarted)
                SendRPC(peer, -2, "RPCMatchInfo", MatchStartTime);
        }
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
    #endregion

    #region Team Manager

    public Team FindTeam(TEAM_TYPE type)
    {
        
        Team team = null;
        Teams.TryGetValue(type, out team);
        return team;
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
