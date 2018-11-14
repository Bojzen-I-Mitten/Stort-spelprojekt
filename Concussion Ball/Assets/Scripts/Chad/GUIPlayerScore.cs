using System;
using System.Collections;
using System.ComponentModel;
using System.Linq;
using ThomasEngine;
using System.Collections.Generic;
public class PlayerTestClass
{
    String Name;
    int goals;
    int Tackles;
    int Tackled;
    int Ping;
   public PlayerTestClass(String Name,int goals,int Tackles,int Tackled,int Ping)
    {
        this.Name = Name;
        this.goals = goals;
        this.Tackles = Tackles;
        this.Tackled = Tackled;
        this.Ping = Ping;
    }

}
public class Playertext
{
    public Text PlayerName;
    public Text PlayerGoals;
    public Text PlayerTackles;
    public Text PlayerTackled;
    public Text PlayerPing;
    Canvas Canvas;
    
    public Playertext(int Number,Font font,Canvas Canvas,String Name, int goals, int Tackles, int Tackled, int Ping, TEAM_TYPE team)
    {
        this.Canvas = Canvas;
        this.PlayerName = Canvas.Add(Name);
        this.PlayerGoals = Canvas.Add(""+goals);
        this.PlayerTackles = Canvas.Add("" + Tackles);
        this.PlayerTackled = Canvas.Add("" + Tackled);
        this.PlayerPing = Canvas.Add("" + Ping);
        this.PlayerName.origin = new Vector2(0, 0);
        this.PlayerGoals.origin = new Vector2(0, 0);
        this.PlayerTackles.origin = new Vector2(0, 0);
        this.PlayerTackled.origin = new Vector2(0, 0);
        this.PlayerPing.origin = new Vector2(0, 0);
        AddFont(font);
        UpdateScale(Vector2.Zero);
        UpdateColor(Color.White);
        if (team == TEAM_TYPE.TEAM_1)
            UpdatePositionTeam1(Number);
        else
            UpdatePositionTeam2(Number);
    }
    public void UpdateScale(Vector2 scale)
    {
        this.PlayerName.scale = scale;
        this.PlayerGoals.scale = scale;
        this.PlayerTackles.scale = scale;
        this.PlayerTackled.scale = scale;
        this.PlayerPing.scale = scale;
    }
    public void AddFont(Font font)
    {
        this.PlayerName.font = font;
        this.PlayerGoals.font = font;
        this.PlayerTackles.font = font;
        this.PlayerTackled.font = font;
        this.PlayerPing.font = font;
    } 

    public void RemoveCanvas()
    {
        Canvas.Remove(this.PlayerName);
        Canvas.Remove(this.PlayerGoals);
        Canvas.Remove(this.PlayerTackles);
        Canvas.Remove(this.PlayerTackled);
        Canvas.Remove(this.PlayerPing);
    }

    public void UpdatePositionTeam1(int NewPositionUpdate)
    {
        float PositionUpdate = NewPositionUpdate * 0.064f;
        this.PlayerName.position    = new Vector2(0.020f, 0.34f + PositionUpdate);
        this.PlayerGoals.position   = new Vector2(0.179f, 0.34f + PositionUpdate);
        this.PlayerTackles.position = new Vector2(0.268f, 0.34f + PositionUpdate);
        this.PlayerTackled.position = new Vector2(0.364f, 0.34f + PositionUpdate);
        this.PlayerPing.position    = new Vector2(0.445f, 0.34f + PositionUpdate);
    }
    public void UpdatePositionTeam2(int NewPositionUpdate)
    {
        float PositionUpdate = NewPositionUpdate * 0.064f;
        this.PlayerName.position    = new Vector2(0.870f, 0.34f + PositionUpdate);
        this.PlayerGoals.position   = new Vector2(0.800f, 0.34f + PositionUpdate);
        this.PlayerTackles.position = new Vector2(0.700f, 0.34f + PositionUpdate);
        this.PlayerTackled.position = new Vector2(0.615f, 0.34f + PositionUpdate);
        this.PlayerPing.position    = new Vector2(0.525f, 0.34f + PositionUpdate);
    }

