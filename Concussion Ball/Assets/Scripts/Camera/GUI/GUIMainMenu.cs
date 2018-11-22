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

    public override void OnAwake()
    {
        
    }

    public override void Start()
    {
        Camera = gameObject.GetComponent<Camera>();
        TakeName = false;
        AddImagesAndText();
    }

    public override void Update()
    {
        if(TakeName)
        {
            GUIInput.AppendString(ref PlayerString, 8);
        }

        if (!Play.Hovered())
        {
            Play.color = Color.FloralWhite;
            //Play.scale = new Vector2(2.5f);
        }
        else
        {
            Play.color = Color.IndianRed;
            //Play.scale = new Vector2(3f);
        }

        if (!Options.Hovered())
        {
            Options.color = Color.FloralWhite;
            //Options.scale = new Vector2(2.5f);
        }
        else
        {
            Options.color = Color.IndianRed;
            //Options.scale = new Vector2(3f);
        }

        if (!Credits.Hovered())
        {
            Credits.color = Color.FloralWhite;
            //Credits.scale = new Vector2(2.5f);
        }
        else
        {
            Credits.color = Color.IndianRed;
            //Credits.scale = new Vector2(3f);
        }

        if (!Exit.Hovered())
        {
            Exit.color = Color.FloralWhite;
            //Exit.scale = new Vector2(2.5f);
        }
        else
        {
            Exit.color = Color.IndianRed;
            //Exit.scale = new Vector2(3.0f);
        }

        if (Input.GetMouseButtonUp(Input.MouseButtons.LEFT))
        {
            if (TextBoxName.Clicked())
            {
                TakeName = true;
                TextBoxName.color = Color.Green;
            }

            if (Play.Clicked())
            {
                CameraMaster.instance.State = CAM_STATE.JOIN_HOST;
            }
        }

        PlayerString = PlayerString.ToUpper();
        PlayerName.text = PlayerString;
    }

    public void AddImagesAndText()
    {
        Canvas = Camera.AddCanvas();

        // *** Text ***
        // Play
        Play = Canvas.Add("Play");
        Play.position = new Vector2(0.425f, 0.11f);
        Play.scale = new Vector2(1.5f);
        Play.interactable = true;
        Play.depth = 0.9f;
        Play.text = "Play";
        Play.font = TextFont;

        //  Options
        Options = Canvas.Add("Options");
        Options.position = new Vector2(0.425f, 0.21f);
        Options.scale = new Vector2(1.5f);
        Options.interactable = true;
        Options.depth = 0.9f;
        Options.text = "Options";
        Options.font = TextFont;

        // Credits
        Credits = Canvas.Add("Credits");
        Credits.position = new Vector2(0.425f, 0.31f);
        Credits.scale = new Vector2(1.5f);
        Credits.interactable = true;
        Credits.depth = 0.9f;
        Credits.text = "Credits";
        Credits.font = TextFont;

        // Exit
        Exit = Canvas.Add("Exit");
        Exit.position = new Vector2(0.425f, 0.41f);
        Exit.scale = new Vector2(1.5f);
        Exit.interactable = true;
        Exit.depth = 0.9f;
        Exit.text = "Exit";
        Exit.font = TextFont;

        // Player name
        PlayerName = Canvas.Add("PlayerName");
        PlayerName.position = new Vector2(0.423f, 0.918f);
        PlayerName.scale = new Vector2(0.9f);
        PlayerName.interactable = true;
        PlayerName.depth = 0.8f;
        PlayerName.text = PlayerString;
        PlayerName.color = Color.Black;
        PlayerName.font = TextFont;

        // *** Images ***
        if (TextBoxBG != null)
        {
            TextBoxBGName = Canvas.Add(TextBoxBG);
            TextBoxBGName.origin = new Vector2(0.5f);
            TextBoxBGName.position = new Vector2(0.485f, 0.94f);
            TextBoxBGName.scale = new Vector2(0.65f, 0.65f);
            TextBoxBGName.depth = 0.9f;
            TextBoxBGName.color = Color.FloralWhite;
        }

        if (TextBox != null)
        {
            TextBoxName = Canvas.Add(TextBox);
            TextBoxName.origin = new Vector2(0.5f);
            TextBoxName.position = new Vector2(0.485f, 0.94f);
            TextBoxName.scale = new Vector2(0.65f, 0.65f);
            TextBoxName.interactable = true;
            TextBoxName.depth = 0.9f;
            TextBoxName.color = Color.Black;
        }
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