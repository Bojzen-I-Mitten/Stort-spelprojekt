using System;
using ThomasEngine;

public class GUIHostMenu : ScriptComponent
{
    Camera Camera;
    public Canvas Canvas;

    public Texture2D ColorSliderTexture { get; set; }
    public Texture2D SliderKnobTexture { get; set; }
    public Texture2D TextBoxTexture { get; set; }
    public Texture2D WhiteBoxTexture { get; set; }
    public Font Font { get; set; }

    //public Vector2 _ServerNamePos { get; set; }          = new Vector2(0.0f, 0.4f);
    //public Vector2 _ServerNameStringPos { get; set; }    = new Vector2(0.0f, 0.4f);
    //public Vector2 _ServerNameBoxPos { get; set; }       = new Vector2(0.0f, 0.4f);

    //public Vector2 _MaxPlayersPos { get; set; }          = new Vector2(0.0f, 0.4f);
    //public Vector2 _MaxPlayersStringPos { get; set; }    = new Vector2(0.0f, 0.4f);
    //public Vector2 _MaxPlayersBoxPos { get; set; }       = new Vector2(0.0f, 0.4f);

    //public Vector2 _TimeRoundPos { get; set; }           = new Vector2(0.0f, 0.4f);
    //public Vector2 _TimeRoundStringPos { get; set; }     = new Vector2(0.0f, 0.4f);
    //public Vector2 _TimeRoundBoxPos { get; set; }        = new Vector2(0.0f, 0.4f);

    //public Vector2 _PublicServerPos { get; set; }        = new Vector2(0.0f, 0.4f);
    //public Vector2 _PublicServerCheckPos { get; set; }   = new Vector2(0.0f, 0.4f);
    //public Vector2 _PublicServerBoxPos { get; set; }     = new Vector2(0.0f, 0.4f);

    //public Vector2 _ScoreLimitPos { get; set; }          = new Vector2(0.0f, 0.4f);
    //public Vector2 _ScoreLimitStringPos { get; set; }    = new Vector2(0.0f, 0.4f);
    //public Vector2 _ScoreLimitBoxPos { get; set; }       = new Vector2(0.0f, 0.4f);

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
    string _ServerNameString = "My server";
    Image ServerNameBox;

    Text MaxPlayers;
    Text MaxPlayersString;
    string _MaxPlayersString = "8";
    Image MaxPlayersBox;

    Text TimeRound;
    Text TimeRoundString;
    string _TimeRoundString = "5";
    Image TimeRoundBox;

    Text PublicServer;
    Text PublicServerCheck;
    Image PublicServerBox;

    Text ScoreLimit;
    Text ScoreLimitString;
    string _ScoreLimitString = "10";
    Image ScoreLimitBox;

    Text PowerUps;
    Text PowerUpsCheck;
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
    string _team1 = "Team 1";
    string _team2 = "Team 2";
    #endregion

    bool InputTeam1Name = false;
    bool InputTeam2Name = false;
    bool NotSameName = true;
    bool NotSimilarColor = true;
    
    public string PortString;

