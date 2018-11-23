using System;
using ThomasEngine;

public class GUIHostMenu : ScriptComponent
{
    Camera Camera;
    public Canvas Canvas;

    public Texture2D ColorSliderTexture { get; set; }
    public Texture2D SliderKnobTexture { get; set; }
    public Texture2D TextBox6010Texture { get; set; }
    public Texture2D TextBox305Texture { get; set; }
    public Texture2D TextBox105Texture { get; set; }
    public Texture2D WhiteBoxTexture { get; set; }
    public Texture2D TextBoxBG55Texture { get; set; }
    public Texture2D TextBoxCross55Texture { get; set; }
    public Font Font { get; set; }

    //public Vector2 _ServerNamePos { get; set; }          = new Vector2(0.0f, 0.4f);
    //public Vector2 _ServerNameStringPos { get; set; }    = new Vector2(0.0f, 0.4f);
    //public Vector2 _ServerNameBoxPos { get; set; }       = new Vector2(0.0f, 0.4f);

    //public Vector2 _MaxPlayersPos { get; set; } = new Vector2(0.0f, 0.15f);
    //public Vector2 _MaxPlayersStringPos { get; set; } = new Vector2(0.14f, 0.15f);
    //public Vector2 _MaxPlayersBoxPos { get; set; } = new Vector2(0.14f, 0.15f);

    //public Vector2 _TimeRoundPos { get; set; } = new Vector2(0.0f, 0.4f);
    //public Vector2 _TimeRoundStringPos { get; set; } = new Vector2(0.14f, 0.21f);
    //public Vector2 _TimeRoundBoxPos { get; set; } = new Vector2(0.14f, 0.21f);

    //public Vector2 _PublicServerPos { get; set; }        = new Vector2(0.0f, 0.4f);
    //public Vector2 _PublicServerCheckPos { get; set; }   = new Vector2(0.0f, 0.4f);
    //public Vector2 _PublicServerBoxPos { get; set; }     = new Vector2(0.0f, 0.4f);

    //public Vector2 _ScoreLimitPos { get; set; } = new Vector2(0.35f, 0.15f);
    //public Vector2 _ScoreLimitStringPos { get; set; } = new Vector2(0.47f, 0.15f);
    //public Vector2 _ScoreLimitBoxPos { get; set; } = new Vector2(0.47f, 0.15f);

    //public Vector2 _PowerUpsPos { get; set; }            = new Vector2(0.0f, 0.4f);
    //public Vector2 _PowerUpsCheckPos { get; set; }       = new Vector2(0.0f, 0.4f);
    //public Vector2 _PowerUpsBoxPos { get; set; }         = new Vector2(0.0f, 0.4f);

    Text HostBtn;
    Text ExitBtn;

    #region Host Setings
    Text HostOptions;
    Image HostBg;

    Text ServerName;
    Text ServerNameString;
    Image ServerNameBox;

    Text PortName;
    Text PortNameString;
    Image PortNameBox;

    Text MaxPlayers;
    Text MaxPlayersString;
    Image MaxPlayersBox;

    Text TimeRound;
    Text TimeRoundString;
    Image TimeRoundBox;

    Text PublicServer;
    Image PublicServerCheck;
    Image PublicServerBox;

    Text ScoreLimit;
    Text ScoreLimitString;
    Image ScoreLimitBox;

    Text PowerUps;
    Image PowerUpsCheck;
    Image PowerUpsBox;

    Text SimilarColor;
    Text SameName;

    #endregion

    #region Teams
    Text Team1;
    Text Team2;

    Image Team1TextBox;
    Image Team1ColorSlider;
    Image Team1SliderKnob;
    Image Team1BG;

    Image Team2TextBox;
    Image Team2ColorSlider;
    Image Team2SliderKnob;
    Image Team2BG;

    bool btnDown;
    #endregion

