using System;
using System.Collections.Generic;
using ThomasEngine;

public class GUIHostMenu : ScriptComponent
{
    Camera Camera;
    public Canvas Canvas;

    public Texture2D ColorSliderTexture { get; set; }
    public Texture2D SliderKnobTexture { get; set; }
    public Texture2D newBG { get; set; }
    public Texture2D TextBoxTexture { get; set; }
    public Texture2D WhiteBoxTexture { get; set; }
    public Font Font { get; set; }
    public int NrTeams { get; set; } = 2;

    //public Vector2 TextBox1Pos { get; set; } = Vector2.Zero;
    //public Vector2 TextBox1Scale { get; set; } = Vector2.One;
    //public Vector4 HostBGColor { get; set; } = Vector4.One;

    #region Host Setings
    Image HostBg;
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
    Image BG;

    bool btnDown;
    string _team1 = "Team 1";
    string _team2 = "Team 2";
    #endregion

    public override void Start()
    {
    }

    public override void Awake()
    {
        Camera = gameObject.GetComponent<Camera>();
        Canvas = Camera.AddCanvas();

        btnDown = false;

        BG = Canvas.Add(newBG);
        BG.depth = 0.6f;

        #region Host Setings

        if(WhiteBoxTexture != null)
        {
            HostBg = Canvas.Add(WhiteBoxTexture);
            HostBg.color = new Color(0.5f, 0.5f, 0.5f, 0.5f);
            HostBg.scale = new Vector2(6.4f, 1.44f); ;
        }

        #endregion

        #region Teams
        if (WhiteBoxTexture != null)
        {
            Team1BG = Canvas.Add(WhiteBoxTexture);
            Team1BG.position = new Vector2(0.0f, 0.4f);
            Team1BG.scale = new Vector2(2, 2.2f);
            Team1BG.color = new Color(0.5f, 0.5f, 0.5f, 0.5f);
            Team1BG.depth = 0.5f;

            Team2BG = Canvas.Add(WhiteBoxTexture);
            Team2BG.position = new Vector2(0.67f, 0.4f);
            Team2BG.scale = new Vector2(2.2f, 2.4f);
            Team2BG.color = new Color(0.5f, 0.5f, 0.5f, 0.5f);
            Team2BG.depth = 0.5f;
        }

        Team1 = Canvas.Add(_team1);
        Team1.position = new Vector2(0.02f, 0.4f);
        Team1.color = Color.Black;
        Team1.font = Font;

        Team2 = Canvas.Add(_team2);
        Team2.position = new Vector2(0.69f, 0.4f);
        Team2.color = Color.Black;
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
            Team1SliderKnob.origin = new Vector2(0.5f);
            Team1SliderKnob.color = HSLColor(0d);

            Team2SliderKnob = Canvas.Add(SliderKnobTexture);
            Team2SliderKnob.position = team2SliderKnobPos;
            Team2SliderKnob.origin = new Vector2(0.5f);
            Team2SliderKnob.color = HSLColor(0d);
        }
        #endregion
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
                    Team1SliderKnob.position = new Vector2(Input.GetMouseX() / Canvas.camera.viewport.size.x, Team1SliderKnob.position.y);
                hue = (Team1SliderKnob.position.x - Team1ColorSlider.position.x) / (Team1ColorSlider.size.x / Canvas.camera.viewport.size.x);
                Team1SliderKnob.color = HSLColor(hue);

                if (Team2ColorSlider.Hovered())
                    Team2SliderKnob.position = new Vector2(Input.GetMouseX() / Canvas.camera.viewport.size.x, Team2SliderKnob.position.y);
                hue = (Team2SliderKnob.position.x - Team2ColorSlider.position.x) / (Team2ColorSlider.size.x / Canvas.camera.viewport.size.x);
                Team2SliderKnob.color = HSLColor(hue);
            }

            if (Input.GetMouseButtonUp(Input.MouseButtons.LEFT))
            {
                btnDown = false;
                Team1TextBox.color = Color.White;
                Team2TextBox.color = Color.White;

                if (Team1TextBox.Hovered())
                { 
                    Team1TextBox.color = Color.Green;
                }
                else if (Team2TextBox.Hovered())
                {
                    Team2TextBox.color = Color.Green;
                }
            }
        }
    }

    private Color HSLColor(double hue)
    {
        double saturation = 1.0d;
        double luminosity = 0.5d;

        byte r, g, b;

        double t1, t2;
        double th = hue; // /6.0d;

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
}