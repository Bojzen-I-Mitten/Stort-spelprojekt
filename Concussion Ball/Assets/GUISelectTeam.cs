using ThomasEngine;

public class GUISelectTeam : ScriptComponent
{
    public Texture2D SelectBox { get; set; }
    public Font TextFont { get; set; }

    Camera Camera;
    private bool Disabled;

    private readonly string Team1 = "Team1";
    private readonly string Team2 = "Team2";
    private readonly string Spectator = "Spectator";
    private readonly string Select = "Select";

    public override void Start()
    {
        Disabled = false;
        Camera = gameObject.GetComponent<Camera>();
        AddImagesAndText();
    }

    public override void Update()
    {
        if (Camera.OnImageClicked(Team1))
        {
            MatchSystem.instance.JoinTeam(TEAM_TYPE.TEAM_1);
            this.enabled = false;
            Camera.enabled = false;
        }
        else if (Camera.OnImageClicked(Team2))
        {
            MatchSystem.instance.JoinTeam(TEAM_TYPE.TEAM_2);
            this.enabled = false;
            Camera.enabled = false;
        }
        else if (Camera.OnImageClicked(Spectator))
        {
            MatchSystem.instance.JoinTeam(TEAM_TYPE.TEAM_SPECTATOR);
            this.enabled = false;
            Camera.enabled = false;
        }

        if (TextFont != null && !Disabled)
        {
            Camera.SetTextFont(Select, TextFont);
            Camera.SetTextFont(Team1, TextFont);
            Camera.SetTextFont(Team2, TextFont);
            Camera.SetTextFont(Spectator, TextFont);
        }
    }

    public void AddImagesAndText()
    {
        Camera.AddImage(Team1, SelectBox, new Vector2(0.13f, 0.23f), new Vector2(1.0f), true);
        Camera.SetImageColor(Team1, MatchSystem.instance.Teams[TEAM_TYPE.TEAM_1].Color.ToVector4());
        Camera.AddImage(Team2, SelectBox, new Vector2(0.23f, 0.23f), new Vector2(1.0f), true);
        Camera.SetImageColor(Team2, MatchSystem.instance.Teams[TEAM_TYPE.TEAM_2].Color.ToVector4());
        Camera.AddImage(Spectator, SelectBox, new Vector2(0.33f, 0.23f), new Vector2(1.0f), true);
        Camera.SetImageColor(Spectator, MatchSystem.instance.Teams[TEAM_TYPE.TEAM_SPECTATOR].Color.ToVector4());

        Camera.AddText(Select, "Select team", new Vector2(0.21f, 0.11f), new Vector2(1.0f));
        Camera.AddText(Team1, MatchSystem.instance.Teams[TEAM_TYPE.TEAM_1].Name, new Vector2(0.10f, 0.18f), new Vector2(1.0f));
        Camera.AddText(Team2, MatchSystem.instance.Teams[TEAM_TYPE.TEAM_2].Name, new Vector2(0.20f, 0.18f), new Vector2(1.0f));
        Camera.AddText(Spectator, MatchSystem.instance.Teams[TEAM_TYPE.TEAM_SPECTATOR].Name, new Vector2(0.31f, 0.18f), new Vector2(1.0f));


    }

    public void ClearImagesAndText()
    {
        Camera.DeleteImage(Select);
        Camera.DeleteImage(Team1);
        Camera.DeleteImage(Team2);
        Camera.DeleteImage(Spectator);
    }
}
