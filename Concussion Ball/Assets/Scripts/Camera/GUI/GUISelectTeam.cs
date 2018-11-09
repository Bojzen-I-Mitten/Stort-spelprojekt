using System.ComponentModel;
using ThomasEngine;

public class GUISelectTeam : ScriptComponent
{
    public Texture2D SelectBox { get; set; }
    public Font TextFont { get; set; }

    Camera Camera;
    private bool Disabled;

    public Canvas Canvas;

    Image Team1Image;
    Image Team2Image;
    Image SpectatorImage;
    Text Select;
    Text Team1Text;
    Text Team2Text;
    Text SpectatorText;

    public override void Awake()
    {
    }

    public override void Start()
    {
        Disabled = false;
        Camera = gameObject.GetComponent<Camera>();
        AddImagesAndText();
    }

    public override void Update()
    {
        if (Input.GetMouseButtonDown(Input.MouseButtons.LEFT))
        {
            if (Team1Image.Clicked())
            {
                MatchSystem.instance.JoinTeam(TEAM_TYPE.TEAM_1);
                Input.SetMouseMode(Input.MouseMode.POSITION_RELATIVE);
                CameraMaster.instance.State = CAM_STATE.CHAD;
                CameraMaster.instance.Canvas.isRendering = false;
                gameObject.GetComponent<SpectatorCam>().enabled = true;
            }
            else if (Team2Image.Clicked())
            {
                MatchSystem.instance.JoinTeam(TEAM_TYPE.TEAM_2);
                Input.SetMouseMode(Input.MouseMode.POSITION_RELATIVE);
                CameraMaster.instance.State = CAM_STATE.CHAD;
                CameraMaster.instance.Canvas.isRendering = false;
                gameObject.GetComponent<SpectatorCam>().enabled = true;
            }
            else if (SpectatorImage.Clicked())
            {
                MatchSystem.instance.JoinTeam(TEAM_TYPE.TEAM_SPECTATOR);
                Input.SetMouseMode(Input.MouseMode.POSITION_RELATIVE);
                CameraMaster.instance.State = CAM_STATE.SPECTATE;
                CameraMaster.instance.Canvas.isRendering = false;
                gameObject.GetComponent<SpectatorCam>().enabled = true;
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

    public void AddImagesAndText()
    {
        Canvas = Camera.AddCanvas();

        Team1Image = Canvas.Add(SelectBox);
        Team1Image.position = new Vector2(0.13f, 0.23f);
        Team1Image.interactable = true;
        Team1Image.color = MatchSystem.instance.Teams[TEAM_TYPE.TEAM_1].Color;

        Team2Image = Canvas.Add(SelectBox);
        Team2Image.position = new Vector2(0.25f, 0.23f);
        Team2Image.interactable = true;
        Team2Image.color = MatchSystem.instance.Teams[TEAM_TYPE.TEAM_2].Color;

        SpectatorImage = Canvas.Add(SelectBox);
        SpectatorImage.position = new Vector2(0.37f, 0.23f);
        SpectatorImage.interactable = true;
        SpectatorImage.color = MatchSystem.instance.Teams[TEAM_TYPE.TEAM_SPECTATOR].Color;

        Select = Canvas.Add("Select Team");
        Select.position = new Vector2(0.21f, 0.11f);
        Select.color = Color.Black;

        Team1Text = Canvas.Add(MatchSystem.instance.Teams[TEAM_TYPE.TEAM_1].Name);
        Team1Text.position = new Vector2(0.10f, 0.18f);
        Team1Text.color = Color.Black;

        Team2Text = Canvas.Add(MatchSystem.instance.Teams[TEAM_TYPE.TEAM_2].Name);
        Team2Text.position = new Vector2(0.22f, 0.18f);
        Team2Text.color = Color.Black;

        SpectatorText = Canvas.Add(MatchSystem.instance.Teams[TEAM_TYPE.TEAM_SPECTATOR].Name);
        SpectatorText.position = new Vector2(0.35f, 0.18f);
        SpectatorText.color = Color.Black;
    }

    public void ClearImagesAndText()
    {
        Canvas.Remove(Select);
        Canvas.Remove(Team1Image);
        Canvas.Remove(Team2Image);
        Canvas.Remove(SpectatorImage);

        Canvas.Remove(Team1Text);
        Canvas.Remove(Team2Text);
        Canvas.Remove(SpectatorText);
    }
}
