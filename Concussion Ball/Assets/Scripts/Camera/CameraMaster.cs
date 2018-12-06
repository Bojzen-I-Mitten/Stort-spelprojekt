using ThomasEngine;

public enum CAM_STATE
{
    JOIN_HOST,
    SELECT_TEAM,
    GAME,
    MAIN_MENU,
    EXIT_MENU,
    HOST_MENU,
    LOADING_SCREEN,
    NUMSTATES
}

public class CameraMaster : ScriptComponent
{
    public GameObject ChadTeam1 { get; set; }
    public GameObject ChadTeam2 { get; set; }

    public static CameraMaster instance;

    public Camera Camera;
    GUIJoinHost JoinHost;
    GUIMainMenu MainMenu;
    GUISelectTeam SelectTeam;
    GUIExitMenu ExitMenu;
    GUIHostMenu HostMenu;
    GUILoadingScreen LoadingScreen;
    ChadCam ChadCam;
    SpectatorCam SpectatorCam;
    ChadHud Hud;

    public Canvas Canvas;
    public CAM_STATE State;

    
    

    public override void OnAwake()
    {
        instance = this;
        Camera = gameObject.GetComponent<Camera>();
        Canvas = Camera.AddCanvas();
    }


    public void StartReplay()
    {
        ChadCam.enabled = false;
        SpectatorCam.enabled = false;
        
    }

    public void StopReplay()
    {
        //ChadCam.enabled = true;
        //SpectatorCam.enabled = false;
    }

    public override void Start()
    {
        State = CAM_STATE.MAIN_MENU;
        

        if (Camera == null)
            Debug.Log("Camera Master cannot find camera");

        JoinHost = gameObject.GetComponent<GUIJoinHost>();
        if (JoinHost == null)
            Debug.Log("Camera Master cannot find GUI script for join/host");
        MainMenu = gameObject.GetComponent<GUIMainMenu>();
        if (MainMenu == null)
            Debug.Log("Camera master could not find GUI script for main menu");
        SelectTeam = gameObject.GetComponent<GUISelectTeam>();
        if (SelectTeam == null)
            Debug.Log("Camera Master cannot find GUI script for select");

        ExitMenu = gameObject.GetComponent<GUIExitMenu>();
        if (ExitMenu == null)
            Debug.Log("Camera Master cannot find GUI script for exit");

        HostMenu = gameObject.GetComponent<GUIHostMenu>();
        if (HostMenu == null)
            Debug.Log("Camera Master cannot find GUI script for host");

        LoadingScreen = gameObject.GetComponent<GUILoadingScreen>();
        if (LoadingScreen == null)
            Debug.Log("Camera Maser cannot find GUI script for loading screen");

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
        //Set all CAM_STATEs exept GAME to false
        MainMenu.Canvas.isRendering = false;
        JoinHost.Canvas.isRendering = false;
        SelectTeam.Canvas.isRendering = false;
        Hud.Canvas.isRendering = false;
        ExitMenu.Canvas.isRendering = false;
        HostMenu.Canvas.isRendering = false;
        LoadingScreen.Canvas.isRendering = false;

        switch (State)
        {
            case CAM_STATE.MAIN_MENU:
                MainMenu.SetUpScene();
                MainMenu.Canvas.isRendering = true;
                break;

            case CAM_STATE.JOIN_HOST:
                MainMenu.SetUpScene();
                JoinHost.Canvas.isRendering = true;
                break;
            case CAM_STATE.SELECT_TEAM:
                SelectTeam.SetUpScene();
                SelectTeam.Canvas.isRendering = true;
                break;
            case CAM_STATE.GAME:
                Hud.Canvas.isRendering = true;
                if(Input.GetKeyDown(Input.Keys.Escape))
                {
                    State = CAM_STATE.EXIT_MENU;
                    Input.SetMouseMode(Input.MouseMode.POSITION_ABSOLUTE);
                }
                break;
            case CAM_STATE.EXIT_MENU:
                ExitMenu.Canvas.isRendering = true;
                if (Input.GetKeyDown(Input.Keys.Escape))
                {
                    State = CAM_STATE.GAME;
                    Input.SetMouseMode(Input.MouseMode.POSITION_RELATIVE);
                }
                break;
            case CAM_STATE.HOST_MENU:
                HostMenu.SetUpScene();
                HostMenu.Canvas.isRendering = true;
                break;
            case CAM_STATE.LOADING_SCREEN:
                LoadingScreen.Canvas.isRendering = true;
                break;
        }
    }
}
