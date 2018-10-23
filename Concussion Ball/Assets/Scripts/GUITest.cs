using ThomasEngine;

public class GUITest : ScriptComponent
{
    public Texture2D Background { get; set; }

    Camera Camera;
    ScriptComponent JoinHost;
    GUISelectTeam SelectTeam;

    public override void Start()
    {
        Camera = gameObject.GetComponent<Camera>();
        JoinHost = gameObject.GetComponent<GUIJoinHost>();
        SelectTeam = gameObject.GetComponent<GUISelectTeam>();
        SelectTeam.enabled = false;

        Vector2 viewPort = Camera.GetViewPort();
        Vector2 scale = viewPort / new Vector2(1920, 1080);

        Camera.AddImage("BG", Background, new Vector2(0, 0), scale, true);
    }

    public override void Update()
    {

    }
}
