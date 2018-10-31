using System;
using System.Collections;
using ThomasEngine;
using ThomasEngine.Network;
public class ChadHud : ScriptComponent
{
    public static ChadHud Instance;
    Camera cam;
    Canvas HUD;
    Text Timer;
    Text Announcement1;
    Text Announcement2;
    Text Score1;
    Text Score2;
    Image AnnouncementBG;
    Image Crosshair;
    Image ChargeBarOutline;
    Image ChargeBar;
    Image TimerBG;
    Image Score1BG;
    Image Score2BG;
    public Curve AnnouncementHorizontalSpeed { get; set; }
    public Font AnnouncementFont { get; set; }
    public Font Numbers { get; set; }
    public Texture2D AnnouncementBackground { get; set; }
    public Texture2D CrosshairTexture { get; set; }
    public Texture2D ChargeBarOutlineTexture { get; set; }
    public Texture2D ChargeBarTexture { get; set; }
    public Texture2D TimerBGTexture { get; set; }
    public Texture2D ScoreBGTexture { get; set; }
    public bool ToggleAim { get; set; } = true;

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
        HUD = cam.AddCanvas();

        Timer = HUD.Add("00:00");
        Timer.scale = new Vector2(2f);
        Timer.position = new Vector2(0.4975f, 0.01f);
        Timer.color = Color.Black.ToVector4();
        Timer.font = Numbers;
        Timer.origin = new Vector2(0.5f, 0);

        TimerBG = HUD.Add(TimerBGTexture);
        TimerBG.position = new Vector2(0.5f, 0);
        TimerBG.origin = new Vector2(0.5f, 0);
        TimerBG.scale = new Vector2(0.6f, 0.7f);
        TimerBG.color = Color.White.ToVector4() - new Vector4(0, 0, 0, 0.25f);
        TimerBG.depth = 0.9f;
        
        //Left of the timer
        Score1BG = HUD.Add(ScoreBGTexture);
        Score1BG.origin = new Vector2(0.5f, 0);
        Score1BG.position = new Vector2(0.4175f, 0);
        Score1BG.color = MatchSystem.instance.Teams[TEAM_TYPE.TEAM_1].Color.ToVector4() - new Vector4(0, 0, 0, 0.25f);
        Score1BG.depth = 1;
        Score1BG.scale = new Vector2(1, 0.7f);
        Score1BG.flip = new Vector2(0, 1);

        Score1 = HUD.Add("");
        Score1.scale = new Vector2(1.6f);
        Score1.origin = new Vector2(0.5f, 0);
        Score1.position = Score1BG.position + new Vector2(0.005f, 0.001f);
        Score1.color = Color.White.ToVector4();
        Score1.font = Numbers;
        
        //Right of the timer
        Score2BG = HUD.Add(ScoreBGTexture);
        Score2BG.origin = new Vector2(0.5f, 0);
        Score2BG.position = new Vector2(0.5825f, 0f);
        Score2BG.color = MatchSystem.instance.Teams[TEAM_TYPE.TEAM_2].Color.ToVector4() - new Vector4(0, 0, 0, 0.25f);
        Score2BG.depth = 1;
        Score2BG.scale = new Vector2(1, 0.7f);

        Score2 = HUD.Add("");
        Score2.scale = new Vector2(1.6f);
        Score2.origin = new Vector2(0.5f, 0);
        Score2.position = Score2BG.position - new Vector2(0.009f, -0.001f);
        Score2.color = Color.White.ToVector4();
        Score2.font = Numbers;

        Announcement1 = HUD.Add("");
        Announcement1.position = new Vector2(0.5f);
        Announcement1.scale = new Vector2(2);
        Announcement1.font = AnnouncementFont;
        Announcement1.color = Color.Green.ToVector4();
        Announcement1.origin = new Vector2(0.5f);

        Announcement2 = HUD.Add("");
        Announcement2.position = new Vector2(0.5f);
        Announcement2.scale = new Vector2(2);
        Announcement2.font = AnnouncementFont;
        Announcement2.color = Color.Green.ToVector4();
        Announcement2.origin = new Vector2(0.5f);

        AnnouncementBG = HUD.Add(AnnouncementBackground);
        AnnouncementBG.position = new Vector2(0.5f);
        AnnouncementBG.scale = Vector2.Zero;
        AnnouncementBG.origin = new Vector2(0.5f);
        AnnouncementBG.depth = 1;

        Crosshair = HUD.Add(CrosshairTexture);
        Crosshair.position = new Vector2(0.5f);
        Crosshair.scale = Vector2.Zero;

