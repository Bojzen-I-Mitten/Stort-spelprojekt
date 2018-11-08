
using ThomasEngine;

public class EscapeMenu : ScriptComponent
{
    public Texture2D Background { get; set; }

    Camera Camera;
    Canvas Canvas;
    Image BG;

    public override void Awake()
    {
        Camera = gameObject.GetComponent<Camera>();

        Canvas = Camera.AddCanvas();
    }

    public override void Start()
    {
        BG = Canvas.Add(Background);
        BG.scale = Vector2.Zero;
    }

    public override void Update()
    {
        
    }

    public void ShowEscMenu()
    {
        BG.scale = Vector2.One;
    }
}
