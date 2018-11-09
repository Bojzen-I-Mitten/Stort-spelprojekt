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

    public Texture2D Team1Bar { get; set; }
    public Texture2D Team2Bar { get; set; }
    public Texture2D AmountOfPlayersBar { get; set; }
    public Vector2 Origin{get;set;}
    private bool Toggle = false;
    public override void Start()
    {
        cam = gameObject.GetComponent<Camera>();
        Canvas = cam.AddCanvas();

        //Team1Bar Image
        team1BarImage = Canvas.Add(Team1Bar);
        team1BarImage.scale = Vector2.Zero;
        team1BarImage.color = Color.Aqua;

        //Team2Bar Image
        team2BarImage = Canvas.Add(Team2Bar);
        team2BarImage.scale = Vector2.Zero;
        team2BarImage.color = Color.Aqua;

        //AmountOfPlayersBar Image
        AmountOfPlayersBarImage = Canvas.Add(AmountOfPlayersBar);
        AmountOfPlayersBarImage.scale = Vector2.Zero;
        AmountOfPlayersBarImage.color = Color.Aqua;
        AmountOfPlayersBarImage.origin = new Vector2(-3.5f, -1.5f);


 //       playerBar.color = MatchSystem.instance.Teams[TEAM_TYPE.TEAM_1].Color;

        TeamAmountOfPlayersText = Canvas.Add(AmountOfPlayersTeam1+"    "+ AmountOfPlayersTeam2);
        TeamAmountOfPlayersText.color = Color.White;
        TeamAmountOfPlayersText.scale = Vector2.Zero;
        TeamAmountOfPlayersText.origin = new Vector2(-6.15f, -3);
    }


    Text TeamAmountOfPlayersText;
    Image AmountOfPlayersBarImage;
    Image team1BarImage;
    Image team2BarImage;
    public override void Awake()
    {

        
    }
    void DisplayBar(Vector2 OnOff)
    {

        TeamAmountOfPlayersText.scale = OnOff;
        AmountOfPlayersBarImage.scale = OnOff;
    }
    void SetOrigin()
    {

      //   TeamAmountOfPlayersText.origin = Origin;
        


    }
    public override void Update()
    {
       // Debug.Log("KAtt");
        if (Input.GetKeyDown(Input.Keys.Tab) || Input.GetKeyDown(Input.Keys.LeftControl))
        {
            if (!Toggle)
            {
                DisplayBar(Vector2.One);
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
