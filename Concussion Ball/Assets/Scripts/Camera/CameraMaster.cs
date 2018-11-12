using ThomasEngine;

public enum CAM_STATE
{
    JOIN_HOST,
    SELECT_TEAM,
    CHAD,
    SPECTATE,
    NUMSTATES
}

public class CameraMaster : ScriptComponent
{
    public static CameraMaster instance;

    public Texture2D Background { get; set; }

    Camera Camera;
    GUIJoinHost JoinHost;
    GUISelectTeam SelectTeam;
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
        
      //  BG = Canvas.Add(Background);
       // BG.interactable = true;

        if (Camera == null)
            Debug.Log("Camera Master cannot find camera");
        JoinHost = gameObject.GetComponent<GUIJoinHost>();
        if (JoinHost == null)
            Debug.Log("Camera Master cannot find GUI script for join/host");
        SelectTeam = gameObject.GetComponent<GUISelectTeam>();
        if (SelectTeam == null)
            Debug.Log("Camera Master cannot find GUI script for select");
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
        switch (State)
        {
            case CAM_STATE.JOIN_HOST:
                SelectTeam.Canvas.isRendering = false;
                Hud.Canvas.isRendering = false;
                JoinHost.Canvas.isRendering = true;
                break;
            case CAM_STATE.SELECT_TEAM:
                SelectTeam.Canvas.isRendering = true;
                Hud.Canvas.isRendering = false;
                JoinHost.Canvas.isRendering = false;
                //if (MatchSystem.instance.LocalChad?.NetPlayer?.Team?.TeamType != TEAM_TYPE.UNASSIGNED)
                //{
                //    Canvas.isRendering = false; //canvas.Remove(BG) if we need to save some RAM.
                //    switch (MatchSystem.instance.LocalChad.gameObject.GetComponent<NetworkPlayer>().Team.TeamType)
                //    {
                //        default:
                //            break;
                //        case TEAM_TYPE.TEAM_1:
                //        case TEAM_TYPE.TEAM_2:
                //            ChadCam.enabled = true;
                //            State = CAM_STATE.CHAD;
                //            break;
                //        case TEAM_TYPE.TEAM_SPECTATOR:
                //            SpectatorCam.enabled = true;
                //            State = CAM_STATE.SPECTATE;
                //            break;
                //    }
                //}
                break;
            case CAM_STATE.CHAD:
                SelectTeam.Canvas.isRendering = false;
                Hud.Canvas.isRendering = true;
                JoinHost.Canvas.isRendering = false;
                break;
            case CAM_STATE.SPECTATE:
                SelectTeam.Canvas.isRendering = false;
                Hud.Canvas.isRendering = true;
                JoinHost.Canvas.isRendering = false;
                break;
        }
    }
}
