using System.ComponentModel;
using ThomasEngine;

public class GUISelectTeam : ScriptComponent
{
    public Texture2D SelectBox { get; set; }
    public Texture2D SpectatorCamTexture { get; set; }
    public Texture2D ExitTexture { get; set; }
    public Font TextFont { get; set; }

    public GameObject ChadTeam1 { get; set; }
    public GameObject ChadTeam2 { get; set; }

    public Animation RunningAnim { get; set; }
    public Animation IdleAnim { get; set; }

    Camera Camera;

    //public Vector2 SelectboxPos { get; set; } = new Vector2(0.37f, 0.33f);
    //public Vector2 SelectboxScale { get; set; } = new Vector2(0.37f, 0.33f);

    public Canvas Canvas;

    Image ExitImage;
    Image Team1Image;
    Image Team2Image;
    Image SpectatorImage;
    Text Select;
    Text Team1Text;
    Text Team2Text;
    Text SpectatorText;
    Text ExitText;

    RenderSkinnedComponent ChadRSC1;
    RenderSkinnedComponent ChadRSC2;
    Material Chad1Mat;
    Material Chad2Mat;

    Color Unselected = Color.FloralWhite;
    Color Selected = Color.IndianRed;

    public override void OnAwake()
    {
    }

    public override void Start()
    {
        Camera = gameObject.GetComponent<Camera>();
        AddImagesAndText();
        if (ChadTeam1 != null)
        {
            ChadRSC1 = ChadTeam1.GetComponent<RenderSkinnedComponent>();
            Chad1Mat = ChadRSC1.material = new Material(ChadRSC1.material);
        }
        if (ChadTeam2 != null)
        {
            ChadRSC2 = ChadTeam2.GetComponent<RenderSkinnedComponent>();
            Chad2Mat = ChadRSC2.material = new Material(ChadRSC2.material);
        }


    }

    public override void Update()
    {
        Team1Text.text = MatchSystem.instance.Teams[TEAM_TYPE.TEAM_1].Name;
        Team2Text.text = MatchSystem.instance.Teams[TEAM_TYPE.TEAM_2].Name;
        Chad1Mat?.SetColor("color", MatchSystem.instance.Teams[TEAM_TYPE.TEAM_1].Color);
        Chad2Mat?.SetColor("color", MatchSystem.instance.Teams[TEAM_TYPE.TEAM_2].Color);

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

        Team1Text.color = Unselected;
        Team2Text.color = Unselected;
        SpectatorText.color = Unselected;
        ExitText.color = Unselected;

        if (Team1Image.Hovered())
        {
            Team1Text.color = Selected;
            if (RunningAnim != null)
            {
                if (ChadRSC1.animation != RunningAnim)
                {
                    ChadRSC1.animation = RunningAnim;
                    ChadRSC2.animation = IdleAnim;
                }
            }
        }
        else if (Team2Image.Hovered())
        {
            Team2Text.color = Selected;
            if (RunningAnim != null)
            {
                if (ChadRSC2.animation != RunningAnim)
                {
                    ChadRSC1.animation = IdleAnim;
                    ChadRSC2.animation = RunningAnim;
                }
            }
        }
        else if (SpectatorImage.Hovered())
        {
            SpectatorText.color = Selected;
            IdleChads();
        }
        else if (ExitText.Hovered())
        {
            ExitText.color = Color.Black;
            
            if (ExitText.Clicked())
            {
                CameraMaster.instance.State = CAM_STATE.MAIN_MENU;
            }
        }
        else
        {
            IdleChads();
        }

        if (TextFont != null)
        {
            Select.font = TextFont;
            Team1Text.font = TextFont;
            Team2Text.font = TextFont;
            SpectatorText.font = TextFont;
            ExitText.font = TextFont;
        }

        //Team1Image.position = SelectboxPos;
        //Team1Image.scale = SelectboxScale;

    }

    public void AddImagesAndText()
    {
        Canvas = Camera.AddCanvas();
        if (SelectBox != null)
        {
            Team1Image = Canvas.Add(SelectBox);
            Team1Image.position = new Vector2(0.235f, 0.58f);
            Team1Image.scale = new Vector2(9, 9);
            Team1Image.origin = new Vector2(0.5f);
            Team1Image.color = new Color(1, 1, 1, 1);
            Team1Image.interactable = true;

            Team2Image = Canvas.Add(SelectBox);
            Team2Image.position = new Vector2(0.777f, 0.58f);
            Team2Image.scale = new Vector2(9, 9);
            Team2Image.origin = new Vector2(0.5f);
            Team2Image.color = new Color(1, 1, 1, 1);
            Team2Image.interactable = true;
        }

        if (SpectatorCamTexture != null)
        {
            SpectatorImage = Canvas.Add(SpectatorCamTexture);
            SpectatorImage.position = new Vector2(0.5f, 0.88f);
            SpectatorImage.origin = new Vector2(0.5f);
            SpectatorImage.interactable = true;
        }

        if(ExitTexture != null)
        {
            ExitImage = Canvas.Add(ExitTexture);
            ExitImage.position = new Vector2(0.05f, 0.8f);
            ExitImage.origin = new Vector2(0.5f);
        }

        Select = Canvas.Add("Select Team");
        Select.position = new Vector2(0.5f, 0.1f);
        Select.origin = new Vector2(0.5f);
        Select.color = Unselected;

        Team1Text = Canvas.Add(MatchSystem.instance.Teams[TEAM_TYPE.TEAM_1].Name);
        Team1Text.position = new Vector2(0.235f, 0.25f);
        Team1Text.origin = new Vector2(0.5f);
        Team1Text.color = Unselected;

        Team2Text = Canvas.Add(MatchSystem.instance.Teams[TEAM_TYPE.TEAM_2].Name);
        Team2Text.position = new Vector2(0.777f, 0.25f);
        Team2Text.origin = new Vector2(0.5f);
        Team2Text.color = Unselected;

        SpectatorText = Canvas.Add(MatchSystem.instance.Teams[TEAM_TYPE.TEAM_SPECTATOR].Name);
        SpectatorText.position = new Vector2(0.5f, 0.8f);
        SpectatorText.origin = new Vector2(0.5f);
        SpectatorText.color = Unselected;

        ExitText = Canvas.Add("Exit");
        ExitText.position = new Vector2(0.05f, 0.85f);
        ExitText.origin = new Vector2(0.5f);
        ExitText.color = Unselected;
        ExitText.interactable = true;
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
        Canvas.Remove(ExitText);
        Canvas.Remove(ExitImage);
    }

    private void IdleChads()
    {
        if (IdleAnim != null)
        {
            ChadRSC1.animation = IdleAnim;
            ChadRSC2.animation = IdleAnim;
        }
    }
}
