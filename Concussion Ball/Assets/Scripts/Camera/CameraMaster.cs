using System.Collections;
using ThomasEngine;

public class CameraMaster : ScriptComponent
{
    public Texture2D Background { get; set; }

    Camera Camera;
    GUIJoinHost JoinHost;
    GUISelectTeam SelectTeam;
    GUIExitMenu ExitMenu;
    ChadCam ChadCam;
    SpectatorCam SpectatorCam;
    ChadHud Hud;


    Canvas Canvas;
    Image BG;

    enum CAMSTATE
    {
        JOIN_HOST,
        SELECT_TEAM,
        CHAD,
        SPECTATE,
        NUMSTATES,
        EXIT_MENU
    }
    CAMSTATE State = CAMSTATE.JOIN_HOST;
    CAMSTATE PreviousState = CAMSTATE.JOIN_HOST;

    public override void Awake()
    {
        Camera = gameObject.GetComponent<Camera>();

        Canvas = Camera.AddCanvas();
    }

    public override void Start()
    {
        Debug.Log("CameraMaster is running now.");
        BG = Canvas.Add(Background);
        BG.interactable = true;

        if (Camera == null)
            Debug.Log("Camera Master cannot find camera");

        JoinHost = gameObject.GetComponent<GUIJoinHost>();
        if (JoinHost == null)
            Debug.Log("Camera Master cannot find GUI script for join/host");

        SelectTeam = gameObject.GetComponent<GUISelectTeam>();
        if (SelectTeam == null)
            Debug.Log("Camera Master cannot find GUI script for select");

        ExitMenu = gameObject.GetComponent<GUIExitMenu>();
        if (SelectTeam == null)
            Debug.Log("Camera Master cannot find GUI script for exit");

        ChadCam = gameObject.GetComponent<ChadCam>();
        if (ChadCam == null)
            Debug.Log("Camera Master cannot find ChadCam");
        else
            ChadCam.enabled = false;

        SpectatorCam = gameObject.GetComponent<SpectatorCam>();
        if (SpectatorCam == null)
            Debug.Log("Camera Master cannot find SpectatorCam");
        else
            SpectatorCam.enabled = false;

        Hud = gameObject.GetComponent<ChadHud>();
        if (Hud == null)
            Debug.Log("Camera Master could not find Hud");



    }

    public override void Update()
    {
        if (Input.GetKeyDown(Input.Keys.Escape))
        {
            if (State == CAMSTATE.EXIT_MENU)
            {
                State = PreviousState;
                if (PreviousState == CAMSTATE.CHAD)
                {
                    ChadCam.enabled = true;
                    if (Canvas.isRendering)
                        Canvas.isRendering = false;
                }
            }
            else
            {
                PreviousState = State;
                State = CAMSTATE.EXIT_MENU;
                if (!Canvas.isRendering)
                    Canvas.isRendering = true;
                ChadCam.enabled = false;
            }
        }

        switch (State)
        {
            case CAMSTATE.JOIN_HOST:
                SelectTeam.Canvas.isRendering = false;
                Hud.Canvas.isRendering = false;
                JoinHost.Canvas.isRendering = true;
                ExitMenu.Canvas.isRendering = false;
                if (JoinHost.GoToTeamSelect)
                {
                    PreviousState = State;
                    State = CAMSTATE.SELECT_TEAM;
                }
                break;
            case CAMSTATE.SELECT_TEAM:
                SelectTeam.Canvas.isRendering = true;
                Hud.Canvas.isRendering = false;
                JoinHost.Canvas.isRendering = false;
                ExitMenu.Canvas.isRendering = false;
                if (SelectTeam.GoToGameCam)
                {
                    Canvas.isRendering = false; //canvas.Remove(BG) if we need to save some RAM.
                    PreviousState = State;
                    switch (MatchSystem.instance.LocalChad.gameObject.GetComponent<NetworkPlayer>().Team.TeamType)
                    {
                        default:
                            break;
                        case TEAM_TYPE.TEAM_1:
                        case TEAM_TYPE.TEAM_2:
                            ChadCam.enabled = true;
                            State = CAMSTATE.CHAD;
                            break;
                        case TEAM_TYPE.TEAM_SPECTATOR:
                            SpectatorCam.enabled = true;
                            State = CAMSTATE.SPECTATE;
                            break;
                    }
                }
                break;
            case CAMSTATE.CHAD:
                SelectTeam.Canvas.isRendering = false;
                Hud.Canvas.isRendering = true;
                JoinHost.Canvas.isRendering = false;
                ExitMenu.Canvas.isRendering = false;
                break;
            case CAMSTATE.SPECTATE:
                SelectTeam.Canvas.isRendering = false;
                Hud.Canvas.isRendering = true;
                JoinHost.Canvas.isRendering = false;
                ExitMenu.Canvas.isRendering = false;
                break;
            case CAMSTATE.EXIT_MENU:
                SelectTeam.Canvas.isRendering = false;
                Hud.Canvas.isRendering = false;
                JoinHost.Canvas.isRendering = false;
                ExitMenu.Canvas.isRendering = true;
                break;
        }
    }
}
