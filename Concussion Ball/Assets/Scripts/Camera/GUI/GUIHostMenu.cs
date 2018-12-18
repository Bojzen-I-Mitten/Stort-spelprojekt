using System;
using System.Collections;
using ThomasEngine;

public class GUIHostMenu : ScriptComponent
{
    Camera Camera;
    public Canvas Canvas;

    public Texture2D HostMenuBGTexture { get; set; }
    public Texture2D ColorSliderTexture { get; set; }
    public Texture2D SliderKnobTexture { get; set; }
    public Texture2D TextBox6010Texture { get; set; }
    public Texture2D TextBox305Texture { get; set; }
    public Texture2D TextBox105Texture { get; set; }
    public Texture2D TextBoxBG55Texture { get; set; }
    public Texture2D TextBoxCross55Texture { get; set; }
    public Font Font { get; set; }

    //public Vector2 _ServerNamePos           { get; set; }
    //public Vector2 _ServerNameStringPos     { get; set; }
    //public Vector2 _ServerNameBoxPos        { get; set; }

    //public Vector2 _MaxPlayersPos           { get; set; }
    //public Vector2 _MaxPlayersStringPos     { get; set; }
    //public Vector2 _MaxPlayersBoxPos        { get; set; }

    //public Vector2 _TimeRoundPos            { get; set; }
    //public Vector2 _TimeRoundStringPos      { get; set; }
    //public Vector2 _TimeRoundBoxPos         { get; set; }

    //public Vector2 _PortNamePos             { get; set; }
    //public Vector2 _PortNameStringPos       { get; set; }
    //public Vector2 _PortNameBoxPos          { get; set; }

    //public Vector2 _PublicServerPos         { get; set; }
    //public Vector2 _PublicServerCheckPos    { get; set; }
    //public Vector2 _PublicServerBoxPos      { get; set; }

    //public Vector2 _ScoreLimitPos           { get; set; }
    //public Vector2 _ScoreLimitStringPos     { get; set; }
    //public Vector2 _ScoreLimitBoxPos        { get; set; }

    //public Vector2 _PowerUpsPos             { get; set; }
    //public Vector2 _PowerUpsCheckPos        { get; set; }
    //public Vector2 _PowerUpsBoxPos          { get; set; }

    Text HostBtn;
    Text ExitBtn;
    Image HostMenuBg;

    #region Host Setings
    Text HostOptions;

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

   /* Text PublicServer;
    Image PublicServerCheck;
    Image PublicServerBox;

   // Text ScoreLimit;
    Text ScoreLimitString;
    Image ScoreLimitBox;

    //Text PowerUps;
    Image PowerUpsCheck;
    Image PowerUpsBox;*/

    Text SimilarColor;
    Text SameName;

    #endregion

    #region Teams
    Text Team1;
    Text Team2;

    Image Team1TextBox;
    Image Team1ColorSlider;
    Image Team1SliderKnob;

    Image Team2TextBox;
    Image Team2ColorSlider;
    Image Team2SliderKnob;

    #endregion
    Text Caret;

    IEnumerator Blink = null;

    public float CaretOffsetBig { get; set; } = 0;
    public float CaretOffsetSmall { get; set; } = 0;

    #region Take Inputs
    bool InputTeam1Name = false;
    bool InputTeam2Name = false;
    bool InputServerName = false;
    bool InputPortName = false;
    bool InputMaxPlayers = false;
    bool InputTimeRound = false;
    bool InputScoreLimit = false;
    #endregion

    bool ClearTeam1 = true;
    bool ClearTeam2 = true;
    bool ClearServerName = true;
    bool ClearPort = false;
    bool ClearTime = false;
    bool ClearMaxPlayers = false;
    bool ClearScoreLimit = false;

    bool NotSameName = true;
    bool NotSimilarColor = true;

    Color Unselected = Color.FloralWhite;
    Color Selected = Color.IndianRed;
    Vector3 HostMenuCamPos;
    Vector3 HostMenuCamRot;
    Vector3 Chad1Pos;
    Vector3 Chad1Rot;
    Vector3 Chad2Pos;
    Vector3 Chad2Rot;

