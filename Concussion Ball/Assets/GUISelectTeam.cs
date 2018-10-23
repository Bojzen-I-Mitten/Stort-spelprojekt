using ThomasEngine;

public class GUISelectTeam : ScriptComponent
{
    public Texture2D SelectBox { get; set; }
    public Font TextFont { get; set; }

    Camera Camera;
    private bool Disabled;

    public override void Start()
    {
        Disabled = false;
        Camera = gameObject.GetComponent<Camera>();
        Vector2 viewPort = Camera.GetViewPort();
        Vector2 scale = viewPort / new Vector2(1920, 1080);
        AddImagesAndText();
    }

    public override void Update()
    {
        if (Camera.OnImageClicked("Team1"))
        {
            MatchSystem.instance.JoinTeam(TEAM_TYPE.TEAM_1);
            this.enabled = false;
            Camera.enabled = false;
        }
        else if (Camera.OnImageClicked("Team2"))
        {
            MatchSystem.instance.JoinTeam(TEAM_TYPE.TEAM_2);
            this.enabled = false;
            Camera.enabled = false;
        }
        else if (Camera.OnImageClicked("Spectator"))
        {
            MatchSystem.instance.JoinTeam(TEAM_TYPE.TEAM_SPECTATOR);
            this.enabled = false;
            Camera.enabled = false;
        }

        if (TextFont != null && !Disabled)
        {
            Camera.SetTextFont("Select", TextFont);
            Camera.SetTextFont("Team1", TextFont);
            Camera.SetTextFont("Team2", TextFont);
            Camera.SetTextFont("Spectator", TextFont);
        }
    }

    public void AddImagesAndText()
    {
        Vector2 viewPort = Camera.GetViewPort();
        Vector2 scale = viewPort / new Vector2(1920, 1080);

        

        Camera.AddImage("Team1", SelectBox, new Vector2(250, 250) * scale, new Vector2(1.0f) * scale, true);
        Camera.SetImageColor("Team1", MatchSystem.instance.Teams[TEAM_TYPE.TEAM_1].Color.ToVector4());
        Camera.AddImage("Team2", SelectBox, new Vector2(450, 250) * scale, new Vector2(1.0f) * scale, true);
        Camera.SetImageColor("Team2", MatchSystem.instance.Teams[TEAM_TYPE.TEAM_2].Color.ToVector4());
        Camera.AddImage("Spectator", SelectBox, new Vector2(650, 250) * scale, new Vector2(1.0f) * scale, true);
        Camera.SetImageColor("Spectator", MatchSystem.instance.Teams[TEAM_TYPE.TEAM_SPECTATOR].Color.ToVector4());

        Camera.AddText("Select", "Select team", new Vector2(400, 120) * scale, new Vector2(1.0f) * scale);
        Camera.AddText("Team1", MatchSystem.instance.Teams[TEAM_TYPE.TEAM_1].Name, new Vector2(200, 200) * scale, new Vector2(1.0f) * scale);
        Camera.AddText("Team2", MatchSystem.instance.Teams[TEAM_TYPE.TEAM_2].Name, new Vector2(400, 200) * scale, new Vector2(1.0f) * scale);
        Camera.AddText("Spectator", MatchSystem.instance.Teams[TEAM_TYPE.TEAM_SPECTATOR].Name, new Vector2(600, 200) * scale, new Vector2(1.0f) * scale);


    }

    public void ClearImagesAndText()
    {
        Camera.DeleteImage("Select");
        Camera.DeleteImage("Team1");
        Camera.DeleteImage("Team2");
        Camera.DeleteImage("Spectator");
    }
}
