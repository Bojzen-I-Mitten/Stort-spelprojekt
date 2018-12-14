using System;
using System.Linq;
using ThomasEngine;
using System.Collections.Generic;

public class PlayerText
{
    private int Sign;

    public Text Name;
    public Text Goals;
    public Text Tackles;
    public Text Score;
    public Text Ping;

    public bool IsRendering
    {
        set
        {
            Name.rendering = value;
            Goals.rendering = value;
            Tackles.rendering = value;
            Score.rendering = value;
            Ping.rendering = value;
        }
    }

    public Color Color
    {
        set
        {
            Name.color = value;
            Goals.color = value;
            Tackles.color = value;
            Score.color = value;
            Ping.color = value;
        }
    }

    public Vector2 Scale
    {
        set
        {
            Name.scale = value;
            Goals.scale = value;
            Tackles.scale = value;
            Score.scale = value;
            Ping.scale = value;
        }
    }

    public Vector2 Origin
    {
        set
        {
            Name.origin = value;
            Goals.origin = value;
            Tackles.origin = value;
            Score.origin = value;
            Ping.origin = value;
        }
    }

    public float Depth
    {
        set
        {
            Name.depth = value;
            Goals.depth = value;
            Tackles.depth = value;
            Score.depth = value;
            Ping.depth = value;
        }
    }

    public PlayerText(Canvas _Canvas, TEAM_TYPE team, Image playerbar)
    {
        Name = _Canvas.Add("");
        Goals = _Canvas.Add("");
        Tackles = _Canvas.Add("");
        Score = _Canvas.Add("");
        Ping = _Canvas.Add("");

        Color = Color.FloralWhite;
        Origin = new Vector2(0.5f);
        Scale = new Vector2(0.5f) * playerbar.scale;
        Depth = 0.9f;
        switch (team)
        {
            case TEAM_TYPE.TEAM_1:
                Sign = -1;
                break;
            case TEAM_TYPE.TEAM_2:
                Sign = 1;
                break;
            default:
                Sign = 0;
                break;
        }

        Name.position = playerbar.position + Sign * new Vector2(playerbar.size.x * 0.85f, 0);
        Score.position = playerbar.position + Sign * new Vector2(playerbar.size.x * 0.625f, 0);
        Goals.position = playerbar.position + Sign * new Vector2(playerbar.size.x * 0.45f, 0);
        Tackles.position = playerbar.position + Sign * new Vector2(playerbar.size.x * 0.30f, 0);
        Ping.position = playerbar.position + Sign * new Vector2(playerbar.size.x * 0.125f, 0);

        IsRendering = false;
    }
}

public class GUIScoreboard : ScriptComponent
{
    public static GUIScoreboard Instance = null;

    Camera cam;
    public Canvas Canvas;

    public Texture2D ScoreBG { get; set; }
    public Texture2D PlayerBG { get; set; }
    public Texture2D TeamBG { get; set; }

    Image Scoreboard;
    Image Team1Bar;
    Image Team2Bar;
    Image[] PlayerHeaderBackground = new Image[2];

    Text ScoreHeader;
    Text[] Score = new Text[2];

    PlayerText[] PlayerHeader = new PlayerText[2];

    public List<PlayerText> Team1Players = new List<PlayerText>();
    public List<PlayerText> Team2Players = new List<PlayerText>();
    public Text[] TeamName = new Text[2];
    public List<Image> PlayerStandardBarTeam1 = new List<Image>();
    public List<Image> PlayerStandardBarTeam2 = new List<Image>();

    [Newtonsoft.Json.JsonIgnore]
    private Team Team1 { get { return MatchSystem.instance.Teams[TEAM_TYPE.TEAM_1]; } }
    [Newtonsoft.Json.JsonIgnore]
    private Team Team2 { get { return MatchSystem.instance.Teams[TEAM_TYPE.TEAM_2]; } }

    [Newtonsoft.Json.JsonIgnore]
    public int Team1PlayerCount { get { return Team1.PlayerCount; } }
    [Newtonsoft.Json.JsonIgnore]
    public int Team2PlayerCount { get { return Team2.PlayerCount; } }

