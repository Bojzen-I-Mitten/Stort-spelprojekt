using ThomasEngine;
using System.Collections;

public class GUIExitMenu : ScriptComponent
{
    public Font Font { get; set; }

    Camera Camera;
    public Canvas Canvas;
    Text ExitMatch;
    Text ExitGame;

    private readonly string _exitMatch = "Exit Match";
    private readonly string _exitGame = "Exit Game";

    public override void Awake()
    {
        Camera = gameObject.GetComponent<Camera>();
        Canvas = Camera.AddCanvas();

        ExitGame = Canvas.Add(_exitGame);
        ExitGame.font = Font;
        ExitGame.position = new Vector2(0.4f, 0.25f);
        ExitGame.color = Color.Black;
        ExitGame.interactable = true;
    }

    public override void Start()
    {
    }

    public override void Update()
    {
        if (Canvas.isRendering)
        {
            if (ExitGame.Hovered())
                ExitGame.color = Color.Red;
            else
                ExitGame.color = Color.Black;

            if (Input.GetMouseButtonUp(Input.MouseButtons.LEFT))
            {
                //if (ExitMatch.Clicked())
                //{
                //    CameraMaster.instance.State = CAM_STATE.JOIN_HOST;
                //    CameraMaster.instance.Canvas.isRendering = true;
                //}
                if (ExitGame.Clicked())
                {
                    Debug.Log("I'm exciting! :^)");
                    ThomasWrapper.IssueStopPlay();
                    Debug.Log("TEST2");
                    //StartCoroutine(Replay());
                    //ThomasWrapper.Exit();
                }
            }
        }
    }
}
