using System.Collections.Generic;
using System.ComponentModel;
using ThomasEngine;

public class GUISelectTeam : ScriptComponent
{
    public Texture2D SelectBox { get; set; }
    public Texture2D SpectatorCamTexture { get; set; }
    public Texture2D TeamListTexture { get; set; }
    public Font ChadFont { get; set; }
    public Font SportFont32 { get; set; }

    //public GameObject ChadTeam1 { get; set; }
    //public GameObject ChadTeam2 { get; set; }

    public Animation RunningAnim { get; set; }
    public Animation IdleAnim { get; set; }

    Camera Camera;

    public Vector2 Team1ListPos { get; set; } = new Vector2(0, 0);
    public Vector2 Team1ListScale { get; set; } = new Vector2(0, 0);
    public Vector2 Team2ListPos { get; set; } = new Vector2(0, 0);
    public Vector2 Team2ListScale { get; set; } = new Vector2(0, 0);
    public Canvas Canvas;

    Image Team1Image;
    Image Team2Image;
    Image Team1List;
    Image Team2List;
    Image SpectatorImage;
    Text Select;
    Text Team1Text;
    Text Team2Text;
    Text PlayersInTeam1;
    Text PlayersInTeam2;
    Text SpectatorText;
    Text ReadyUp;
    Text StartGame;

    List<Text> Team1Players = new List<Text>();
    List<Text> Team2Players = new List<Text>();

    RenderSkinnedComponent ChadRSC1;
    RenderSkinnedComponent ChadRSC2;
    Material Chad1Mat;
    Material Chad2Mat;

    Color Unselected = Color.FloralWhite;
    Color Selected = Color.IndianRed;
    Vector3 SelectTeamCamPos;
    Vector3 SelectTeamCamRot;
    Vector3 Chad1Pos;
    Vector3 Chad1Rot;
    Vector3 Chad2Pos;
    Vector3 Chad2Rot;

    public override void Start()
    {
        Camera = gameObject.GetComponent<Camera>();
        AddImagesAndText();
        SelectTeamCamPos = new Vector3(0, -198.5f, 8.2f);
        SelectTeamCamRot = new Vector3(MathHelper.Pi, 0.0f, 0.0f);
        Chad1Pos = new Vector3(1.7f, 0, 0);
        Chad1Rot = new Vector3(45, 0, 0);
        Chad2Pos = new Vector3(-1.7f, 0, 0);
        Chad2Rot = new Vector3(-20, 0, 0);
        
        if (CameraMaster.instance.ChadTeam1 != null)
        {
            ChadRSC1 = CameraMaster.instance.ChadTeam1.GetComponent<RenderSkinnedComponent>();
            Chad1Mat = ChadRSC1.material = new Material(ChadRSC1.material);
        }
        if (CameraMaster.instance.ChadTeam2 != null)
        {
            ChadRSC2 = CameraMaster.instance.ChadTeam2.GetComponent<RenderSkinnedComponent>();
            Chad2Mat = ChadRSC2.material = new Material(ChadRSC2.material);
        }


    }

