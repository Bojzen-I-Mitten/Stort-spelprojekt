using System;
using System.Collections;
using ThomasEngine;
using ThomasEngine.Network;
public class ChadHud : ScriptComponent
{
    public static ChadHud Instance;
    Camera cam;

    public Curve AnnouncementHorizontalSpeed { get; set; }
    public Font AnnouncementFont { get; set; }
    public Font Numbers { get; set; }
    public Texture2D AnnouncementBackground { get; set; }
    public Texture2D CrosshairTexture { get; set; }
    public Texture2D ChargeBarOutlineTexture { get; set; }
    public Texture2D ChargeBarTexture { get; set; }

    private readonly string Crosshair = "Crosshair";
    private readonly string ChargeBarOutline = "ChargeBarOutline";
    private readonly string ChargeBar = "ChargeBar";

    public override void Awake()
    {
        if (!Instance)
            Instance = this;
        else
        {
            Destroy(this);
            return;
        }
        cam = gameObject.GetComponent<Camera>();
        cam.AddText("MatchTime", "00:00", new Vector2(0.5f, 0.02f), new Vector2(1, 1), Numbers, Color.WhiteSmoke.ToVector4());
        cam.AddText("AnnouncementText", "", new Vector2(0.5f, 0.5f), new Vector2(2.0f), AnnouncementFont, Color.Green.ToVector4());
        cam.SetTextFont("AnnouncementText", AnnouncementFont);
        cam.AddText("AnnouncementText2", "", new Vector2(0.5f, 0.5f), new Vector2(2.0f), AnnouncementFont, Color.Green.ToVector4());
        cam.SetTextOrigin("MatchTime", new Vector2(0.5f));
        cam.SetTextOrigin("AnnouncementText", new Vector2(0.5f));
        cam.SetTextOrigin("AnnouncementText2", new Vector2(0.5f));

        cam.AddImage("AnnouncementBg", AnnouncementBackground, new Vector2(0.5f, 0.5f), Vector2.Zero, false);
        cam.SetImageOrigin("AnnouncementBg", new Vector2(0.5f));

        cam.AddImage(Crosshair, CrosshairTexture, new Vector2(0.5f), Vector2.Zero, false);
        cam.AddImage(ChargeBarOutline, ChargeBarOutlineTexture, new Vector2(0.9f, 0.1f), Vector2.Zero, false);
        cam.AddImage(ChargeBar, ChargeBarTexture, new Vector2(0.9f, 0.1f + ((ChargeBarTexture.height*9.0f) / 1080.0f)), Vector2.Zero, false);  //Need to move the bar its own height down one step.
        //Need to rotate the bar 180, because positive x is down on the screen.
        cam.SetImageOrigin(ChargeBar, new Vector2(1, 0));
        cam.SetImageRotation(ChargeBar, 3.14f);

    }

    public override void OnDestroy()
    {
        if (Instance == this)
            Instance = null;
    }


    Color GetRainbowColor(float time, float timePerPart)
    {
        time = time % (timePerPart * 6);
        
        Color c = Color.Red;
        c = Color.Lerp(c, Color.Magenta,    (time - timePerPart * 0) / timePerPart);
        c = Color.Lerp(c, Color.Blue,       (time - timePerPart * 1) / timePerPart);
        c = Color.Lerp(c, Color.Cyan,       (time - timePerPart * 2) / timePerPart);
        c = Color.Lerp(c, Color.Green,      (time - timePerPart * 3) / timePerPart);
        c = Color.Lerp(c, Color.Yellow,     (time - timePerPart * 4) / timePerPart);
        c = Color.Lerp(c, Color.Red,        (time - timePerPart * 5) / timePerPart);

        return c;
    }

