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
    public Texture2D HostBtn { get; set; }
    public Texture2D TextBox { get; set; }
    public Font TextFont { get; set; }

    Camera Camera;

    private string IPText;
    private string PortText;
    private bool TakeIP;
    private bool TakePort;

    public override void Start()
    {
        IPText = "";
        PortText = "";
        TakeIP = false;
        TakePort = false;
        Camera = gameObject.GetComponent<Camera>();

        if (TextFont != null)
        {
            Camera.AddText("IPText", IPText, new Vector2(250, 150), TextFont);
            Camera.AddText("PortText", PortText, new Vector2(250, 250), TextFont);
        }
        else
        {
            Camera.AddText("IPText", IPText, new Vector2(250, 150));
            Camera.AddText("PortText", PortText, new Vector2(250, 250));
        }

        Camera.AddText("IP", "IP, needed to join", new Vector2(260, 120), new Vector2(0.5f));
        Camera.AddText("Port", "PORT, needed for both host and join", new Vector2(260, 220), new Vector2(0.5f));

        Camera.AddImage("BG", Background, new Vector2(200, 30), new Vector2(1), true);
        Camera.AddImage("Join", JoinBtn, new Vector2(700, 152), new Vector2(0.25f), true);
        Camera.AddImage("TextBoxIP", TextBox, new Vector2(250, 145), new Vector2(0.70f, 0.5f), 0.0f, new Vector4(0.0f, 0.0f, 0.0f, 1.0f), true);
        Camera.AddImage("TextBoxPort", TextBox, new Vector2(250, 245), new Vector2(0.70f, 0.5f), 0.0f, new Vector4(0.0f, 0.0f, 0.0f, 1.0f), true);
        Camera.AddImage("Host", HostBtn, new Vector2(700, 252), new Vector2(0.25f), true);
        
    }

    public override void Update()
    {
        if (TakeIP)
            InputGUI.AppendIPString(ref IPText, 15);
        if (TakePort)
            InputGUI.AppendIPString(ref PortText, 5);


        if (Camera.OnImageClicked("TextBoxIP"))
        {
            TakePort = false;
            TakeIP = true;
            Camera.SetImageColor("TextBoxPort", new Vector4(0.0f, 0.0f, 0.0f, 1.0f));
            Camera.SetImageColor("TextBoxIP", new Vector4(0.0f, 1.0f, 0.0f, 1.0f));
        }
        else if (Camera.OnImageClicked("TextBoxPort"))
        {
            TakeIP = false;
            TakePort = true;
            Camera.SetImageColor("TextBoxIP", new Vector4(0.0f, 0.0f, 0.0f, 1.0f));
            Camera.SetImageColor("TextBoxPort", new Vector4(0.0f, 1.0f, 0.0f, 1.0f));
        }
        else if (Camera.OnImageClicked("BG"))
        {
            TakePort = false;
            TakeIP = false;
            Camera.SetImageColor("TextBoxIP", new Vector4(0.0f, 0.0f, 0.0f, 1.0f));
            Camera.SetImageColor("TextBoxPort", new Vector4(0.0f, 0.0f, 0.0f, 1.0f));
        }



        if (Camera.OnImageClicked("Join"))
        {
            if (IPText != "" && PortText != "")
            {
                MatchSystem.instance.LocalPort = IPText == "127.0.0.1" ? 0 : Convert.ToInt32(PortText);
                MatchSystem.instance.TargetPort = Convert.ToInt32(PortText);
                MatchSystem.instance.TargetIP = IPText;
                MatchSystem.instance.Init();
                MatchSystem.instance.Connect();
                Camera.enabled = false;
                this.enabled = false;
            }
            else
            {
                if(IPText == "")
                    Camera.SetImageColor("TextBoxIP", new Vector4(1.0f, 0.0f, 0.0f, 1.0f));
                if(PortText == "")

                    Camera.SetImageColor("TextBoxPort", new Vector4(1.0f, 0.0f, 0.0f, 1.0f));
            }
                
        }
        if (Camera.OnImageClicked("Host"))
        {
            if (PortText != "")
            {
                MatchSystem.instance.LocalPort = Convert.ToInt32(PortText);
                MatchSystem.instance.Init();
                MatchSystem.instance.Host();
                Camera.enabled = false;
                this.enabled = false;
            }
            else
            {
                Camera.SetImageColor("TextBoxPort", new Vector4(1.0f, 0.0f, 0.0f, 1.0f));
                Camera.AddText("IP", "IP, needed to join", new Vector2(260, 20), new Vector2(0.5f));
            }
        }

        Camera.SetText("IPText", IPText);
        Camera.SetText("PortText", PortText);
        if (TextFont != null)
        {
            Camera.SetTextFont("IPText", TextFont);
            Camera.SetTextFont("PortText", TextFont);
        }
    }
}
