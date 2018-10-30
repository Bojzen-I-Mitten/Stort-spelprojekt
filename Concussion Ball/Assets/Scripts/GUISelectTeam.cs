using ThomasEngine;

public class GUISelectTeam : ScriptComponent
{
    public Texture2D SelectBox { get; set; }
    public Font TextFont { get; set; }

    Camera Camera;
    private bool Disabled;

    Canvas GUI;

    Image Team1Image;
    Image Team2Image;
    Image SpectatorImage;
    Text Select;
    Text Team1Text;
    Text Team2Text;
    Text SpectatorText;

    public override void Start()
    {
        Disabled = false;
        Camera = gameObject.GetComponent<Camera>();
        AddsAndText();
    }

    public override void Update()
    {
        if (Input.GetMouseButtonDown(Input.MouseButtons.LEFT))
        {
            if (Team1Image.Clicked())
            {
                MatchSystem.instance.JoinTeam(TEAM_TYPE.TEAM_1);
                MatchSystem.instance.LocalChad.ActivateCamera();
                this.enabled = false;
                Camera.enabled = false;
            }
            else if (Team2Image.Clicked())
            {
                MatchSystem.instance.JoinTeam(TEAM_TYPE.TEAM_2);
                MatchSystem.instance.LocalChad.ActivateCamera();
                this.enabled = false;
                Camera.enabled = false;
            }
            else if (SpectatorImage.Clicked())
            {
                MatchSystem.instance.JoinTeam(TEAM_TYPE.TEAM_SPECTATOR);
                this.enabled = false;
                Camera.enabled = false;
            }
        }
        if (TextFont != null && !Disabled)
        {
            Select.font = TextFont;
            Team1Text.font = TextFont;
            Team2Text.font = TextFont;
            SpectatorText.font = TextFont;
        }
    }

    public void AddsAndText()
    {
        GUI = Camera.AddCanvas();

        Team1Image = GUI.Add(SelectBox);
        Team1Image.position = new Vector2(0.13f, 0.23f);
        Team1Image.interactable = true;
        Team1Image.color = MatchSystem.instance.Teams[TEAM_TYPE.TEAM_1].Color.ToVector4();

        Team2Image = GUI.Add(SelectBox);
        Team2Image.position = new Vector2(0.25f, 0.23f);
        Team2Image.interactable = true;
        Team2Image.color = MatchSystem.instance.Teams[TEAM_TYPE.TEAM_2].Color.ToVector4();

        SpectatorImage = GUI.Add(SelectBox);
        SpectatorImage.position = new Vector2(0.25f, 0.23f);
        SpectatorImage.interactable = true;
        SpectatorImage.color = MatchSystem.instance.Teams[TEAM_TYPE.TEAM_SPECTATOR].Color.ToVector4();

        Select = GUI.Add("Select Team");
        Select.position = new Vector2(0.21f, 0.11f);

        Team1Text = GUI.Add(MatchSystem.instance.Teams[TEAM_TYPE.TEAM_1].Name);
        Team1Text.position = new Vector2(0.10f, 0.18f);

        Team1Text = GUI.Add(MatchSystem.instance.Teams[TEAM_TYPE.TEAM_2].Name);
        Team1Text.position = new Vector2(0.22f, 0.18f);

        Team1Text = GUI.Add(MatchSystem.instance.Teams[TEAM_TYPE.TEAM_SPECTATOR].Name);
        Team1Text.position = new Vector2(0.35f, 0.18f);
    }

    public void ClearImagesAndText()
    {
        GUI.Remove(Select);
        GUI.Remove(Team1Image);
        GUI.Remove(Team2Image);
        GUI.Remove(SpectatorImage);
        GUI.Remove(Team1Text);
        GUI.Remove(Team2Text);
        GUI.Remove(SpectatorText);
    }
}
