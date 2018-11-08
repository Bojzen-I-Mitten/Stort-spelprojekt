using ThomasEngine;

public class GUIExitMenu : ScriptComponent
{
    public Texture2D ExitMatchBtn { get; set; }
    public Texture2D ExitGameBtn { get; set; }

    Camera Camera;
    public Canvas Canvas;

    public override void Awake()
    {
        Camera = gameObject.GetComponent<Camera>();

        Canvas = Camera.AddCanvas();
    }

    public override void Start()
    {
    }

    public override void Update()
    {

    }

    public void ShowEscMenu()
    {

    }
}
