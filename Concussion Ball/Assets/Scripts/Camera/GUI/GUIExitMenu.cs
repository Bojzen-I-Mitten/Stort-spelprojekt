using ThomasEngine;
using System.Collections;

public class GUIExitMenu : ScriptComponent
{
    public Font SportNumbers32 { get; set; }

    Camera Camera;
    public Canvas Canvas;
    //Text ExitMatch;
    Text ExitGame;
    Text SwitchTeam;

    //private readonly string _exitMatch = "Exit Match";
    private readonly string _exitGame = "Exit Game";

    public override void OnAwake()
    {
        Camera = gameObject.GetComponent<Camera>();
        Canvas = Camera.AddCanvas();
        AddImagesAndText();
        
    }

    public override void Start()
    {
    }

    public override void Update()
    {
        ExitGame.color = Color.Black;
        SwitchTeam.color = Color.Black;

        if (ExitGame.Hovered())
            ExitGame.color = Color.IndianRed;
        else if (SwitchTeam.Hovered())
            SwitchTeam.color = Color.IndianRed;

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
        else if (SwitchTeam.Clicked())
        {
            CameraMaster.instance.State = CAM_STATE.SELECT_TEAM;
        }
    }

    private void AddImagesAndText()
    {
        #region Text

        #region Exit Game
        ExitGame = Canvas.Add(_exitGame);
        ExitGame.position = new Vector2(0.4f, 0.25f);
        ExitGame.color = Color.Black;
        ExitGame.font = SportNumbers32;
        ExitGame.interactable = true;
        ExitGame.outline = true;
        ExitGame.outlineColor = Color.White;
        #endregion

        #region Switch Team
        SwitchTeam = Canvas.Add("Switch Team");
        SwitchTeam.position = new Vector2(0.4f, 0.30f);
        SwitchTeam.color = Color.Black;
        SwitchTeam.font = SportNumbers32;
        SwitchTeam.interactable = true;
        SwitchTeam.outline = true;
        SwitchTeam.outlineColor = Color.White;
        #endregion

        #endregion
    }
}