    public override void Update()
    {
        Team1Text.text = MatchSystem.instance.Teams[TEAM_TYPE.TEAM_1].Name;
        Team2Text.text = MatchSystem.instance.Teams[TEAM_TYPE.TEAM_2].Name;
        Chad1Mat?.SetColor("color", MatchSystem.instance.Teams[TEAM_TYPE.TEAM_1].Color);
        Chad2Mat?.SetColor("color", MatchSystem.instance.Teams[TEAM_TYPE.TEAM_2].Color);

        ShowPlayers();

        if (Team1Image.Clicked())
        {
            MatchSystem.instance.JoinTeam(TEAM_TYPE.TEAM_1);
            MatchSystem.instance.LocalChad.NetPlayer.Ready(false);
            ReadyUp.scale = Vector2.One;
        }
        else if (Team2Image.Clicked())
        {
            MatchSystem.instance.JoinTeam(TEAM_TYPE.TEAM_2);
            MatchSystem.instance.LocalChad.NetPlayer.Ready(false);
            ReadyUp.scale = Vector2.One;
        }
        else if (SpectatorImage.Clicked())
        {
            MatchSystem.instance.JoinTeam(TEAM_TYPE.TEAM_SPECTATOR);
            MatchSystem.instance.LocalChad.NetPlayer.Ready(true);
            ReadyUp.scale = Vector2.Zero;
        }
        else if (ReadyUp.Clicked())
        {
            Debug.Log("Ready up clicked!!");
            MatchSystem.instance.LocalChad.NetPlayer.Ready(true);
        }
        else if (StartGame.Clicked())
        {
            Input.SetMouseMode(Input.MouseMode.POSITION_RELATIVE);
            CameraMaster.instance.State = CAM_STATE.GAME;
            CameraMaster.instance.Canvas.isRendering = false;
            gameObject.GetComponent<SpectatorCam>().enabled = true;
        }

        Team1Text.color = Unselected;
        Team2Text.color = Unselected;
        SpectatorText.color = Unselected;
        ReadyUp.color = Unselected;
        StartGame.color = Unselected;

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
        else if (SpectatorImage.Hovered() || SpectatorText.Hovered())
        {
            SpectatorText.color = Selected;
            IdleChads();
        }
        else if (ReadyUp.Hovered() && ReadyUp.scale != Vector2.Zero)
        {
            ReadyUp.color = Selected;
        }
        else if (StartGame.Hovered() && CheckReadyPlayers())
        {
            StartGame.color = Selected;
        }
        else
        {
            IdleChads();
        }

        if (CheckReadyPlayers())
        {
            StartGame.scale = Vector2.One;
            StartGame.interactable = true;
        }
        else
        {
            StartGame.scale = Vector2.Zero;
            StartGame.interactable = false;
        }

        Team1List.position = Team1ListPos;
        Team1List.scale = Team1ListScale;
        Team2List.position = Team2ListPos;
        Team2List.scale = Team2ListScale;

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

        Select = Canvas.Add("Select Team");
        Select.position = new Vector2(0.5f, 0.1f);
        Select.origin = new Vector2(0.5f);
        Select.color = Unselected;

        Team1Text = Canvas.Add(MatchSystem.instance.Teams[TEAM_TYPE.TEAM_1].Name);
        Team1Text.position = new Vector2(0.235f, 0.25f);
        Team1Text.origin = new Vector2(0.5f);
        Team1Text.font = ChadFont;
        Team1Text.color = Unselected;

        Team2Text = Canvas.Add(MatchSystem.instance.Teams[TEAM_TYPE.TEAM_2].Name);
        Team2Text.position = new Vector2(0.777f, 0.25f);
        Team2Text.origin = new Vector2(0.5f);
        Team2Text.font = ChadFont;
        Team2Text.color = Unselected;

        SpectatorText = Canvas.Add(MatchSystem.instance.Teams[TEAM_TYPE.TEAM_SPECTATOR].Name);
        SpectatorText.position = new Vector2(0.5f, 0.8f);
        SpectatorText.origin = new Vector2(0.5f);
        SpectatorText.font = SportFont32;
        SpectatorText.interactable = true;
        SpectatorText.color = Unselected;

        ReadyUp = Canvas.Add("Ready Up!");
        ReadyUp.position = new Vector2(0.5f, 0.25f);
        ReadyUp.origin = new Vector2(0.5f);
        ReadyUp.font = SportFont32;
        ReadyUp.scale = Vector2.Zero;
        ReadyUp.interactable = true;
        ReadyUp.color = Unselected;

        StartGame = Canvas.Add("Start Game");
        StartGame.position = new Vector2(0.5f, 0.35f);
        StartGame.origin = new Vector2(0.5f);
        StartGame.scale = Vector2.Zero;
        StartGame.font = SportFont32;
        StartGame.interactable = true;
        StartGame.color = Unselected;

        PlayersInTeam1 = Canvas.Add(MatchSystem.instance.Teams[TEAM_TYPE.TEAM_1].PlayerCount + " out of " + MatchSystem.instance.MaxPlayers / 2);
        PlayersInTeam1.position = new Vector2(0, 0.05f);
        PlayersInTeam1.color = Unselected;

        PlayersInTeam2 = Canvas.Add(MatchSystem.instance.Teams[TEAM_TYPE.TEAM_2].PlayerCount + " out of " + MatchSystem.instance.MaxPlayers / 2);
        PlayersInTeam2.origin = new Vector2(1, 0);
        PlayersInTeam2.position = new Vector2(1, 0.05f);
        PlayersInTeam2.color = Unselected;

        if (TeamListTexture != null)
        {
            Team1List = Canvas.Add(TeamListTexture);
            Team1List.color = new Color(0.5f, 0.5f, 0.5f, 0.5f);

            Team2List = Canvas.Add(TeamListTexture);
            Team2List.color = new Color(0.5f, 0.5f, 0.5f, 0.5f);
        }

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
        DeletePlayersLists();
    }

