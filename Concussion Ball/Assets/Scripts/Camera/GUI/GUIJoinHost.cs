using System;
using System.Collections;
using ThomasEngine;
using LiteNetLib;

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

    private bool hasConnected = false;

    public Canvas Canvas;

    Image TextBoxIP;
    Image TextBoxPort;
    Image Join;
    Image Host;

    Text IPText;
    Text PortText;
    Text IP;
    Text Port;
    Text ConnectingText;
    public bool GoToTeamSelect;
    IEnumerator test;

    public override void Awake()
    {
    }

    public override void Start()
    {
        IPString = "192.168.1.";
        PortString = "9050";
        TakeIP = false;
        TakePort = false;
        Disabled = false;
        Camera = gameObject.GetComponent<Camera>();
        AddImagesAndText();
        GoToTeamSelect = false;
        MatchSystem.instance.Listener.PeerDisconnectedEvent += Listener_PeerDisconnectedEvent;
        MatchSystem.instance.Listener.PeerConnectedEvent += Listener_PeerConnectedEvent;
    }

    public void Listener_PeerConnectedEvent(NetPeer peer)
    {
        MatchSystem.instance.Listener.PeerConnectedEvent -= Listener_PeerConnectedEvent;
        MatchSystem.instance.Listener.PeerDisconnectedEvent -= Listener_PeerDisconnectedEvent;

        if (!hasConnected)
        {
            CameraMaster.instance.State = CAM_STATE.SELECT_TEAM;
            Join.interactable = true;
            Host.interactable = true;
            hasConnected = true;
        }


    }

    public void Listener_PeerDisconnectedEvent(NetPeer peer, DisconnectInfo disconnectInfo)
    {
        ConnectingText.text = "Connection failed:\n" + disconnectInfo.Reason.ToString();
        //ConnectingText.position = TextBoxIP.position + TextBoxIP.size / Canvas.viewport.size * 1.5f;
        Join.interactable = true;
        Host.interactable = true;
        StopCoroutine(test);
    }

    public override void Update()
    {
        if (Canvas.isRendering)
        {
            //if (TakeIP)
                //InputGUI.AppendIPString(ref IPString, 15);
            //if (TakePort)
                //InputGUI.AppendIPString(ref PortString, 5);

            if (Input.GetMouseButtonUp(Input.MouseButtons.LEFT))
            {
                if (TextBoxIP.Clicked())
                {
                    TakePort = false;
                    TakeIP = true;
                    TextBoxPort.color = Color.Black;
                    TextBoxIP.color = Color.Green;
                }
                else if (TextBoxPort.Clicked())
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
                        if (IPString == "127.0.0.1")
                            MatchSystem.instance.LocalPort = 0;
                        else
                            MatchSystem.instance.LocalPort = Convert.ToInt32(PortString);
                        MatchSystem.instance.TargetPort = Convert.ToInt32(PortString);
                        MatchSystem.instance.TargetIP = IPString;
                        MatchSystem.instance.Init();
                        MatchSystem.instance.Connect();
                        ConnectingText.text = "Connecting";
                        ConnectingText.position = new Vector2(0.75f, 0.9f);
                        test = Connecting();
                        StartCoroutine(test);
                        Join.interactable = false;
                        Host.interactable = false;
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
                        CameraMaster.instance.State = CAM_STATE.HOST_MENU;
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
                    Port.font = TextFont;
                }
            }
        }
    }

    public void AddImagesAndText()
    {
        Canvas = Camera.AddCanvas();

        IPText = Canvas.Add(IPString);
        IPText.position = new Vector2(0.1f, 0.11f);
        IPText.color = Color.Black;
        IPText.depth = 0.9f;


        PortText = Canvas.Add(PortString);
        PortText.position = new Vector2(0.1f, 0.21f);
        PortText.color = Color.Black;
        PortText.depth = 0.9f;


        IP = Canvas.Add("IP, needed to join");
        IP.position = new Vector2(0.1f, 0.07f);
        IP.scale = new Vector2(0.7f);
        IP.color = Color.Black;
        IP.depth = 0.9f;


        Port = Canvas.Add("PORT, needed for both host and join");
        Port.position = new Vector2(0.1f, 0.17f);
        Port.scale = new Vector2(0.7f);
        Port.color = Color.Black;
        Port.depth = 0.9f;


        if (JoinBtn != null)
        {
            Join = Canvas.Add(JoinBtn);
            Join.position = new Vector2(0.325f, 0.11f);
            Join.scale = new Vector2(0.25f);
            Join.interactable = true;
            Join.depth = 0.9f;

        }

        if (HostBtn != null)
        {
            Host = Canvas.Add(HostBtn);
            Host.position = new Vector2(0.325f, 0.21f);
            Host.scale = new Vector2(0.25f);
            Host.interactable = true;
            Host.depth = 0.9f;

        }

        if (TextBox != null)
        {
            TextBoxIP = Canvas.Add(TextBox);
            TextBoxIP.position = new Vector2(0.1f);
            TextBoxIP.scale = new Vector2(0.7f, 0.5f);
            TextBoxIP.color = Color.Black;
            TextBoxIP.interactable = true;
            TextBoxIP.depth = 0.9f;


            TextBoxPort = Canvas.Add(TextBox);
            TextBoxPort.position = new Vector2(0.1f, 0.2f);
            TextBoxPort.scale = new Vector2(0.7f, 0.5f);
            TextBoxPort.color = Color.Black;
            TextBoxPort.interactable = true;
            TextBoxPort.depth = 0.9f;

        }
        ConnectingText = Canvas.Add("");
        ConnectingText.color = Color.Black;
        ConnectingText.depth = 0;
        ConnectingText.font = TextFont;
    }

    public void ClearImagesAndText()
    {
        Canvas.Remove(Join);
        Canvas.Remove(Host);
        Canvas.Remove(TextBoxIP);
        Canvas.Remove(TextBoxPort);
        Canvas.Remove(ConnectingText);

        Canvas.Remove(IPText);
        Canvas.Remove(PortText);
        Canvas.Remove(IP);
        Canvas.Remove(Port);
    }

    IEnumerator Connecting()
    {
        int ticks = 0;
        while (true)
        {
            ++ticks;
            if ((ticks % 4) == 0)
            {
                ConnectingText.text = "Connecting";
                ticks = 0;
            }
            else
                ConnectingText.text += ".";

            yield return new WaitForSecondsRealtime(0.25f);
        }
    }
}
