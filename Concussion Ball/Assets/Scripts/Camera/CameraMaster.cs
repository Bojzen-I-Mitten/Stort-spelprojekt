﻿using ThomasEngine;
using System.Configuration;
using System.Collections.Generic;

public enum CAM_STATE
{
    JOIN_HOST,
    SELECT_TEAM,
    GAME,
    MAIN_MENU,
    EXIT_MENU,
    HOST_MENU,
    OPTIONS_MENU,
    LOADING_SCREEN,
    SCORE_SCREEN,
    SCOREBOARD,
    NUMSTATES
}

public class CameraMaster : ScriptComponent
{
    public GameObject ChadMainMenu { get; set; }
    public GameObject ChadTeam1 { get; set; }
    public GameObject ChadTeam2 { get; set; }
    public GameObject ChadPreviewArea { get; set; }
    public LightComponent Light1 { get; set; }
    public LightComponent Light2 { get; set; }

    public static CameraMaster instance;

    public Camera Camera;
    GUIJoinHost JoinHost;
    GUIMainMenu MainMenu;
    GUISelectTeam SelectTeam;
    GUIExitMenu ExitMenu;
    GUIHostMenu HostMenu;
    GUIOptionsMenu OptionsMenu;
    GUILoadingScreen LoadingScreen;
    ChadCam ChadCam;
    SpectatorCam SpectatorCam;
    ChadHud Hud;
    SoundComponent MenuSound;
    ReplayCamera ReplayCam;

    public Canvas Canvas;
    public CAM_STATE State;

    public int SelectedHat;
    Hatter ChadMMHat = null;
    Hatter ChadT1Hat = null;
    Hatter ChadT2Hat = null;

    public override void OnAwake()
    {
        instance = this;
        Camera = gameObject.GetComponent<Camera>();
        Canvas = Camera.AddCanvas();

        MenuSound = gameObject.AddComponent<SoundComponent>();
        MenuSound.Clip = (AudioClip)Resources.LoadThomasPath("%THOMAS_ASSETS%/Sounds/MenuSoundMetal.mp3");
        MenuSound.Looping = false;
        MenuSound.Type = SoundComponent.SoundType.Music;
        MenuSound.Is3D = false;
        MenuSound.Volume = 0.1f;
        MenuSound.enabled = false;
    
        
        if (Light1 != null && Light2 != null)
        {
            Light1.Intensity = 5;
            Light2.Intensity = 0.5f;
        }
    }

