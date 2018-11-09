﻿using System;
using System.Collections;
using System.Linq;
using ThomasEngine;
using ThomasEngine.Network;
public class ChadHud : ScriptComponent
{
    public static ChadHud Instance;
    private Ball _Ball;
    private Ball Ball
    {
        get
        {
            if (!_Ball)
                _Ball = GetObjectsOfType<Ball>().FirstOrDefault();
            return _Ball;
        }
    }
    Camera cam;
    #region GUI Stuff
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
    Image BallArrow;
    #endregion
    public Curve AnnouncementHorizontalSpeed { get; set; }
    public Font AnnouncementFont { get; set; }
    public Font Numbers { get; set; }
    public Texture2D AnnouncementBackground { get; set; }
    public Texture2D CrosshairTexture { get; set; }
    public Texture2D ChargeBarOutlineTexture { get; set; }
    public Texture2D ChargeBarTexture { get; set; }
    public Texture2D TimerBGTexture { get; set; }
    public Texture2D ScoreBGTexture { get; set; }
    public Texture2D BallArrowTexture { get; set; }
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

        #region Timer stuff
        Timer = HUD.Add("00:00");
        Timer.scale = new Vector2(2f);
        Timer.position = new Vector2(0.4975f, 0.01f);
        Timer.color = Color.Black;
        Timer.font = Numbers;
        Timer.origin = new Vector2(0.5f, 0);
        Timer.depth = 0.8f;

        TimerBG = HUD.Add(TimerBGTexture);
        TimerBG.position = new Vector2(0.5f, 0);
        TimerBG.origin = new Vector2(0.5f, 0);
        TimerBG.scale = new Vector2(0.6f, 0.7f);
        TimerBG.color = Color.GhostWhite;
        TimerBG.depth = 0.9f;
        
        //Left of the timer
        Score1BG = HUD.Add(ScoreBGTexture);
        Score1BG.origin = new Vector2(0.5f, 0);
        Score1BG.position = new Vector2(0.4175f, 0);
        Score1BG.color = MatchSystem.instance.Teams[TEAM_TYPE.TEAM_1].Color;
        Score1BG.depth = 1;
        Score1BG.scale = new Vector2(1, 0.7f);
        Score1BG.flip = new Vector2(0, 1);

        Score1 = HUD.Add("");
        Score1.scale = new Vector2(1.6f);
        Score1.origin = new Vector2(0.5f, 0);
        Score1.position = Score1BG.position + new Vector2(0.005f, 0.001f);
        Score1.color = Color.White;
        Score1.font = Numbers;
        Score1.depth = 0.8f;
        
        //Right of the timer
        Score2BG = HUD.Add(ScoreBGTexture);
        Score2BG.origin = new Vector2(0.5f, 0);
        Score2BG.position = new Vector2(0.5825f, 0f);
        Score2BG.color = MatchSystem.instance.Teams[TEAM_TYPE.TEAM_2].Color;
        Score2BG.depth = 1;
        Score2BG.scale = new Vector2(1, 0.7f);

        Score2 = HUD.Add("");
        Score2.scale = new Vector2(1.6f);
        Score2.origin = new Vector2(0.5f, 0);
        Score2.position = Score2BG.position - new Vector2(0.009f, -0.001f);
        Score2.color = Color.White;
        Score2.font = Numbers;
        Score2.depth = 0.8f;
        #endregion

        #region Announcement stuff
        Announcement1 = HUD.Add("");
        Announcement1.position = new Vector2(0.5f);
        Announcement1.scale = new Vector2(2);
        Announcement1.font = AnnouncementFont;
        Announcement1.color = Color.Green;
        Announcement1.origin = new Vector2(0.5f);

        Announcement2 = HUD.Add("");
        Announcement2.position = new Vector2(0.5f);
        Announcement2.scale = new Vector2(2);
        Announcement2.font = AnnouncementFont;
        Announcement2.color = Color.Green;
        Announcement2.origin = new Vector2(0.5f);

        AnnouncementBG = HUD.Add(AnnouncementBackground);
        AnnouncementBG.position = new Vector2(0.5f);
        AnnouncementBG.scale = Vector2.Zero;
        AnnouncementBG.origin = new Vector2(0.5f);
        AnnouncementBG.depth = 1;
        #endregion

