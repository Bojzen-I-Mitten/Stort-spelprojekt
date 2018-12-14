using ThomasEngine;
using System.Collections;

public class GUIExitMenu : ScriptComponent
{
    public Font SportNumbers32 { get; set; }

    Camera Camera;
    public Canvas Canvas;
    Text MainMenu;
    Text SwitchTeam;
    Text OptionsMenu;
    Text BackToGame;
    Text ExitGame;

    public bool _CanSwitchTeam = true;
    Color Selected;
    Color Unselected;

    public override void OnAwake()
    {
        Camera = gameObject.GetComponent<Camera>();
        Canvas = Camera.AddCanvas();
        AddImagesAndText();
        Selected = Color.IndianRed;
        Unselected = Color.FloralWhite;
    }

    public override void Update()
    {
        MainMenu.color = Unselected;
        SwitchTeam.color = Unselected;
        OptionsMenu.color = Unselected;
        BackToGame.color = Unselected;
        ExitGame.color = Unselected;

        if (MainMenu.Hovered())
            MainMenu.color = Selected;
        else if (SwitchTeam.Hovered() && _CanSwitchTeam)
            SwitchTeam.color = Selected;
        else if (OptionsMenu.Hovered())
            OptionsMenu.color = Selected;
        else if (BackToGame.Hovered())
            BackToGame.color = Selected;
        else if (ExitGame.Hovered())
            ExitGame.color = Selected;

        if (MainMenu.Clicked())
        {
            if (ThomasWrapper.IsPlaying())
            {
                Input.SetMouseMode(Input.MouseMode.POSITION_ABSOLUTE);
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
        else if (OptionsMenu.Clicked())
        {
            GUIOptionsMenu.instance.ActivatedfromExitmenu = true;
            CameraMaster.instance.SetState(CAM_STATE.OPTIONS_MENU);
        }
        else if (BackToGame.Clicked())
        {
            CameraMaster.instance.SetState(CAM_STATE.GAME);
            Input.SetMouseMode(Input.MouseMode.POSITION_RELATIVE);
        }
        else if (ExitGame.Clicked())
        {
            ThomasWrapper.IssueShutdown();
        }
    }

    private void AddImagesAndText()
    {
        #region Text

        #region Main Menu
        MainMenu = Canvas.Add("Main Menu");
        MainMenu.position = new Vector2(0.5f, 0.25f);
        MainMenu.origin = new Vector2(0.5f);
        MainMenu.color = Unselected;
        MainMenu.font = SportNumbers32;
        MainMenu.interactable = true;
        MainMenu.outline = true;
        MainMenu.outlineColor = Color.Black;
        #endregion

        #region Switch Team
        SwitchTeam = Canvas.Add("Switch Team");
        SwitchTeam.position = new Vector2(0.5f, 0.30f);
        SwitchTeam.origin = new Vector2(0.5f);
        SwitchTeam.color = Unselected;
        SwitchTeam.font = SportNumbers32;
        SwitchTeam.interactable = true;
        SwitchTeam.outline = true;
        SwitchTeam.outlineColor = Color.Black;
        #endregion

        #region Options Menu
        OptionsMenu = Canvas.Add("Options Menu");
        OptionsMenu.position = new Vector2(0.5f, 0.35f);
        OptionsMenu.origin = new Vector2(0.5f);
        OptionsMenu.color = Unselected;
        OptionsMenu.font = SportNumbers32;
        OptionsMenu.interactable = true;
        OptionsMenu.outline = true;
        OptionsMenu.outlineColor = Color.Black;
        #endregion

        #region Back To Game
        BackToGame = Canvas.Add("Back To Game");
        BackToGame.position = new Vector2(0.5f, 0.40f);
        BackToGame.origin = new Vector2(0.5f);
        BackToGame.color = Unselected;
        BackToGame.font = SportNumbers32;
        BackToGame.interactable = true;
        BackToGame.outline = true;
        BackToGame.outlineColor = Color.Black;
        #endregion

        #region Exit Game
        ExitGame = Canvas.Add("Exit Game");
        ExitGame.position = new Vector2(0.5f, 0.45f);
        ExitGame.origin = new Vector2(0.5f);
        ExitGame.color = Unselected;
        ExitGame.font = SportNumbers32;
        ExitGame.interactable = true;
        ExitGame.outline = true;
        ExitGame.outlineColor = Color.Black;
        #endregion

        #endregion
    }
}
