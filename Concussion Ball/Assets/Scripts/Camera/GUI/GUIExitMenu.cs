using ThomasEngine;

public class GUIExitMenu : ScriptComponent
{
    public Font Font { get; set; }

    Camera Camera;
    public Canvas Canvas;
    //Text ExitMatch;
    Text ExitGame;

    //private readonly string _exitMatch = "Exit Match";
    private readonly string _exitGame = "Exit Game";

    public override void OnAwake()
    {
        Camera = gameObject.GetComponent<Camera>();
        Canvas = Camera.AddCanvas();

        ExitGame = Canvas.Add(_exitGame);
        ExitGame.font = Font;
        ExitGame.position = new Vector2(0.4f, 0.25f);
        ExitGame.color = Color.Black;
        ExitGame.interactable = true;

        //ExitMatch = Canvas.Add(_exitMatch);
        //ExitMatch.font = Font;
        //ExitMatch.position = new Vector2(0.4f, 0.35f);
        //ExitMatch.color = Color.Black;
        //ExitMatch.interactable = true;
    }

    public override void Start()
    {
    }

    public override void Update()
    {
        //if (ExitMatch.Hovered())
        //    ExitMatch.color = Color.Blue;
        //else
        //    ExitMatch.color = Color.Black;

        if (ExitGame.Hovered())
            ExitGame.color = Color.Red;
        else
            ExitGame.color = Color.Black;
        //if (ExitMatch.Clicked())
        //{
        //    CameraMaster.instance.State = CAM_STATE.JOIN_HOST;
        //    CameraMaster.instance.Canvas.isRendering = true;
        //}
        if (ExitGame.Clicked())
        {
            Debug.Log("I'm exiting!");
            ThomasWrapper.Exit();
        }
    }
}
