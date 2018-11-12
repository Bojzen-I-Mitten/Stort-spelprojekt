using System;
using System.Collections.Generic;
using ThomasEngine;

public class GUIHostMenu : ScriptComponent
{
    Camera Camera;
    public Canvas Canvas;

    public Texture2D ColorSliderTexture { get; set; }
    public Texture2D SliderKnobTexture { get; set; }
    public Font Font { get; set; }
    public int NrTeams { get; set; } = 2;

    Text SelectTeamColor;
    List<Text> Teams = new List<Text>();
    List<Image> ColorSliders = new List<Image>();
    List<Image> SliderKnob = new List<Image>();
    List<Image> SelectTiles = new List<Image>();

    bool btnDown;

    public override void Start()
    {
        
    }

    public override void Awake()
    {
        Camera = gameObject.GetComponent<Camera>();
        Canvas = Camera.AddCanvas();

        btnDown = false;

        SelectTeamColor = Canvas.Add("Select team color!");
        SelectTeamColor.color = Color.Black;
        SelectTeamColor.position = new Vector2(0.3f, 0.1f);
        SelectTeamColor.origin = new Vector2(0.5f);
        SelectTeamColor.font = Font;

        if (ColorSliderTexture != null)
        {
            for (int team = 0; team < NrTeams; team++)
            {
                Text t = Canvas.Add("Team " + (team + 1) + ":");
                t.position = new Vector2(0.05f, 0.25f + 0.10f * team);
                t.color = Color.Black;
                t.font = Font;
                Teams.Add(t);

                Image i = Canvas.Add(ColorSliderTexture);
                i.position = new Vector2(0.05f + 0.65f * team, 0.7f);
                i.interactable = true;
                ColorSliders.Add(i);
            }
        }

        if (SliderKnobTexture != null)
        {
            for (int team = 0; team < NrTeams; team++)
            {
                Image i = Canvas.Add(SliderKnobTexture);
                i.position = new Vector2(0.05f + 0.65f * team, 0.7f);
                i.origin = new Vector2(0.5f);
                i.interactable = true;
                SliderKnob.Add(i);
            }
        }
    }

    public override void Update()
    {
        if (Canvas.isRendering)
        {
            if (Input.GetMouseButtonDown(Input.MouseButtons.LEFT) || btnDown)
            {
                btnDown = true;
                for (int i = 0; i < ColorSliders.Count; i++)
                {
                    if(ColorSliders[i].Hovered())
                        SliderKnob[i].position = new Vector2(Input.GetMouseX() / Canvas.camera.viewport.size.x, SliderKnob[i].position.y);
                }
            }

            if(Input.GetMouseButtonUp(Input.MouseButtons.LEFT))
                btnDown = false;

            //if (Input.GetMouseButtonUp(Input.MouseButtons.LEFT))
            //{
            //for (int team = 0; team < NrTeams; team++)
            //{
            //    for (int color = 0; color < Colors.Count; color++)
            //    {
            //        if ((TeamColors[team][color].Hovered() || color == SelectedColors[team]) && color != SelectedColors[(team+1) % 2])
            //        {
            //            SelectedColors[team] = color;
            //            SelectTiles[team].position = TilePosition(team, color);
            //        }
            //    }
            //}
            //}
        }
    }

    private Vector2 TilePosition(int team, int color)
    {
        return new Vector2(0.1f + 0.1f * (color + 1), 0.25f + 0.1f * team);
    }

    private Vector3 HSLColor(double hue)
    {
        double saturation = 1.0d;
        double luminosity = 0.5d;

        byte r, g, b;

        double t1, t2;
        double th = hue / 6.0d;

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
        return new Vector3(r, g, b);
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