    public bool Toggle = false;

    public Vector2 Scale { get; set; } = new Vector2(0.75f);

    public override void OnAwake()
    {
        Instance = this;
        cam = gameObject.GetComponent<Camera>();
        Canvas = cam.AddCanvas();

        //Scoreboard Image
        Scoreboard = Canvas.Add(ScoreBG);
        Scoreboard.color = Color.White;
        Scoreboard.origin = new Vector2(0.5f);
        Scoreboard.position = new Vector2(0.5f, 0.11f);
        Scoreboard.depth = 0.9f;
        Scoreboard.scale = Scale;

        //Team1BG Image
        Team1Bar = Canvas.Add(TeamBG);
        Team1Bar.color = Team1.Color;
        Team1Bar.origin = new Vector2(1, 0.5f);
        Team1Bar.position = Scoreboard.position + new Vector2(-Scoreboard.size.x * 0.5f, 0);
        Team1Bar.depth = 1;
        Team1Bar.flip = new Vector2(0, 1);
        Team1Bar.scale = Scale;

        //Team2BG Image
        Team2Bar = Canvas.Add(TeamBG);
        Team2Bar.color = Team2.Color;
        Team2Bar.origin = new Vector2(0, 0.5f);
        Team2Bar.position = Scoreboard.position + new Vector2(Scoreboard.size.x * 0.5f, 0);
        Team2Bar.depth = 1;
        Team2Bar.scale = Scale;

        //Team1 Score
        Score[0] = Canvas.Add(Team1.Score.ToString());
        Score[0].color = Color.Black;
        Score[0].origin = new Vector2(0.5f);
        Score[0].position = Scoreboard.position + new Vector2(-Scoreboard.size.x * 0.27f, Scoreboard.size.y * 0.09f);
        Score[0].scale = Scale;

        //Team2 Score
        Score[1] = Canvas.Add(Team2.Score.ToString());
        Score[1].color = Color.Black;
        Score[1].origin = new Vector2(0.5f);
        Score[1].position = Scoreboard.position + new Vector2(Scoreboard.size.x * 0.25f, Scoreboard.size.y * 0.09f);
        Score[1].scale = Scale;

        //Score text
        ScoreHeader = Canvas.Add("Score");
        ScoreHeader.color = Color.Black;
        ScoreHeader.origin = new Vector2(0.5f);
        ScoreHeader.scale = new Vector2(0.5f) * Scale;
        ScoreHeader.position = Scoreboard.position + new Vector2(0, -Scoreboard.size.y * 0.2875f);

        //Team1 name
        TeamName[0] = Canvas.Add(Team1.Name);
        TeamName[0].origin = new Vector2(0, 0.5f);
        TeamName[0].color = Color.FloralWhite;
        TeamName[0].scale = new Vector2(0.5f) * Scale;
        TeamName[0].position = Team1Bar.position - new Vector2(Team1Bar.size.x * 0.5f, 0);

        //Team2 name
        TeamName[1] = Canvas.Add(Team2.Name);
        TeamName[1].origin = new Vector2(1, 0.5f);
        TeamName[1].color = Color.FloralWhite;
        TeamName[1].scale = new Vector2(0.5f) * Scale;
        TeamName[1].position = Team2Bar.position + new Vector2(Team2Bar.size.x * 0.5f, 0);

        //Add one row for every player. They are then only drawn based on players in each team.
        for (int i = 0; i < MatchSystem.instance.MaxPlayers / 2; i++)
        {
            PlayerStandardBarTeam1.Add(Canvas.Add(PlayerBG));
            PlayerStandardBarTeam1[i].color = Team1.Color;
            PlayerStandardBarTeam1[i].origin = new Vector2(1, 0.5f);
            PlayerStandardBarTeam1[i].position = new Vector2(0.5f, 0.32f + (i * 0.063f));
            PlayerStandardBarTeam1[i].depth = 1;
            PlayerStandardBarTeam1[i].rendering = false;
            PlayerStandardBarTeam1[i].scale = Scale;
            Team1Players.Add(new PlayerText(Canvas, TEAM_TYPE.TEAM_1, PlayerStandardBarTeam1[i]));

            PlayerStandardBarTeam2.Add(Canvas.Add(PlayerBG));
            PlayerStandardBarTeam2[i].color = Team2.Color;
            PlayerStandardBarTeam2[i].origin = new Vector2(0, 0.5f);
            PlayerStandardBarTeam2[i].position = new Vector2(0.5f, 0.32f + (i * 0.063f));
            PlayerStandardBarTeam2[i].flip = new Vector2(0, 1);
            PlayerStandardBarTeam2[i].depth = 1;
            PlayerStandardBarTeam2[i].rendering = false;
            PlayerStandardBarTeam2[i].scale = Scale;
            Team2Players.Add(new PlayerText(Canvas, TEAM_TYPE.TEAM_2, PlayerStandardBarTeam2[i]));
        }

        //Header background above Team1 players
        PlayerHeaderBackground[0] = Canvas.Add(PlayerBG);
        PlayerHeaderBackground[0].position = new Vector2(0.5f, 0.32f + (-1 * 0.063f));
        PlayerHeaderBackground[0].color = Team1.Color;
        PlayerHeaderBackground[0].origin = new Vector2(1, 0.5f);
        PlayerHeaderBackground[0].depth = 1;
        PlayerHeaderBackground[0].scale = Scale;

        //Header background above Team2 players
        PlayerHeaderBackground[1] = Canvas.Add(PlayerBG);
        PlayerHeaderBackground[1].position = new Vector2(0.5f, 0.32f + (-1 * 0.063f));
        PlayerHeaderBackground[1].color = Team2.Color;
        PlayerHeaderBackground[1].origin = new Vector2(0, 0.5f);
        PlayerHeaderBackground[1].flip = new Vector2(0, 1);
        PlayerHeaderBackground[1].depth = 1;
        PlayerHeaderBackground[1].scale = Scale;

        //Header text above Team1 players
        PlayerHeader[0] = new PlayerText(Canvas, TEAM_TYPE.TEAM_1, PlayerHeaderBackground[0]);
        PlayerHeader[0].Name.text = "Name";
        PlayerHeader[0].Goals.text = "Goals";
        PlayerHeader[0].Tackles.text = "Tackles";
        PlayerHeader[0].Score.text = "Score";
        PlayerHeader[0].Ping.text = "Ping";
        PlayerHeader[0].Scale = new Vector2(0.33f) * Scale;
        PlayerHeader[0].IsRendering = true;

        //Header text above Team2 players
        PlayerHeader[1] = new PlayerText(Canvas, TEAM_TYPE.TEAM_2, PlayerHeaderBackground[1]);
        PlayerHeader[1].Name.text = "Name";
        PlayerHeader[1].Goals.text = "Goals";
        PlayerHeader[1].Tackles.text = "Tackles";
        PlayerHeader[1].Score.text = "Score";
        PlayerHeader[1].Ping.text = "Ping";
        PlayerHeader[1].Scale = new Vector2(0.33f) * Scale;
        PlayerHeader[1].IsRendering = true;
    }

