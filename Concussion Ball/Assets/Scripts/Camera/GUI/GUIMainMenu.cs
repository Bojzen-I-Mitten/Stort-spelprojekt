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
    Text Caret;

    IEnumerator Blink = null;

    private bool TakeName;
    public static string PlayerString = "CHAD";

    public float CaretOffset { get; set; } = 0.19f;

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

        if (PlayerString == "")
        {
            TextBoxName.color = Color.Red;
        }
        else
        {
            TextBoxName.color = Color.Black;
        }

        if (TextBoxName.Clicked())
        {
            TakeName = true;
            TextBoxName.color = Selected;
            if (Blink == null)
            {
                Blink = CaretBlink();
                StartCoroutine(Blink);
            }
            if (ClearName)
            {
                PlayerString = "";
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

        if (Play.Clicked() && PlayerString != "")
        {
            CameraMaster.instance.State = CAM_STATE.JOIN_HOST;
            TakeName = false;
        }

        PlayerString = PlayerString.ToUpper();
        PlayerName.text = PlayerString;

        if (TakeName)
        {
            GUIInput.AppendString(ref PlayerString, 9);
        }

        Caret.position = PlayerName.position + new Vector2(PlayerName.size.x / 2 - 0.005f, CaretOffset);
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
        PlayerName = Canvas.Add(PlayerString);
        PlayerName.origin = new Vector2(0.5f);
        PlayerName.position = new Vector2(0.5f, 0.94f);
        PlayerName.scale = new Vector2(0.9f);
        PlayerName.interactable = true;
        PlayerName.depth = 0.8f;
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

        if (TextBox != null)
        {
            TextBoxName = Canvas.Add(TextBox);
            TextBoxName.origin = new Vector2(0.5f);
            TextBoxName.position = new Vector2(0.5f, 0.94f);
            TextBoxName.interactable = true;
            TextBoxName.depth = 0.8f;
            TextBoxName.color = Color.Black;
        }

        if (TextBoxBG != null)
        {
            TextBoxBGName = Canvas.Add(TextBoxBG);
            TextBoxBGName.origin = new Vector2(0.5f);
            TextBoxBGName.position = new Vector2(0.5f, 0.94f);
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
        Canvas.Remove(Caret);
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