    public override void Start()
    {
        #region Init GUI
        if (Camera == null)
            Debug.Log("Camera Master cannot find camera");

        JoinHost = gameObject.GetComponent<GUIJoinHost>();
        if (JoinHost == null)
            Debug.Log("Camera Master cannot find GUI script for join/host");
        MainMenu = gameObject.GetComponent<GUIMainMenu>();
        if (MainMenu == null)
            Debug.Log("Camera master could not find GUI script for main menu");
        SelectTeam = gameObject.GetComponent<GUISelectTeam>();
        if (SelectTeam == null)
            Debug.Log("Camera Master cannot find GUI script for select");

        ExitMenu = gameObject.GetComponent<GUIExitMenu>();
        if (ExitMenu == null)
            Debug.Log("Camera Master cannot find GUI script for exit");

        HostMenu = gameObject.GetComponent<GUIHostMenu>();
        if (HostMenu == null)
            Debug.Log("Camera Master cannot find GUI script for host");

        OptionsMenu = gameObject.GetComponent<GUIOptionsMenu>();
        if (OptionsMenu == null)
            Debug.Log("Camera Master cannot find GUI script for Options");


        LoadingScreen = gameObject.GetComponent<GUILoadingScreen>();
        if (LoadingScreen == null)
            Debug.Log("Camera Maser cannot find GUI script for loading screen");

        ChadCam = gameObject.GetComponent<ChadCam>();
        if (ChadCam == null)
            Debug.Log("Camera Master cannot find ChadCam");
        else
            ChadCam.enabled = false;

        SpectatorCam = gameObject.GetComponent<SpectatorCam>();
        if (SpectatorCam == null)
            Debug.Log("Camera Master cannot find SpectatorCam");
        else
            SpectatorCam.enabled = false;

        Hud = gameObject.GetComponent<ChadHud>();
        if (Hud == null)
            Debug.Log("Camera Master could not find Hud");

        ReplayCam = gameObject.GetComponent<ReplayCamera>();
        ReplayCam.enabled = false;
        #endregion

        #region Chad Hats
        string settingsHat = UserSettings.GetSetting("Hat");

        if (settingsHat != null)
        {
            SelectedHat = System.Convert.ToInt32(settingsHat);
        }
        else
        {
            SelectedHat = (int)(Random.Range(0.0f, 1.0f) * (HatManager.Instance.Hats.Count - 2)) + 1;
            settingsHat = SelectedHat.ToString();
            UserSettings.AddOrUpdateAppSetting("Hat", settingsHat);
        }

        if (ChadMainMenu != null)
        {
            ChadMMHat = ChadMainMenu.GetComponent<Hatter>();
            ChadMMHat.SetHat(SelectedHat);
        }
        if (ChadTeam1 != null)
        {
            ChadT1Hat = ChadTeam1.GetComponent<Hatter>();
            ChadT1Hat.SetHat(SelectedHat);
        }
        if (ChadTeam1 != null)
        {
            ChadT2Hat = ChadTeam2.GetComponent<Hatter>();
            ChadT2Hat.SetHat(SelectedHat);
        }
        #endregion

        SetState(CAM_STATE.MAIN_MENU);
    }

    public void StartReplay()
    {
        ChadCam.enabled = false;
        SpectatorCam.enabled = false;
        ReplayCam.enabled = true;
    }

    public void StopReplay()
    {
        ReplayCam.enabled = false;
        //ChadCam.enabled = true;
        //SpectatorCam.enabled = false;
    }

    public string GetPlayerName()
    {
        return MainMenu != null ? MainMenu.GetPlayerName() : "Chad";
    }

    public void DisableSwitchTeam()
    {
        ExitMenu._CanSwitchTeam = false;
    }

    public void EnableSwitchTeam()
    {
        ExitMenu._CanSwitchTeam = true;
    }

    public void TurnOffLights()
    {
        if (Light1 != null)
        {
            if (Light1.Intensity == 5)
                Light1.Intensity = 0;
        }
        if (Light2 != null)
        {
            if (Light2.Intensity == 0.5f)
                Light2.Intensity = 0;
        }   
    }

    public void TurnOnLights()
    {
        if (Light1 != null)
        {
            if (Light1.Intensity == 0)
                Light1.Intensity = 5;
        }
        if (Light2 != null)
        {
            if (Light2.Intensity == 0)
                Light2.Intensity = 0.5f;
        }
    }

    public override void Update()
    {
        switch (State)
        {
            case CAM_STATE.GAME:
                if (Input.GetKeyDown(Input.Keys.Escape))
                {

                    SetState(CAM_STATE.EXIT_MENU);
                    Input.SetMouseMode(Input.MouseMode.POSITION_ABSOLUTE);
                }
                if (Input.GetMouseMode() == Input.MouseMode.POSITION_ABSOLUTE && Input.GetMouseButtonUp(Input.MouseButtons.LEFT))
                    Input.SetMouseMode(Input.MouseMode.POSITION_RELATIVE);
                break;
            case CAM_STATE.EXIT_MENU:
                if (Input.GetKeyDown(Input.Keys.Escape))
                {
                    SetState(CAM_STATE.GAME);
                    Input.SetMouseMode(Input.MouseMode.POSITION_RELATIVE);
                }
                break;
        }
        UpdateHats();
    }

