﻿using System;
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
        cam.AddText("MatchTime", "00:00", new Vector2(800/2.0f, 0), new Vector2(1, 1), Numbers, Color.DarkGray.ToVector4());
        cam.AddText("AnnouncementText", "", new Vector2(800 / 2.0f, 600 / 2.0f), new Vector2(2.0f), AnnouncementFont, Color.Green.ToVector4());
        cam.SetTextFont("AnnouncementText", AnnouncementFont);
        cam.AddText("AnnouncementText2", "", new Vector2(800 / 2.0f, 600 / 2.0f), new Vector2(2.0f), AnnouncementFont, Color.Green.ToVector4());
        cam.SetTextOrigin("MatchTime", new Vector2(0.5f));
        cam.SetTextOrigin("AnnouncementText", new Vector2(0.5f));
        cam.SetTextOrigin("AnnouncementText2", new Vector2(0.5f));

        cam.AddImage("AnnouncementBg", AnnouncementBackground, new Vector2(800 / 2.0f, 500 / 2.0f), Vector2.Zero, false);
        cam.SetImageOrigin("AnnouncementBg", new Vector2(0.5f));

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
        c = Color.Lerp(c, Color.Magenta, (time - timePerPart * 0)/timePerPart);
        c = Color.Lerp(c, Color.Blue, (time - timePerPart * 1) / timePerPart);
        c = Color.Lerp(c, Color.Cyan, (time - timePerPart * 2) / timePerPart);
        c = Color.Lerp(c, Color.Green, (time - timePerPart * 3) / timePerPart);
        c = Color.Lerp(c, Color.Yellow, (time - timePerPart * 4) / timePerPart);
        c = Color.Lerp(c, Color.Red, (time - timePerPart * 5) / timePerPart);

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

        Vector2 text1Size = cam.GetTextSize("AnnouncementText");
        Vector2 text2Size = cam.GetTextSize("AnnouncementText2");
        while (time < duration)
        {
            time += Time.ActualDeltaTime;
            xPos = (float)AnnouncementHorizontalSpeed.GetYFromX((double)time / duration);

            Vector2 text1Pos = new Vector2(0, 500 / 2.0f);
            text1Pos.x = MathHelper.Lerp(-text1Size.x / 2.0f, 800 / 2.0f, xPos);

            Vector2 text2Pos = new Vector2(0, 600 / 2.0f);
            text2Pos.x = MathHelper.Lerp(800 + text1Size.x / 2.0f, 800 / 2.0f, xPos);

            cam.SetTextPosition("AnnouncementText", text1Pos);
            cam.SetTextPosition("AnnouncementText2", text2Pos);

            Color c = GetRainbowColor(time, 0.5f);
            Color c2 = GetRainbowColor(time+1.0f, 0.5f);
            cam.SetTextColor("AnnouncementText2", c.ToVector4());

            if (showBG)
            {
                cam.SetImageScale("AnnouncementBg", new Vector2(1920, 1.0f + (float)Math.Sin((float)time * 0.5f) * 0.5f));
                cam.SetImageColor("AnnouncementBg", c2.ToVector4());
            }

            yield return null;
        }
        cam.SetText("AnnouncementText", "");
        cam.SetText("AnnouncementText2", "");
        cam.SetImageScale("AnnouncementBg", Vector2.Zero);
    }

    public void EnableGoldenGoal()
    {

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
    
    public override void Update()
    {
        int matchTimeLeft = MatchSystem.instance.MatchTimeLeft;
        int minutes = matchTimeLeft / 60;
        int seconds = matchTimeLeft % 60;
        cam.SetText("MatchTime", String.Format("{0}:{1}", minutes.ToString("00"), seconds.ToString("00")));
    }
}