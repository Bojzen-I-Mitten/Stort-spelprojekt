using System;
using System.Collections;
using System.ComponentModel;
using System.Linq;
using ThomasEngine;

public class GUIPlayerScore : ScriptComponent
{
    Camera cam;
    Canvas Canvas;

    public String AmountOfPlayersTeam1 { get; set; }
    public String AmountOfPlayersTeam2 { get; set; }
    public String Test { get; set; } = "hm";
    public Texture2D PlayerBarTeam1 { get; set; }
    public Texture2D PlayerBarTeam2 { get; set; }
    public Texture2D Team1Bar { get; set; }
    public Texture2D Team2Bar { get; set; }
    public int AmountOfPlayersInTeam1 { get; set; } = 2;
    public int AmountOfPlayersInTeam2 { get; set; } = 2;
    public Texture2D AmountOfPlayersBar { get; set; }
    public Vector2 OriginText{get;set;}
    public Vector2 ScaleText { get; set; }
    private bool Toggle = false;
    public Font Font { get; set; }
    public override void Start()
    {
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
            PlayerStandardBarTeam1[i] = Canvas.Add(PlayerBarTeam1);
            PlayerStandardBarTeam1[i].scale = Vector2.Zero;
            PlayerStandardBarTeam1[i].color = Color.Red;
            PlayerStandardBarTeam1[i].origin = new Vector2(0, -3.75f-(i*0.75f));
        }
        for (int i = 0; i < AmountOfPlayersInTeam2; i++)
        {
            PlayerStandardBarTeam2[i] = Canvas.Add(PlayerBarTeam2);
            PlayerStandardBarTeam2[i].scale = Vector2.Zero;
            PlayerStandardBarTeam2[i].color = Color.Blue;
            PlayerStandardBarTeam2[i].origin = new Vector2(-0.84f, -3.75f - (i * 0.75f));
        }

    }


    Image AmountOfPlayersBarImage;
    Text TeamAmountOfPlayersText;

    Image team1BarImage;
    Image team2BarImage;

    Text[] PlayerstandardText = new Text[2];
    Image[] PlayerStandardbar = new Image[2];
    Image[] PlayerStandardBarTeam1 = new Image[15];
    Image[] PlayerStandardBarTeam2 = new Image[15];
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

    //    PlayerStandardbar[1].origin = OriginImage;
    //      PlayerstandardText[1].origin = OriginText;
    //      PlayerstandardText[1].text = Test;
    //      PlayerstandardText[1].scale = ScaleText;

    }
    void UpdatePlayerBars()
    {

    }

    public override void Update()
    {

        if (Input.GetKeyDown(Input.Keys.Tab) || Input.GetKeyDown(Input.Keys.LeftControl))
        {
            if (!Toggle)
            {
                DisplayBar(new Vector2(1.5f, 1.5f));//DisplayBar(Vector2.One);
                  Toggle = true;
            }
            else
            {
                DisplayBar(Vector2.Zero);
                Toggle = false;
            }
        }
        SetOrigin();
    }
}