    public override void Start()
    {
        Camera = gameObject.GetComponent<Camera>();
        Canvas = Camera.AddCanvas();

        btnDown = false;

        HostBtn = Canvas.Add("Host Game");
        HostBtn.color = Color.FloralWhite;
        HostBtn.position = new Vector2(0.47f, 0.33f);
        HostBtn.font = Font;
        HostBtn.interactable = true;

        ExitBtn = Canvas.Add("Exit");
        ExitBtn.color = Color.FloralWhite;
        ExitBtn.position = new Vector2(0.32f, 0.33f);
        ExitBtn.font = Font;
        ExitBtn.interactable = true;

        #region Host Setings

        HostOptions = Canvas.Add("Host Options");
        HostOptions.position = new Vector2(0.5f, 0f);
        HostOptions.origin = new Vector2(0.5f, 0.0f);
        HostOptions.font = Font;
        HostOptions.color = Color.FloralWhite;

        if (WhiteBoxTexture != null)
        {
            HostBg = Canvas.Add(WhiteBoxTexture);
            HostBg.color = new Color(0.5f, 0.5f, 0.5f, 0.5f);
            HostBg.scale = new Vector2(6.4f, 1.44f);
            HostBg.depth = 0.7f;
        }

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

        float row1 = 0.07f;
        float row2 = 0.14f;
        float row3 = 0.21f;

        #region Server Name
        ServerName = Canvas.Add("Server name:");
        ServerName.position = new Vector2(column1, row1);
        ServerName.scale = new Vector2(0.5f);
        ServerName.color = Color.FloralWhite;
        ServerName.font = Font;
        ServerName.depth = 0.4f;

        ServerNameString = Canvas.Add(_ServerNameString);
        ServerNameString.position = new Vector2(column2, row1);
        ServerNameString.scale = new Vector2(0.5f);
        ServerNameString.color = Color.FloralWhite;
        ServerNameString.font = Font;
        ServerNameString.depth = 0.4f;

        if (TextBoxTexture != null)
        {
            ServerNameBox = Canvas.Add(TextBoxTexture);
            ServerNameBox.position = new Vector2(column2, row1);
            ServerNameBox.scale = new Vector2(0.5f);
            ServerNameBox.color = Color.FloralWhite;
            ServerNameBox.interactable = true;
            ServerNameBox.depth = 0.5f;

        }
        #endregion

        #region Max Players
        MaxPlayers = Canvas.Add("Max players:");
        MaxPlayers.position = new Vector2(column1, row2);
        MaxPlayers.scale = new Vector2(0.5f);
        MaxPlayers.color = Color.FloralWhite;
        MaxPlayers.font = Font;
        MaxPlayers.depth = 0.4f;

        MaxPlayersString = Canvas.Add(_MaxPlayersString);
        MaxPlayersString.position = new Vector2(column2, row2);
        MaxPlayersString.scale = new Vector2(0.5f);
        MaxPlayersString.color = Color.FloralWhite;
        MaxPlayersString.font = Font;
        MaxPlayersString.depth = 0.4f;

        if (TextBoxTexture != null)
        {
            MaxPlayersBox = Canvas.Add(TextBoxTexture);
            MaxPlayersBox.position = new Vector2(column2, row2);
            MaxPlayersBox.scale = new Vector2(0.5f);
            MaxPlayersBox.interactable = true;
            MaxPlayersBox.depth = 0.5f;
        }
        #endregion

        #region Time Round
        TimeRound = Canvas.Add("Time per round:");
        TimeRound.position = new Vector2(column1, row3);
        TimeRound.scale = new Vector2(0.5f);
        TimeRound.color = Color.FloralWhite;
        TimeRound.font = Font;
        TimeRound.depth = 0.4f;

        TimeRoundString = Canvas.Add(_TimeRoundString);
        TimeRoundString.position = new Vector2(column2, row3);
        TimeRoundString.scale = new Vector2(0.5f);
        TimeRoundString.color = Color.FloralWhite;
        TimeRoundString.font = Font;
        TimeRoundString.depth = 0.4f;

        if (TextBoxTexture != null)
        {
            TimeRoundBox = Canvas.Add(TextBoxTexture);
            TimeRoundBox.position = new Vector2(column2, row3);
            TimeRoundBox.scale = new Vector2(0.5f);
            TimeRoundBox.interactable = true;
            TimeRoundBox.depth = 0.5f;
        }
        #endregion

        #region Public Server
        PublicServer = Canvas.Add("Public server:");
        PublicServer.position = new Vector2(column3, row1);
        PublicServer.scale = new Vector2(0.5f);
        PublicServer.color = Color.FloralWhite;
        PublicServer.font = Font;
        PublicServer.depth = 0.4f;

        PublicServerCheck = Canvas.Add("X");
        PublicServerCheck.position = new Vector2(column4, row1);
        PublicServerCheck.scale = new Vector2(0.5f);
        PublicServerCheck.color = Color.Black;
        PublicServerCheck.font = Font;
        PublicServerCheck.depth = 0.4f;

        if (WhiteBoxTexture != null)
        {
            PublicServerBox = Canvas.Add(WhiteBoxTexture);
            PublicServerBox.position = new Vector2(column4, row1);
            PublicServerBox.scale = new Vector2(0.1f);
            PublicServerBox.interactable = true;
            PublicServerBox.depth = 0.5f;
        }
        #endregion

        #region Score limit
        ScoreLimit = Canvas.Add("Score Limit:");
        ScoreLimit.position = new Vector2(column3, row2);
        ScoreLimit.scale = new Vector2(0.5f);
        ScoreLimit.color = Color.FloralWhite;
        ScoreLimit.font = Font;
        ScoreLimit.depth = 0.4f;

        ScoreLimitString = Canvas.Add(_ScoreLimitString);
        ScoreLimitString.position = new Vector2(column4, row2);
        ScoreLimitString.scale = new Vector2(0.5f);
        ScoreLimitString.color = Color.FloralWhite;
        ScoreLimitString.font = Font;
        ScoreLimitString.depth = 0.4f;

        if (TextBoxTexture != null)
        {
            ScoreLimitBox = Canvas.Add(TextBoxTexture);
            ScoreLimitBox.position = new Vector2(column4, row2);
            ScoreLimitBox.scale = new Vector2(0.5f);
            ScoreLimitBox.interactable = true;
            ScoreLimitBox.depth = 0.5f;
        }
        #endregion

        #region Power Ups
        PowerUps = Canvas.Add("Power ups:");
        PowerUps.position = new Vector2(column3, row3);
        PowerUps.scale = new Vector2(0.5f);
        PowerUps.color = Color.FloralWhite;
        PowerUps.font = Font;
        PowerUps.depth = 0.4f;

        PowerUpsCheck = Canvas.Add("X");
        PowerUpsCheck.position = new Vector2(column4, row3);
        PowerUpsCheck.scale = new Vector2(0.5f);
        PowerUpsCheck.color = Color.Black;
        PowerUpsCheck.font = Font;
        PowerUpsCheck.depth = 0.4f;

        if (WhiteBoxTexture != null)
        {
            PowerUpsBox = Canvas.Add(WhiteBoxTexture);
            PowerUpsBox.position = new Vector2(column4, row3);
            PowerUpsBox.scale = new Vector2(0.1f);
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

        Team1 = Canvas.Add(_team1);
        Team1.position = new Vector2(0.02f, 0.4f);
        Team1.color = Color.FloralWhite;
        Team1.font = Font;

        Team2 = Canvas.Add(_team2);
        Team2.position = new Vector2(0.69f, 0.4f);
        Team2.color = Color.FloralWhite;
        Team2.font = Font;

        if (TextBoxTexture != null)
        {
            Team1TextBox = Canvas.Add(TextBoxTexture);
            Team1TextBox.position = new Vector2(0.01f, 0.39f);
            Team1TextBox.scale = new Vector2(0.9f, 1f);
            Team1TextBox.interactable = true;

            Team2TextBox = Canvas.Add(TextBoxTexture);
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
            //Team1SliderKnob.color = MatchSystem.instance.Teams[TEAM_TYPE.TEAM_1].Color;

            Team2SliderKnob = Canvas.Add(SliderKnobTexture);
            Team2SliderKnob.position = team2SliderKnobPos + Team2ColorSlider.size * new Vector2(0.66f, 0);  //+... Is to offset it to blue color. Derp.
            Team2SliderKnob.origin = new Vector2(0.5f, 0.0f);
            //Team2SliderKnob.color = MatchSystem.instance.Teams[TEAM_TYPE.TEAM_2].Color;
        }
        #endregion
    }

    public override void Awake()
    {
    }

    public override void Update()
    {
        if (Canvas.isRendering)
        {
            if (Input.GetMouseButtonDown(Input.MouseButtons.LEFT) || btnDown)
            {
                btnDown = true;
                float hue = 0.0f;

                if (Team1ColorSlider.Hovered())
                {
                    Team1SliderKnob.position = new Vector2(Input.GetMouseX() / Canvas.camera.viewport.size.x, Team1SliderKnob.position.y);
                    hue = (Team1SliderKnob.position.x - Team1ColorSlider.position.x) / Team1ColorSlider.size.x;
                    //Team1SliderKnob.color = HSLColor(hue);
                    MatchSystem.instance.Teams[TEAM_TYPE.TEAM_1].Color = HSLColor(hue);
                }

                if (Team2ColorSlider.Hovered())
                {
                    Team2SliderKnob.position = new Vector2(Input.GetMouseX() / Canvas.camera.viewport.size.x, Team2SliderKnob.position.y);
                    hue = (Team2SliderKnob.position.x - Team2ColorSlider.position.x) / Team2ColorSlider.size.x;
                    //Team2SliderKnob.color = HSLColor(hue);
                    MatchSystem.instance.Teams[TEAM_TYPE.TEAM_2].Color = HSLColor(hue);
                }
            }

            if (Input.GetMouseButtonUp(Input.MouseButtons.LEFT))
            {
                btnDown = false;
                Team1TextBox.color = Color.White;
                Team2TextBox.color = Color.White;

                if (Team1TextBox.Hovered())
                {
                    InputTeam1Name = true;
                    InputTeam2Name = false;
                    Team1TextBox.color = Color.Green;
                }
                else if (Team2TextBox.Hovered())
                {
                    InputTeam1Name = false;
                    InputTeam2Name = true;
                    Team2TextBox.color = Color.Green;
                }
            }

            if (HostBtn.Hovered())
            {
                HostBtn.color = Color.IndianRed;
                if (Input.GetMouseButtonUp(Input.MouseButtons.LEFT))
                {
                    if (NotSimilarColor && NotSameName)
                    {
                        CameraMaster.instance.State = CAM_STATE.SELECT_TEAM;
                        MatchSystem.instance.Teams[TEAM_TYPE.TEAM_1].Name = Team1.text;
                        MatchSystem.instance.Teams[TEAM_TYPE.TEAM_2].Name = Team2.text;
                        MatchSystem.instance.Init();
                        MatchSystem.instance.Host();
                    }
                }
            }
            else
                HostBtn.color = Color.FloralWhite;

            if (ExitBtn.Hovered())
            {
                ExitBtn.color = Color.IndianRed;
                if (Input.GetMouseButtonUp(Input.MouseButtons.LEFT))
                {
                    CameraMaster.instance.State = CAM_STATE.MAIN_MENU;
                }
            }
            else
                ExitBtn.color = Color.FloralWhite;

            if (InputTeam1Name)
            {
                GUIInput.AppendString(ref _team1, 15);
                Team1.text = _team1;
            }
            if (InputTeam2Name)
            {
                GUIInput.AppendString(ref _team2, 15);
                Team2.text = _team2;
            }

            NotSimilarColorTest(MatchSystem.instance.Teams[TEAM_TYPE.TEAM_1].Color, MatchSystem.instance.Teams[TEAM_TYPE.TEAM_2].Color);
            NotSameNameTest(Team1.text, Team2.text);

            //ServerName.position = _ServerNamePos;
            //ServerNameString.position = _ServerNameStringPos;
            //ServerNameBox.position = _ServerNameBoxPos;

            //MaxPlayers.position = _MaxPlayersPos;
            //MaxPlayersString.position = _MaxPlayersStringPos;
            //MaxPlayersBox.position = _MaxPlayersBoxPos;

            //TimeRound.position = _TimeRoundPos;
            //TimeRoundString.position = _TimeRoundStringPos;
            //TimeRoundBox.position = _TimeRoundBoxPos;

            //PublicServer.position = _PublicServerPos;
            //PublicServerCheck.position = _PublicServerCheckPos;
            //PublicServerBox.position = _PublicServerBoxPos;

            //ScoreLimit.position = _ScoreLimitPos;
            //ScoreLimitString.position = _ScoreLimitStringPos;
            //ScoreLimitBox.position = _ScoreLimitBoxPos;

            //PowerUps.position = _PowerUpsPos;
            //PowerUpsCheck.position = _PowerUpsCheckPos;
            //PowerUpsBox.position = _PowerUpsBoxPos;


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
}
