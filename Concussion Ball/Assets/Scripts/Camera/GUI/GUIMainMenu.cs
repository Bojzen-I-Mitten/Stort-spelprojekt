using System;

using System.Collections;
using ThomasEngine;

public class GUIMainMenu : ScriptComponent
{
    public Texture2D TextBox { get; set; }
    public Texture2D TextBoxBG { get; set; }
    public Font TextFont { get; set; }

    Camera Camera;

    public Canvas Canvas;

    Image TextBoxName;
    Image TextBoxBGName;
    Text Play;
    Text Options;
    Text Exit;
    Text Credits;
    Text PlayerName;

    private bool TakeName;
    public static string PlayerString = "CHAD";

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
        Options.color = Unselected;
        Credits.color = Unselected;
        Exit.color = Unselected;

        if (Play.Hovered())
            Play.color = Selected;
        else if (Options.Hovered())
            Options.color = Selected;
        else if (Credits.Hovered())
            Credits.color = Selected;
        else if (Exit.Hovered())
            Exit.color = Selected;
        if (Input.GetMouseButtonUp(Input.MouseButtons.LEFT))
        {
            TextBoxName.color = Color.Black;
            TakeName = false;
            if (TextBoxName.Clicked())
            {
                TakeName = true;
                TextBoxName.color = Selected;
            }
        }

        if (Play.Clicked())
        {
            TakeName = false;
            CameraMaster.instance.State = CAM_STATE.JOIN_HOST;
        }

        PlayerString = PlayerString.ToUpper();
        PlayerName.text = PlayerString;

        if (TakeName)
        {
            GUIInput.AppendString(ref PlayerString, 14);
        }
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
        Play.text = "Play";
        #endregion

        #region  Options
        Options = Canvas.Add("Options");
        Options.position = new Vector2(0.1f, 0.21f);
        Options.interactable = true;
        Options.depth = 0.9f;
        Options.text = "Options";
        #endregion

        #region Credits
        Credits = Canvas.Add("Credits");
        Credits.position = new Vector2(0.1f, 0.31f);
        Credits.interactable = true;
        Credits.depth = 0.9f;
        Credits.text = "Credits";
        #endregion

        #region Exit
        Exit = Canvas.Add("Exit");
        Exit.position = new Vector2(0.1f, 0.41f);
        Exit.interactable = true;
        Exit.depth = 0.9f;
        Exit.text = "Exit";
        #endregion

        #region Player name
        PlayerName = Canvas.Add("PlayerName");
        PlayerName.position = new Vector2(0.423f, 0.918f);
        PlayerName.scale = new Vector2(0.9f);
        PlayerName.interactable = true;
        PlayerName.depth = 0.8f;
        PlayerName.text = PlayerString;
        PlayerName.color = Color.Black;
        PlayerName.font = TextFont;
        #endregion
        #endregion

        #region Images

        if (TextBox != null)
        {
            TextBoxName = Canvas.Add(TextBox);
            TextBoxName.origin = new Vector2(0.5f);
            TextBoxName.position = new Vector2(0.485f, 0.94f);
            TextBoxName.interactable = true;
            TextBoxName.depth = 0.8f;
            TextBoxName.color = Color.Black;
        }

        if (TextBoxBG != null)
        {
            TextBoxBGName = Canvas.Add(TextBoxBG);
            TextBoxBGName.origin = new Vector2(0.5f);
            TextBoxBGName.position = new Vector2(0.485f, 0.94f);
            TextBoxBGName.depth = 0.9f;
            TextBoxBGName.color = Unselected;
        }

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
        Canvas.Remove(Options);
        Canvas.Remove(Credits);
        Canvas.Remove(Exit);
        Canvas.Remove(PlayerName);
        Canvas.Remove(TextBoxBGName);
        Canvas.Remove(TextBoxName);
    }
}