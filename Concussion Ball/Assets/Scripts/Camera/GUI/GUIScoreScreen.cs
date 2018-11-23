using System;
using System.Collections;
using System.ComponentModel;
using System.Linq;
using ThomasEngine;
using ThomasEngine.Network;

public class GUIScoreScreen : ScriptComponent
{
    public static GUIScoreScreen Instance = null;
    Camera cam;
    Canvas Canvas;
    Text Lobby;
    Text Playagain;
    Text MainMenu;
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
        Lobby = Canvas.Add("LOBBY");
        Lobby.font = GUIPlayerScore.Instance.Font;
        Lobby.scale = Vector2.Zero;
        Lobby.position = new Vector2(0.2f, 0.04f);
    }
    public override void Update()
    {
       
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
            Lobby.scale = Vector2.Zero;
        }
        else
        {
            Lobby.scale = new Vector2(1.5f, 1.5f);
        }

    }
    public bool getToggleBool()
    {
        return ToggleBool;
    }
}
