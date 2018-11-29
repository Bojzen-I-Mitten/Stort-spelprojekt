using System;
using System.Collections;
using System.ComponentModel;
using System.Linq;
using ThomasEngine;
using System.Collections.Generic;

public class Playertext
{
    public Text PlayerName;
    public Text PlayerGoals;
    public Text PlayerTackles;
    public Text PlayerScore;
    public Text PlayerPing;
    Canvas Canvas;
    
    public Playertext(int Number,Font font,Canvas Canvas,String Name, int goals, int Tackles, int score, int Ping, TEAM_TYPE team, Image playerbar)
    {
        this.Canvas = Canvas;
        this.PlayerName = Canvas.Add(Name);
        this.PlayerGoals = Canvas.Add(""+goals);
        this.PlayerTackles = Canvas.Add("" + Tackles);
        this.PlayerScore = Canvas.Add("" + score);
        this.PlayerPing = Canvas.Add("" + Ping);
        this.PlayerName.origin = new Vector2(0, 0);
        this.PlayerGoals.origin = new Vector2(0, 0);
        this.PlayerTackles.origin = new Vector2(0, 0);
        this.PlayerScore.origin = new Vector2(0, 0);
        this.PlayerPing.origin = new Vector2(0, 0);

        AddFont(font);
        UpdateScale(Vector2.Zero);
        UpdateColor(Color.White);

        if (team == TEAM_TYPE.TEAM_1)
            UpdatePositionTeam1(playerbar);
        else
            UpdatePositionTeam2(playerbar);
    }

    public void UpdateScale(Vector2 scale)
    {
        this.PlayerName.scale = scale;
        this.PlayerGoals.scale = scale;
        this.PlayerTackles.scale = scale;
        this.PlayerScore.scale = scale;
        this.PlayerPing.scale = scale;
    }

    public void AddFont(Font font)
    {
        this.PlayerName.font = font;
        this.PlayerGoals.font = font;
        this.PlayerTackles.font = font;
        this.PlayerScore.font = font;
        this.PlayerPing.font = font;
    } 

    public void RemoveCanvas()
    {
        Canvas.Remove(this.PlayerName);
        Canvas.Remove(this.PlayerGoals);
        Canvas.Remove(this.PlayerTackles);
        Canvas.Remove(this.PlayerScore);
        Canvas.Remove(this.PlayerPing);
    }

    public void UpdatePositionTeam1( Image Playerbar1)
    {
        //float PositionUpdate = NewPositionUpdate * 0.064f;
        this.PlayerName.position = Playerbar1.position + new Vector2(0.0725f,0.042f); //new Vector2(0.020f, 0.34f + PositionUpdate);
        this.PlayerScore.position   = Playerbar1.position + new Vector2(0.19f, 0.042f);//new Vector2(0.179f, 0.34f + PositionUpdate);
        this.PlayerGoals.position = Playerbar1.position + new Vector2(0.29f, 0.042f);//new Vector2(0.268f, 0.34f + PositionUpdate);
        this.PlayerTackles.position = Playerbar1.position + new Vector2(0.38f, 0.042f); ;//new Vector2(0.364f, 0.34f + PositionUpdate);
        this.PlayerPing.position    = Playerbar1.position + new Vector2(0.455f, 0.042f); ;//new Vector2(0.445f, 0.34f + PositionUpdate);
       
        this.PlayerName.origin = new Vector2(0.5f, 0.5f); //new Vector2(0.020f, 0.34f + PositionUpdate);
        this.PlayerGoals.origin = new Vector2(0.5f, 0.5f);//new Vector2(0.179f, 0.34f + PositionUpdate);
        this.PlayerTackles.origin = new Vector2(0.5f, 0.5f);//new Vector2(0.268f, 0.34f + PositionUpdate);
        this.PlayerScore.origin = new Vector2(0.5f, 0.5f);//new Vector2(0.364f, 0.34f + PositionUpdate);
        this.PlayerPing.origin = new Vector2(0.5f, 0.5f);//new Vector2(0.445f, 0.34f + PositionUpdate);


    }

