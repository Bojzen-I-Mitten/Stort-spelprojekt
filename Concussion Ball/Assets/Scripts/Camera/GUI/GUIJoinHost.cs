using System;
using System.Collections;
using ThomasEngine;
using LiteNetLib;

public class GUIJoinHost : ScriptComponent
{
    public Texture2D TextBox { get; set; }
    public Texture2D TextBoxBG { get; set; }

    Camera Camera;

    private bool TakeIP;
    private bool TakePort;

    private bool ClearIP = false;
    private bool ClearPort = false;

    private bool hasConnected = false;

    public Canvas Canvas;

    Image TextBoxIP;
    Image TextBoxPort;
    Image TextBoxBGIP;
    Image TextBoxBGPort;

    Text Join;
    Text Back;
    Text IPText;
    Text PortText;
    Text IP;
    Text Port;
    Text ConnectingText;

    Text Caret;

    IEnumerator Blink = null;

    public bool GoToTeamSelect;
    IEnumerator Connect = null;

    public float CaretOffset { get; set; } = 0.0f;

    public override void OnAwake()
    {
        TakeIP = false;
        TakePort = false;
        Camera = gameObject.GetComponent<Camera>();
        AddImagesAndText();
        GoToTeamSelect = false;
    }

    public override void Start()
    {
        


    }

    public void Listener_AllPeersConnectedEvent()
    {
        MatchSystem.instance.Listener.AllPeersConnectedEvent -= Listener_AllPeersConnectedEvent;
        MatchSystem.instance.Listener.PeerDisconnectedEvent -= Listener_PeerDisconnectedEvent;

        if (!hasConnected)
        {
            CameraMaster.instance.SetState(CAM_STATE.SELECT_TEAM);
            Join.interactable = true;
            hasConnected = true;
        }
    }

    public void Listener_PeerDisconnectedEvent(NetPeer peer, DisconnectInfo disconnectInfo)
    {
        MatchSystem.instance.Listener.AllPeersConnectedEvent -= Listener_AllPeersConnectedEvent;
        MatchSystem.instance.Listener.PeerDisconnectedEvent -= Listener_PeerDisconnectedEvent;
        switch (disconnectInfo.Reason)
        {
            case DisconnectReason.RemoteConnectionClose:
            case DisconnectReason.DisconnectPeerCalled:
                ConnectingText.text = "The peer you where connected to has disconnected with the IP\n" + peer.EndPoint.ToString();
                break;
            case DisconnectReason.Timeout:
                ConnectingText.text = "Connection to peer " + peer.EndPoint.Address.ToString() + " timed out";
                break;
            case DisconnectReason.ConnectionRejected:
                CameraMaster.instance.SetState(CAM_STATE.JOIN_HOST);
                ConnectingText.text = "Connection to peer " + peer.EndPoint.Address.ToString() + " rejected";
                break;
            case DisconnectReason.ConnectionFailed:
                CameraMaster.instance.SetState(CAM_STATE.JOIN_HOST);
                ConnectingText.text = "Failed to establish connection to\n" + peer.EndPoint.Address.ToString();
                break;
            case DisconnectReason.SocketReceiveError:
                CameraMaster.instance.SetState(CAM_STATE.JOIN_HOST);
                ConnectingText.text = "Connection to peer " + peer.EndPoint.Address.ToString() + " failed, peer socket closed"; //Could be the other way around
                break;
            case DisconnectReason.SocketSendError:
                ConnectingText.text = "Connection to peer " + peer.EndPoint.Address.ToString() + " failed, local socket closed"; //^^^^^^^^^^^^^^^^^^^^^^^^^^^^
                break;
        }
        ConnectingText.scale = new Vector2(0.5f);
        Join.interactable = true;
        StopCoroutine(Connect);
    }

