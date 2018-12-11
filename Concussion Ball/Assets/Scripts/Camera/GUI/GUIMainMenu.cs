using System.Collections;
using ThomasEngine;

public class GUIMainMenu : ScriptComponent
{
    public Texture2D TextBox { get; set; }
    public Texture2D TextBoxBG { get; set; }
    public Texture2D MyNameTexture { get; set; }
    public Texture2D ArrowTexture { get; set; }
    public Texture2D FrameTexture { get; set; }
    public Font TextFont { get; set; }

    Camera Camera;

    public Canvas Canvas;

    Image MyNameSticker;
    Image SelectHatRight;
    Image SelectHatLeft;
    Image ChadArea;
    Text Play;
    Text HostGame;
    Text Options;
    Text Exit;
    Text Credits;
    Text PlayerName;
    Text RandomHat;
    Text Caret;

    IEnumerator Blink = null;

    private bool TakeName;

    public float CaretOffset { get; set; }
    public float NameRotation { get; set; }
    private bool ClearName = true;
    bool _RotateChad;
    float rotationSpeed;

    Color Unselected = Color.FloralWhite;
    Color Selected = Color.IndianRed;
    Vector3 MainMenuCamPos;
    Vector3 MainMenuAspectRation;
    Vector3 MainMenuCamRot;

    

    public override void OnAwake()
    {
        CaretOffset = -0.02f;
        NameRotation = -0.1f;
        Camera = gameObject.GetComponent<Camera>();
        TakeName = false;
        AddImagesAndText();
        MainMenuCamPos = new Vector3(0, -195.442f, -7.084f);
        MainMenuAspectRation = new Vector3(16, 9, 1);
        MainMenuCamRot = Vector3.Zero;
    }

    public override void Update()
    {
        if (Canvas.isRendering)
        {
            Play.color = Unselected;
            HostGame.color = Unselected;
            Options.color = Unselected;
            Credits.color = Unselected;
            Exit.color = Unselected;
            RandomHat.color = Unselected;

            if (Play.Hovered())
                Play.color = Selected;
            else if (HostGame.Hovered())
                HostGame.color = Selected;
            else if (Options.Hovered())
                Options.color = Selected;
            else if (Credits.Hovered())
                Credits.color = Selected;
            else if (Exit.Hovered())
                Exit.color = Selected;
            else if (RandomHat.Hovered())
                RandomHat.color = Selected;

            if (Input.GetMouseButtonDown(Input.MouseButtons.LEFT))
            {
                if (ChadArea.Hovered())
                {
                    Input.SetMouseMode(Input.MouseMode.POSITION_RELATIVE);
                    _RotateChad = true;
                }
            }

            if (MyNameSticker.Clicked())
            {
                TakeName = true;
                if (Blink == null)
                {
                    Blink = CaretBlink();
                    StartCoroutine(Blink);
                }
                if (ClearName)
                {
                    PlayerName.text = "";
                    ClearName = false;
                }
            }
            else if (Input.GetMouseButtonUp(Input.MouseButtons.LEFT))
            {
                Input.SetMouseMode(Input.MouseMode.POSITION_ABSOLUTE);
                _RotateChad = false;
                TakeName = false;
                if (Blink != null)
                {
                    StopCoroutine(Blink);
                    Blink = null;
                }
                Caret.text = "";
            }

            #region Clicked
            if (Play.Clicked() && PlayerName.text != "")
            {
                CameraMaster.instance.SetState(CAM_STATE.JOIN_HOST);
                TakeName = false;
                UserSettings.AddOrUpdateAppSetting("Hat", CameraMaster.instance.SelectedHat.ToString());
                UserSettings.AddOrUpdateAppSetting("PlayerName", PlayerName.text);
            }
            else if (HostGame.Clicked())
            {
                CameraMaster.instance.SetState(CAM_STATE.HOST_MENU);
                TakeName = false;
                UserSettings.AddOrUpdateAppSetting("Hat", CameraMaster.instance.SelectedHat.ToString());
                UserSettings.AddOrUpdateAppSetting("PlayerName", PlayerName.text);
            }
            else if (Exit.Clicked())
            {
                ThomasWrapper.IssueShutdown();
            }
            else if (SelectHatRight.Clicked())
            {
                CameraMaster.instance.SelectedHat += 1;
            }
            else if (SelectHatLeft.Clicked())
            {
                CameraMaster.instance.SelectedHat -= 1;
            }
            else if (RandomHat.Clicked())
            {
                CameraMaster.instance.SelectedHat = (int)(Random.Range(0.0f, 1.0f) * (HatManager.Instance.Hats.Count - 2)) + 1;
            }
            #endregion

            if (TakeName)
            {
                string playerString = PlayerName.text;
                GUIInput.AppendString(ref playerString, 9);
                PlayerName.text = playerString;
            }
            RotateChad();

            Caret.position = PlayerName.position + new Vector2(PlayerName.size.x / 2 - 0.005f, CaretOffset + PlayerName.size.x * NameRotation);
        }
    }


    public string GetPlayerName()
    {
        return PlayerName.text;
    }