    public void UpdatePositionTeam2(Image Playerbar2)
    {

        this.PlayerName.position = Playerbar2.position + new Vector2(0.425f, 0.042f);
        this.PlayerScore.position = Playerbar2.position + new Vector2(0.3f, 0.042f);
        this.PlayerGoals.position = Playerbar2.position + new Vector2(0.21f, 0.042f);
        this.PlayerTackles.position = Playerbar2.position + new Vector2(0.11f, 0.042f);
        this.PlayerPing.position = Playerbar2.position + new Vector2(0.04f, 0.042f); ;


        this.PlayerName.origin = new Vector2(0.5f, 0.5f); 
        this.PlayerGoals.origin = new Vector2(0.5f, 0.5f);
        this.PlayerTackles.origin = new Vector2(0.5f, 0.5f);
        this.PlayerScore.origin = new Vector2(0.5f, 0.5f);
        this.PlayerPing.origin = new Vector2(0.5f, 0.5f);


    }

    void UpdateColor(Color color)
    {
        this.PlayerName.color = color;
        this.PlayerGoals.color = color;
        this.PlayerTackles.color = color;
        this.PlayerScore.color = color;
        this.PlayerPing.color = color;
    }
}

public class GUIPlayerScore : ScriptComponent
{
    Camera cam;
    Canvas Canvas;
    public Texture2D PlayerBarTeam1 { get; set; }
    public Texture2D PlayerBarTeam2 { get; set; }
    public Texture2D Team1Bar { get; set; }
    public Texture2D Team2Bar { get; set; }
    public int AmountOfPlayersInTeam1 { get; set; } = 0;
    public int AmountOfPlayersInTeam2 { get; set; } = 0;
    public Texture2D AmountOfPlayersBar { get; set; }
    public bool Toggle = false;
    public Font Font { get; set; }
    public Vector2 Textposition { get; set; }
    public Vector2 Textorigin { get; set; }
    Image AmountOfPlayersBarImage;
    Text[] ScoreText = new Text[2];
    Text TextScore;
    Image team1BarImage;
    Image team2BarImage;
   public List<Playertext> Team1Players = new List<Playertext>();
   public List<Playertext> Team2Players = new List<Playertext>();
   public Text[] PlayerstandardText = new Text[2];
   public Text[] TeamName = new Text[2];
   public Image[] PlayerStandardbar = new Image[2];
   public List<Image> PlayerStandardBarTeam1 = new List<Image>();
   public List<Image> PlayerStandardBarTeam2 = new List<Image>();


