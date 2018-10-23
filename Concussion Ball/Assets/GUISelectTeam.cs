using ThomasEngine;

public class GUISelectTeam : ScriptComponent
{
    public Texture2D SelectBox { get; set; }

    Camera Camera;

    public override void Start()
    {
        Camera = gameObject.GetComponent<Camera>();
        Vector2 viewPort = Camera.GetViewPort();
        Vector2 scale = viewPort / new Vector2(1920, 1080);
        AddImagesAndText();
    }

    public override void Update()
    {
        if (Camera.OnImageClicked("Team1"))
        {

        }
        else if (Camera.OnImageClicked("TextBoxPort"))
        {

        }
        else if (Camera.OnImageClicked("BG"))
        {

        }
    }

    public void AddImagesAndText()
    {
        Vector2 viewPort = Camera.GetViewPort();
        Vector2 scale = viewPort / new Vector2(1920, 1080);

        Camera.AddImage("Team1", SelectBox, new Vector2(250, 250) * scale, new Vector2(1.0f) * scale, true);
        Camera.AddImage("Team2", SelectBox, new Vector2(450, 250) * scale, new Vector2(1.0f) * scale, true);

    }

    public void ClearImagesAndText()
    {
        Camera.DeleteImage("Team1");
        Camera.DeleteImage("Team2");
    }
}