    IEnumerator AnnouncementAnimation(float duration, string text, string text2, Color color, bool showBG = true)
    {
        
        float time = 0.0f;
        float xPos = 0;
        //When xPos = 1 it will be at the center
        cam.SetText("AnnouncementText", text);
        cam.SetTextColor("AnnouncementText", color.ToVector4());
        cam.SetText("AnnouncementText2", text2);

        Vector2 text1Size = cam.GetTextSize("AnnouncementText") / cam.viewport;
        Vector2 text2Size = cam.GetTextSize("AnnouncementText2") / cam.viewport;
        while (time < duration)
        {
            time += Time.ActualDeltaTime;
            xPos = (float)AnnouncementHorizontalSpeed.GetYFromX((double)time / duration);

            Vector2 text1Pos = new Vector2(0, 0.46f);
            text1Pos.x = MathHelper.Lerp(-text1Size.x, 0.55f, xPos);

            Vector2 text2Pos = new Vector2(0, 0.55f);
            text2Pos.x = MathHelper.Lerp(1 + text1Size.x, 0.45f, xPos);

            cam.SetTextPosition("AnnouncementText", text1Pos);
            cam.SetTextPosition("AnnouncementText2", text2Pos);

            Color c = GetRainbowColor(time, 0.5f);
            Color c2 = GetRainbowColor(time + 1.0f, 0.5f);
            cam.SetTextColor("AnnouncementText2", c.ToVector4());

            if (showBG)
            {
                cam.SetImageScale("AnnouncementBg", new Vector2(10000, 1.0f + (float)Math.Sin((float)time) * 0.5f));
                cam.SetImageColor("AnnouncementBg", c2.ToVector4());
            }
            if(MatchSystem.instance.MatchTimeLeft > 0.0f)
                MatchSystem.instance.MatchStartTime += Time.ActualDeltaTime;
            yield return null;
        }
        cam.SetText("AnnouncementText", "");
        cam.SetText("AnnouncementText2", "");
        cam.SetImageScale("AnnouncementBg", Vector2.Zero);
    }


    IEnumerator Countdown(float duration)
    {
        StartCoroutine(AnnouncementAnimation(duration, "Round starts in", "0", Color.Cyan, false));
        cam.SetTextFont("AnnouncementText2", Numbers);
        float time = duration;
        while(time > 0.0f)
        {
            time -= Time.DeltaTime;
            int timeLeft = (int)time;
            cam.SetText("AnnouncementText2", String.Format("{0}",timeLeft+1));
            yield return null;
        }
        cam.SetTextFont("AnnouncementText2", AnnouncementFont);
    }

    public void StartCountdown(float duration)
    {
        cam.SetTextColor("MatchTime", Color.WhiteSmoke.ToVector4());
        StartCoroutine(Countdown(duration));
    }

    public void OnGoal(Team team, float duration)
    {
        StartCoroutine(AnnouncementAnimation(duration, team.Name, "Scored!!!", team.Color));
    }

    public void OnMatchEnd(Team winningTeam, float duration)
    {
        StartCoroutine(AnnouncementAnimation(duration, winningTeam.Name, "Wins!!!", winningTeam.Color));
    }
    
    public void ActivateCrosshair()
    {
        cam.SetImageScale(Crosshair, Vector2.One);
        cam.SetImageColor(Crosshair, new Vector4(1.0f, 0.0f, 0.0f, 1.0f));
    }

    public void DeactivateCrosshair()
    {
        cam.SetImageScale(Crosshair, Vector2.Zero);
    }

    public void ActivateChargeBar()
    {
        cam.SetImageScale(ChargeBarOutline, new Vector2(2.0f, 9.0f));
        cam.SetImageColor(ChargeBar, new Vector4(1.0f, 0.0f, 0.0f, 1.0f));
    }

    public void DeactivateChargeBar()
    {
        cam.SetImageScale(ChargeBarOutline, Vector2.Zero);
        cam.SetImageScale(ChargeBar, Vector2.Zero);
    }

    //Charge: 0 to 1
    public void ChargeChargeBar(float charge)
    {
        Color c = Color.Blue;
        float timePerPart = 0.5f;
        c = Color.Lerp(c, Color.Blue, (charge - timePerPart * 0) / timePerPart);
        c = Color.Lerp(c, Color.Red, (charge - timePerPart * 1) / timePerPart);
        cam.SetImageColor(ChargeBar, c.ToVector4());
        cam.SetImageScale(ChargeBar, new Vector2(2.0f, charge*9.0f));
    }

    public override void Update()
    {
        int matchTimeLeft = MatchSystem.instance.MatchTimeLeft;
        int minutes = matchTimeLeft / 60;
        int seconds = matchTimeLeft % 60;

        if (MatchSystem.instance.GoldenGoal)
        {
            cam.SetText("MatchTime", "GOLDEN GOAL}");
            cam.SetTextColor("MatchTime", Color.Gold.ToVector4());
        }else
        {
            cam.SetText("MatchTime", String.Format("{0}:{1}", minutes.ToString("00"), seconds.ToString("00")));
        }
            
        
    }
}