    public static GUIPlayerScore Instance = null;
    public override void Start()
    {
        Instance = this;
        cam = gameObject.GetComponent<Camera>();
        Canvas = cam.AddCanvas();
     
        //Team1Bar Image
        team1BarImage = Canvas.Add(Team1Bar);
        team1BarImage.scale = Vector2.Zero;
        team1BarImage.color = MatchSystem.instance.Teams[TEAM_TYPE.TEAM_1].Color;
        team1BarImage.position = new Vector2(0, 0.172f);
        
        //Team2Bar Image
        team2BarImage = Canvas.Add(Team2Bar);
        team2BarImage.scale = Vector2.Zero;
        team2BarImage.color = MatchSystem.instance.Teams[TEAM_TYPE.TEAM_2].Color;
        team2BarImage.position = new Vector2(0.596f,0.172f);

        //AmountOfPlayersBar Image
        AmountOfPlayersBarImage = Canvas.Add(AmountOfPlayersBar);
        AmountOfPlayersBarImage.scale = Vector2.Zero;
        AmountOfPlayersBarImage.color = Color.White;
        AmountOfPlayersBarImage.position = new Vector2(0.4f, 0.11f);

        ScoreText[0] = Canvas.Add(""+MatchSystem.instance.Teams[TEAM_TYPE.TEAM_1].Score);
        ScoreText[1] = Canvas.Add("" + MatchSystem.instance.Teams[TEAM_TYPE.TEAM_2].Score);
        for(int i=0;i<2;i++)
        {
            ScoreText[i].color = Color.Black;
            ScoreText[i].scale = Vector2.Zero;
            ScoreText[i].font = Font;
        }
        TextScore = Canvas.Add("Score");
        TextScore.color = Color.Black;
        TextScore.scale = Vector2.Zero;
        TextScore.font = Font;
        TextScore.position = new Vector2(0.45f, 0.1225f);
        PlayerStandardbar[0] = Canvas.Add(PlayerBarTeam1);
        PlayerStandardbar[1] = Canvas.Add(PlayerBarTeam2);
        TeamName[0] = Canvas.Add(MatchSystem.instance.Teams[TEAM_TYPE.TEAM_1].Name);
        TeamName[1] = Canvas.Add(MatchSystem.instance.Teams[TEAM_TYPE.TEAM_2].Name);
        PlayerstandardText[1] = Canvas.Add("Ping  Tackles    Goals     Score        Player");
        PlayerstandardText[0] = Canvas.Add("     Player        Score      Goals   Tackles  Ping");
        for (int i = 0; i < 2; i++)
        {
            PlayerStandardbar[i].scale = Vector2.Zero;
            PlayerstandardText[i].font = Font;
            PlayerstandardText[i].scale = Vector2.Zero;
            TeamName[i].font = Font;
            TeamName[i].scale = Vector2.Zero;
        }
        PlayerstandardText[0].position = new Vector2(0.003f, 0.27f);
        PlayerstandardText[1].position = new Vector2(0.52f, 0.27f);
        PlayerStandardbar[0].position = new Vector2(0, 0.32f + (-1 * 0.063f));
        PlayerStandardbar[0].color = MatchSystem.instance.Teams[TEAM_TYPE.TEAM_1].Color;//Color.Red; 
        PlayerStandardbar[1].color = MatchSystem.instance.Teams[TEAM_TYPE.TEAM_2].Color;//Color.Blue;
        PlayerStandardbar[1].position = new Vector2(0.5f, 0.32f + (-1 * 0.063f)); 

        TeamName[0].position = new Vector2(0.11f, 0.173f);
        TeamName[0].origin = Vector2.Zero;
        TeamName[1].position = new Vector2(0.885f, 0.173f);
        TeamName[1].origin = new Vector2(1, 0);
        TeamName[0].color = Color.White;
        TeamName[1].color = Color.White;
    }