    public override void Update()
    {
        Join.color = Color.FloralWhite;
        Back.color = Color.FloralWhite;

        if (TakeIP)
        {
            string str = IPText.text;
            GUIInput.AppendString(ref str, 15);
            IPText.text = str;
            Caret.position = IPText.position + new Vector2(IPText.size.x / 2 - 0.005f, CaretOffset);
        }
        if (TakePort)
        {
            string str = PortText.text;
            GUIInput.AppendString(ref str, 5);
            PortText.text = str;
            Caret.position = PortText.position + new Vector2(PortText.size.x / 2 - 0.005f, CaretOffset);
        }

        if (Input.GetMouseButtonUp(Input.MouseButtons.LEFT))
        {
            TakePort = false;
            TakeIP = false;
            TextBoxIP.color = Color.Black;
            TextBoxPort.color = Color.Black;
            if (Blink == null)
            {
                Blink = CaretBlink();
                StartCoroutine(Blink);
            }
        }

        if (Join.Clicked())
        {
            TakeIP = false;
            TakePort = false;
            ConnectingText.text = "";
            System.Net.IPAddress ipaddress;
            UserSettings.AddOrUpdateAppSetting("LastUsedIP", IPText.text);
            try
            {
                ipaddress = NetUtils.ResolveAddress(IPText.text);
            }
            catch (Exception e)
            {
                ConnectingText.text = e.Message;
                return;
            }
            if (PortText.text != "")
            {
                try
                {
                    if (IPText.text == "127.0.0.1")
                        MatchSystem.instance.LocalPort = 0;
                    else
                        MatchSystem.instance.LocalPort = Convert.ToInt32(PortText.text);
                    MatchSystem.instance.TargetPort = Convert.ToInt32(PortText.text);
                    MatchSystem.instance.TargetIP = IPText.text;

                    MatchSystem.instance.Listener.AllPeersConnectedEvent += Listener_AllPeersConnectedEvent;
                    MatchSystem.instance.Listener.PeerDisconnectedEvent += Listener_PeerDisconnectedEvent;

                    MatchSystem.instance.Init();
                    MatchSystem.instance.Connect();
                    ConnectingText.text = "Connecting";
                    ConnectingText.scale = new Vector2(1);
                    ConnectingText.position = new Vector2(0.75f, 0.9f);
                    Connect = Connecting();
                    StartCoroutine(Connect);
                    Join.interactable = false;
                    return;
                }
                catch (Exception e) //Hopefully this catches the exception thrown in ServerInfoEventHandler
                {
                    ConnectingText.text = e.Message;
                    Join.interactable = true;
                    StopCoroutine(Connect);
                }
            }
            else
            {
                if (IPText.text == "")
                    TextBoxIP.color = Color.Red;
                if (PortText.text == "")
                    TextBoxPort.color = Color.Red;
            }
        }

        if (Back.Clicked())
        {
            TakeIP = false;
            TakePort = false;
            CameraMaster.instance.SetState(CAM_STATE.MAIN_MENU);
            ConnectingText.text = "";
            UserSettings.AddOrUpdateAppSetting("LastUsedIP", IPText.text);
        }

        if (TextBoxIP.Clicked())
        {
            ConnectingText.text = "";
            TakeIP = true;
            if (ClearIP)
            {
                IPText.text = "";
                ClearIP = false;
            }
        }
        else if (TextBoxPort.Clicked())
        {
            ConnectingText.text = "";
            TakePort = true;
            if (ClearPort)
            {
                PortText.text = "";
                ClearPort = false;
            }
        }
        else if (Input.GetMouseButtonUp(Input.MouseButtons.LEFT))
        {
            if (Blink != null)
            {
                StopCoroutine(Blink);
                Blink = null;
                Caret.text = "";
            }
        }

        if (Join.Hovered())
        {
            Join.color = Color.IndianRed;
        }
        else if (Back.Hovered())
        {
            Back.color = Color.IndianRed;
        }
    }

