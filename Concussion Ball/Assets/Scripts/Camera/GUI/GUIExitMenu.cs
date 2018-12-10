using ThomasEngine;
using System.Collections;

public class GUIExitMenu : ScriptComponent
{
    public Font Font { get; set; }

    Camera Camera;
    public Canvas Canvas;
    //Text ExitMatch;
    Text ExitGame;
    Text OptionsMenu;
    //private readonly string _exitMatch = "Exit Match";
    private readonly string _exitGame = "Exit Game";

    public override void OnAwake()
    {
        Camera = gameObject.GetComponent<Camera>();
        Canvas = Camera.AddCanvas();

        ExitGame = Canvas.Add(_exitGame);
        ExitGame.font = Font;
        ExitGame.position = new Vector2(0.4f, 0.25f);
        ExitGame.color = Color.Black;
        ExitGame.interactable = true;

        OptionsMenu = Canvas.Add("Options Menu");
        OptionsMenu.font = Font;
        OptionsMenu.position = new Vector2(0.4f, 0.4f);
        OptionsMenu.color = Color.Black;
        OptionsMenu.interactable = true;

    }

    public override void Start()
    {
    }

    public override void Update()
    {
        if (ExitGame.Hovered())
            ExitGame.color = Color.Red;
        else
            ExitGame.color = Color.Black;

        if (Input.GetMouseButtonUp(Input.MouseButtons.LEFT))
        {
            if (ExitGame.Clicked())
            {
                Debug.Log("I'm exciting! :^)");
                if (ThomasWrapper.IsPlaying())
                {
                    //Input.SetMouseMode(Input.MouseMode.POSITION_ABSOLUTE);
                    CameraMaster.instance.State = CAM_STATE.LOADING_SCREEN;
                    ThomasWrapper.IssueRestart();
                    //if (ThomasWrapper.IsPlaying())
                    //{
                    //    Debug.Log("Finished reloading to main menu.");
                    //    CameraMaster.instance.State = CAM_STATE.MAIN_MENU;
                    //}
                }       
            }
        }

        if (OptionsMenu.Hovered())
            OptionsMenu.color = Color.Red;
        else
            OptionsMenu.color = Color.Black;


        if (OptionsMenu.Clicked())
        {
            GUIOptionsMenu.instance.ActivatedfromExitmenu = true;
            CameraMaster.instance.State = CAM_STATE.OPTIONS_MENU;
        }

    }
}