    void DisplayBar(Vector2 OnOff)
    {
        TextScore.scale = OnOff;
        AmountOfPlayersBarImage.scale = OnOff;
        team1BarImage.scale = OnOff;
        team2BarImage.scale = OnOff;
        for (int i = 0; i < 2; i++)
        {
            PlayerStandardbar[i].scale = OnOff;
            if (OnOff.x == 0)
            {
                ScoreText[i].scale = Vector2.Zero;
                TeamName[i].scale = new Vector2(0, 0);
                PlayerstandardText[i].scale = new Vector2(0, 0);
            }
            else
            {
                ScoreText[i].scale = new Vector2(2, 2);
                TeamName[i].scale = OnOff;
                PlayerstandardText[i].scale = new Vector2(1.0f, 1.0f);//new Vector2(0.8f, 0.6f);
            }
        }
        if(OnOff.x != 0)
        { 
            for(int i = 0;i< Team1Players.Count;i++)
            {
                Team1Players[i].UpdateScale(new Vector2(1.1f, 1.1f));
            }
            for (int i = 0; i < Team2Players.Count; i++)
            {
                Team2Players[i].UpdateScale(new Vector2(1.1f, 1.1f));
            }
        }
        else
        {
            for (int i = 0; i < Team1Players.Count; i++)
            {
                Team1Players[i].UpdateScale(OnOff);
            }
            for (int i = 0; i < Team2Players.Count; i++)
            {
                Team2Players[i].UpdateScale(OnOff);
            }
        }
        for (int i = 0; i < AmountOfPlayersInTeam1; i++)
        {
            PlayerStandardBarTeam1[i].scale = OnOff;
        }
        for (int i = 0; i < AmountOfPlayersInTeam2; i++)
        {
            PlayerStandardBarTeam2[i].scale = OnOff;

        }
    }
    void SetOrigin()
    {
    }
    void UpdatePlayerBars()
    {
        if(PlayerStandardBarTeam2.Count != AmountOfPlayersInTeam2 || PlayerStandardBarTeam1.Count != AmountOfPlayersInTeam1)
        {
            if (PlayerStandardBarTeam2.Count< AmountOfPlayersInTeam2)
            {                
                while (PlayerStandardBarTeam2.Count != AmountOfPlayersInTeam2)
                {
                    int i = PlayerStandardBarTeam2.Count;
                    PlayerStandardBarTeam2.Add(Canvas.Add(PlayerBarTeam2));
                    PlayerStandardBarTeam2[i].scale = Vector2.Zero;
                    PlayerStandardBarTeam2[i].color = MatchSystem.instance.Teams[TEAM_TYPE.TEAM_2].Color;//Color.Blue;
                    PlayerStandardBarTeam2[i].position = new Vector2(0.5f, 0.32f + (i*0.063f));//new Vector2(-0.84f, -3.75f - (i * 0.75f));
                    Team2Players.Add(new Playertext(i, Font, Canvas, "jonn " + i, 2, 34, 4, 94, TEAM_TYPE.TEAM_2, PlayerStandardBarTeam2[i]));
                }
            }
            else if(PlayerStandardBarTeam2.Count > AmountOfPlayersInTeam2)
            {
                while (PlayerStandardBarTeam2.Count != AmountOfPlayersInTeam2)
                {
                    Image tempRef = PlayerStandardBarTeam2[PlayerStandardBarTeam2.Count - 1];
                    PlayerStandardBarTeam2.RemoveAt(PlayerStandardBarTeam2.Count - 1);
                    Canvas.Remove(tempRef);
                }
                for(int i=0;i<Team2Players.Count;i++)
                {
                    Team2Players[i].RemoveCanvas();
                }
                Team2Players.Clear();
                for (int i = 0; i < PlayerStandardBarTeam2.Count; i++)
                {
                    Team2Players.Add(new Playertext(i, Font, Canvas, "jonn " + i, 2, 34, 4, 94, TEAM_TYPE.TEAM_2, PlayerStandardBarTeam2[i]));
                }
            }
            if (PlayerStandardBarTeam1.Count < AmountOfPlayersInTeam1)
            {            
                while (PlayerStandardBarTeam1.Count != AmountOfPlayersInTeam1)
                {
                    int i = PlayerStandardBarTeam1.Count;
                    PlayerStandardBarTeam1.Add(Canvas.Add(PlayerBarTeam1));
                    PlayerStandardBarTeam1[i].scale = Vector2.Zero;
                    PlayerStandardBarTeam1[i].color = MatchSystem.instance.Teams[TEAM_TYPE.TEAM_1].Color;//Color.Red;
                    PlayerStandardBarTeam1[i].position = new Vector2(0, 0.32f + (i * 0.063f));//new Vector2(0, -3.75f - (i * 0.75f));
                    Team1Players.Add(new Playertext(i, Font, Canvas, "jonn " + i, 2, 34, 4, 94, TEAM_TYPE.TEAM_1, PlayerStandardBarTeam1[i]));
                }
            }
            else if (PlayerStandardBarTeam1.Count > AmountOfPlayersInTeam1)
            {
                while (PlayerStandardBarTeam1.Count != AmountOfPlayersInTeam1)
                {
                    Image tempRef = PlayerStandardBarTeam1[PlayerStandardBarTeam1.Count - 1];
                    PlayerStandardBarTeam1.RemoveAt(PlayerStandardBarTeam1.Count - 1);
                    Canvas.Remove(tempRef);
                }
                for (int i = 0; i < Team1Players.Count; i++)
                {
                    Team1Players[i].RemoveCanvas();
                }
                Team1Players.Clear();
                for(int i=0;i<PlayerStandardBarTeam1.Count;i++)
                {
                    Team1Players.Add(new Playertext(i, Font, Canvas, "jonn " + i, 2, 34, 4, 94, TEAM_TYPE.TEAM_1, PlayerStandardBarTeam1[i]));
                }
            }
        }
    }
    void UpdatePlayertextbasedonscore()
    {

    }
    void UpdateStatistics()
    {
        Canvas.Remove(ScoreText[0]);
        Canvas.Remove(ScoreText[1]);
        ScoreText[0] = Canvas.Add(""+MatchSystem.instance.Teams[TEAM_TYPE.TEAM_1].Score);
        ScoreText[1] = Canvas.Add("" + MatchSystem.instance.Teams[TEAM_TYPE.TEAM_2].Score);
        for(int i = 0;i<2;i++)
        { 
        ScoreText[i].color = Color.Black;
        ScoreText[i].scale = Vector2.Zero;
        ScoreText[i].font = Font;
        }
        ScoreText[0].position = new Vector2(0.435f, 0.18f);
        ScoreText[1].position = new Vector2(0.525f,0.18f);

        if (MatchSystem.instance.Teams[TEAM_TYPE.TEAM_1].Name.Length >= 13)
            TeamName[0].text = MatchSystem.instance.Teams[TEAM_TYPE.TEAM_1].Name.Substring(0, 13) + "-";
        else
            TeamName[0].text = MatchSystem.instance.Teams[TEAM_TYPE.TEAM_1].Name.ToString();

        if (MatchSystem.instance.Teams[TEAM_TYPE.TEAM_2].Name.Length >= 13)
            TeamName[1].text = MatchSystem.instance.Teams[TEAM_TYPE.TEAM_2].Name.Substring(0, 13) + "-";
        else
            TeamName[1].text = MatchSystem.instance.Teams[TEAM_TYPE.TEAM_2].Name.ToString();
        team1BarImage.color = MatchSystem.instance.Teams[TEAM_TYPE.TEAM_1].Color;
        team2BarImage.color = MatchSystem.instance.Teams[TEAM_TYPE.TEAM_2].Color;

        PlayerStandardbar[0].color = MatchSystem.instance.Teams[TEAM_TYPE.TEAM_1].Color;//Color.Red; 
        PlayerStandardbar[1].color = MatchSystem.instance.Teams[TEAM_TYPE.TEAM_2].Color;//Color.Blue;
        for(int i = 0;i< Math.Min(Team1Players.Count, AmountOfPlayersInTeam1); i++)
        {
            if(MatchSystem.instance.Teams[TEAM_TYPE.TEAM_1].Players[i].PlayerName.Length > 9)
                Team1Players[i].PlayerName.text = MatchSystem.instance.Teams[TEAM_TYPE.TEAM_1].Players[i].PlayerName.Substring(0,9)+"-";
            else
                Team1Players[i].PlayerName.text = MatchSystem.instance.Teams[TEAM_TYPE.TEAM_1].Players[i].PlayerName;
            Team1Players[i].PlayerGoals.text = MatchSystem.instance.Teams[TEAM_TYPE.TEAM_1].Players[i].GoalsScored.ToString();
            Team1Players[i].PlayerTackles.text = MatchSystem.instance.Teams[TEAM_TYPE.TEAM_1].Players[i].HasTackled.ToString();
            Team1Players[i].PlayerScore.text = (MatchSystem.instance.Teams[TEAM_TYPE.TEAM_1].Players[i].HasTackled * 10 + 100 * MatchSystem.instance.Teams[TEAM_TYPE.TEAM_1].Players[i].GoalsScored - 100 * MatchSystem.instance.Teams[TEAM_TYPE.TEAM_1].Players[i].Owngoal).ToString();
            Team1Players[i].PlayerPing.text = MatchSystem.instance.Teams[TEAM_TYPE.TEAM_1].Players[i].GetPing().ToString();
            MatchSystem.instance.Teams[TEAM_TYPE.TEAM_1].Players[i].Score = (MatchSystem.instance.Teams[TEAM_TYPE.TEAM_1].Players[i].HasTackled * 10 + 100 * MatchSystem.instance.Teams[TEAM_TYPE.TEAM_1].Players[i].GoalsScored - 100 * MatchSystem.instance.Teams[TEAM_TYPE.TEAM_1].Players[i].Owngoal);
        }
        for (int i = 0; i < Math.Min(Team2Players.Count, AmountOfPlayersInTeam2); i++)
        {
            if (MatchSystem.instance.Teams[TEAM_TYPE.TEAM_2].Players[i].PlayerName.Length > 9)
                Team2Players[i].PlayerName.text = MatchSystem.instance.Teams[TEAM_TYPE.TEAM_2].Players[i].PlayerName.Substring(0, 9) + "-";
            else
                Team2Players[i].PlayerName.text = MatchSystem.instance.Teams[TEAM_TYPE.TEAM_2].Players[i].PlayerName;
            Team2Players[i].PlayerGoals.text = MatchSystem.instance.Teams[TEAM_TYPE.TEAM_2].Players[i].GoalsScored.ToString();
            Team2Players[i].PlayerTackles.text = MatchSystem.instance.Teams[TEAM_TYPE.TEAM_2].Players[i].HasTackled.ToString();
            Team2Players[i].PlayerScore.text = (MatchSystem.instance.Teams[TEAM_TYPE.TEAM_2].Players[i].HasTackled * 10 + 100 * MatchSystem.instance.Teams[TEAM_TYPE.TEAM_2].Players[i].GoalsScored - 100 * MatchSystem.instance.Teams[TEAM_TYPE.TEAM_2].Players[i].Owngoal).ToString();
            Team2Players[i].PlayerPing.text = MatchSystem.instance.Teams[TEAM_TYPE.TEAM_2].Players[i].GetPing().ToString();
            MatchSystem.instance.Teams[TEAM_TYPE.TEAM_2].Players[i].Score = (MatchSystem.instance.Teams[TEAM_TYPE.TEAM_2].Players[i].HasTackled * 10 + 100 * MatchSystem.instance.Teams[TEAM_TYPE.TEAM_2].Players[i].GoalsScored - 100 * MatchSystem.instance.Teams[TEAM_TYPE.TEAM_2].Players[i].Owngoal);
        }
    }
    public void lastupdate()
    {
        AmountOfPlayersInTeam1 = MatchSystem.instance.Teams[TEAM_TYPE.TEAM_1].PlayerCount;
        AmountOfPlayersInTeam2 = MatchSystem.instance.Teams[TEAM_TYPE.TEAM_2].PlayerCount;
        UpdateStatistics();
        UpdatePlayerBars();
    }
    public void UpdateList()
    {
        if(MatchSystem.instance.Teams[TEAM_TYPE.TEAM_1].Players.Count>1)
            MatchSystem.instance.Teams[TEAM_TYPE.TEAM_1].Players.Sort((x, y) => y.Score.CompareTo(x.Score));
        if (MatchSystem.instance.Teams[TEAM_TYPE.TEAM_2].Players.Count > 1)
            MatchSystem.instance.Teams[TEAM_TYPE.TEAM_2].Players.Sort((x, y) => y.Score.CompareTo(x.Score));
    }

    public override void Update()
    {
        if(GUIScoreScreen.Instance.ToggleBool)
        { 
            AmountOfPlayersInTeam1 = MatchSystem.instance.Teams[TEAM_TYPE.TEAM_1].PlayerCount;
            AmountOfPlayersInTeam2 = MatchSystem.instance.Teams[TEAM_TYPE.TEAM_2].PlayerCount;
            UpdateStatistics();
            UpdatePlayerBars();
            UpdateList();
        }
        ChadHud.Instance.ToggleScoreVisability(Toggle);
        if (Toggle)
            DisplayBar(new Vector2(1.5f, 1.5f));//DisplayBar(Vector2.One);
        else
            DisplayBar(Vector2.Zero);
        if (((Input.GetKeyDown(Input.Keys.Tab) || Input.GetKeyDown(Input.Keys.I)) &&(MatchSystem.instance.MatchStarted)) || !GUIScoreScreen.Instance.ToggleBool)
        {
            if (!Toggle)
            {
                Toggle = true;
            }
        }
        if((Input.GetKeyUp(Input.Keys.Tab) || Input.GetKeyUp(Input.Keys.I)) )
        {
            Toggle = false;
        }
        SetOrigin();
    }
}
