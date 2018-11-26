using System;
using System.Collections;
using System.ComponentModel;
using System.Linq;
using ThomasEngine;
using ThomasEngine.Network;
using System.Collections.Generic;
public class GUIScoreScreen : ScriptComponent
{
    public static GUIScoreScreen Instance = null;
    Camera cam;
    Canvas Canvas;
    public int ScoreScreenTimeLast { get; set; } = 10;
    List<Text> Textdatalist = new List<Text>();
    Color Unselected = Color.FloralWhite;
    Color Selected = Color.IndianRed;
    public Font Font { get; set; }
    public bool ToggleBool = true;
    public override void Awake()
    {
        if (!Instance)
            Instance = this;
        else
        {
            Destroy(this);
            return;
        }
        ToggleBool = true;
    }
    public override void Start()
    {
        Canvas = gameObject.GetComponent<Camera>().AddCanvas();
        ToggleBool = true;
        Textdatalist.Add(Canvas.Add("Lobby"));
        Textdatalist.Add(Canvas.Add("Play again"));
        Textdatalist.Add(Canvas.Add("Main Menu"));
        for(int i = 0;i<Textdatalist.Count;i++)
        {
            Textdatalist[i].font = Font;
            Textdatalist[i].scale = Vector2.Zero;
            Textdatalist[i].interactable = true;
            Textdatalist[i].depth = 0.9f;
        }
        Textdatalist[0].position = new Vector2(0.15f, 0.7f);
        Textdatalist[1].position = new Vector2(0.35f, 0.7f);
        Textdatalist[2].position = new Vector2(0.65f, 0.7f);

        //        Lobby.position = new Vector2(0.2f, 0.04f);
    }
    public override void Update()
    {
        foreach (Text Textdata in Textdatalist)
        { 
                if (Textdata.Hovered())
            {
                Textdata.color = Selected;
            }
            else
                Textdata.color = Unselected;
        }


        if (Textdatalist[0].Clicked())//Lobby
        {

        }
        if (Textdatalist[1].Clicked())//Play again
        {
            foreach (Text Textdata in Textdatalist)
                Textdata.scale = Vector2.Zero;
            Debug.Log("button was pressed");
            ToggleBool = true;

        }
        if (Textdatalist[2].Clicked()) // Main Menu
        {

        }

        if (MatchSystem.instance.PlayAgain != null)
            Textdatalist[1].text = "Play again : ";//+MatchSystem.instance.PlayAgain.timeLeft;

    }
    public void DisplayBar(Vector2 OnOff)
    {

    }
   public void Toggle(bool OnOff)
    {
        ToggleBool = OnOff;
        GUIPlayerScore.Instance.lastupdate();
        if (!OnOff)
        { 
            GUIPlayerScore.Instance.Toggle = false;
            for (int i = 0; i < Textdatalist.Count; i++)
                Textdatalist[i].scale = new Vector2(1.5f);
            
        }
        else
        {
            foreach (Text Textdata in Textdatalist)
                Textdata.scale = Vector2.Zero;
        }

        
  
        
        

    }
    public bool getToggleBool()
    {
        return ToggleBool;
    }
}