    bool InputTeam1Name = false;
    bool InputTeam2Name = false;
    bool InputServerName = false;
    bool InputPortName = false;
    bool InputMaxPlayers = false;
    bool InputTimeRound = false;
    bool InputScoreLimit = false;

    bool NotSameName = true;
    bool NotSimilarColor = true;

    Color Unselected = Color.FloralWhite;
    Color Selected = Color.IndianRed;

    public override void Start()
    {
        Camera = gameObject.GetComponent<Camera>();
        Canvas = Camera.AddCanvas();

        btnDown = false;

        HostBtn = Canvas.Add("Host Game");
        HostBtn.color = Unselected;
        HostBtn.position = new Vector2(0.47f, 0.33f);
        HostBtn.font = Font;
        HostBtn.interactable = true;

        ExitBtn = Canvas.Add("Exit");
        ExitBtn.color = Unselected;
        ExitBtn.position = new Vector2(0.32f, 0.33f);
        ExitBtn.font = Font;
        ExitBtn.interactable = true;

        #region Host Setings

        HostOptions = Canvas.Add("Host Options");
        HostOptions.position = new Vector2(0.5f, 0f);
        HostOptions.origin = new Vector2(0.5f, 0.0f);
        HostOptions.font = Font;
        HostOptions.color = Unselected;

        #region Host BG
        if (WhiteBoxTexture != null)
        {
            HostBg = Canvas.Add(WhiteBoxTexture);
            HostBg.color = new Color(0.5f, 0.5f, 0.5f, 0.5f);
            HostBg.scale = new Vector2(6.4f, 1.44f);
            HostBg.depth = 0.7f;
        }
        #endregion

        #region Color Check
        SimilarColor = Canvas.Add("Teams have similar color, can't host");
        SimilarColor.position = new Vector2(0.5f, 0.52f);
        SimilarColor.origin = new Vector2(0.5f);
        SimilarColor.color = Color.Red;
        SimilarColor.scale = Vector2.Zero;
        SimilarColor.font = Font;
        SimilarColor.depth = 0;
        #endregion

        #region Name Check
        SameName = Canvas.Add("Teams have same name, can't host");
        SameName.position = new Vector2(0.5f, 0.58f);
        SameName.origin = new Vector2(0.5f);
        SameName.color = Color.Red;
        SameName.scale = Vector2.Zero;
        SameName.font = Font;
        SameName.depth = 0;
        #endregion

        float column1 = 0f;
        float column2 = 0.14f;
        float column3 = 0.35f;
        float column4 = 0.47f;

        float row1 = 0.09f;
        float row2 = 0.16f;
        float row3 = 0.23f;
        float row4 = 0.30f;

        #region Server Name
        ServerName = Canvas.Add("Server name:");
        ServerName.position = new Vector2(column1, row1);
        ServerName.scale = new Vector2(0.5f);
        ServerName.origin = new Vector2(0.0f, 0.5f);
        ServerName.color = Unselected;
        ServerName.font = Font;
        ServerName.depth = 0.4f;

        ServerNameString = Canvas.Add("Pelles server");
        ServerNameString.position = new Vector2(column2, row1);
        ServerNameString.scale = new Vector2(0.5f);
        ServerNameString.origin = new Vector2(0.0f, 0.5f);
        ServerNameString.color = Unselected;
        ServerNameString.font = Font;
        ServerNameString.depth = 0.4f;

        if (TextBox305Texture != null)
        {
            ServerNameBox = Canvas.Add(TextBox305Texture);
            ServerNameBox.position = new Vector2(column2, row1);
            ServerNameBox.origin = new Vector2(0.0f, 0.5f);
            ServerNameBox.color = Unselected;
            ServerNameBox.interactable = true;
            ServerNameBox.depth = 0.5f;
        }
        #endregion

        #region Server Port
        PortName = Canvas.Add("Port:");
        PortName.position = new Vector2(column1, row2);
        PortName.scale = new Vector2(0.5f);
        PortName.origin = new Vector2(0.0f, 0.5f);
        PortName.color = Unselected;
        PortName.font = Font;
        PortName.depth = 0.4f;

        PortNameString = Canvas.Add("9050");
        PortNameString.position = new Vector2(column2, row2);
        PortNameString.scale = new Vector2(0.5f);
        PortNameString.origin = new Vector2(0.0f, 0.5f);
        PortNameString.color = Unselected;
        PortNameString.font = Font;
        PortNameString.depth = 0.4f;

        if (TextBox305Texture != null)
        {
            PortNameBox = Canvas.Add(TextBox305Texture);
            PortNameBox.position = new Vector2(column2, row2);
            PortNameBox.origin = new Vector2(0.0f, 0.5f);
            PortNameBox.color = Unselected;
            PortNameBox.interactable = true;
            PortNameBox.depth = 0.5f;
        }
        #endregion

        #region Max Players
        MaxPlayers = Canvas.Add("Max players:");
        MaxPlayers.position = new Vector2(column1, row3);
        MaxPlayers.scale = new Vector2(0.5f);
        MaxPlayers.origin = new Vector2(0.0f, 0.5f);
        MaxPlayers.color = Unselected;
        MaxPlayers.font = Font;
        MaxPlayers.depth = 0.4f;

        MaxPlayersString = Canvas.Add("8");
        MaxPlayersString.position = new Vector2(column2, row3);
        MaxPlayersString.scale = new Vector2(0.5f);
        MaxPlayersString.origin = new Vector2(0.0f, 0.5f);
        MaxPlayersString.color = Unselected;
        MaxPlayersString.font = Font;
        MaxPlayersString.depth = 0.4f;

        if (TextBox105Texture != null)
        {
            MaxPlayersBox = Canvas.Add(TextBox105Texture);
            MaxPlayersBox.position = new Vector2(column2, row3);
            MaxPlayersBox.origin = new Vector2(0.0f, 0.5f);
            MaxPlayersBox.interactable = true;
            MaxPlayersBox.depth = 0.5f;
        }
        #endregion

        #region Time Round
        TimeRound = Canvas.Add("Time per round:");
        TimeRound.position = new Vector2(column1, row4);
        TimeRound.scale = new Vector2(0.5f);
        TimeRound.origin = new Vector2(0.0f, 0.5f);
        TimeRound.color = Unselected;
        TimeRound.font = Font;
        TimeRound.depth = 0.4f;

        TimeRoundString = Canvas.Add("5");
        TimeRoundString.position = new Vector2(column2, row4);
        TimeRoundString.scale = new Vector2(0.5f);
        TimeRoundString.origin = new Vector2(0.0f, 0.5f);
        TimeRoundString.color = Unselected;
        TimeRoundString.font = Font;
        TimeRoundString.depth = 0.4f;

        if (TextBox105Texture != null)
        {
            TimeRoundBox = Canvas.Add(TextBox105Texture);
            TimeRoundBox.position = new Vector2(column2, row4);
            //TimeRoundBox.scale = new Vector2(0.5f);
            TimeRoundBox.origin = new Vector2(0.0f, 0.5f);
            TimeRoundBox.interactable = true;
            TimeRoundBox.depth = 0.5f;
        }
        #endregion

        #region Public Server
        PublicServer = Canvas.Add("Public server:");
        PublicServer.position = new Vector2(column3, row1);
        PublicServer.scale = new Vector2(0.5f);
        PublicServer.origin = new Vector2(0.0f, 0.5f);
        PublicServer.color = Unselected;
        PublicServer.font = Font;
        PublicServer.depth = 0.4f;

        if (TextBoxCross55Texture != null)
        {
            PublicServerCheck = Canvas.Add(TextBoxCross55Texture);
            PublicServerCheck.position = new Vector2(column4 + PublicServerCheck.size.x / 2, row1);
            PublicServerCheck.origin = new Vector2(0.5f);
            PublicServerCheck.depth = 0.4f;
        }

        if (TextBoxBG55Texture != null)
        {
            PublicServerBox = Canvas.Add(TextBoxBG55Texture);
            PublicServerBox.position = new Vector2(column4 + PublicServerBox.size.x / 2, row1);
            PublicServerBox.origin = new Vector2(0.5f);
            PublicServerBox.interactable = true;
            PublicServerBox.depth = 0.5f;
        }
        #endregion

        #region Score limit
        ScoreLimit = Canvas.Add("Score Limit:");
        ScoreLimit.position = new Vector2(column3, row2);
        ScoreLimit.scale = new Vector2(0.5f);
        ScoreLimit.origin = new Vector2(0.0f, 0.5f);
        ScoreLimit.color = Unselected;
        ScoreLimit.font = Font;
        ScoreLimit.depth = 0.4f;

        ScoreLimitString = Canvas.Add("10");
        ScoreLimitString.position = new Vector2(column4, row2);
        ScoreLimitString.scale = new Vector2(0.5f);
        ScoreLimitString.origin = new Vector2(0.0f, 0.5f);
        ScoreLimitString.color = Unselected;
        ScoreLimitString.font = Font;
        ScoreLimitString.depth = 0.4f;

        if (TextBox105Texture != null)
        {
            ScoreLimitBox = Canvas.Add(TextBox105Texture);
            ScoreLimitBox.position = new Vector2(column4, row2);
            //ScoreLimitBox.scale = new Vector2(0.5f);
            ScoreLimitBox.origin = new Vector2(0.0f, 0.5f);
            ScoreLimitBox.interactable = true;
            ScoreLimitBox.depth = 0.5f;
        }
        #endregion

        #region Power Ups
        PowerUps = Canvas.Add("Power ups:");
        PowerUps.position = new Vector2(column3, row3);
        PowerUps.scale = new Vector2(0.5f);
        PowerUps.origin = new Vector2(0.0f, 0.5f);
        PowerUps.color = Unselected;
        PowerUps.font = Font;
        PowerUps.depth = 0.4f;

        if (TextBoxCross55Texture != null)
        {
            PowerUpsCheck = Canvas.Add(TextBoxCross55Texture);
            PowerUpsCheck.position = new Vector2(column4 + PowerUpsCheck.size.x / 2, row3);
            PowerUpsCheck.origin = new Vector2(0.5f);
            PowerUpsCheck.depth = 0.4f;
        }

        if (TextBoxBG55Texture != null)
        {
            PowerUpsBox = Canvas.Add(TextBoxBG55Texture);
            PowerUpsBox.position = new Vector2(column4 + PowerUpsBox.size.x / 2, row3);
            PowerUpsBox.origin = new Vector2(0.5f);
            PowerUpsBox.interactable = true;
            PowerUpsBox.depth = 0.5f;
        }
        #endregion

        #endregion

        #region Teams
        if (WhiteBoxTexture != null)
        {
            Team1BG = Canvas.Add(WhiteBoxTexture);
            Team1BG.position = new Vector2(0.0f, 0.398f);
            Team1BG.scale = new Vector2(2, 2.2f);
            Team1BG.color = new Color(0.5f, 0.5f, 0.5f, 0.5f);
            Team1BG.depth = 0.7f;

            Team2BG = Canvas.Add(WhiteBoxTexture);
            Team2BG.position = new Vector2(0.67f, 0.398f);
            Team2BG.scale = new Vector2(2.25f, 2.4f);
            Team2BG.color = new Color(0.5f, 0.5f, 0.5f, 0.5f);
            Team2BG.depth = 0.7f;
        }

        Team1 = Canvas.Add("Team 1");
        Team1.position = new Vector2(0.02f, 0.4f);
        Team1.color = Unselected;
        Team1.font = Font;

        Team2 = Canvas.Add("Team 2");
        Team2.position = new Vector2(0.69f, 0.4f);
        Team2.color = Unselected;
        Team2.font = Font;

        if (TextBox6010Texture != null)
        {
            Team1TextBox = Canvas.Add(TextBox6010Texture);
            Team1TextBox.position = new Vector2(0.01f, 0.39f);
            Team1TextBox.scale = new Vector2(0.9f, 1f);
            Team1TextBox.interactable = true;

            Team2TextBox = Canvas.Add(TextBox6010Texture);
            Team2TextBox.position = new Vector2(0.68f, 0.39f);
            Team2TextBox.scale = new Vector2(0.9f, 1f);
            Team2TextBox.interactable = true;
        }

        Vector2 team1SliderKnobPos = new Vector2(0.05f, 0.7f);
        Vector2 team2SliderKnobPos = new Vector2(0.70f, 0.7f);

        if (ColorSliderTexture != null)
        {
            Team1ColorSlider = Canvas.Add(ColorSliderTexture);
            Team1ColorSlider.position = team1SliderKnobPos;
            Team1ColorSlider.interactable = true;

            Team2ColorSlider = Canvas.Add(ColorSliderTexture);
            Team2ColorSlider.position = team2SliderKnobPos;
            Team2ColorSlider.interactable = true;
        }

        if (SliderKnobTexture != null)
        {
            Team1SliderKnob = Canvas.Add(SliderKnobTexture);
            Team1SliderKnob.position = team1SliderKnobPos;
            Team1SliderKnob.origin = new Vector2(0.5f, 0.0f);

            Team2SliderKnob = Canvas.Add(SliderKnobTexture);
            Team2SliderKnob.position = team2SliderKnobPos + Team2ColorSlider.size * new Vector2(0.66f, 0);  //+... Is to offset it to blue color. Derp.
            Team2SliderKnob.origin = new Vector2(0.5f, 0.0f);
        }
        #endregion
    }

