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
    public Canvas Canvas;
    public int ScoreScreenTimeLast { get; set; } = 10;
    public int Timer;
    List<Text> Textdatalist = new List<Text>();
    Color Unselected = Color.FloralWhite;
    Color Selected = Color.IndianRed;
    bool Init = true;

    public override void OnAwake()
    {
        Instance = this;

        Timer = ScoreScreenTimeLast + 1;
        Canvas = gameObject.GetComponent<Camera>().AddCanvas();
        Textdatalist.Add(Canvas.Add("Play again: "));
        Textdatalist.Add(Canvas.Add("Main Menu"));
        Textdatalist.Add(Canvas.Add(""));
        for (int i = 0; i < Textdatalist.Count; i++)
        {
            Textdatalist[i].scale = new Vector2(0.5f);
            Textdatalist[i].interactable = true;
            Textdatalist[i].depth = 0.9f;
            Textdatalist[i].origin = new Vector2(0.5f);
        }
        Textdatalist[0].position = new Vector2(0.33f, 0.7f);
        Textdatalist[1].position = new Vector2(0.67f, 0.7f);
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

        }
        if (Textdatalist[2].Clicked()) // Main Menu
        {

            if (ThomasWrapper.IsPlaying())
            {
                Input.SetMouseMode(Input.MouseMode.POSITION_ABSOLUTE);
                CameraMaster.instance.State = CAM_STATE.LOADING_SCREEN;
                ThomasWrapper.IssueRestart();
            }
        }

        Textdatalist[2].position = Textdatalist[0].position + new Vector2(Textdatalist[0].size.x / 2 + Textdatalist[2].size.x / 2, 0);
    }

    public void UpdateTimer()
    {
        Textdatalist[2].text = (Timer -= 1).ToString();
    }
    
    public override void OnDisable()
    {
        if(Canvas != null)
            Canvas.isRendering = false;

        if (GUIScoreboard.Instance)
            GUIScoreboard.Instance.enabled = false;

        if (MatchSystem.instance.MatchStarted)
        {
            Input.SetMouseMode(Input.MouseMode.POSITION_RELATIVE);
            Init = true;
        }
    }

    public override void OnEnable()
    {
        if (Canvas != null)
            Canvas.isRendering = true;

        if (GUIScoreboard.Instance)
        {
            GUIScoreboard.Instance.enabled = true;
        }

        if(Init)
        {
            Timer = 10;
            Init = false;
        }
    }
}
