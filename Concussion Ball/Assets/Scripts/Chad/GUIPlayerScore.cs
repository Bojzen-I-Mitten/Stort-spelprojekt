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
    public Texture2D PlayerBar { get; set; }
    public Texture2D Team1Bar { get; set; }
    public Texture2D Team2Bar { get; set; }
    public Texture2D AmountOfPlayersBar { get; set; }
    public Vector2 OriginText{get;set;}
    public Vector2 OriginImage { get; set; }
    private bool Toggle = false;
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
        team2BarImage.flip = Vector2.One;
        team2BarImage.origin = new Vector2(-1.48f, -1.9f);
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

        for(int i = 0;i<2;i++)
        {
            PlayerStandardbar[i] = Canvas.Add(PlayerBar);
            PlayerStandardbar[i].scale = Vector2.Zero;

            PlayerstandardText[i] = Canvas.Add("hm");
            PlayerstandardText[i].scale = Vector2.Zero;
        }
        PlayerStandardbar[0].origin = new Vector2(0, -3);
        PlayerStandardbar[0].color = Color.Red;
        PlayerStandardbar[1].color = Color.Blue;
        PlayerStandardbar[1].flip = Vector2.One;
        PlayerStandardbar[1].origin = new Vector2(-0.84f, -2.85f);
    }


    Image AmountOfPlayersBarImage;
    Text TeamAmountOfPlayersText;

    Image team1BarImage;
    Image team2BarImage;

    Text[] PlayerstandardText = new Text[2];
    Image[] PlayerStandardbar = new Image[2];

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
            PlayerstandardText[i].scale = Vector2.One;
        }

    }
    void SetOrigin()
    {

    //    PlayerStandardbar[1].origin = OriginImage;
     //   PlayerstandardText[0].origin = OriginText;
     //   PlayerstandardText[0].text = Test;
        

    }
    public override void Update()
    {
       // Debug.Log("KAtt");
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
