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

    public Playertext(int Number,Font font,Canvas Canvas,String Name, int goals, int Tackles, int Tackled, int Ping,int team)
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
        if (team == 1)
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
    ~Playertext()
    {
        Canvas.Remove(this.PlayerName);
        Canvas.Remove(this.PlayerGoals);
        Canvas.Remove(this.PlayerTackles);
        Canvas.Remove(this.PlayerTackled);
        Canvas.Remove(this.PlayerPing);
    }
    public void UpdatePositionTeam1(int NewPositionUpdate)
    {
        float PositionUpdate = NewPositionUpdate * 0.065f;
        this.PlayerName.position    = new Vector2(0.020f, 0.34f + PositionUpdate);
        this.PlayerGoals.position   = new Vector2(0.179f, 0.34f + PositionUpdate);
        this.PlayerTackles.position = new Vector2(0.268f, 0.34f + PositionUpdate);
        this.PlayerTackled.position = new Vector2(0.364f, 0.34f + PositionUpdate);
        this.PlayerPing.position    = new Vector2(0.445f, 0.34f + PositionUpdate);
    }
    public void UpdatePositionTeam2(int NewPositionUpdate)
    {
        float PositionUpdate = NewPositionUpdate * 0.065f;
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
    public String AmountOfPlayersTeam1 { get; set; }
    public String AmountOfPlayersTeam2 { get; set; }
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
    public List<PlayerTestClass> PlayerTestClassTestTeam1;
    public List<PlayerTestClass> PlayerTestClassTestTeam2;
    public int OriginText { get; set; }
    public Vector2 OriginText1 { get; set; }
    public Vector2 OriginText2 { get; set; }
    public Vector2 OriginText3 { get; set; }
    public Vector2 OriginText4 { get; set; }
    public Vector2 OriginText5 { get; set; }
    Image AmountOfPlayersBarImage;
    Text TeamAmountOfPlayersText;

    Image team1BarImage;
    Image team2BarImage;
    Playertext Playertest;
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


        //       playerBar.color = MatchSystem.instance.Teams[TEAM_TYPE.TEAM_1].Color;
        TeamAmountOfPlayersText = Canvas.Add(AmountOfPlayersTeam1+"    "+ AmountOfPlayersTeam2);
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


       


        for(int i=0;i< AmountOfPlayersInTeam1;i++)
        {
           
            PlayerStandardBarTeam1.Add(Canvas.Add(PlayerBarTeam1));
            PlayerStandardBarTeam1[i].scale = Vector2.Zero;
            PlayerStandardBarTeam1[i].color = Color.Red;
            PlayerStandardBarTeam1[i].origin = new Vector2(0, -3.75f-(i*0.75f));
        }
        for (int i = 0; i < AmountOfPlayersInTeam2; i++)
        {
            PlayerStandardBarTeam2.Add(Canvas.Add(PlayerBarTeam2));
            PlayerStandardBarTeam2[i].scale = Vector2.Zero;
            PlayerStandardBarTeam2[i].color = Color.Blue;
            PlayerStandardBarTeam2[i].origin = new Vector2(-0.84f, -3.75f - (i * 0.75f));
        }

        Playertest = new Playertext(0, Font, Canvas, "jonn", 2, 34, 4, 94,2);
    }



    public override void Awake()
    {

        
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

        if (OnOff.x == 0)
            Playertest.UpdateScale(Vector2.Zero);
        else
            Playertest.UpdateScale(new Vector2(0.8f, 0.6f));

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
        //Playertest.UpdatePositionTeam2(OriginText);
     /*   Playertest.PlayerName.position = OriginText1;
        Playertest.PlayerGoals.position = OriginText2;
        Playertest.PlayerTackles.position = OriginText3;
        Playertest.PlayerTackled.position = OriginText4;
        Playertest.PlayerPing.position = OriginText5;*/
    }
    void UpdatePlayerBars()
    {
        if(PlayerStandardBarTeam2.Count != AmountOfPlayersInTeam2 || PlayerStandardBarTeam1.Count != AmountOfPlayersInTeam1)
        { 
            if(PlayerStandardBarTeam2.Count< AmountOfPlayersInTeam2)
            {
                
                while (PlayerStandardBarTeam2.Count != AmountOfPlayersInTeam2)
                {
                    int i = PlayerStandardBarTeam2.Count;
                    PlayerStandardBarTeam2.Add(Canvas.Add(PlayerBarTeam2));
                    PlayerStandardBarTeam2[i].scale = Vector2.Zero;
                    PlayerStandardBarTeam2[i].color = Color.Blue;
                    PlayerStandardBarTeam2[i].origin = new Vector2(-0.84f, -3.75f - (i * 0.75f));
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
            }
        }
    }

    public override void Update()
    {

        if (Input.GetKeyDown(Input.Keys.Tab) || Input.GetKeyDown(Input.Keys.LeftControl))
        {
            if (!Toggle)
            {
                UpdatePlayerBars();
                DisplayBar(new Vector2(1.5f, 1.5f));//DisplayBar(Vector2.One);
                  Toggle = true;
            }
            else
            {
                UpdatePlayerBars();
                DisplayBar(Vector2.Zero);
                Toggle = false;
            }
        }
        SetOrigin();
    }
}
