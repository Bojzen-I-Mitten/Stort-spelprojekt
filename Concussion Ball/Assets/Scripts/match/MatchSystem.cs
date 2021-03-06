﻿//#define T_FOR_RESET
using System;
using System.Collections.Generic;
using ThomasEngine;
using ThomasEngine.Network;
using LiteNetLib;
using System.ComponentModel;
using System.Collections;
using LiteNetLib.Utils;

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

    public WaitForSecondsRealtime PlayAgain;//toggle how long
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

    public string ServerName = "Pelles server";
    public bool PublicServer = true;
    public bool SpawnPowerupsDuringGame = true;
    public int ScoreLimit = 0;
    //public int MaxPlayers = 8;


    private bool timePaused = false;

    public float ElapsedMatchTime = 0.0f;
    public int MatchTimeLeft
    {
        get
        {
            return MatchLength - (int)ElapsedMatchTime;
        }
    }

    public ReplaySystem ReplaySystem;
    public PowerupManager PowerupManager;
    public static new MatchSystem instance
    {
        get { return NetworkManager.instance as MatchSystem; }
    }

    public bool hasScored { get; private set; } = false;

    public bool GoldenGoal = false;

    [Browsable(false)]
    public bool MatchStarted { get; private set; } = false;

    [Browsable(false)]
    [Newtonsoft.Json.JsonIgnore]
    public bool Connected { get { return InternalManager.ConnectedPeerList.Count > 0; } }

    public MatchSystem() : base()
    {
        Teams = new Dictionary<TEAM_TYPE, Team>();

        Teams[TEAM_TYPE.TEAM_SPECTATOR] = new Team(TEAM_TYPE.TEAM_SPECTATOR, "Spectators", Color.Gray);

        Teams[TEAM_TYPE.TEAM_1] = new Team(TEAM_TYPE.TEAM_1, "Team 1", Color.Red);
        Teams[TEAM_TYPE.TEAM_2] = new Team(TEAM_TYPE.TEAM_2, "Team 2", Color.Blue);

        Teams[TEAM_TYPE.UNASSIGNED] = new Team(TEAM_TYPE.UNASSIGNED, "Unassigned", new Color(0, 0, 0, 0));
    }
    public override void OnAwake()
    {
        base.OnAwake();
        Ball = GameObject.Instantiate(BallPrefab);
    }

    public override void Start()
    {
        base.Start();
        //Ball.SetActive(true);
        PowerupManager = gameObject.GetComponent<PowerupManager>();

        ReplaySystem = gameObject.GetComponent<ReplaySystem>();

        countdownSound = gameObject.AddComponent<SoundComponent>();
        countdownSound.Clip = countdownSoundClip;
        countdownSound.Looping = false;
        countdownSound.Volume = 0.2f;

        endroundSound = gameObject.AddComponent<SoundComponent>();
        endroundSound.Clip = endroundSoundClip;
        endroundSound.Looping = false;
        endroundSound.Is3D = false;
        endroundSound.Volume = 0.2f;


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
                if (MatchStarted)
                {
                    //if (Input.GetKeyDown(Input.Keys.D1))
                    //    localPlayer.JoinTeam(TEAM_TYPE.TEAM_1);
                    //if (Input.GetKeyDown(Input.Keys.D2))
                    //    localPlayer.JoinTeam(TEAM_TYPE.TEAM_2);
                    //if (Input.GetKeyDown(Input.Keys.D3))
                    //    localPlayer.JoinTeam(TEAM_TYPE.TEAM_SPECTATOR);
                }
#if T_FOR_RESET
                if(Input.GetKeyDown(Input.Keys.T))
                {
                    SendRPC(-2, "OnRoundStart");
                    OnRoundStart();
                }

#endif
                if (Input.GetKeyDown(Input.Keys.F9))
                    ShowPlayers();
                if (Input.GetKeyDown(Input.Keys.F10))
                    ShowOwnedObjects();
            }
        }


        if (ServerOwner && MatchStarted && !timePaused)
        {
            ElapsedMatchTime += Time.ActualDeltaTime;
        }

        if (MatchTimeLeft <= 0 && MatchStarted && !GoldenGoal)
        {
            OnMatchEnd();
        }
    }


    public void Pause()
    {
        timePaused = true;
    }
    public void UnPause()
    {
        timePaused = false;
    }


    private void ShowOwnedObjects()
    {
        Debug.Log("##################################");
        Debug.Log("Owned objects:");
        foreach (var objects in Scene.ObjectOwners[LocalPeer])
        {
            Debug.Log(objects.gameObject.Name);
        }
        Debug.Log("##################################");
    }

    private void ShowPlayers()
    {
        Debug.Log("##################################");
        Debug.Log("Players:");
        foreach (var player in Scene.Players)
        {
            if (player.Key != LocalPeer)
            {
                ChadControls chad = player.Value.gameObject.GetComponent<ChadControls>();
                Debug.Log(player.Key.EndPoint.Address + ":\t" + chad?.rBody.Position);
            }
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

    void ResetNetworkplayerPoints()
    {
        foreach (NetworkPlayer Player in Teams[TEAM_TYPE.TEAM_1].Players)
        {
            Player.GoalsScored = 0;
            Player.HasTackled = 0;
            Player.Owngoal = 0;
        }
        foreach (NetworkPlayer Player in Teams[TEAM_TYPE.TEAM_2].Players)
        {
            Player.GoalsScored = 0;
            Player.HasTackled = 0;
            Player.Owngoal = 0;
        }

    }
    #region Coroutines
    IEnumerator MatchEndCoroutine(Team winningTeam, float duration)
    {

        MatchStarted = false;
        ChadHud.Instance.OnMatchEnd(winningTeam, duration);
        yield return new WaitForSecondsRealtime(duration);
        GoldenGoal = false;

        CameraMaster.instance.SetState(CAM_STATE.SCORE_SCREEN);
        yield return new WaitForSecondsRealtime(10);
        ResetNetworkplayerPoints();
        RPCStartMatch();
        CameraMaster.instance.SetState(CAM_STATE.GAME);
    }

    IEnumerator RoundStartCountdown(float duration)
    {
        ResetPlayers();
        Ball.GetComponent<Ball>().Reset();
        PowerupManager.ResetPowerups();
        LocalChad.Locked = true;

        countdownSound.Play();

        ChadHud.Instance.StartCountdown(duration);
        yield return new WaitForSecondsRealtime(duration);
        LocalChad.Locked = false;
        CameraMaster.instance.EnableSwitchTeam();
        hasScored = false;
    }

    IEnumerator OnGoalCoroutine(Team teamThatScored)
    {
        endroundSound.Play();

        ChadHud.Instance.OnGoal(teamThatScored, 5.0f);
        Time.TimeScale = 0.5f;

        yield return new WaitForSecondsRealtime(2.0f);
        ReplaySystem.recordGame = false;
        yield return new WaitForSecondsRealtime(3.0f);
        Time.TimeScale = 1.0f;
        ReplaySystem.StartReplay(teamThatScored);
        yield return new WaitForSecondsRealtime(ReplaySystem.durationInSeconds + 1.0f);
        ReplaySystem.recordGame = true;
        if (GoldenGoal)
            OnMatchEnd();
        else
        {
            OnRoundEnd();
            OnRoundStart();
        }
    }
    #endregion

    #region Read/Write
    public override void OnWrite(NetDataWriter writer)
    {

        writer.Put(MatchStarted);

        writer.Put(GoldenGoal);
        writer.Put(ElapsedMatchTime);
        writer.Put(MatchLength);
        writer.Put(MaxPlayers);
        writer.Put(ServerName);

        writer.Put(Teams[TEAM_TYPE.TEAM_1].Color);
        writer.Put(Teams[TEAM_TYPE.TEAM_1].Name);
        writer.Put(Teams[TEAM_TYPE.TEAM_1].Score);

        writer.Put(Teams[TEAM_TYPE.TEAM_2].Color);
        writer.Put(Teams[TEAM_TYPE.TEAM_2].Name);
        writer.Put(Teams[TEAM_TYPE.TEAM_2].Score);

    }
    public override void OnRead(NetDataReader reader)
    {

        MatchStarted = reader.GetBool();
        GoldenGoal = reader.GetBool();
        ElapsedMatchTime = reader.GetFloat();
        MatchLength = reader.GetInt();
        MaxPlayers = reader.GetInt();
        ServerName = reader.GetString();

        Teams[TEAM_TYPE.TEAM_1].Color = reader.GetColor();
        Teams[TEAM_TYPE.TEAM_1].Name = reader.GetString();
        Teams[TEAM_TYPE.TEAM_1].Score = reader.GetInt();

        Teams[TEAM_TYPE.TEAM_2].Color = reader.GetColor();
        Teams[TEAM_TYPE.TEAM_2].Name = reader.GetString();
        Teams[TEAM_TYPE.TEAM_2].Score = reader.GetInt();
    }
    #endregion

    #region RPC


    public void RPCStartMatch()
    {
        AnnouncerSoundManager.Instance.Announce(ANNOUNCEMENT_TYPE.WELCOME);
        MatchStarted = true;
        ElapsedMatchTime = 0.0f;
        foreach (var team in Teams)
        {
            team.Value.ResetScore();
        }
        OnRoundStart();
    }
    public void RPCEndMatch()
    {
        if(!MatchStarted || MatchTimeLeft > 0.0f)
        {
            Debug.Log("trying to end match before it is over");
            return;
        }

        hasScored = true;
        if (Teams[TEAM_TYPE.TEAM_1].Score == Teams[TEAM_TYPE.TEAM_2].Score)
        {
            GoldenGoal = true;
            AnnouncerSoundManager.Instance.Announce(ANNOUNCEMENT_TYPE.GOLDENGOAL);
            OnRoundEnd();
            OnRoundStart();
        }
        else
        {
            Team winningTeam = Teams[TEAM_TYPE.TEAM_1].Score > Teams[TEAM_TYPE.TEAM_2].Score ? Teams[TEAM_TYPE.TEAM_1] : Teams[TEAM_TYPE.TEAM_2];
            AnnouncerSoundManager.Instance.Announce(ANNOUNCEMENT_TYPE.WELLPLAYED);
            StartCoroutine(MatchEndCoroutine(winningTeam, 10.0f));
        }
    }

    public void RPCOnGoal(int teamType)
    {
        AnnouncerSoundManager.Instance.Announce(ANNOUNCEMENT_TYPE.GOAL);
        hasScored = true;
        TEAM_TYPE type = (TEAM_TYPE)teamType;
        Team team = FindTeam(type);
        team?.AddScore();
        StartCoroutine(OnGoalCoroutine(team));
        CameraMaster.instance.DisableSwitchTeam();
    }

    #endregion

    #region Match functions

    void OnMatchEnd()
    { 
        if (MatchStarted)
        {
            RPCEndMatch();
            if(ServerOwner)
                SendRPC(-2, "RPCEndMatch");
        }
        
        //Clean up
        //MatchStarted = false;
    }


    public void OnMatchStart()
    {
        
        if (MatchStarted)
            return;

        
        RPCStartMatch();
        SendRPC(-2, "RPCStartMatch");

    }


    public void OnRoundStart()
    {
        if (Teams[TEAM_TYPE.TEAM_1].Score > 0 || Teams[TEAM_TYPE.TEAM_2].Score > 0)
            AnnouncerSoundManager.Instance.Announce(ANNOUNCEMENT_TYPE.WELLPLAYED);

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
        int playersInTeam = Teams[team].PlayerCount;
        if ((MaxPlayers / 2.0f) > playersInTeam)
            np.JoinTeam(team);
        else
            Debug.Log(team + " is already full");
    }

    #endregion

    #region peer connection

    protected override void OnPeerJoin(NetPeer peer)
    {

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
