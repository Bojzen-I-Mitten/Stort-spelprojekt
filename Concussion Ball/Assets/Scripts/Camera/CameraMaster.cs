using ThomasEngine;
using System.Configuration;

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
    public GameObject ChadMainMenu { get; set; }
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

    public int SelectedHat;
    Hatter ChadMMHat = null;
    Hatter ChadT1Hat = null;
    Hatter ChadT2Hat = null;

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
        #region Init GUI
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
        #endregion

        #region Chad Hats

        string settingsHat = UserSettings.GetSetting("Hat");
        
        if(settingsHat != null)
        {
            SelectedHat = System.Convert.ToInt32(settingsHat);
        }
        else
        {
            SelectedHat = (int)(Random.Range(0.0f, 1.0f) * (HatManager.Instance.Hats.Count - 2)) + 1;
            settingsHat = SelectedHat.ToString();
            UserSettings.AddOrUpdateAppSetting("Hat", settingsHat);
        }


        if (ChadMainMenu != null)
        {
            ChadMMHat = ChadMainMenu.GetComponent<Hatter>();
            ChadMMHat.SetHat(SelectedHat);
        }
        if (ChadTeam1 != null)
        {
            ChadT1Hat = ChadTeam1.GetComponent<Hatter>();
            ChadT1Hat.SetHat(SelectedHat);
        }
        if (ChadTeam1 != null)
        {
            ChadT2Hat = ChadTeam2.GetComponent<Hatter>();
            ChadT2Hat.SetHat(SelectedHat);
        }
        #endregion

    }

    public string GetPlayerName()
    {
        return MainMenu != null ? MainMenu.GetPlayerName() : "Chad";
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
                Camera.fixedAspectRatio = true;
                Camera.orthographic = true;
                MainMenu.Canvas.isRendering = true;
                break;

            case CAM_STATE.JOIN_HOST:
                MainMenu.SetUpScene();
                Camera.fixedAspectRatio = true;
                Camera.orthographic = true;
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
                Camera.fixedAspectRatio = false;
                Camera.orthographic = false;
                HostMenu.Canvas.isRendering = true;
                break;
            case CAM_STATE.LOADING_SCREEN:
                LoadingScreen.Canvas.isRendering = true;
                break;
        }
        UpdateHats();
    }

    void UpdateHats()
    {
        SelectedHat = SelectedHat == -1 ? ChadMMHat.GetHatCount()-1 : SelectedHat;
        SelectedHat %= ChadMMHat.GetHatCount();

        ChadMMHat.SetHat(SelectedHat);
        ChadT1Hat.SetHat(SelectedHat);
        ChadT2Hat.SetHat(SelectedHat);
    }
}