        ChargeBarOutline = HUD.Add(ChargeBarOutlineTexture);
        ChargeBarOutline.position = new Vector2(0.9f, 0.1f);
        ChargeBarOutline.scale = Vector2.Zero;

        ChargeBar = HUD.Add(ChargeBarTexture);
        ChargeBar.position = new Vector2(0.9f, 0.1f + ((ChargeBarTexture.height * 9.0f) / 1080.0f)); //Need to move the bar its own height down one step.
        ChargeBar.scale = Vector2.Zero;
        ChargeBar.origin = new Vector2(1, 0);
        ChargeBar.rotation = MathHelper.Pi; //Need to rotate the bar 180, because positive x is down on the screen.
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
        Announcement1.text = text;
        Announcement1.color = color.ToVector4();
        Announcement2.text = text2;

        Vector2 text1Size = Announcement1.size / (HUD.viewport.size * cam.viewport.size);
        Vector2 text2Size = Announcement2.size / (HUD.viewport.size * cam.viewport.size);
        while (time < duration)
        {
            time += Time.ActualDeltaTime;
            xPos = (float)AnnouncementHorizontalSpeed.GetYFromX((double)time / duration);

            Vector2 text1Pos = new Vector2(0, 0.46f);
            text1Pos.x = MathHelper.Lerp(-text1Size.x, 0.55f, xPos);

            Vector2 text2Pos = new Vector2(0, 0.55f);
            text2Pos.x = MathHelper.Lerp(1 + text1Size.x, 0.45f, xPos);

            Announcement1.position = text1Pos;
            Announcement2.position = text2Pos;

            Color c = GetRainbowColor(time, 0.5f);
            Color c2 = GetRainbowColor(time + 1.0f, 0.5f);
            Announcement2.color = c.ToVector4();

            if (showBG)
            {
                AnnouncementBG.scale = new Vector2(10000, 1.0f + (float)Math.Sin(time * 0.5f));
                AnnouncementBG.color = c2.ToVector4();
            }
            if(MatchSystem.instance.MatchTimeLeft > 0.0f)
                MatchSystem.instance.MatchStartTime += Time.ActualDeltaTime;
            yield return null;
        }
        Announcement1.text = "";
        Announcement2.text = "";
        AnnouncementBG.scale = Vector2.Zero;
    }


    IEnumerator Countdown(float duration)
    {
        StartCoroutine(AnnouncementAnimation(duration, "Round starts in", "0", Color.Cyan, false));
        Announcement2.font = Numbers;
        float time = duration;
        while(time > 0.0f)
        {
            time -= Time.DeltaTime;
            int timeLeft = (int)time;
            Announcement2.text = string.Format("{0}", timeLeft + 1);
            yield return null;
        }
        Announcement2.font = AnnouncementFont;
    }

    public void StartCountdown(float duration)
    {
        Timer.color = Color.Black.ToVector4();
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
        if (ToggleAim)
        {
            Crosshair.scale = Vector2.One;
            Crosshair.color = Color.Red.ToVector4();
        }
    }

    public void DeactivateCrosshair()
    {
        Crosshair.scale = Vector2.Zero;
    }

    public void ActivateChargeBar()
    {
        ChargeBarOutline.scale = new Vector2(2.0f, 9.0f);
        ChargeBar.color = Color.Red.ToVector4();
    }

    public void DeactivateChargeBar()
    {
        ChargeBarOutline.scale = Vector2.Zero;
        ChargeBar.scale = Vector2.Zero;
    }

    //Charge: 0 to 1
    public void ChargeChargeBar(float charge)
    {
        Color c = Color.Blue;
        float timePerPart = 0.5f;
        c = Color.Lerp(c, Color.Blue, (charge - timePerPart * 0) / timePerPart);
        c = Color.Lerp(c, Color.Red, (charge - timePerPart * 1) / timePerPart);
        ChargeBar.color = c.ToVector4();
        ChargeBar.scale = new Vector2(2.0f, charge*9.0f);
    }

    public override void Update()
    {
        int matchTimeLeft = MatchSystem.instance.MatchTimeLeft;
        int minutes = matchTimeLeft / 60;
        int seconds = matchTimeLeft % 60;

        if (MatchSystem.instance.GoldenGoal)
        {
            Timer.text = "GOLDEN GOAL";
            Timer.color = Color.Gold.ToVector4();
        }
        else
        {
            Timer.text = string.Format("{0}:{1}", minutes.ToString("00"), seconds.ToString("00"));
        }

        Score1.text = string.Format("{0}", MatchSystem.instance.Teams[TEAM_TYPE.TEAM_1].Score);
        Score2.text = string.Format("{0}", MatchSystem.instance.Teams[TEAM_TYPE.TEAM_2].Score);
    }
}