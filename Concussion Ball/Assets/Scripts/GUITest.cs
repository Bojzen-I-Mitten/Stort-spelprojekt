using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ThomasEngine;
using InputIP;

public class GUITest : ScriptComponent
{
    public Texture2D Background { get; set; }
    public Texture2D JoinBtn { get; set; }
    public Font TextFont { get; set; }
    private Vector4 OriginalColor;
    private Vector4 RedColor;
    Camera Camera;

    private string IP;
    private bool TakeIP = false;
    public override void Start()
    {
        IP = "";
        Camera = gameObject.GetComponent<Camera>();
        Camera.AddText("IP", IP, new Vector2(250, 150));
        Camera.SetTextColor("IP", new Vector4(0, 0, 0, 1));
        Camera.AddImage("BG", Background, new Vector2(200, 30), new Vector2(1), true);
        Camera.AddImage("Join", JoinBtn, new Vector2(700, 150), new Vector2(0.25f), true);
        OriginalColor = new Vector4(1.0f, 1.0f, 1.0f, 0.5f);
        RedColor = new Vector4(0.0f, 0.0f, 1.0f, 1.0f);
    }

    public override void Update()
    {
        if (TakeIP)
            InputGUI.AppendIPString(ref IP);
        if (Camera.OnImageClicked("BG"))
            TakeIP = !TakeIP;
        if (Camera.OnImageClicked("Join"))
            Debug.Log("Join lobby");
        Camera.SetText("IP", IP);
    }
}