    public override void Update()
    {
        if (Canvas.isRendering)
        {
            NotSimilarColorTest(MatchSystem.instance.Teams[TEAM_TYPE.TEAM_1].Color, MatchSystem.instance.Teams[TEAM_TYPE.TEAM_2].Color);
            NotSameNameTest(Team1.text, Team2.text);

            if (Input.GetMouseButtonDown(Input.MouseButtons.LEFT) || btnDown)
            {
                btnDown = true;
                float hue = 0.0f;

                if (Team1ColorSlider.Hovered())
                {
                    Team1SliderKnob.position = new Vector2(Input.GetMouseX() / Canvas.camera.viewport.size.x, Team1SliderKnob.position.y);
                    hue = (Team1SliderKnob.position.x - Team1ColorSlider.position.x) / Team1ColorSlider.size.x;
                    MatchSystem.instance.Teams[TEAM_TYPE.TEAM_1].Color = HSLColor(hue);
                }

                if (Team2ColorSlider.Hovered())
                {
                    Team2SliderKnob.position = new Vector2(Input.GetMouseX() / Canvas.camera.viewport.size.x, Team2SliderKnob.position.y);
                    hue = (Team2SliderKnob.position.x - Team2ColorSlider.position.x) / Team2ColorSlider.size.x;
                    MatchSystem.instance.Teams[TEAM_TYPE.TEAM_2].Color = HSLColor(hue);
                }
            }

            if (Input.GetMouseButtonUp(Input.MouseButtons.LEFT))
            {
                btnDown = false;
                CheckForSelectedInputBoxes();
            }

            if (HostBtn.Hovered())
            {
                HostBtn.color = Selected;
                if (Input.GetMouseButtonUp(Input.MouseButtons.LEFT))
                {
                    if (NotSimilarColor && NotSameName)
                    {
                        CameraMaster.instance.State = CAM_STATE.SELECT_TEAM;
                        //Set match options
                        MatchSystem.instance.Teams[TEAM_TYPE.TEAM_1].Name = Team1.text;
                        MatchSystem.instance.Teams[TEAM_TYPE.TEAM_2].Name = Team2.text;
                        MatchSystem.instance.MaxPlayers = ConvertToInt(MaxPlayersString.text);
                        MatchSystem.instance.MatchLength = ConvertToInt(TimeRoundString.text) * 60; //Convert to seconds.
                        MatchSystem.instance.ScoreLimit = ConvertToInt(ScoreLimitString.text);
                        MatchSystem.instance.ServerName = ServerNameString.text;
                        MatchSystem.instance.PublicServer = PublicServerCheck.scale != Vector2.Zero ? true : false;
                        MatchSystem.instance.SpawnPowerupsDuringGame = PowerUpsCheck.scale != Vector2.Zero ? true : false;

                        MatchSystem.instance.LocalPort = ConvertToInt(PortNameString.text);
                        MatchSystem.instance.Init();
                        MatchSystem.instance.Host();
                    }
                }
            }
            else
                HostBtn.color = Unselected;

            if (ExitBtn.Hovered())
            {
                ExitBtn.color = Selected;
                if (Input.GetMouseButtonUp(Input.MouseButtons.LEFT))
                {
                    CameraMaster.instance.State = CAM_STATE.MAIN_MENU;
                }
            }
            else
                ExitBtn.color = Unselected;

            TakeKeyboardInput();
        }
    }