    public override void OnAwake()
    {
        Camera = gameObject.GetComponent<Camera>();
        Canvas = Camera.AddCanvas();

        CaretOffsetBig = 0.04f;

        HostMenuCamPos = new Vector3(40, -198.108f, 8.208f);
        HostMenuCamRot = new Vector3(MathHelper.Pi, 0.0f, 0.0f);
        Chad1Pos = new Vector3(0.866f, 0, 0.561f);
        Chad1Rot = new Vector3(45, 0, 0);
        Chad2Pos = new Vector3(-0.717f, 0, 0.554f);
        Chad2Rot = new Vector3(-20, 0, 0);

        HostBtn = Canvas.Add("Host Game");
        HostBtn.color = Unselected;
        HostBtn.position = new Vector2(0.47f, 0.32f);
        HostBtn.interactable = true;

        ExitBtn = Canvas.Add("Exit");
        ExitBtn.color = Unselected;
        ExitBtn.position = new Vector2(0.32f, 0.32f);
        ExitBtn.interactable = true;

        if (HostMenuBGTexture != null)
        {
            HostMenuBg = Canvas.Add(HostMenuBGTexture);
            HostMenuBg.color = new Color(0.5f, 0.5f, 0.5f, 0.5f);
            HostMenuBg.depth = 0.7f;
        }

        #region Host Setings

        HostOptions = Canvas.Add("Host Options");
        HostOptions.position = new Vector2(0.5f, 0f);
        HostOptions.origin = new Vector2(0.5f, 0.0f);
        HostOptions.color = Unselected;

        #region Color Check
        SimilarColor = Canvas.Add("Teams have similar color, can't host");
        SimilarColor.position = new Vector2(0.5f, 0.52f);
        SimilarColor.origin = new Vector2(0.5f);
        SimilarColor.color = Color.Red;
        SimilarColor.scale = Vector2.Zero;
        SimilarColor.depth = 0;
        #endregion

        #region Name Check
        SameName = Canvas.Add("Teams have same name, can't host");
        SameName.position = new Vector2(0.5f, 0.58f);
        SameName.origin = new Vector2(0.5f);
        SameName.color = Color.Red;
        SameName.scale = Vector2.Zero;
        SameName.depth = 0;
        #endregion

        float column1 = 0.00f;
        float column2 = 0.17f;
        float column3 = 0.52f;
        float column4 = 0.7f;

        float row1 = 0.130f;
        float row2 = 0.185f;
        float row3 = 0.244f;
        float row4 = 0.299f;

        #region Server Name
        ServerName = Canvas.Add("Server name");
        ServerName.position = new Vector2(column1, row1);
        ServerName.scale = new Vector2(0.5f);
        ServerName.origin = new Vector2(0.0f, 0.5f);
        ServerName.color = Unselected;
        ServerName.depth = 0.4f;

        ServerNameString = Canvas.Add("Pelles server");
        ServerNameString.position = new Vector2(column2, row1);
        ServerNameString.scale = new Vector2(0.5f);
        ServerNameString.origin = new Vector2(0.0f, 0.5f);
        ServerNameString.color = Unselected;
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
        PortName = Canvas.Add("Port");
        PortName.position = new Vector2(column1, row2);
        PortName.scale = new Vector2(0.5f);
        PortName.origin = new Vector2(0.0f, 0.5f);
        PortName.color = Unselected;
        PortName.depth = 0.4f;

        PortNameString = Canvas.Add("9050");
        PortNameString.position = new Vector2(column2, row2);
        PortNameString.scale = new Vector2(0.5f);
        PortNameString.origin = new Vector2(0.0f, 0.5f);
        PortNameString.color = Unselected;
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
        MaxPlayers = Canvas.Add("Max players");
        MaxPlayers.position = new Vector2(column1, row3);
        MaxPlayers.scale = new Vector2(0.5f);
        MaxPlayers.origin = new Vector2(0.0f, 0.5f);
        MaxPlayers.color = Unselected;
        MaxPlayers.depth = 0.4f;

        MaxPlayersString = Canvas.Add("8");
        MaxPlayersString.position = new Vector2(column2, row3);
        MaxPlayersString.scale = new Vector2(0.5f);
        MaxPlayersString.origin = new Vector2(0.0f, 0.5f);
        MaxPlayersString.color = Unselected;
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
        TimeRound = Canvas.Add("Time per round");
        TimeRound.position = new Vector2(column1, row4);
        TimeRound.scale = new Vector2(0.5f);
        TimeRound.origin = new Vector2(0.0f, 0.5f);
        TimeRound.color = Unselected;
        TimeRound.depth = 0.4f;

        TimeRoundString = Canvas.Add("5");
        TimeRoundString.position = new Vector2(column2, row4);
        TimeRoundString.scale = new Vector2(0.5f);
        TimeRoundString.origin = new Vector2(0.0f, 0.5f);
        TimeRoundString.color = Unselected;
        TimeRoundString.depth = 0.4f;

        if (TextBox105Texture != null)
        {
            TimeRoundBox = Canvas.Add(TextBox105Texture);
            TimeRoundBox.position = new Vector2(column2, row4);
            TimeRoundBox.origin = new Vector2(0.0f, 0.5f);
            TimeRoundBox.interactable = true;
            TimeRoundBox.depth = 0.5f;
        }
        #endregion
        /*
                #region Public Server
                PublicServer = Canvas.Add("Public server");
                PublicServer.position = new Vector2(column3, row1);
                PublicServer.scale = new Vector2(0.5f);
                PublicServer.origin = new Vector2(0.0f, 0.5f);
                PublicServer.color = Unselected;
                PublicServer.depth = 0.4f;

                if (TextBoxCross55Texture != null)
                {
                    PublicServerCheck = Canvas.Add(TextBoxCross55Texture);
                    PublicServerCheck.position = new Vector2(column4 , row1);
                    PublicServerCheck.origin = new Vector2(0.0f, 0.5f);
                    PublicServerCheck.depth = 0.4f;
                }

                if (TextBoxBG55Texture != null)
                {
                    PublicServerBox = Canvas.Add(TextBoxBG55Texture);
                    PublicServerBox.position = new Vector2(column4 , row1);
                    PublicServerBox.origin = new Vector2(0.0f, 0.5f);
                    PublicServerBox.interactable = true;
                    PublicServerBox.depth = 0.5f;
                }
                #endregion

                #region Score limit
                ScoreLimit = Canvas.Add("Score Limit");
                ScoreLimit.position = new Vector2(column3, row2);
                ScoreLimit.scale = new Vector2(0.5f);
                ScoreLimit.origin = new Vector2(0.0f, 0.5f);
                ScoreLimit.color = Unselected;
                ScoreLimit.depth = 0.4f;

                ScoreLimitString = Canvas.Add("10");
                ScoreLimitString.position = new Vector2(column4, row2);
                ScoreLimitString.scale = new Vector2(0.5f);
                ScoreLimitString.origin = new Vector2(0.0f, 0.5f);
                ScoreLimitString.color = Unselected;
                ScoreLimitString.depth = 0.4f;

                if (TextBox105Texture != null)
                {
                    ScoreLimitBox = Canvas.Add(TextBox105Texture);
                    ScoreLimitBox.position = new Vector2(column4, row2);
                    ScoreLimitBox.origin = new Vector2(0.0f, 0.5f);
                    ScoreLimitBox.interactable = true;
                    ScoreLimitBox.depth = 0.5f;
                }
                #endregion
                
        #region Power Ups
        PowerUps = Canvas.Add("Power ups");
        PowerUps.position = new Vector2(column3, row3);
        PowerUps.scale = new Vector2(0.5f);
        PowerUps.origin = new Vector2(0.0f, 0.5f);
        PowerUps.color = Unselected;
        PowerUps.depth = 0.4f;

        if (TextBoxCross55Texture != null)
        {
            PowerUpsCheck = Canvas.Add(TextBoxCross55Texture);
            PowerUpsCheck.position = new Vector2(column4 , row3);
            PowerUpsCheck.origin = new Vector2(0.0f, 0.5f);
            PowerUpsCheck.depth = 0.4f;
        }

        if (TextBoxBG55Texture != null)
        {
            PowerUpsBox = Canvas.Add(TextBoxBG55Texture);
            PowerUpsBox.position = new Vector2(column4, row3);
            PowerUpsBox.origin = new Vector2(0.0f, 0.5f);
            PowerUpsBox.interactable = true;
            PowerUpsBox.depth = 0.5f;
        }
        #endregion
*/
        #endregion

        #region Teams

        Team1 = Canvas.Add("Team 1");
        Team1.position = new Vector2(0.02f, 0.4f);
        Team1.color = Unselected;
        Team1.font = Font;

        Team2 = Canvas.Add("Team 2");
        Team2.position = new Vector2(0.72f, 0.4f);
        Team2.color = Unselected;
        Team2.font = Font;

        if (TextBox6010Texture != null)
        {
            Team1TextBox = Canvas.Add(TextBox6010Texture);
            Team1TextBox.position = new Vector2(0.01f, 0.39f);
            Team1TextBox.scale = new Vector2(0.9f, 1f);
            Team1TextBox.interactable = true;

            Team2TextBox = Canvas.Add(TextBox6010Texture);
            Team2TextBox.position = new Vector2(0.99f, 0.39f);
            Team2TextBox.origin = new Vector2(1, 0);
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

        Caret = Canvas.Add("");
        Caret.origin = new Vector2(0, 0.5f);
        Caret.scale = new Vector2(0.5f);
        Caret.interactable = false;
        Caret.depth = 0.6f;
        Caret.color = Color.Black;
    }

    public override void Update()
    {
        NotSimilarColorTest(MatchSystem.instance.Teams[TEAM_TYPE.TEAM_1].Color, MatchSystem.instance.Teams[TEAM_TYPE.TEAM_2].Color);
        NotSameNameTest(Team1.text, Team2.text);
        AdjustMaxPlayers();

        if (Input.GetMouseButton(Input.MouseButtons.LEFT))
        {
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


        CheckButtonInteraction();
        CheckForSelectedInputBoxes();
        TakeKeyboardInput();

        #region Debug
        //ServerName.position = _ServerNamePos;
        //ServerNameString.position = _ServerNameStringPos;
        //ServerNameBox.position = _ServerNameBoxPos;

        //MaxPlayers.position = _MaxPlayersPos;
        //MaxPlayersString.position = _MaxPlayersStringPos;
        //MaxPlayersBox.position = _MaxPlayersBoxPos;

        //TimeRound.position = _TimeRoundPos;
        //TimeRoundString.position = _TimeRoundStringPos;
        //TimeRoundBox.position = _TimeRoundBoxPos;

        //PortName.position = _PortNamePos;
        //PortNameString.position = _PortNameStringPos;
        //PortNameBox.position = _PortNameBoxPos;

        //PublicServer.position = _PublicServerPos;
        //PublicServerCheck.position = _PublicServerCheckPos;
        //PublicServerBox.position = _PublicServerBoxPos;

        //ScoreLimit.position = _ScoreLimitPos;
        //ScoreLimitString.position = _ScoreLimitStringPos;
        //ScoreLimitBox.position = _ScoreLimitBoxPos;

        //PowerUps.position = _PowerUpsPos;
        //PowerUpsCheck.position = _PowerUpsCheckPos;
        //PowerUpsBox.position = _PowerUpsBoxPos;
        #endregion
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
        if (distance >= 0.25f)
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
        if (Input.GetMouseButtonUp(Input.MouseButtons.LEFT))
        {
            if (Blink == null)
            {
                Blink = CaretBlink();
                StartCoroutine(Blink);
            }
            InputTeam1Name = false;
            InputTeam2Name = false;
            InputServerName = false;
            InputPortName = false;
            InputMaxPlayers = false;
            InputTimeRound = false;
            InputScoreLimit = false;

            Team1TextBox.color = Unselected;
            Team2TextBox.color = Unselected;
            ServerNameBox.color = Unselected;
            PortNameBox.color = Unselected;
            MaxPlayersBox.color = Unselected;
            TimeRoundBox.color = Unselected;
          //  ScoreLimitBox.color = Unselected;

            if (Team1TextBox.Clicked())
            {
                InputTeam1Name = true;
                if(ClearTeam1)
                {
                    Team1.text = "";
                    ClearTeam1 = false;
                }
            }
            else if (Team2TextBox.Clicked())
            {
                InputTeam2Name = true;
                if (ClearTeam2)
                {
                    Team2.text = "";
                    ClearTeam2 = false;
                }
            }
            else if (ServerNameBox.Clicked())
            {
                InputServerName = true;
                if(ClearServerName)
                {
                    ServerNameString.text = "";
                    ClearServerName = false;
                }
            }
            else if (PortNameBox.Clicked())
            {
                InputPortName = true;
                if(ClearPort)
                {
                    PortNameString.text = "";
                    ClearPort = true;
                }
            }
            else if (MaxPlayersBox.Clicked())
            {
                InputMaxPlayers = true;
                if(ClearMaxPlayers)
                {
                    MaxPlayersString.text = "";
                    ClearMaxPlayers = false;
                }
            }
            else if (TimeRoundBox.Clicked())
            {
                InputTimeRound = true;
                if(ClearTime)
                {
                    TimeRoundString.text = "";
                    ClearTime = false;
                }
            }
           /* else if (ScoreLimitBox.Clicked())
            {
                InputScoreLimit = true;
                if(ClearScoreLimit)
                {
                    ScoreLimitString.text = "";
                    ClearScoreLimit = false;
                }
            }*/
            else
            {
                StopCoroutine(Blink);
                Blink = null;
                Caret.text = "";
            }
/*
            if (PublicServerBox.Clicked())
            {
                if (PublicServerCheck.scale != Vector2.Zero)
                    PublicServerCheck.scale = Vector2.Zero;
                else
                    PublicServerCheck.scale = Vector2.One;
            }
            else if (PowerUpsBox.Clicked())
            {
                if (PowerUpsCheck.scale != Vector2.Zero)
                    PowerUpsCheck.scale = Vector2.Zero;
                else
                    PowerUpsCheck.scale = Vector2.One;
            }*/
        }
    }

    private void CheckButtonInteraction()
    {
        if (HostBtn.Hovered())
        {
            HostBtn.color = Selected;
            if (HostBtn.Clicked())
            {
                if (NotSimilarColor && NotSameName && Team1.text != "" && Team2.text != "")
                {
                    DontTakeInput();
                    AdjustMaxPlayers();

                    CameraMaster.instance.SetState(CAM_STATE.SELECT_TEAM);
                    //Set match options
                    MatchSystem.instance.Teams[TEAM_TYPE.TEAM_1].Name = Team1.text;
                    MatchSystem.instance.Teams[TEAM_TYPE.TEAM_2].Name = Team2.text;
                    MatchSystem.instance.MaxPlayers = ConvertToInt(MaxPlayersString.text);
                    MatchSystem.instance.MatchLength = ConvertToInt(TimeRoundString.text) * 60; //Convert to seconds.
                  //  MatchSystem.instance.ScoreLimit = ConvertToInt(ScoreLimitString.text);
                    MatchSystem.instance.ServerName = ServerNameString.text;
                  //  MatchSystem.instance.PublicServer = PublicServerCheck.scale != Vector2.Zero ? true : false;
                  //  MatchSystem.instance.SpawnPowerupsDuringGame = PowerUpsCheck.scale != Vector2.Zero ? true : false;

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
            if (ExitBtn.Clicked())
            {
                DontTakeInput();
                CameraMaster.instance.SetState(CAM_STATE.MAIN_MENU);
            }
        }
        else
            ExitBtn.color = Unselected;
    }

    private void TakeKeyboardInput()
    {
        Caret.scale = new Vector2(0.5f);
        if (InputTeam1Name)
        {
            string str = Team1.text;
            GUIInput.AppendString(ref str, 15);
            Team1.text = str;
            Caret.position = Team1.position + new Vector2(Team1.size.x, CaretOffsetBig);
            Caret.scale = new Vector2(1);
        }
        else if (InputTeam2Name)
        {
            string str = Team2.text;
            GUIInput.AppendString(ref str, 15);
            Team2.text = str;
            Caret.position = Team2.position + new Vector2(Team2.size.x, CaretOffsetBig);
            Caret.scale = new Vector2(1);
        }
        else if (InputServerName)
        {
            string str = ServerNameString.text;
            GUIInput.AppendString(ref str, 15);
            ServerNameString.text = str;
            Caret.position = ServerNameString.position + new Vector2(ServerNameString.size.x, CaretOffsetSmall);
        }
        else if (InputPortName)
        {
            string str = PortNameString.text;
            GUIInput.AppendString(ref str, 4);
            PortNameString.text = str;
            Caret.position = PortNameString.position + new Vector2(PortNameString.size.x, CaretOffsetSmall);
        }
        else if (InputMaxPlayers)
        {
            string str = MaxPlayersString.text;
            GUIInput.AppendString(ref str, 3);
            MaxPlayersString.text = str;
            Caret.position = MaxPlayersString.position + new Vector2(MaxPlayersString.size.x, -MaxPlayersString.size.y * CaretOffsetSmall);
        }
        else if (InputTimeRound)
        {
            string str = TimeRoundString.text;
            GUIInput.AppendString(ref str, 2);
            TimeRoundString.text = str;
            Caret.position = TimeRoundString.position + new Vector2(TimeRoundString.size.x, -TimeRoundString.size.y * CaretOffsetSmall);
        }
      /*  else if (InputScoreLimit)
        {
            string str = ScoreLimitString.text;
            GUIInput.AppendString(ref str, 3);
            ScoreLimitString.text = str;
            Caret.position = ScoreLimitString.position + new Vector2(ScoreLimitString.size.x, CaretOffsetSmall);
        }*/
    }

    private void DontTakeInput()
    {
        InputTeam1Name = false;
        InputTeam2Name = false;
        InputServerName = false;
        InputPortName = false;
        InputMaxPlayers = false;
        InputTimeRound = false;
        InputScoreLimit = false;
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

    public void SetUpScene()
    {
        if (CameraMaster.instance.ChadTeam1 != null)
        {
            CameraMaster.instance.ChadTeam1.transform.localPosition = Chad1Pos;
            CameraMaster.instance.ChadTeam1.transform.localRotation = Quaternion.CreateFromYawPitchRoll(Chad1Rot.x * MathHelper.Pi / 180, 0, 0);
        }
        if (CameraMaster.instance.ChadTeam2 != null)
        {
            CameraMaster.instance.ChadTeam2.transform.localPosition = Chad2Pos;
            CameraMaster.instance.ChadTeam2.transform.localRotation = Quaternion.CreateFromYawPitchRoll(Chad2Rot.x * MathHelper.Pi / 180, 0, 0);
        }
        transform.position = HostMenuCamPos;
        transform.rotation = Quaternion.CreateFromYawPitchRoll(HostMenuCamRot.x, HostMenuCamRot.y, HostMenuCamRot.z);
        transform.scale = Vector3.One;

    }

    private void AdjustMaxPlayers()
    {
        if (!InputMaxPlayers)
        {
            if (ConvertToInt(MaxPlayersString.text) <= 1)
                MaxPlayersString.text = 2.ToString();
            else
                MaxPlayersString.text = (ConvertToInt(MaxPlayersString.text) + ConvertToInt(MaxPlayersString.text) % 2).ToString();
        }
    }
}