    private void IdleChads()
    {
        if (IdleAnim != null)
        {
            ChadRSC1.animation = IdleAnim;
            ChadRSC2.animation = IdleAnim;
        }
    }

    public void SetUpScene()
    {
        if (CameraMaster.instance.ChadTeam1 != null)
        {
            CameraMaster.instance.ChadTeam1.transform.localPosition = Chad1Pos;
            CameraMaster.instance.ChadTeam1.transform.localRotation = Quaternion.CreateFromYawPitchRoll(Chad1Rot.x * MathHelper.Pi / 180, 0, 0);
        }
        if (CameraMaster.instance.ChadTeam2 != null)
        {
            CameraMaster.instance.ChadTeam2.transform.localPosition = Chad2Pos;
            CameraMaster.instance.ChadTeam2.transform.localRotation = Quaternion.CreateFromYawPitchRoll(Chad2Rot.x * MathHelper.Pi / 180, 0, 0);
        }
        transform.position = SelectTeamCamPos;
        transform.rotation = Quaternion.CreateFromYawPitchRoll(SelectTeamCamRot.x, SelectTeamCamRot.y, SelectTeamCamRot.z);
    }

    private void ShowPlayers()
    {
        List<NetworkPlayer> team1 = MatchSystem.instance.Teams[TEAM_TYPE.TEAM_1].Players;
        List<NetworkPlayer> team2 = MatchSystem.instance.Teams[TEAM_TYPE.TEAM_2].Players;

        DeletePlayersLists();
        int i = 0;
        foreach (NetworkPlayer player in team1)
        {
            Text p = Canvas.Add(player.PlayerName);
            p.position = new Vector2(0.0f, 0.15f + 0.15f * i);
            p.font = SportFont32;
            if (player.ReadyToStart)
                p.color = Color.Green;
            i++;
            Team1Players.Add(p);
        }
        i = 0;
        foreach (NetworkPlayer player in team2)
        {
            Text p = Canvas.Add(player.PlayerName);
            p.origin = new Vector2(1, 0);
            p.position = new Vector2(1.0f, 0.15f + 0.15f * i);
            p.font = SportFont32;
            if (player.ReadyToStart)
                p.color = Color.Green;
            i++;
            Team2Players.Add(p);
        }

        PlayersInTeam1.text = MatchSystem.instance.Teams[TEAM_TYPE.TEAM_1].PlayerCount + " out of " + MatchSystem.instance.MaxPlayers / 2;
        PlayersInTeam2.text = MatchSystem.instance.Teams[TEAM_TYPE.TEAM_2].PlayerCount + " out of " + MatchSystem.instance.MaxPlayers / 2;
    }

    private void DeletePlayersLists()
    {
        foreach (Text t in Team1Players)
            Canvas.Remove(t);
        foreach (Text t in Team2Players)
            Canvas.Remove(t);
    }

    private bool CheckReadyPlayers()
    {
        int ready = 0;
        int players = 0;
        foreach (var player in MatchSystem.instance.Scene.Players)
        {
            NetworkPlayer np = player.Value.gameObject?.GetComponent<NetworkPlayer>();
            players++;
            if (np.ReadyToStart)
                ready++;
        }

        if (ready < players && players > 0)
            return false;
        return true;
    }
}