    public void AddImagesAndText()
    {
        Canvas = Camera.AddCanvas();

        #region Text
        #region  Play
        Play = Canvas.Add("Join Game");
        Play.position = new Vector2(0.1f, 0.11f);
        Play.interactable = true;
        Play.depth = 0.9f;
        #endregion

        #region Host Game
        HostGame = Canvas.Add("Host Game");
        HostGame.position = new Vector2(0.1f, 0.21f);
        HostGame.interactable = true;
        HostGame.depth = 0.9f;
        #endregion

        #region  Options
        Options = Canvas.Add("Options");
        Options.position = new Vector2(0.1f, 0.31f);
        Options.interactable = true;
        Options.depth = 0.9f;
        #endregion

        #region Credits
        Credits = Canvas.Add("Credits");
        Credits.position = new Vector2(0.1f, 0.41f);
        Credits.interactable = true;
        Credits.depth = 0.9f;
        #endregion

        #region Exit
        Exit = Canvas.Add("Exit");
        Exit.position = new Vector2(0.1f, 0.51f);
        Exit.interactable = true;
        Exit.depth = 0.9f;
        #endregion

        #region Player name

        string playerString =  UserSettings.GetSetting("PlayerName");
        if (playerString == null)
            playerString = "CHAD";

        PlayerName = Canvas.Add(playerString);
        PlayerName.origin = new Vector2(0.5f);
        PlayerName.position = new Vector2(0.55f, 0.17f);
        PlayerName.scale = new Vector2(0.9f);
        PlayerName.interactable = true;
        PlayerName.depth = 0.8f;
        PlayerName.rotation = NameRotation;
        PlayerName.color = Color.Black;
        PlayerName.font = TextFont;
        #endregion

        #region Random Hat
        RandomHat = Canvas.Add("Random Hat");
        RandomHat.origin = new Vector2(0.5f);
        RandomHat.scale = new Vector2(0.6f);
        RandomHat.position = new Vector2(0.76f, 0.17f);
        RandomHat.interactable = true;
        RandomHat.depth = 0.8f;
        #endregion

        #region Caret
        Caret = Canvas.Add("");
        Caret.origin = new Vector2(0, 0.5f);
        Caret.scale = new Vector2(1.2f);
        Caret.interactable = false;
        Caret.depth = 0.8f;
        Caret.rotation = NameRotation;
        Caret.color = Color.Black;
        Caret.font = TextFont;
        #endregion
        #endregion

        #region Images
        #region NameSticker
        if (MyNameTexture != null)
        {
            MyNameSticker = Canvas.Add(MyNameTexture);
            MyNameSticker.origin = new Vector2(0.5f);
            MyNameSticker.position = new Vector2(0.55f, 0.14f);
            MyNameSticker.scale = new Vector2(0.5f);
            MyNameSticker.interactable = true;
            MyNameSticker.depth = 0.9f;
        }
        #endregion

        #region Arrows
        if (ArrowTexture != null)
        {
            SelectHatRight = Canvas.Add(ArrowTexture);
            SelectHatRight.origin = new Vector2(0.5f);
            SelectHatRight.scale = new Vector2(0.25f);
            SelectHatRight.position = new Vector2(0.85f, 0.25f);
            SelectHatRight.interactable = true;
            SelectHatRight.depth = 0.9f;

            SelectHatLeft = Canvas.Add(ArrowTexture);
            SelectHatLeft.origin = new Vector2(0.5f);
            SelectHatLeft.scale = new Vector2(0.25f);
            SelectHatLeft.position = new Vector2(0.68f, 0.25f);
            SelectHatLeft.rotation = (float)System.Math.PI;
            SelectHatLeft.interactable = true;
            SelectHatLeft.depth = 0.9f;
        }
        #endregion

        #region Chad Area
        if (FrameTexture != null)
        {
            ChadArea = Canvas.Add(FrameTexture);
            ChadArea.position = new Vector2(0.62f, 0.32f);
            ChadArea.scale = new Vector2(5, 6);
            ChadArea.interactable = true;
            ChadArea.color = new Color(1, 1, 1, 0);
            ChadArea.depth = 0.9f;
        }
        #endregion

        #endregion
    }

    public void SetUpScene()
    {
        transform.position = MainMenuCamPos;
        transform.scale = MainMenuAspectRation;
        transform.rotation = Quaternion.CreateFromYawPitchRoll(MainMenuCamRot.x, MainMenuCamRot.y, MainMenuCamRot.z);
    }

    public void ClearImagesAndText()
    {
        Canvas.Remove(Play);
        Canvas.Remove(HostGame);
        Canvas.Remove(Options);
        Canvas.Remove(Credits);
        Canvas.Remove(Exit);
        Canvas.Remove(PlayerName);
        Canvas.Remove(Caret);
        Canvas.Remove(SelectHatRight);
        Canvas.Remove(SelectHatLeft);
    }

    IEnumerator CaretBlink()
    {
        bool underscore = true;
        while (true)
        {
            if (underscore)
            {
                Caret.text = "|";
                underscore = false;
            }
            else
            {
                Caret.text = "";
                underscore = true;
            }

            yield return new WaitForSecondsRealtime(0.5f);
        }
    }

    public void RotateChad()
    {
        if (_RotateChad)
        {
            if(Input.GetMouseX() < 500)
                rotationSpeed += Input.GetMouseX() * 0.1f * Time.DeltaTime;
        }
        else
        {
            rotationSpeed = MathHelper.Lerp(rotationSpeed, 0, Time.DeltaTime);
        }
        
        CameraMaster.instance.ChadMainMenu.transform.RotateByAxis(Vector3.Up, rotationSpeed);
        
    }
}