    /*
     * Can't get this to work properly.
     */
    private float HueFromRGB(Color rgba)
    {
        Vector3 rgb = rgba.ToVector3() / 255.0f;
        float hue = 0;

        float _min = Math.Min(Math.Min(rgb.x, rgb.y), rgb.z);
        float _max = Math.Max(Math.Max(rgb.x, rgb.y), rgb.z);
        float _delta = _max - _min;
        if (_delta != 0)
        {
            if (rgb.x == _max)
                hue = (rgb.y - rgb.z) / _delta;
            else if (rgb.y == _max)
                hue = 2.0f + (rgb.z - rgb.x) / _delta;
            else if (rgb.z == _max)
                hue = 4.0f + (rgb.x - rgb.y) / _delta;
        }
        Debug.Log("Hue: " + hue);
        Debug.Log("R: " + rgb.x);
        Debug.Log("G: " + rgb.y);
        Debug.Log("B: " + rgb.z);
        return hue;
    }

    private Color HSLColor(double hue)
    {
        double saturation = 1.0d;
        double luminosity = 0.5d;

        byte r, g, b;

        double t1, t2;
        double th = hue;

        t2 = (luminosity + saturation) - (luminosity * saturation);
        t1 = 2d * luminosity - t2;

        double tr, tg, tb;
        tr = th + (1.0d / 3.0d);
        tg = th;
        tb = th - (1.0d / 3.0d);

        tr = ColorCalc(tr, t1, t2);
        tg = ColorCalc(tg, t1, t2);
        tb = ColorCalc(tb, t1, t2);
        r = (byte)Math.Round(tr * 255d);
        g = (byte)Math.Round(tg * 255d);
        b = (byte)Math.Round(tb * 255d);
        return new Color(r, g, b);
    }

