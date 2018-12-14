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

    Text OptionsMenu;
    public bool _CanSwitchTeam = true;

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
        ExitGame.color = Color.White;
        SwitchTeam.color = Color.White;
        OptionsMenu.color = Color.FloralWhite;

        if (ExitGame.Hovered())
            ExitGame.color = Color.IndianRed;
        else if (SwitchTeam.Hovered() && _CanSwitchTeam)
            SwitchTeam.color = Color.IndianRed;
        else if (OptionsMenu.Hovered())
            OptionsMenu.color = Color.IndianRed;

        if (ExitGame.Clicked())
        {
            if (ThomasWrapper.IsPlaying())
            {
                CameraMaster.instance.SetState(CAM_STATE.LOADING_SCREEN);
                ThomasWrapper.IssueRestart();
            } 
        }
        else if (SwitchTeam.Clicked() && _CanSwitchTeam)
        {
            CameraMaster.instance.Canvas.isRendering = true;
            gameObject.GetComponent<ChadCam>().enabled = false;
            CameraMaster.instance.SetState(CAM_STATE.SELECT_TEAM);
        }



        if (OptionsMenu.Clicked())
        {
            GUIOptionsMenu.instance.ActivatedfromExitmenu = true;
            CameraMaster.instance.SetState(CAM_STATE.OPTIONS_MENU);
        }

    }

    private void AddImagesAndText()
    {
        #region Text

        #region Exit Game
        ExitGame = Canvas.Add(_exitGame);
        ExitGame.position = new Vector2(0.5f, 0.25f);
        ExitGame.origin = new Vector2(0.5f);
        ExitGame.color = Color.White;
        ExitGame.font = SportNumbers32;
        ExitGame.interactable = true;
        ExitGame.outline = true;
        ExitGame.outlineColor = Color.Black;
        #endregion

      OptionsMenu = Canvas.Add("Options Menu");
        OptionsMenu.position = new Vector2(0.5f, 0.35f);
        OptionsMenu.origin = new Vector2(0.5f);
        OptionsMenu.color = Color.White;
        OptionsMenu.font = SportNumbers32;
        OptionsMenu.interactable = true;
        OptionsMenu.outline = true;
        OptionsMenu.outlineColor = Color.Black;
    
        #region Switch Team
        SwitchTeam = Canvas.Add("Switch Team");
        SwitchTeam.position = new Vector2(0.5f, 0.30f);
        SwitchTeam.origin = new Vector2(0.5f);
        SwitchTeam.color = Color.White;
        SwitchTeam.font = SportNumbers32;
        SwitchTeam.interactable = true;
        SwitchTeam.outline = true;
        SwitchTeam.outlineColor = Color.Black;
        #endregion
        
        
        
        

        #endregion
    }
}
