//#define T_FOR_RESET
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

    public ReplaySystem ReplaySystem;
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

        endroundSound = gameObject.AddComponent<SoundComponent>();
        endroundSound.Clip = endroundSoundClip;
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

        if (MatchTimeLeft <= 0 && MatchStarted && !GoldenGoal)
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
        CameraMaster.instance.EnableSwitchTeam();
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

        CameraMaster.instance.State = CAM_STATE.SCORE_SCREEN;
        for (int i = 0; i < GUIScoreScreen.Instance.ScoreScreenTimeLast; i++)
        {
            GUIScoreScreen.Instance.UpdateTimer();
            yield return new WaitForSecondsRealtime(1);
            if (!GUIScoreScreen.Instance.enabled)
                break;
        }
        ResetNetworkplayerPoints();
        RPCStartMatch();
        CameraMaster.instance.State = CAM_STATE.GAME;
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

    #region RPC

    public void RPCMatchInfo(bool matchStarted, float startTime, int length, bool goldenGoal, int powerupID, int players, string name,
        int team1Score, int team2Score,
        Color team1Color, Color team2Color,
        string team1Name, string team2Name)
    {

        MatchStarted = matchStarted;
        PowerupManager.NextPowerupID = powerupID;
        GoldenGoal = goldenGoal;
        MatchStartTime = startTime;
        MatchLength = length;
        MaxPlayers = players;
        ServerName = name;

        Teams[TEAM_TYPE.TEAM_1].Color = team1Color;
        Teams[TEAM_TYPE.TEAM_2].Color = team2Color;
        Teams[TEAM_TYPE.TEAM_1].Name = team1Name;
        Teams[TEAM_TYPE.TEAM_2].Name = team2Name;
        Teams[TEAM_TYPE.TEAM_1].Score = team1Score;
        Teams[TEAM_TYPE.TEAM_2].Score = team2Score;
        Debug.Log("Got matchinfo from network!");
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
        hasScored = true;
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
        //if(peer == LocalPeer)
        //{
        //    NetworkPlayer np = Scene.Players[peer].gameObject.GetComponent<NetworkPlayer>();
        //    np.PlayerName = NetUtils.GetLocalIp(LocalAddrType.IPv4);
        //}
        if (peer != LocalPeer && Ball.GetComponent<NetworkIdentity>().Owner)
        {
            SendRPC(peer, -2, "RPCMatchInfo", MatchStarted, MatchStartTime, MatchLength, GoldenGoal, PowerupManager.NextPowerupID, MaxPlayers, ServerName,
                Teams[TEAM_TYPE.TEAM_1].Score, Teams[TEAM_TYPE.TEAM_2].Score,
                Teams[TEAM_TYPE.TEAM_1].Color, Teams[TEAM_TYPE.TEAM_2].Color,
                Teams[TEAM_TYPE.TEAM_1].Name, Teams[TEAM_TYPE.TEAM_2].Name);
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
