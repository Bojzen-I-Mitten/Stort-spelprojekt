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

        if (ExitGame.Hovered())
            ExitGame.color = Color.IndianRed;
        else if (SwitchTeam.Hovered())
            SwitchTeam.color = Color.IndianRed;

        if (ExitGame.Clicked())
        {
            if (ThomasWrapper.IsPlaying())
            {
                CameraMaster.instance.State = CAM_STATE.LOADING_SCREEN;
                ThomasWrapper.IssueRestart();
            } 
        }
        else if (SwitchTeam.Clicked())
        {
            CameraMaster.instance.Canvas.isRendering = true;
            gameObject.GetComponent<ChadCam>().enabled = false;
            CameraMaster.instance.State = CAM_STATE.SELECT_TEAM;
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
