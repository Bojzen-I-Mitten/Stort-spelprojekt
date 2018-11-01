using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ThomasEngine;
using InputIP;
using System.Collections;

public class GUIJoinHost : ScriptComponent
{
    public Texture2D JoinBtn { get; set; }
    public Texture2D HostBtn { get; set; }
    public Texture2D TextBox { get; set; }
    public Font TextFont { get; set; }

    Camera Camera;

    private bool Disabled;
    private string IPString;
    private string PortString;
    private bool TakeIP;
    private bool TakePort;

    Canvas GUI;

    Image BG;

    Image TextBoxIP;
    Image TextBoxPort;
    Image Join;
    Image Host;

    Text IPText;
    Text PortText;
    Text IP;
    Text Port;

    public override void Start()
    {
        IPString = "";
        PortString = "";
        TakeIP = false;
        TakePort = false;
        Disabled = false;
        Camera = gameObject.GetComponent<Camera>();
        AddImagesAndText();
    }

    public override void Update()
    {
        if (TakeIP)
            InputGUI.AppendIPString(ref IPString, 15);
        if (TakePort)
            InputGUI.AppendIPString(ref PortString, 5);

        if(Input.GetMouseButtonDown(Input.MouseButtons.LEFT))
        {
            if(TextBoxIP.Clicked())
            {
                TakePort = false;
                TakeIP = true;
                TextBoxPort.color = Color.Black;
                TextBoxIP.color = Color.Green;
            }
            else if(TextBoxPort.Clicked())
            {
                TakeIP = false;
                TakePort = true;
                TextBoxIP.color = Color.Black;
                TextBoxPort.color = Color.Green;
            }
            else if (Join.Clicked())
            {
                if (IPString != "" && PortString != "")
                {
                    MatchSystem.instance.LocalPort = 0;
                    MatchSystem.instance.TargetPort = Convert.ToInt32(PortString);
                    MatchSystem.instance.TargetIP = IPString;
                    MatchSystem.instance.Init();
                    MatchSystem.instance.Connect();
                    
                    ClearImagesAndText();
                    enabled = false;
                    return;
                }
                else
                {
                    if (IPString == "")
                        TextBoxIP.color = Color.Red;
                    if (PortString == "")
                        TextBoxPort.color = Color.Red;
                }

            }
            else if (Host.Clicked())
            {
                if (PortString != "")
                {
                    MatchSystem.instance.LocalPort = Convert.ToInt32(PortString);
                    MatchSystem.instance.Init();
                    MatchSystem.instance.Host();
                    
                    ClearImagesAndText();
                    enabled = false;
                    return;
                }
                else
                {
                    TextBoxPort.color = Color.Red;
                }
            }
            else
            {
                TakePort = false;
                TakeIP = false;
                TextBoxIP.color = Color.Black;
                TextBoxPort.color = Color.Black;
            }
        }

        if (!Disabled)
        {
            IPText.text = IPString;
            PortText.text = PortString;
            if (TextFont != null)
            {
                IPText.font = TextFont;
                PortText.font = TextFont;
                IP.font = TextFont;
            }
        }
    }


    public void AddImagesAndText()
    {
        GUI = Camera.AddCanvas();

        IPText = GUI.Add(IPString);
        IPText.position = new Vector2(0.1f, 0.11f);

        PortText = GUI.Add(PortString);
        PortText.position = new Vector2(0.1f, 0.21f);

        IP = GUI.Add("IP, needed to join");
        IP.position = new Vector2(0.1f, 0.07f);

        IP.scale = new Vector2(0.7f);
        Port = GUI.Add("PORT, needed for both host and join");

        Port.position = new Vector2(0.1f, 0.17f);
        Port.scale = new Vector2(0.7f);

        Join = GUI.Add(JoinBtn);
        Join.position = new Vector2(0.325f, 0.11f);
        Join.scale = new Vector2(0.25f);
        Join.interactable = true;

        Host = GUI.Add(HostBtn);
        Host.position = new Vector2(0.325f, 0.21f);
        Host.scale = new Vector2(0.25f);
        Host.interactable = true;

        TextBoxIP = GUI.Add(TextBox);
        TextBoxIP.position = new Vector2(0.1f);
        TextBoxIP.scale = new Vector2(0.7f, 0.5f);
        TextBoxIP.color = Color.Black;
        TextBoxIP.interactable = true;

        TextBoxPort = GUI.Add(TextBox);
        TextBoxPort.position = new Vector2(0.1f, 0.2f);
        TextBoxPort.scale = new Vector2(0.7f, 0.5f);
        TextBoxPort.color = Color.Black;
        TextBoxPort.interactable = true;
    }


    public void ClearImagesAndText()
    {
        //GUI.Remove(BG);

        GUI.Remove(Join);
        GUI.Remove(Host);
        GUI.Remove(TextBoxIP);
        GUI.Remove(TextBoxPort);

        GUI.Remove(IPText);
        GUI.Remove(PortText);
        GUI.Remove(IP);
        GUI.Remove(Port);
    }
}
