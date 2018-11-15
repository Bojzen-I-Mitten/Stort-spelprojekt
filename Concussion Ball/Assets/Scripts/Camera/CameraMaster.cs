using ThomasEngine;

public enum CAM_STATE
{
    JOIN_HOST,
    SELECT_TEAM,
    GAME,
    NUMSTATES,
    EXIT_MENU
}

public class CameraMaster : ScriptComponent
{
    public static CameraMaster instance;

    public Texture2D Background { get; set; }

    public Camera Camera;
    GUIJoinHost JoinHost;
    GUISelectTeam SelectTeam;
    GUIExitMenu ExitMenu;
    ChadCam ChadCam;
    SpectatorCam SpectatorCam;
    ChadHud Hud;

    public Canvas Canvas;
    Image BG;
    public CAM_STATE State = CAM_STATE.JOIN_HOST;



    public override void Awake()
    {
        Camera = gameObject.GetComponent<Camera>();

        Canvas = Camera.AddCanvas();
    }

    public override void Start()
    {
        instance = this;
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
        SelectTeam.Canvas.isRendering = false;
        Hud.Canvas.isRendering = false;
        ExitMenu.Canvas.isRendering = false;
        JoinHost.Canvas.isRendering = false;

        switch (State)
        {
            case CAM_STATE.JOIN_HOST:
                JoinHost.Canvas.isRendering = true;
                break;
            case CAM_STATE.SELECT_TEAM:
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
        }
    }
}