    public void AddImagesAndText()
    {
        Canvas = Camera.AddCanvas();
        string SettingIP = UserSettings.GetSetting("LastUsedIP");
        if (SettingIP != null)
            IPText = Canvas.Add(SettingIP);
        else
            IPText = Canvas.Add("192.168.1.");
        IPText.origin = new Vector2(0.5f);
        IPText.position = new Vector2(0.2f, 0.15f);
        IPText.color = Color.Black;
        IPText.depth = 0.8f;




        PortText = Canvas.Add("9050");
        PortText.origin = new Vector2(0.5f);
        PortText.position = new Vector2(0.2f, 0.35f);
        PortText.color = Color.Black;
        PortText.depth = 0.8f;

        IP = Canvas.Add("IP");
        IP.origin = new Vector2(0.5f);
        IP.position = new Vector2(0.2f, 0.08f);
        IP.scale = new Vector2(0.7f);
        IP.color = Color.FloralWhite;
        IP.depth = 0.9f;

        Port = Canvas.Add("Port");
        Port.origin = new Vector2(0.5f);
        Port.position = new Vector2(0.2f, 0.28f);
        Port.scale = new Vector2(0.7f);
        Port.color = Color.FloralWhite;
        Port.depth = 0.9f;

        if (TextBox != null)
        {
            TextBoxIP = Canvas.Add(TextBox);
            TextBoxIP.origin = new Vector2(0.5f);
            TextBoxIP.position = new Vector2(0.2f, 0.15f);
            TextBoxIP.interactable = true;
            TextBoxIP.depth = 0.8f;
            TextBoxIP.color = Color.Black;

            TextBoxPort = Canvas.Add(TextBox);
            TextBoxPort.origin = new Vector2(0.5f);
            TextBoxPort.position = new Vector2(0.2f, 0.35f);
            TextBoxPort.interactable = true;
            TextBoxPort.depth = 0.8f;
            TextBoxPort.color = Color.Black;
        }

        if (TextBoxBG != null)
        {
            TextBoxBGIP = Canvas.Add(TextBoxBG);
            TextBoxBGIP.origin = new Vector2(0.5f);
            TextBoxBGIP.position = new Vector2(0.2f, 0.15f);
            TextBoxBGIP.depth = 0.9f;
            TextBoxBGIP.color = Color.FloralWhite;

            TextBoxBGPort = Canvas.Add(TextBoxBG);
            TextBoxBGPort.origin = new Vector2(0.5f);
            TextBoxBGPort.position = new Vector2(0.2f, 0.35f);
            TextBoxBGPort.depth = 0.9f;
            TextBoxBGPort.color = Color.FloralWhite;
        }

        if (TextBoxIP != null)
        {
            Join = Canvas.Add("Join");
            Join.origin = new Vector2(0.5f);
            Join.position = new Vector2(0.4f, 0.25f);
            Join.interactable = true;
            Join.depth = 0.9f;
            Join.color = Color.FloralWhite;
        }

        Back = Canvas.Add("Back");
        Back.origin = new Vector2(0.5f);
        Back.position = new Vector2(0.1f, 0.8f);
        Back.interactable = true;
        Back.depth = 0.9f;
        Back.color = Color.FloralWhite;

        ConnectingText = Canvas.Add("");
        ConnectingText.origin = new Vector2(0.5f);
        ConnectingText.color = Color.FloralWhite;
        ConnectingText.depth = 0;
        ConnectingText.position = new Vector2(0.5f, 0.75f);

        Caret = Canvas.Add("");
        Caret.origin = new Vector2(0, 0.5f);
        Caret.scale = IPText.scale;
        Caret.interactable = false;
        Caret.depth = 0.8f;
        Caret.color = Color.Black;
    }

    public void ClearImagesAndText()
    {
        Canvas.Remove(Join);
        Canvas.Remove(Back);
        Canvas.Remove(TextBoxIP);
        Canvas.Remove(TextBoxPort);
        Canvas.Remove(ConnectingText);

        Canvas.Remove(IPText);
        Canvas.Remove(PortText);
        Canvas.Remove(IP);
        Canvas.Remove(Port);
        Canvas.Remove(Caret);
        UserSettings.AddOrUpdateAppSetting("LastUsedIP", IPText.text);

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

    IEnumerator CaretBlink()
    {
        bool underscore = true;
        while (true)
        {
            if (underscore)
            {
                Caret.text = "|";
                underscore = false;
            }
            else
            {
                Caret.text = "";
                underscore = true;
            }

            yield return new WaitForSecondsRealtime(0.5f);
        }
    }
}