        #region Aiming Stuff
        Crosshair = HUD.Add(CrosshairTexture);
        Crosshair.origin = new Vector2(0.5f);
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
        #endregion

        BallArrow = HUD.Add(BallArrowTexture);
        BallArrow.origin = new Vector2(0.5f);
        BallArrow.scale = new Vector2(2);
        BallArrow.position = new Vector2(-1000);
        BallArrow.color = Score1BG.color + Score2BG.color;
        //BallArrow.depth = 0.1f;
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
        //Announcement1.color = color;
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
            Color c2 = GetRainbowColor(time, 0.5f);
            Announcement1.color = c2;
            Announcement2.color = c;

            if (showBG)
            {
                AnnouncementBG.scale = new Vector2(10000, 1.0f + (float)Math.Sin(time * 0.5f));
                AnnouncementBG.color = c2;
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
        Timer.color = Color.Black;
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
            Crosshair.color = Color.Red;
        }
    }

    public void DeactivateCrosshair()
    {
        Crosshair.scale = Vector2.Zero;
    }

    public void ActivateChargeBar()
    {
        ChargeBarOutline.scale = new Vector2(2.0f, 9.0f);
        ChargeBar.color = Color.Red;
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
        ChargeBar.color = c;
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
            Timer.color = Color.Gold;
        }
        else
        {
            Timer.text = string.Format("{0}:{1}", minutes.ToString("00"), seconds.ToString("00"));
        }

        Score1.text = string.Format("{0}", MatchSystem.instance.Teams[TEAM_TYPE.TEAM_1].Score);
        Score2.text = string.Format("{0}", MatchSystem.instance.Teams[TEAM_TYPE.TEAM_2].Score);

        BallIndicator();
    }

    private void BallIndicator()
    {
        Vector3 screenPos = cam.WorldToViewport(Vector3.Zero, Ball.transform.world);
        if (!(screenPos.z > 0 && screenPos.z < 1 &&
            screenPos.x > 0 && screenPos.x < cam.viewport.size.x &&
            screenPos.y > 0 && screenPos.y < cam.viewport.size.y)) //Offscreen check
        {
            //Adjust for center of screen
            Vector3 screenCenter = new Vector3(cam.viewport.size, 0) / 2;
            screenPos -= screenCenter;

            //flip coordinates if more than 90 deg away
            if (screenPos.z > 1)
                screenPos *= -1;

            float angle = (float)Math.Atan2(-screenPos.y, screenPos.x);
            angle -= MathHelper.PiOver2;

            float cos = (float)Math.Cos(angle);
            float sin = -(float)Math.Sin(angle);

            //screenPos = screenCenter + new Vector3(sin * )

            float m = screenPos.y / screenPos.x;

            Vector3 screenBounds = screenCenter; //473.5, 253.5, 0

            if (screenPos.y > 0)
                screenPos = new Vector3(screenBounds.y / m, screenBounds.y, 0);
            else
                screenPos = new Vector3(-screenBounds.y / m, -screenBounds.y, 0);

            if (screenPos.x > screenBounds.x)
                screenPos = new Vector3(screenBounds.x, screenBounds.x * m, 0);
            else if (screenPos.x < -screenBounds.x)
                screenPos = new Vector3(-screenBounds.x, -screenBounds.x * m, 0);

            //Re-adjust for screen center
            screenPos += screenCenter;
            //Adjust for texture size
            Vector2 size = new Vector2(BallArrowTexture.width, BallArrowTexture.height) * BallArrow.scale / 2;
            screenPos -= new Vector3(size, 0);
            //Make sure the arrow is drawn on-screen
            screenPos = Vector3.Clamp(screenPos, Vector3.Zero + new Vector3(size, 0), new Vector3(cam.viewport.size - size, 0));

            screenPos /= new Vector3(cam.viewport.size, 1);

            BallArrow.position = new Vector2(screenPos.x, screenPos.y);
            BallArrow.rotation = -angle;
        }
        else
        {
            BallArrow.position = new Vector2(-1000);
        }
    }
}