    void UpdateHats()
    {
        SelectedHat = SelectedHat == -1 ? ChadMMHat.GetHatCount()-1 : SelectedHat;
        SelectedHat %= ChadMMHat.GetHatCount();

        ChadMMHat.SetHat(SelectedHat);
        ChadT1Hat.SetHat(SelectedHat);
        ChadT2Hat.SetHat(SelectedHat);
    }

    public void SetState(CAM_STATE newState)
    {
        ClearIsRendering();
        State = newState;
        UpdateState();
    }

    public CAM_STATE GetState()
    {
        return State;
    }

    void ClearIsRendering()
    {
        if (MainMenu.Canvas != null)
            MainMenu.Canvas.isRendering = false;
        if (JoinHost.Canvas != null)
            JoinHost.Canvas.isRendering = false;
        if (SelectTeam.Canvas != null)
            SelectTeam.Canvas.isRendering = false;
        if (Hud.Canvas != null)
            Hud.Canvas.isRendering = false;
        if (ExitMenu.Canvas != null)
            ExitMenu.Canvas.isRendering = false;
        if (HostMenu.Canvas != null)
            HostMenu.Canvas.isRendering = false;
        if (LoadingScreen.Canvas != null)
            LoadingScreen.Canvas.isRendering = false;
        if (GUIScoreScreen.Instance)
        {
            GUIScoreScreen.Instance.enabled = false;
            GUIScoreScreen.Instance.Canvas.isRendering = false;
        }
        if (GUIScoreboard.Instance)
        {
            GUIScoreboard.Instance.enabled = false;
            GUIScoreboard.Instance.Canvas.isRendering = false;
        }
        if(OptionsMenu)
            OptionsMenu.Canvas.isRendering = false;
    }

    private void UpdateState()
    {
        switch (State)
        {
            case CAM_STATE.MAIN_MENU:
                TurnOnLights();
                if (!MenuSound.IsPlaying())
                    MenuSound.Play();
                MainMenu.SetUpScene();
                Camera.fixedAspectRatio = true;
                Camera.orthographic = true;
                MainMenu.Canvas.isRendering = true;
                break;

            case CAM_STATE.JOIN_HOST:
                TurnOnLights();
                if (!MenuSound.IsPlaying())
                    MenuSound.Play();
                MainMenu.SetUpScene();
                Camera.fixedAspectRatio = true;
                Camera.orthographic = true;
                JoinHost.Canvas.isRendering = true;
                break;
            case CAM_STATE.SELECT_TEAM:
                TurnOnLights();
                if (!MenuSound.IsPlaying())
                    MenuSound.Play();
                SelectTeam.SetUpScene();
                SelectTeam.Canvas.isRendering = true;
                Camera.fixedAspectRatio = false;
                Camera.orthographic = false;
                break;
            case CAM_STATE.GAME:
                if (MenuSound.IsPlaying())
                    MenuSound.Stop();
                TurnOffLights();
                Hud.Canvas.isRendering = true;
                if (GUIScoreboard.Instance)
                    GUIScoreboard.Instance.enabled = true;
                break;
            case CAM_STATE.EXIT_MENU:
                TurnOffLights();
                ExitMenu.Canvas.isRendering = true;
                break;
            case CAM_STATE.HOST_MENU:
                if (!MenuSound.IsPlaying())
                    MenuSound.Play();
                TurnOnLights();
                HostMenu.SetUpScene();
                Camera.fixedAspectRatio = false;
                Camera.orthographic = false;
                HostMenu.Canvas.isRendering = true;
                break;
            case CAM_STATE.OPTIONS_MENU:
                OptionsMenu.Canvas.isRendering = true;
                break;
            case CAM_STATE.LOADING_SCREEN:
                TurnOffLights();
                LoadingScreen.Canvas.isRendering = true;
                break;
            case CAM_STATE.SCORE_SCREEN:
                if (GUIScoreScreen.Instance)
                {
                    GUIScoreScreen.Instance.enabled = true;
                    GUIScoreScreen.Instance.Canvas.isRendering = true; 
                }
                break;
        }
    }
}
