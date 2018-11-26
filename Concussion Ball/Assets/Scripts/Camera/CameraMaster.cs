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
    public static CameraMaster instance;

    public Texture2D Background { get; set; }

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

    Vector3 MainMenuCamPos;
    Vector3 MainMenuCamRot;
    Vector3 SelectTeamCamPos;
    Vector3 SelectTeamCamRot;

    public override void OnAwake()
    {
        Camera = gameObject.GetComponent<Camera>();
        Canvas = Camera.AddCanvas();
    }

    public override void Start()
    {
        instance = this;
        State = CAM_STATE.MAIN_MENU;
        MainMenuCamPos = new Vector3(0, -195.442f, -7.084f);
        MainMenuCamRot = Vector3.Zero;
        SelectTeamCamPos = new Vector3(0, -198.5f, 8.2f);
        SelectTeamCamRot = new Vector3(MathHelper.Pi, 0.0f, 0.0f);
        //BG = Canvas.Add(Background);
        //BG.interactable = true;

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
        SelectTeam.Canvas.isRendering = false;
        Hud.Canvas.isRendering = false;
        ExitMenu.Canvas.isRendering = false;
        JoinHost.Canvas.isRendering = false;
        MainMenu.Canvas.isRendering = false;
        HostMenu.Canvas.isRendering = false;
        LoadingScreen.Canvas.isRendering = false;

        switch (State)
        {
            case CAM_STATE.MAIN_MENU:
                transform.position = MainMenuCamPos;
                transform.rotation = Quaternion.CreateFromYawPitchRoll(MainMenuCamRot.x, MainMenuCamRot.y, MainMenuCamRot.z);
                MainMenu.Canvas.isRendering = true;
                break;

            case CAM_STATE.JOIN_HOST:
                JoinHost.Canvas.isRendering = true;
                break;
            case CAM_STATE.SELECT_TEAM:
                transform.position = SelectTeamCamPos;
                transform.rotation = Quaternion.CreateFromYawPitchRoll(SelectTeamCamRot.x, SelectTeamCamRot.y, SelectTeamCamRot.z);
                SelectTeam.Canvas.isRendering = true;
                break;
            case CAM_STATE.GAME:
                Hud.Canvas.isRendering = true;
                if(Input.GetKeyDown(Input.Keys.Escape))
                    State = CAM_STATE.EXIT_MENU;
                break;
            case CAM_STATE.EXIT_MENU:
                ExitMenu.Canvas.isRendering = true;
                if (Input.GetKeyDown(Input.Keys.Escape))
                    State = CAM_STATE.GAME;
                break;
            case CAM_STATE.HOST_MENU:
                transform.position = SelectTeamCamPos;
                transform.rotation = Quaternion.CreateFromYawPitchRoll(SelectTeamCamRot.x, SelectTeamCamRot.y, SelectTeamCamRot.z);
                HostMenu.Canvas.isRendering = true;
                break;
            case CAM_STATE.LOADING_SCREEN:
                LoadingScreen.Canvas.isRendering = true;
                break;
        }
    }
}
