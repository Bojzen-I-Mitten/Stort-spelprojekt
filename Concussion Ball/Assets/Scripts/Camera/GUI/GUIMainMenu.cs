using System;

using System.Collections;
using ThomasEngine;

public class GUIMainMenu : ScriptComponent
{
    public Font TextFont { get; set; }

    Camera Camera;

    public Canvas Canvas;

    Text Play;
    Text Options;
    Text Exit;
    Text Credits;

    public override void Awake()
    {
    }

    public override void Start()
    {
        Camera = gameObject.GetComponent<Camera>();
        AddImagesAndText();
    }

    public override void Update()
    {
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
            if (Play.Clicked())
            {
                CameraMaster.instance.State = CAM_STATE.JOIN_HOST;
            }
        }
    }

    public void AddImagesAndText()
    {

        Canvas = Camera.AddCanvas();

        Play = Canvas.Add("Play");
        Play.position = new Vector2(0.425f, 0.11f);
        Play.scale = new Vector2(2.5f);
        Play.interactable = true;
        Play.depth = 0.9f;
        Play.text = "Play";
        Play.font = TextFont;

        Options = Canvas.Add("Options");
        Options.position = new Vector2(0.425f, 0.21f);
        Options.scale = new Vector2(2.5f);
        Options.interactable = true;
        Options.depth = 0.9f;
        Options.text = "Options";
        Options.font = TextFont;

        Credits = Canvas.Add("Credits");
        Credits.position = new Vector2(0.425f, 0.31f);
        Credits.scale = new Vector2(2.5f);
        Credits.interactable = true;
        Credits.depth = 0.9f;
        Credits.text = "Credits";
        Credits.font = TextFont;

        Exit = Canvas.Add("Exit");
        Exit.position = new Vector2(0.425f, 0.41f);
        Exit.scale = new Vector2(2.5f);
        Exit.interactable = true;
        Exit.depth = 0.9f;
        Exit.text = "Exit";
        Exit.font = TextFont;
    }

    public void ClearImagesAndText()
    {
        Canvas.Remove(Play);
    }

}