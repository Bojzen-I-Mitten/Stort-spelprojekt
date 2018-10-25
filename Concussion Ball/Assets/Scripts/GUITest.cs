using ThomasEngine;

public class GUITest : ScriptComponent
{
    public Texture2D Background { get; set; }

    Camera Camera;
    ScriptComponent JoinHost;
    GUISelectTeam SelectTeam;

    private readonly string BG = "BG";

    public override void Start()
    {
        Camera = gameObject.GetComponent<Camera>();
        JoinHost = gameObject.GetComponent<GUIJoinHost>();
        SelectTeam = gameObject.GetComponent<GUISelectTeam>();
        SelectTeam.enabled = false;

        Camera.AddImage(BG, Background, new Vector2(0.0f, 0.0f), new Vector2(1.0f, 1.0f), true);
    }

    public override void Update()
    {

    }
}
