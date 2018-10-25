using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ThomasEngine;
using InputIP;

public class GUIJoinHost : ScriptComponent
{
    public Texture2D JoinBtn { get; set; }
    public Texture2D HostBtn { get; set; }
    public Texture2D TextBox { get; set; }
    public Font TextFont { get; set; }

    Camera Camera;

    private string IPText;
    private string PortText;
    private bool TakeIP;
    private bool TakePort;
    private bool Disabled;

    private readonly string TextBoxIP = "TextBoxIP";
    private readonly string TextBoxPort = "TextBoxPort";
    private readonly string BG = "BG";
    private readonly string Join = "Join";
    private readonly string Host = "Host";

    public override void Start()
    {
        IPText = "";
        PortText = "";
        TakeIP = false;
        TakePort = false;
        Disabled = false;
        Camera = gameObject.GetComponent<Camera>();
        AddImagesAndText();
    }

    public override void Update()
    {
        if (TakeIP)
            InputGUI.AppendIPString(ref IPText, 15);
        if (TakePort)
            InputGUI.AppendIPString(ref PortText, 5);


        if (Camera.OnImageClicked(TextBoxIP))
        {
            TakePort = false;
            TakeIP = true;
            Camera.SetImageColor(TextBoxPort, new Vector4(0.0f, 0.0f, 0.0f, 1.0f));
            Camera.SetImageColor(TextBoxIP, new Vector4(0.0f, 1.0f, 0.0f, 1.0f));
        }
        else if (Camera.OnImageClicked(TextBoxPort))
        {
            TakeIP = false;
            TakePort = true;
            Camera.SetImageColor(TextBoxIP, new Vector4(0.0f, 0.0f, 0.0f, 1.0f));
            Camera.SetImageColor(TextBoxPort, new Vector4(0.0f, 1.0f, 0.0f, 1.0f));
        }
        else if (Camera.OnImageClicked(BG))
        {
            TakePort = false;
            TakeIP = false;
            Camera.SetImageColor(TextBoxIP, new Vector4(0.0f, 0.0f, 0.0f, 1.0f));
            Camera.SetImageColor(TextBoxPort, new Vector4(0.0f, 0.0f, 0.0f, 1.0f));
        }



        if (Camera.OnImageClicked(Join))
        {
            if (IPText != "" && PortText != "")
            {
                MatchSystem.instance.LocalPort = 0;
                MatchSystem.instance.TargetPort = Convert.ToInt32(PortText);
                MatchSystem.instance.TargetIP = IPText;
                MatchSystem.instance.Init();
                MatchSystem.instance.Connect();

                this.enabled = false;
                gameObject.GetComponent<GUISelectTeam>().enabled = true;
                Disabled = true;
            }
            else
            {
                if (IPText == "")
                    Camera.SetImageColor(TextBoxIP, new Vector4(1.0f, 0.0f, 0.0f, 1.0f));
                if (PortText == "")
                    Camera.SetImageColor(TextBoxPort, new Vector4(1.0f, 0.0f, 0.0f, 1.0f));
            }

        }
        else if (Camera.OnImageClicked(Host))
        {
            if (PortText != "")
            {
                MatchSystem.instance.LocalPort = Convert.ToInt32(PortText);
                MatchSystem.instance.Init();
                MatchSystem.instance.Host();
                
                this.enabled = false;
                gameObject.GetComponent<GUISelectTeam>().enabled = true;
                Disabled = true;
            }
            else
            {
                Camera.SetImageColor(TextBoxPort, new Vector4(1.0f, 0.0f, 0.0f, 1.0f));
            }
        }

        if (!Disabled)
        {
            Camera.SetText("IPText", IPText);
            Camera.SetText("PortText", PortText);
            if (TextFont != null)
            {
                Camera.SetTextFont("IPText", TextFont);
                Camera.SetTextFont("PortText", TextFont);
                Camera.SetTextFont("IP", TextFont);
                Camera.SetTextFont("Port", TextFont);
            }
        }
        else
            ClearImagesAndText();
    }


    public void AddImagesAndText()
    {
        Disabled = false;

        Camera.AddText("IPText", IPText, new Vector2(0.1f, 0.11f), new Vector2(1.0f));
        Camera.AddText("PortText", PortText, new Vector2(0.1f, 0.21f), new Vector2(1.0f));
        Camera.AddText("IP", "IP, needed to join", new Vector2(0.1f, 0.07f), new Vector2(0.7f));
        Camera.AddText("Port", "PORT, needed for both host and join", new Vector2(0.1f, 0.17f), new Vector2(0.7f));

        Camera.AddImage(Join, JoinBtn, new Vector2(0.325f, 0.11f), new Vector2(0.25f), true);
        Camera.AddImage(Host, HostBtn, new Vector2(0.325f, 0.21f), new Vector2(0.25f), true);
        Camera.AddImage(TextBoxIP, TextBox, new Vector2(0.1f, 0.10f), new Vector2(0.70f, 0.5f), 0.0f, new Vector4(0.0f, 0.0f, 0.0f, 1.0f), true);
        Camera.AddImage(TextBoxPort, TextBox, new Vector2(0.1f, 0.20f), new Vector2(0.70f, 0.5f), 0.0f, new Vector4(0.0f, 0.0f, 0.0f, 1.0f), true);
    }

    public void ClearImagesAndText()
    {
        Camera.DeleteImage(Join);
        Camera.DeleteImage(Host);
        Camera.DeleteImage(TextBoxIP);
        Camera.DeleteImage(TextBoxPort);

        Camera.DeleteText("IPText");
        Camera.DeleteText("PortText");
        Camera.DeleteText("IP");
        Camera.DeleteText("Port");
    }
}