    void UpdateColor(Color color)
    {
        this.PlayerName.color = color;
        this.PlayerGoals.color = color;
        this.PlayerTackles.color = color;
        this.PlayerTackled.color = color;
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
    public int AmountOfPlayersInTeam1 { get; set; } = 2;
    public int AmountOfPlayersInTeam2 { get; set; } = 2;
    public Texture2D AmountOfPlayersBar { get; set; }
    public Vector2 ScaleText { get; set; }
    private bool Toggle = false;
    public Font Font { get; set; }
    public List<NetworkPlayer> NetworkPlayersTeam1;
    public List<NetworkPlayer> NetworkPlayersTeam2;

    Image AmountOfPlayersBarImage;
    Text TeamAmountOfPlayersText;
    Image team1BarImage;
    Image team2BarImage;
    List<Playertext> Team1Players = new List<Playertext>();
    List<Playertext> Team2Players = new List<Playertext>();
    Text[] PlayerstandardText = new Text[2];
    Image[] PlayerStandardbar = new Image[2];
    List<Image> PlayerStandardBarTeam1 = new List<Image>();
    List<Image> PlayerStandardBarTeam2 = new List<Image>();

    public override void Start()
    {
 
       // PlayerTestClassTestTeam1.Add(new PlayerTestClass("TestNameTeam1", 1, 1, 1, 50));
       // PlayerTestClassTestTeam2.Add(new PlayerTestClass("TestNameTeam2", 1, 2, 1, 50));
        //testa koden här senare:P
        //AmountOfPlayersInTeam1 = MatchSystem.instance.FindTeam(TEAM_TYPE.TEAM_1).PlayerCount;
        //AmountOfPlayersInTeam2 = MatchSystem.instance.FindTeam(TEAM_TYPE.TEAM_2).PlayerCount;
        //NetworkPlayersTeam1 = MatchSystem.instance.FindTeam(TEAM_TYPE.TEAM_1).Players;
        //NetworkPlayersTeam2 = MatchSystem.instance.FindTeam(TEAM_TYPE.TEAM_2).Players;

        cam = gameObject.GetComponent<Camera>();
        Canvas = cam.AddCanvas();
     
        //Team1Bar Image
        team1BarImage = Canvas.Add(Team1Bar);
        team1BarImage.scale = Vector2.Zero;
        team1BarImage.color = Color.Red;
        team1BarImage.origin = new Vector2(0, -2.025f);

        //Team2Bar Image
        team2BarImage = Canvas.Add(Team2Bar);
        team2BarImage.scale = Vector2.Zero;
        team2BarImage.color = Color.Blue;
        team2BarImage.origin = new Vector2(-1.48f, -2.025f);

        //AmountOfPlayersBar Image
        AmountOfPlayersBarImage = Canvas.Add(AmountOfPlayersBar);
        AmountOfPlayersBarImage.scale = Vector2.Zero;
        AmountOfPlayersBarImage.color = Color.Blue;
        AmountOfPlayersBarImage.origin = new Vector2(-1.92f, -0.8f);

        //playerBar.color = MatchSystem.instance.Teams[TEAM_TYPE.TEAM_1].Color;
        TeamAmountOfPlayersText = Canvas.Add(AmountOfPlayersInTeam1 + "    "+ AmountOfPlayersInTeam2);
        TeamAmountOfPlayersText.color = Color.White;
        TeamAmountOfPlayersText.scale = Vector2.Zero;
        TeamAmountOfPlayersText.origin = new Vector2(-3.5f, -2.65f);

        PlayerStandardbar[0] = Canvas.Add(PlayerBarTeam1);
        PlayerStandardbar[1] = Canvas.Add(PlayerBarTeam2);
        PlayerstandardText[1] = Canvas.Add("Ping  Tackled  Tackles  Goals   Player");
        PlayerstandardText[0] = Canvas.Add("    Player    Goals   Tackles Tackled   Ping");
        for (int i = 0; i < 2; i++)
        {
            PlayerStandardbar[i].scale = Vector2.Zero;
            PlayerstandardText[i].font = Font;
            PlayerstandardText[i].scale = Vector2.Zero;
        }
        PlayerstandardText[0].origin = new Vector2(-0.01f, -10.2f);
        PlayerstandardText[1].origin = new Vector2(-1.25f, -10.2f);
        PlayerStandardbar[0].origin = new Vector2(0, -3);
        PlayerStandardbar[0].color = Color.Red;
        PlayerStandardbar[1].color = Color.Blue;
        PlayerStandardbar[1].origin = new Vector2(-0.84f, -3);

    }

    void DisplayBar(Vector2 OnOff)
    {
        
        TeamAmountOfPlayersText.scale = OnOff;
        AmountOfPlayersBarImage.scale = OnOff;
        team1BarImage.scale = OnOff;
        team2BarImage.scale = OnOff;
        for (int i = 0; i < 2; i++)
        {
            PlayerStandardbar[i].scale = OnOff;
            if(OnOff.x == 0)
                PlayerstandardText[i].scale = new Vector2(0, 0);
            else
               PlayerstandardText[i].scale = new Vector2(0.8f,0.6f);
        }

        if(OnOff.x != 0)
        { 
            for(int i = 0;i< Team1Players.Count;i++)
            {
                Team1Players[i].UpdateScale(new Vector2(0.8f, 0.6f));
            }
            for (int i = 0; i < Team2Players.Count; i++)
            {
                Team2Players[i].UpdateScale(new Vector2(0.8f, 0.6f));
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
            //playerBar.color = MatchSystem.instance.Teams[TEAM_TYPE.TEAM_1].Color;

            Canvas.Remove(TeamAmountOfPlayersText);
            TeamAmountOfPlayersText = Canvas.Add(AmountOfPlayersInTeam1 + "    " + AmountOfPlayersInTeam2);
            TeamAmountOfPlayersText.color = Color.White;
            TeamAmountOfPlayersText.scale = Vector2.Zero;
            TeamAmountOfPlayersText.position = new Vector2(0.435f, 0.1755f);
            //TeamAmountOfPlayersText.origin = new Vector2(-3.5f, -2.65f);

            if (PlayerStandardBarTeam2.Count< AmountOfPlayersInTeam2)
            {
                
                while (PlayerStandardBarTeam2.Count != AmountOfPlayersInTeam2)
                {
                    int i = PlayerStandardBarTeam2.Count;
                    PlayerStandardBarTeam2.Add(Canvas.Add(PlayerBarTeam2));
                    PlayerStandardBarTeam2[i].scale = Vector2.Zero;
                    PlayerStandardBarTeam2[i].color = Color.Blue;
                    PlayerStandardBarTeam2[i].origin = new Vector2(-0.84f, -3.75f - (i * 0.75f));
                    Team2Players.Add(new Playertext(i, Font, Canvas, "jonn " + i, 2, 34, 4, 94, TEAM_TYPE.TEAM_2));
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
                    Team2Players.Add(new Playertext(i, Font, Canvas, "jonn " + i, 2, 34, 4, 94, TEAM_TYPE.TEAM_2));
                }
            }

            if (PlayerStandardBarTeam1.Count < AmountOfPlayersInTeam1)
            {
             
                while (PlayerStandardBarTeam1.Count != AmountOfPlayersInTeam1)
                {
                    int i = PlayerStandardBarTeam1.Count;
                    PlayerStandardBarTeam1.Add(Canvas.Add(PlayerBarTeam1));
                    PlayerStandardBarTeam1[i].scale = Vector2.Zero;
                    PlayerStandardBarTeam1[i].color = Color.Red;
                    PlayerStandardBarTeam1[i].origin = new Vector2(0, -3.75f - (i * 0.75f));
                    Team1Players.Add(new Playertext(i, Font, Canvas, "jonn " + i, 2, 34, 4, 94, TEAM_TYPE.TEAM_1));
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
                    Team1Players.Add(new Playertext(i, Font, Canvas, "jonn " + i, 2, 34, 4, 94, TEAM_TYPE.TEAM_1));
                }
            }
        }
    }

    public override void Update()
    {
        UpdatePlayerBars();
        if (Toggle)
            DisplayBar(new Vector2(1.5f, 1.5f));//DisplayBar(Vector2.One);
        else
            DisplayBar(Vector2.Zero);
        if (Input.GetKeyDown(Input.Keys.Tab) || Input.GetKeyDown(Input.Keys.LeftControl))
        {
            if (!Toggle)
            {
             //   UpdatePlayerBars();
               // DisplayBar(new Vector2(1.5f, 1.5f));//DisplayBar(Vector2.One);
                  Toggle = true;
            }
            else
            {
           //     UpdatePlayerBars();
              //  DisplayBar(Vector2.Zero);
                Toggle = false;
            }
        }



        SetOrigin();
    }
}
