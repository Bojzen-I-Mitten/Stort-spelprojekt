using System.Collections;
using ThomasEngine;

public class CameraMaster : ScriptComponent
{
    public Texture2D Background { get; set; }

    Camera Camera;
    ScriptComponent JoinHost;
    GUISelectTeam SelectTeam;
    ChadCam ChadCam;
    SpectatorCam SpectatorCam;

    Canvas canvas;

    Image BG;

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
        SpectatorCam = gameObject.GetComponent<SpectatorCam>();
        if (SpectatorCam == null)
            Debug.Log("Camera Master cannot find SpectatorCam");

        SelectTeam.enabled = false;
        ChadCam.enabled = false;
        SpectatorCam.enabled = false;

        canvas = Camera.AddCanvas();
        BG = canvas.Add(Background);
        BG.interactable = true;
    }

    public override void Update()
    {
        if (!JoinHost.enabled)
            SelectTeam.enabled = true;
        if (!SelectTeam.enabled)
            switch (MatchSystem.instance.LocalChad.NetworkPlayer.Team.TeamType)
            {
                case TEAM_TYPE.TEAM_1:
                case TEAM_TYPE.TEAM_2:
                    ChadCam.enabled = true;
                    break;
                case TEAM_TYPE.TEAM_SPECTATOR:
                    SpectatorCam.enabled = true;
                    break;
            }
    }
}