    void UpdatePlayerBars()
    {
        for (int i = Team1PlayerCount; i < MatchSystem.instance.MaxPlayers / 2; i++)
        {
            PlayerStandardBarTeam1[i].rendering = false;
            Team1Players[i].IsRendering = false;
        }
        for (int i = Team2PlayerCount; i < MatchSystem.instance.MaxPlayers / 2; i++)
        {
            PlayerStandardBarTeam2[i].rendering = false;
            Team2Players[i].IsRendering = false;
        }
    }
    void UpdatePlayertextbasedonscore()
    {

    }
    void UpdateStatistics()
    {
        Score[0].text = Team1.Score.ToString();
        Score[1].text = Team2.Score.ToString();

        if (Team1.Name.Length >= 13)
            TeamName[0].text = Team1.Name.Substring(0, 11) + "...";
        else
            TeamName[0].text = Team1.Name.ToString();

        if (Team2.Name.Length >= 13)
            TeamName[1].text = Team2.Name.Substring(0, 11) + "...";
        else
            TeamName[1].text = Team2.Name.ToString();

        Team1Bar.color = Team1.Color;
        Team2Bar.color = Team2.Color;

        PlayerHeaderBackground[0].color = Team1.Color;
        PlayerHeaderBackground[1].color = Team2.Color;

        for (int i = 0; i < Team1PlayerCount; i++)
        {
            PlayerStandardBarTeam1[i].rendering = true;
            Team1Players[i].IsRendering = true;
            PlayerStandardBarTeam1[i].color = Team1Bar.color;
            if (Team1.Players[i].PlayerName.Length > 9)
                Team1Players[i].Name.text = Team1.Players[i].PlayerName.Substring(0, 9) + "-";
            else
                Team1Players[i].Name.text = Team1.Players[i].PlayerName;
            Team1Players[i].Goals.text = Team1.Players[i].GoalsScored.ToString();
            Team1Players[i].Tackles.text = Team1.Players[i].HasTackled.ToString();
            Team1Players[i].Score.text = (Team1.Players[i].HasTackled * 10 + 100 * Team1.Players[i].GoalsScored - 100 * Team1.Players[i].Owngoal).ToString();
            Team1Players[i].Ping.text = Team1.Players[i].GetPing().ToString();
            Team1.Players[i].Score = (Team1.Players[i].HasTackled * 10 + 100 * Team1.Players[i].GoalsScored - 100 * Team1.Players[i].Owngoal);
        }
        for (int i = 0; i < Team2PlayerCount; i++)
        {
            PlayerStandardBarTeam2[i].rendering = true;
            Team2Players[i].IsRendering = true;
            PlayerStandardBarTeam2[i].rendering = true;
            PlayerStandardBarTeam2[i].color = Team2Bar.color;
            if (Team2.Players[i].PlayerName.Length > 9)
                Team2Players[i].Name.text = Team2.Players[i].PlayerName.Substring(0, 9) + "-";
            else
                Team2Players[i].Name.text = Team2.Players[i].PlayerName;
            Team2Players[i].Goals.text = Team2.Players[i].GoalsScored.ToString();
            Team2Players[i].Tackles.text = Team2.Players[i].HasTackled.ToString();
            Team2Players[i].Score.text = (Team2.Players[i].HasTackled * 10 + 100 * Team2.Players[i].GoalsScored - 100 * Team2.Players[i].Owngoal).ToString();
            Team2Players[i].Ping.text = Team2.Players[i].GetPing().ToString();
            Team2.Players[i].Score = (Team2.Players[i].HasTackled * 10 + 100 * Team2.Players[i].GoalsScored - 100 * Team2.Players[i].Owngoal);
        }
    }

