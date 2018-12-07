﻿using System;

using System.Collections;
using ThomasEngine;

public class GUIMainMenu : ScriptComponent
{
    public Texture2D TextBox { get; set; }
    public Texture2D TextBoxBG { get; set; }
    public Texture2D MyNameTexture { get; set; }
    public Texture2D ArrowTexture { get; set; }
    public Font TextFont { get; set; }

    Camera Camera;

    public Canvas Canvas;

    Image MyNameSticker;
    Image SelectHatRight;
    Image SelectHatLeft;
    Text Play;
    Text HostGame;
    Text Options;
    Text Exit;
    Text Credits;
    Text PlayerName;
    Text Caret;

    IEnumerator Blink = null;

    private bool TakeName;

    public float CaretOffset { get; set; } = -0.015f;

    private bool ClearName = true;

    Color Unselected = Color.FloralWhite;
    Color Selected = Color.IndianRed;
    Vector3 MainMenuCamPos;
    Vector3 MainMenuCamRot;

    public override void OnAwake()
    {

    }

    public override void Start()
    {

        

        Camera = gameObject.GetComponent<Camera>();
        TakeName = false;
        AddImagesAndText();
        MainMenuCamPos = new Vector3(0, -195.442f, -7.084f);
        MainMenuCamRot = Vector3.Zero;
    }

    public override void Update()
    {
        Play.color = Unselected;
        HostGame.color = Unselected;
        Options.color = Unselected;
        Credits.color = Unselected;
        Exit.color = Unselected;

        if (Play.Hovered())
            Play.color = Selected;
        else if (HostGame.Hovered())
            HostGame.color = Selected;
        else if (Options.Hovered())
            Options.color = Selected;
        else if (Credits.Hovered())
            Credits.color = Selected;
        else if (Exit.Hovered())
            Exit.color = Selected;

        if (MyNameSticker.Clicked())
        {
            TakeName = true;
            if (Blink == null)
            {
                Blink = CaretBlink();
                StartCoroutine(Blink);
            }
            if (ClearName)
            {
                PlayerName.text = "";
                ClearName = false;
            }
        }
        else if (Input.GetMouseButtonUp(Input.MouseButtons.LEFT))
        {
            TakeName = false;
            if (Blink != null)
            {
                StopCoroutine(Blink);
                Blink = null;
            }
            Caret.text = "";
        }

        if (Play.Clicked() && PlayerName.text != "")
        {
            CameraMaster.instance.State = CAM_STATE.JOIN_HOST;
            TakeName = false;
            UserSettings.AddOrUpdateAppSetting("Hat", CameraMaster.instance.SelectedHat.ToString());
            UserSettings.AddOrUpdateAppSetting("PlayerName", PlayerName.text);
        }
        else if (HostGame.Clicked())
        {
            CameraMaster.instance.State = CAM_STATE.HOST_MENU;
            TakeName = false;
            UserSettings.AddOrUpdateAppSetting("Hat", CameraMaster.instance.SelectedHat.ToString());
            UserSettings.AddOrUpdateAppSetting("PlayerName", PlayerName.text);
        }
        else if (Exit.Clicked())
        {
            ThomasWrapper.IssueShutdown();
        }
        else if (SelectHatRight.Clicked())
        {
            CameraMaster.instance.SelectedHat += 1;
        }
        else if (SelectHatLeft.Clicked())
        {
            CameraMaster.instance.SelectedHat -= 1;
        }
       
        if (TakeName)
        {
            string playerString = PlayerName.text;
            GUIInput.AppendString(ref playerString, 9);
            PlayerName.text = playerString;
        }
        Caret.position = PlayerName.position + new Vector2(PlayerName.size.x / 2 - 0.005f, CaretOffset);
    }


    public string GetPlayerName()
    {
        return PlayerName != null ? PlayerName.text : "Chad";
    }