    private double ColorCalc(double c, double t1, double t2)
    {

        if (c < 0) c += 1d;
        if (c > 1) c -= 1d;
        if (6.0d * c < 1.0d) return t1 + (t2 - t1) * 6.0d * c;
        if (2.0d * c < 1.0d) return t2;
        if (3.0d * c < 2.0d) return t1 + (t2 - t1) * (2.0d / 3.0d - c) * 6.0d;
        return t1;
    }

    private void NotSimilarColorTest(Color c1, Color c2)
    {
        float distance = Vector3.Distance(c1.ToVector3(), c2.ToVector3());
        if(distance >= 0.25f)
        {
            NotSimilarColor = true;
            SimilarColor.scale = Vector2.Zero;
        }
        else
        {
            NotSimilarColor = false;
            SimilarColor.scale = Vector2.One;
        }
    }

    private void NotSameNameTest(string n1, string n2)
    {
        if (n1 != n2)
        {
            NotSameName = true;
            SameName.scale = Vector2.Zero;
        }
        else
        {
            NotSameName = false;
            SameName.scale = Vector2.One;
        }
    }

    private int ConvertToInt(string number)
    {
        int result = -1;
        if (Int32.TryParse(number, out result))
        {
            return result;
        }
        else
        {
            Debug.LogError("Not a number");
            return result;
        }
    }

