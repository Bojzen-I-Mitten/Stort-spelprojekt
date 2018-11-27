using System;
using System.Collections;
using ThomasEngine;
using LiteNetLib;

public class GUIJoinHost : ScriptComponent
{
    public Texture2D TextBox { get; set; }
    public Texture2D TextBoxBG { get; set; }

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
    Image TextBoxBGIP;
    Image TextBoxBGPort;

    Text Join;
    Text Host;
    Text Back;
    Text IPText;
    Text PortText;
    Text IP;
    Text Port;
    Text ConnectingText;

    public bool GoToTeamSelect;
    IEnumerator test;

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
        MatchSystem.instance.Listener.PeerConnectedEvent -= Listener_PeerConnectedEvent;
        MatchSystem.instance.Listener.PeerDisconnectedEvent -= Listener_PeerDisconnectedEvent;
        ConnectingText.text = "Connection failed:\n" + disconnectInfo.Reason.ToString();
        //ConnectingText.position = TextBoxIP.position + TextBoxIP.size / Canvas.viewport.size * 1.5f;
        Join.interactable = true;
        Host.interactable = true;
        StopCoroutine(test);
    }

    public override void Update()
    {
        Join.color = Color.FloralWhite;
        Host.color = Color.FloralWhite;
        Back.color = Color.FloralWhite;

        if (TakeIP)
            GUIInput.AppendString(ref IPString, 30);
        if (TakePort)
            GUIInput.AppendString(ref PortString, 5);

        if (Back.Clicked())
        {
            CameraMaster.instance.State = CAM_STATE.MAIN_MENU;
            ConnectingText.text = "";
        }
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
        else if (Join.Clicked())
        {
            ConnectingText.text = "";
            System.Net.IPAddress ipaddress;
            try
            {
                ipaddress = NetUtils.ResolveAddress(IPString);
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

                MatchSystem.instance.Listener.PeerConnectedEvent += Listener_PeerConnectedEvent;
                MatchSystem.instance.Listener.PeerDisconnectedEvent += Listener_PeerDisconnectedEvent;

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
            ConnectingText.text = "";
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
        else if (Input.GetMouseButtonUp(Input.MouseButtons.LEFT))
        {
            TakePort = false;
            TakeIP = false;
            TextBoxIP.color = Color.Black;
            TextBoxPort.color = Color.Black;
        }
        else if (Join.Hovered())
        {
            Join.color = Color.IndianRed;
        }
        else if (Host.Hovered())
        {
            Host.color = Color.IndianRed;
        }
        else if (Back.Hovered())
        {
            Back.color = Color.IndianRed;
        }

        if (!Disabled)
        {
            IPText.text = IPString;
            PortText.text = PortString;
        }
    }

    public void AddImagesAndText()
{
    Canvas = Camera.AddCanvas();

    IPText = Canvas.Add(IPString);
    IPText.origin = new Vector2(0.5f);
    IPText.position = new Vector2(0.5f, 0.11875f);
    IPText.color = Color.Black;
    IPText.depth = 0.8f;

    PortText = Canvas.Add(PortString);
    PortText.origin = new Vector2(0.5f);
    PortText.position = new Vector2(0.5f, 0.26875f);
    PortText.color = Color.Black;
    PortText.depth = 0.8f;

    IP = Canvas.Add("IP");
    IP.origin = new Vector2(0.5f);
    IP.position = new Vector2(0.5f, 0.05f);
    IP.scale = new Vector2(0.7f);
    IP.color = Color.FloralWhite;
    IP.depth = 0.9f;

    Port = Canvas.Add("Port");
    Port.origin = new Vector2(0.5f);
    Port.position = new Vector2(0.5f, 0.2f);
    Port.scale = new Vector2(0.7f);
    Port.color = Color.FloralWhite;
    Port.depth = 0.9f;

    if (TextBox != null)
    {
        TextBoxIP = Canvas.Add(TextBox);
        TextBoxIP.origin = new Vector2(0.5f);
        TextBoxIP.position = new Vector2(0.5f, 0.1f);
        TextBoxIP.scale = new Vector2(1, 0.75f);
        TextBoxIP.interactable = true;
        TextBoxIP.depth = 0.9f;
        TextBoxIP.color = Color.Black;

        TextBoxPort = Canvas.Add(TextBox);
        TextBoxPort.origin = new Vector2(0.5f);
        TextBoxPort.position = new Vector2(0.5f, 0.25f);
        TextBoxPort.scale = new Vector2(1, 0.75f);
        TextBoxPort.interactable = true;
        TextBoxPort.depth = 0.9f;
        TextBoxPort.color = Color.Black;
    }

    if (TextBoxBG != null)
    {
        TextBoxBGIP = Canvas.Add(TextBoxBG);
        TextBoxBGIP.origin = new Vector2(0.5f);
        TextBoxBGIP.position = new Vector2(0.5f, 0.1f);
        TextBoxBGIP.scale = new Vector2(1, 0.75f);
        TextBoxBGIP.depth = 0.9f;
        TextBoxBGIP.color = Color.FloralWhite;

        TextBoxBGPort = Canvas.Add(TextBoxBG);
        TextBoxBGPort.origin = new Vector2(0.5f);
        TextBoxBGPort.position = new Vector2(0.5f, 0.25f);
        TextBoxBGPort.scale = new Vector2(1, 0.75f);
        TextBoxBGPort.depth = 0.9f;
        TextBoxBGPort.color = Color.FloralWhite;
    }

    if (TextBoxIP != null)
    {
        Join = Canvas.Add("Join");
        Join.origin = new Vector2(0.5f);
        Join.position = new Vector2(TextBoxIP.position.x + TextBoxIP.size.x / 2 + Join.size.x / 2, 0.11875f);
        Join.interactable = true;
        Join.depth = 0.9f;
        Join.color = Color.FloralWhite;
    }

    if (TextBoxPort != null)
    {
        Host = Canvas.Add("Host");
        Host.origin = new Vector2(0.5f);
        Host.position = new Vector2(TextBoxPort.position.x + TextBoxPort.size.x / 2 + Host.size.x / 2, 0.26875f);
        Host.interactable = true;
        Host.depth = 0.9f;
        Host.color = Color.FloralWhite;
    }

    Back = Canvas.Add("Back");
    Back.origin = new Vector2(0.5f);
    Back.position = new Vector2(0.575f, 0.36f);
    Back.interactable = true;
    Back.depth = 0.9f;
    Back.color = Color.FloralWhite;

    ConnectingText = Canvas.Add("");
    ConnectingText.origin = new Vector2(0.5f);
    ConnectingText.color = Color.FloralWhite;
    ConnectingText.depth = 0;
    ConnectingText.position = new Vector2(0.5f, 0.75f);
}

public void ClearImagesAndText()
{
    Canvas.Remove(Join);
    Canvas.Remove(Host);
    Canvas.Remove(Back);
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
