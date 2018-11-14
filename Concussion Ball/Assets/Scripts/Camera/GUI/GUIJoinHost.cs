using System;
using System.Collections;
using ThomasEngine;
using LiteNetLib;
using LiteNetLib.Utils;

public class GUIJoinHost : ScriptComponent
{
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

    Text Join;
    Text Host;
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
        if (TakeIP)
            GUIInput.AppendString(ref IPString, 30);
        if (TakePort)
            GUIInput.AppendString(ref PortString, 5);

        if (Input.GetMouseButtonUp(Input.MouseButtons.LEFT))
        {
            if (TextBoxIP.Clicked())
            {
                ConnectingText.text = "";
                TakePort = false;
                TakeIP = true;
                TextBoxPort.color = Color.Black;
                TextBoxIP.color = Color.Green;
            }
            else if (TextBoxPort.Clicked())
            {
                ConnectingText.text = "";
                TakeIP = false;
                TakePort = true;
                TextBoxIP.color = Color.Black;
                TextBoxPort.color = Color.Green;
            }
            else if (Join.Hovered())
            {
                ConnectingText.text = "";
                System.Net.IPAddress ipaddress;
                try
                {
                    ipaddress = LiteNetLib.NetUtils.ResolveAddress(IPString);
                }
                catch (Exception e)
                {
                    ConnectingText.text = e.Message;
                    return;
                }
                if (PortString != "")
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
            else if (Host.Hovered())
            {
                ConnectingText.text = "";
                if (PortString != "")
                {
                    MatchSystem.instance.LocalPort = Convert.ToInt32(PortString);
                    MatchSystem.instance.Init();
                    MatchSystem.instance.Host();
                    CameraMaster.instance.State = CAM_STATE.SELECT_TEAM;
                    hasConnected = true;


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

    public void AddImagesAndText()
    {
        Canvas = Camera.AddCanvas();

        IPText = Canvas.Add(IPString);
        IPText.origin = new Vector2(0, 0.5f);
        IPText.position = new Vector2(0.1f, 0.11875f);
        IPText.color = Color.Black;
        IPText.depth = 0.9f;

        PortText = Canvas.Add(PortString);
        PortText.origin = new Vector2(0, 0.5f);
        PortText.position = new Vector2(0.1f, 0.21875f);
        PortText.color = Color.Black;
        PortText.depth = 0.9f;

        IP = Canvas.Add("IP");
        IP.origin = new Vector2(0, 0.5f);
        IP.position = new Vector2(0.1f, 0.07f);
        IP.scale = new Vector2(0.7f);
        IP.color = Color.Black;
        IP.depth = 0.9f;

        Port = Canvas.Add("Port");
        Port.origin = new Vector2(0, 0.5f);
        Port.position = new Vector2(0.1f, 0.17f);
        Port.scale = new Vector2(0.7f);
        Port.color = Color.Black;
        Port.depth = 0.9f;

        if (TextBox != null)
        {
            TextBoxIP = Canvas.Add(TextBox);
            TextBoxIP.origin = new Vector2(0, 0.5f);
            TextBoxIP.position = new Vector2(0.1f);
            TextBoxIP.scale = new Vector2(1, 0.5f);
            TextBoxIP.color = Color.Black;
            TextBoxIP.interactable = true;
            TextBoxIP.depth = 0.9f;

            TextBoxPort = Canvas.Add(TextBox);
            TextBoxPort.origin = new Vector2(0, 0.5f);
            TextBoxPort.position = new Vector2(0.1f, 0.2f);
            TextBoxPort.scale = new Vector2(1, 0.5f);
            TextBoxPort.color = Color.Black;
            TextBoxPort.interactable = true;
            TextBoxPort.depth = 0.9f;
        }

        if (TextBoxIP != null)
        {
            Join = Canvas.Add("Join");
            Join.origin = new Vector2(0, 0.5f);
            Join.position = new Vector2(TextBoxIP.position.x + TextBoxIP.size.x, 0.11875f);
            Join.interactable = true;
            Join.depth = 0.9f;
            Join.color = Color.Black;
            Join.font = TextFont;
        }

        if (TextBoxPort != null)
        {
            Host = Canvas.Add("Host");
            Host.origin = new Vector2(0, 0.5f);
            Host.position = new Vector2(TextBoxPort.position.x +TextBoxPort.size.x, 0.21875f);
            Host.interactable = true;
            Host.depth = 0.9f;
            Host.color = Color.Black;
            Host.font = TextFont;
        }

        ConnectingText = Canvas.Add("");
        ConnectingText.origin = new Vector2(0, 0.5f);
        ConnectingText.color = Color.Black;
        ConnectingText.depth = 0;
        ConnectingText.font = TextFont;
        ConnectingText.position = new Vector2(0.5f, 0.11875f);
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