    private void CheckForSelectedInputBoxes()
    {
        InputTeam1Name = false;
        InputTeam2Name = false;
        InputServerName = false;
        InputPortName = false;
        InputMaxPlayers = false;
        InputTimeRound = false;

        Team1TextBox.color = Unselected;
        Team2TextBox.color = Unselected;
        ServerNameBox.color = Unselected;
        PortNameBox.color = Unselected;
        MaxPlayersBox.color = Unselected;
        TimeRoundBox.color = Unselected;

        if (Team1TextBox.Hovered())
        {
            InputTeam1Name = true;
            Team1TextBox.color = Selected;
        }
        else if (Team2TextBox.Hovered())
        {
            InputTeam2Name = true;
            Team2TextBox.color = Selected;
        }
        else if (ServerNameBox.Hovered())
        {
            InputServerName = true;
            ServerNameBox.color = Selected;
        }
        else if (PortNameBox.Hovered())
        {
            InputPortName = true;
            PortNameBox.color = Selected;
        }
        else if (MaxPlayersBox.Hovered())
        {
            InputMaxPlayers = true;
            MaxPlayersBox.color = Selected;
        }
        else if (TimeRoundBox.Hovered())
        {
            InputTimeRound = true;
            TimeRoundBox.color = Selected;
        }
        else if (ScoreLimitBox.Hovered())
        {
            InputScoreLimit = true;
            ScoreLimitBox.color = Selected;
        }
        else if (PublicServerBox.Hovered())
        {
            if (PublicServerCheck.scale != Vector2.Zero)
                PublicServerCheck.scale = Vector2.Zero;
            else
                PublicServerCheck.scale = Vector2.One;
        }
        else if (PowerUpsBox.Hovered())
        {
            if (PowerUpsCheck.scale != Vector2.Zero)
                PowerUpsCheck.scale = Vector2.Zero;
            else
                PowerUpsCheck.scale = Vector2.One;
        }
    }

    private void TakeKeyboardInput()
    {
        if (InputTeam1Name)
        {
            string str = Team1.text;
            GUIInput.AppendString(ref str, 15);
            Team1.text = str;
        }
        else if (InputTeam2Name)
        {
            string str = Team2.text;
            GUIInput.AppendString(ref str, 15);
            Team2.text = str;
        }
        else if (InputServerName)
        {
            string str = ServerNameString.text;
            GUIInput.AppendString(ref str, 15);
            ServerNameString.text = str;
        }
        else if (InputPortName)
        {
            string str = PortNameString.text;
            GUIInput.AppendString(ref str, 4);
            PortNameString.text = str;
        }
        else if (InputMaxPlayers)
        {
            string str = MaxPlayersString.text;
            GUIInput.AppendString(ref str, 3);
            MaxPlayersString.text = str;
        }
        else if (InputTimeRound)
        {
            string str = TimeRoundString.text;
            GUIInput.AppendString(ref str, 2);
            TimeRoundString.text = str;
        }
        else if (InputScoreLimit)
        {
            string str = ScoreLimitString.text;
            GUIInput.AppendString(ref str, 3);
            ScoreLimitString.text = str;
        }
    }
}