    public void LastUpdate()
    {
        UpdateStatistics();
        UpdatePlayerBars();
    }

    public void UpdateList()
    {
        if (Team1.Players.Count > 1)
            Team1.Players = Team1.Players.OrderByDescending(x => x.Score).ToList();
        if (Team2.Players.Count > 1)
            Team2.Players = Team2.Players.OrderByDescending(x => x.Score).ToList();
    }

    public override void Update()
    {
        UpdatePlayerBars();
        UpdateStatistics();
        UpdateList();

        if ((Input.GetKey(Input.Keys.Tab) && MatchSystem.instance.MatchStarted) || GUIScoreScreen.Instance.enabled)
            Toggle = true;

        if (Input.GetKeyUp(Input.Keys.Tab) && !GUIScoreScreen.Instance.enabled)
            Toggle = false;

        Canvas.isRendering = Toggle;
        if (MatchSystem.instance.MatchStarted)
            ChadHud.Instance.Canvas.isRendering = !Toggle;
    }

    public override void OnEnable()
    {
        if (Canvas != null /*&& GUIScoreScreen.Instance.enabled*/)
            Canvas.isRendering = true;

        Toggle = false;
    }

    public override void OnDisable()
    {
        if (Canvas != null)
            Canvas.isRendering = false;
    }
}
