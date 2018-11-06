using System.Collections;
using ThomasEngine;

public class CameraMaster : ScriptComponent
{
    public Texture2D Background { get; set; }

    Camera Camera;
    GUIJoinHost JoinHost;
    GUISelectTeam SelectTeam;
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
        NUMSTATES
    }
    CAMSTATE State = CAMSTATE.JOIN_HOST;

    public override void Start()
    {
        Camera = gameObject.GetComponent<Camera>();
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


        Canvas = Camera.AddCanvas();
        BG = Canvas.Add(Background);
        BG.interactable = true;
        BG.depth = 1;
    }

    public override void Update()
    {
        switch (State)
        {
            case CAMSTATE.JOIN_HOST:
                SelectTeam.Canvas.isRendering = false;
                Hud.Canvas.isRendering = false;
                JoinHost.Canvas.isRendering = true;
                if (JoinHost.GoToTeamSelect)
                    State = CAMSTATE.SELECT_TEAM;
                break;
            case CAMSTATE.SELECT_TEAM:
                SelectTeam.Canvas.isRendering = true;
                Hud.Canvas.isRendering = false;
                JoinHost.Canvas.isRendering = false;
                if (SelectTeam.GoToGameCam)
                {
                    Canvas.isRendering = false; //canvas.Remove(BG) if we need to save some RAM.
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
                break;
            case CAMSTATE.SPECTATE:
                SelectTeam.Canvas.isRendering = false;
                Hud.Canvas.isRendering = true;
                JoinHost.Canvas.isRendering = false;
                break;
        }
    }
}