    public void AddImagesAndText()
    {
        Canvas = Camera.AddCanvas();

        #region Text
        #region  Play
        Play = Canvas.Add("Play");
        Play.position = new Vector2(0.1f, 0.11f);
        Play.interactable = true;
        Play.depth = 0.9f;
        #endregion

        #region Host Game
        HostGame = Canvas.Add("Host Game");
        HostGame.position = new Vector2(0.1f, 0.21f);
        HostGame.interactable = true;
        HostGame.depth = 0.9f;
        #endregion

        #region  Options
        Options = Canvas.Add("Options");
        Options.position = new Vector2(0.1f, 0.31f);
        Options.interactable = true;
        Options.depth = 0.9f;
        #endregion

        #region Credits
        Credits = Canvas.Add("Credits");
        Credits.position = new Vector2(0.1f, 0.41f);
        Credits.interactable = true;
        Credits.depth = 0.9f;
        #endregion

        #region Exit
        Exit = Canvas.Add("Exit");
        Exit.position = new Vector2(0.1f, 0.51f);
        Exit.interactable = true;
        Exit.depth = 0.9f;
        #endregion

        #region Player name

        string playerString =  UserSettings.GetSetting("PlayerName");
        if (playerString == null)
            playerString = "CHAD";

        PlayerName = Canvas.Add(playerString);
        PlayerName.origin = new Vector2(0.5f);
        PlayerName.position = new Vector2(0.55f, 0.17f);
        PlayerName.scale = new Vector2(0.9f);
        PlayerName.interactable = true;
        PlayerName.depth = 0.8f;
        PlayerName.rotation = -0.1f;
        PlayerName.color = Color.Black;
        PlayerName.font = TextFont;
        #endregion

        #region Caret
        Caret = Canvas.Add("");
        Caret.origin = new Vector2(0, 0.5f);
        Caret.scale = new Vector2(1.2f);
        Caret.interactable = false;
        Caret.depth = 0.8f;
        Caret.color = Color.Black;
        Caret.font = TextFont;
        #endregion
        #endregion

        #region Images
        #region NameSticker
        if (MyNameTexture != null)
        {
            MyNameSticker = Canvas.Add(MyNameTexture);
            MyNameSticker.origin = new Vector2(0.5f);
            MyNameSticker.position = new Vector2(0.55f, 0.14f);
            MyNameSticker.scale = new Vector2(0.5f);
            MyNameSticker.interactable = true;
            MyNameSticker.depth = 0.9f;
        }
        #endregion

        #region Arrows
        if (ArrowTexture != null)
        {
            SelectHatRight = Canvas.Add(ArrowTexture);
            SelectHatRight.origin = new Vector2(0.5f);
            SelectHatRight.scale = new Vector2(0.25f);
            SelectHatRight.position = new Vector2(0.85f, 0.25f);
            SelectHatRight.interactable = true;
            SelectHatRight.depth = 0.9f;

            SelectHatLeft = Canvas.Add(ArrowTexture);
            SelectHatLeft.origin = new Vector2(0.5f);
            SelectHatLeft.scale = new Vector2(0.25f);
            SelectHatLeft.position = new Vector2(0.68f, 0.25f);
            SelectHatLeft.rotation = (float)Math.PI;
            SelectHatLeft.interactable = true;
            SelectHatLeft.depth = 0.9f;
        }
        #endregion
        #endregion
    }

    public void SetUpScene()
    {
        transform.position = MainMenuCamPos;
        transform.rotation = Quaternion.CreateFromYawPitchRoll(MainMenuCamRot.x, MainMenuCamRot.y, MainMenuCamRot.z);

    }

    public void ClearImagesAndText()
    {
        Canvas.Remove(Play);
        Canvas.Remove(HostGame);
        Canvas.Remove(Options);
        Canvas.Remove(Credits);
        Canvas.Remove(Exit);
        Canvas.Remove(PlayerName);
        Canvas.Remove(Caret);
        Canvas.Remove(SelectHatRight);
        Canvas.Remove(SelectHatLeft);
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