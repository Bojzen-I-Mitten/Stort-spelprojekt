﻿using System;
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
    public Camera Camera { get; set; }
    //public Camera spectatorCamera { get; set; }


    private SoundComponent countdownSound;
    private SoundComponent endroundSound;

    public AudioClip countdownSoundClip { get; set; }
    public AudioClip endroundSoundClip { get; set; }

    public int MatchLength { get; set; } = 10 * 60; // Match time in seconds

    public float lostTime = 0.0f;
    public float ElapsedMatchTime
    {
        get
        {
            if (!MatchStarted)
                return 0.0f;

            return TimeSinceServerStarted - MatchStartTime - lostTime;
        }
    }

    public int MatchTimeLeft
    {
        get
        {
            return MatchLength - (int)ElapsedMatchTime;
        }
    }

    public PowerupManager PowerupManager;
    public static new MatchSystem instance
    {
        get { return NetworkManager.instance as MatchSystem; }
    }

    public bool hasScored { get; private set; } = false;

    public bool GoldenGoal = false;

    public float MatchStartTime;
    [Browsable(false)]
    public bool MatchStarted { get; private set; } = false;
    public MatchSystem() : base()
    {
        Teams = new Dictionary<TEAM_TYPE, Team>();

        Teams[TEAM_TYPE.TEAM_SPECTATOR] = new Team(TEAM_TYPE.TEAM_SPECTATOR, "Spectators", Color.Gray);

        Teams[TEAM_TYPE.TEAM_1] = new Team(TEAM_TYPE.TEAM_1, "Team 1", Color.Red);
        Teams[TEAM_TYPE.TEAM_2] = new Team(TEAM_TYPE.TEAM_2, "Team 2", Color.Blue);

        Teams[TEAM_TYPE.UNASSIGNED] = new Team(TEAM_TYPE.UNASSIGNED, "Unassigned", new Color(0, 0, 0, 0));
    }

  

    public override void Start()
    {
        Ball = GameObject.Instantiate(BallPrefab);
        base.Start();

        PowerupManager = gameObject.GetComponent<PowerupManager>();

       
        countdownSound = gameObject.AddComponent<SoundComponent>();
        countdownSound.clip = countdownSoundClip;
        countdownSound.Looping = false;
       
        endroundSound = gameObject.AddComponent<SoundComponent>();
        endroundSound.clip = endroundSoundClip;
        endroundSound.Looping = false;
        
        //StartCoroutine(ResetCoroutine(10));
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
                if (Input.GetKeyDown(Input.Keys.F10))
                    ShowOwnedObjects();
            }
        }

        if(MatchTimeLeft <= 0 && MatchStarted && !GoldenGoal)
        {
            OnMatchEnd();
        }

        //if (MatchStarted) //Sync time. (not done atm)
        //{
        //    if(MatchTimeLeft % 5 == 0)
        //        SendRPC(-2, "RPCMatchInfo", MatchStartTime, GoldenGoal);
        //}
    }

    private void ShowOwnedObjects()
    {
        Debug.Log("##################################");
        Debug.Log("Owned objects:");
        foreach(var objects in Scene.ObjectOwners[LocalPeer])
        {
            Debug.Log(objects.gameObject.Name);
        }
        Debug.Log("##################################");
    }

    #region Utility

    void ResetPlayers()
    {
        foreach (var team in Teams)
        {
            team.Value.ResetPlayers();
        }
    }
    #endregion

    #region Coroutines
    IEnumerator MatchEndCoroutine(Team winningTeam, float duration)
    {
        MatchStarted = false;
        ChadHud.Instance.OnMatchEnd(winningTeam, duration);
        yield return new WaitForSecondsRealtime(duration);
        GoldenGoal = false;
        RPCStartMatch();
    }

    IEnumerator RoundStartCountdown(float duration)
    {
        ResetPlayers();
        Ball.GetComponent<Ball>().Reset();
        PowerupManager.ResetPowerups();
        LocalChad.Locked = true;

        countdownSound.PlayOneShot();

        ChadHud.Instance.StartCountdown(duration);
        yield return new WaitForSecondsRealtime(duration);
        LocalChad.Locked = false;

        hasScored = false;
    }

    IEnumerator OnGoalCoroutine(Team teamThatScored)
    {
        endroundSound.PlayOneShot();
        ChadHud.Instance.OnGoal(teamThatScored, 7.0f);
        Time.TimeScale = 0.5f;

        Time.TimeScale = 0.5f;
        yield return new WaitForSecondsRealtime(7.0f);
        Time.TimeScale = 1.0f;
        OnRoundEnd();
        OnRoundStart();
    }
    #endregion

    #region RPC

    public void RPCMatchInfo(float startTime, bool goldenGoal, int powerupID, int team1Score, int team2Score)
    {
        Debug.Log("matchInfo!");
        if (!MatchStarted)
        {
            MatchStarted = true;
            PowerupManager.NextPowerupID = powerupID;
            GoldenGoal = goldenGoal;
            MatchStartTime = startTime;

            Teams[TEAM_TYPE.TEAM_1].Score = team1Score;
            Teams[TEAM_TYPE.TEAM_2].Score = team2Score;


            Debug.Log(startTime);
        }
    }

    public void RPCStartMatch()
    {
        MatchStarted = true;
        MatchStartTime = TimeSinceServerStarted;
        foreach (var team in Teams)
        {
            team.Value.ResetScore();
        }
        OnRoundStart();
    }
    public void RPCEndMatch()
    {
        if (Teams[TEAM_TYPE.TEAM_1].Score == Teams[TEAM_TYPE.TEAM_2].Score)
        {
            GoldenGoal = true;
            OnRoundEnd();
            OnRoundStart();
        }
        else
        {
            Team winningTeam = Teams[TEAM_TYPE.TEAM_1].Score > Teams[TEAM_TYPE.TEAM_2].Score ? Teams[TEAM_TYPE.TEAM_1] : Teams[TEAM_TYPE.TEAM_2];
            StartCoroutine(MatchEndCoroutine(winningTeam, 10.0f));
        }
    }

    public void RPCOnGoal(int teamType)
    {
        hasScored = true;
        TEAM_TYPE type = (TEAM_TYPE)teamType;
        Team team = FindTeam(type);
        team?.AddScore();
        if (GoldenGoal)
            OnMatchEnd();
        else
            StartCoroutine(OnGoalCoroutine(team));
    }

    #endregion

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
        SendRPC(-2, "RPCOnGoal", (int)teamThatScored);
        RPCOnGoal((int)teamThatScored);
    }

    public void JoinTeam(TEAM_TYPE team)
    {
        NetworkPlayer np = Scene.Players[LocalPeer].gameObject.GetComponent<NetworkPlayer>();
        np.JoinTeam(team);
    }

    #endregion

    #region peer connection

    protected override void OnPeerJoin(NetPeer peer)
    {
        //Disable the players gameObject and place him in team Spectator.
        //Give him a NetworkPlayer object.
        Debug.Log("peer joined!");
        //NetworkPlayer np = Scene.Players[peer].gameObject.GetComponent<NetworkPlayer>();
        
        //np.JoinTeam(Teams[TEAM_TYPE.UNASSIGNED]);

        if (peer != LocalPeer)
        {
            Debug.Log(MatchStarted);
            if (MatchStarted)
                SendRPC(peer, -2, "RPCMatchInfo", MatchStartTime, GoldenGoal, PowerupManager.NextPowerupID, Teams[TEAM_TYPE.TEAM_1].Score, Teams[TEAM_TYPE.TEAM_2].Score);
        }
    }

    protected override void OnPeerLeave(NetPeer peer)
    {
        NetworkPlayer np = Scene.Players[peer].gameObject.GetComponent<NetworkPlayer>();
        if (!np)
            Debug.LogError("Failed to find network player for peer:" + peer);
        else
        {
            np.JoinTeam(null);
            np.gameObject.GetComponent<ChadControls>().OnDisconnect();
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

    public TEAM_TYPE GetPlayerTeam(GameObject player)
    {
        NetworkPlayer localPlayer = player.GetComponent<NetworkPlayer>();
        return localPlayer.Team.TeamType;
    }
    #endregion


}
