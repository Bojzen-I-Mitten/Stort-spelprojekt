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

    public override void Start()
    {
        Disabled = false;
        Camera = gameObject.GetComponent<Camera>();
        AddImagesAndText();
    }

    public override void Update()
    {
        if (Canvas.isRendering)
        {
            Team1Image.color = MatchSystem.instance.Teams[TEAM_TYPE.TEAM_1].Color;
            Team1Text.text = MatchSystem.instance.Teams[TEAM_TYPE.TEAM_1].Name;
            Team2Image.color = MatchSystem.instance.Teams[TEAM_TYPE.TEAM_2].Color;
            Team2Text.text = MatchSystem.instance.Teams[TEAM_TYPE.TEAM_2].Name;

            if (Input.GetMouseButtonUp(Input.MouseButtons.LEFT))
            {
                if (Team1Image.Clicked())
                {
                    MatchSystem.instance.JoinTeam(TEAM_TYPE.TEAM_1);
                    Input.SetMouseMode(Input.MouseMode.POSITION_RELATIVE);
                    CameraMaster.instance.State = CAM_STATE.GAME;
                    CameraMaster.instance.Canvas.isRendering = false;
                    gameObject.GetComponent<SpectatorCam>().enabled = true;
                }
                else if (Team2Image.Clicked())
                {
                    MatchSystem.instance.JoinTeam(TEAM_TYPE.TEAM_2);
                    Input.SetMouseMode(Input.MouseMode.POSITION_RELATIVE);
                    CameraMaster.instance.State = CAM_STATE.GAME;
                    CameraMaster.instance.Canvas.isRendering = false;
                    gameObject.GetComponent<SpectatorCam>().enabled = true;
                }
                else if (SpectatorImage.Clicked())
                {
                    MatchSystem.instance.JoinTeam(TEAM_TYPE.TEAM_SPECTATOR);
                    Input.SetMouseMode(Input.MouseMode.POSITION_RELATIVE);
                    CameraMaster.instance.State = CAM_STATE.GAME;
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
    }

    public void AddImagesAndText()
    {
        Canvas = Camera.AddCanvas();

        Team1Image = Canvas.Add(SelectBox);
        Team1Image.position = new Vector2(0.37f, 0.33f);
        Team1Image.origin = new Vector2(0.5f);
        Team1Image.interactable = true;
        Team1Image.color = MatchSystem.instance.Teams[TEAM_TYPE.TEAM_1].Color;

        Team2Image = Canvas.Add(SelectBox);
        Team2Image.position = new Vector2(0.67f, 0.33f);
        Team2Image.origin = new Vector2(0.5f);
        Team2Image.interactable = true;
        Team2Image.color = MatchSystem.instance.Teams[TEAM_TYPE.TEAM_2].Color;

        SpectatorImage = Canvas.Add(SelectBox);
        SpectatorImage.position = new Vector2(0.85f, 0.23f);
        SpectatorImage.origin = new Vector2(0.5f);
        SpectatorImage.interactable = true;
        SpectatorImage.color = MatchSystem.instance.Teams[TEAM_TYPE.TEAM_SPECTATOR].Color;

        Select = Canvas.Add("Select Team");
        Select.position = new Vector2(0.5f, 0.1f);
        Select.origin = new Vector2(0.5f);
        Select.color = Color.FloralWhite;

        Team1Text = Canvas.Add(MatchSystem.instance.Teams[TEAM_TYPE.TEAM_1].Name);
        Team1Text.position = new Vector2(0.36f, 0.25f);
        Team1Text.origin = new Vector2(0.5f);
        Team1Text.color = Color.FloralWhite;

        Team2Text = Canvas.Add(MatchSystem.instance.Teams[TEAM_TYPE.TEAM_2].Name);
        Team2Text.position = new Vector2(0.66f, 0.25f);
        Team2Text.origin = new Vector2(0.5f);
        Team2Text.color = Color.FloralWhite;

        SpectatorText = Canvas.Add(MatchSystem.instance.Teams[TEAM_TYPE.TEAM_SPECTATOR].Name);
        SpectatorText.position = new Vector2(0.85f, 0.15f);
        SpectatorText.origin = new Vector2(0.5f);
        SpectatorText.color = Color.FloralWhite;
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
