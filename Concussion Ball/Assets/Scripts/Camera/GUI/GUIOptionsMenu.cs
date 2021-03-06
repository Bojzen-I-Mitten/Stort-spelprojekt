﻿using System;
using System.Collections;
using ThomasEngine;
using System.Collections.Generic;
public class ImageBaradjustment
{
    Image[] Image = new Image[(int)Imagestate.NUMSTATES];
    Text number;
    public int numbervalue = 0;
    public float LastMousePosition = 0;
    public Canvas Canvas;
    public bool fullscreen;
    public bool borderless;
    public enum Imagestate
    {
        MUSIC_VOLUME_IMAGE,
        MUSIC_VOLUME_TOGGLE_IMAGE,
        MUSIC_VOLUME_TOGGLE_AFTERMATH_IMAGE,
        NUMSTATES
    }

    public Imagestate ImageBarstate;
    public ImageBaradjustment(Vector2 input, Canvas Canvas, Texture2D ImageBackground, Texture2D ImageToggle, Texture2D ImageAftermathToggle)
    {
        this.Canvas = Canvas;
        Image[(int)Imagestate.MUSIC_VOLUME_IMAGE] = Canvas.Add(ImageBackground);
        Image[(int)Imagestate.MUSIC_VOLUME_TOGGLE_IMAGE] = Canvas.Add(ImageToggle);
        Image[(int)Imagestate.MUSIC_VOLUME_TOGGLE_AFTERMATH_IMAGE] = Canvas.Add(ImageAftermathToggle);
        Image[(int)Imagestate.MUSIC_VOLUME_IMAGE].scale = new Vector2(0.5f, 1);
        foreach (Image Images in Image)
        {
            Images.interactable = true;
            Images.position = input;
        }
        number = Canvas.Add("0");
        number.scale = new Vector2(0.5f);
        number.position = new Vector2(0.165f + Image[(int)Imagestate.MUSIC_VOLUME_IMAGE].position.x, Image[(int)Imagestate.MUSIC_VOLUME_IMAGE].position.y);
    }

    public ImageBaradjustment(Canvas Canvas)
    {
        this.Canvas = Canvas;
    }

    public ImageBaradjustment(Canvas Canvas, Texture2D ImageBackground, Texture2D ImageToggle, Texture2D ImageAftermathToggle)
    {
        this.Canvas = Canvas;
        Image[(int)Imagestate.MUSIC_VOLUME_IMAGE] = Canvas.Add(ImageBackground);
        Image[(int)Imagestate.MUSIC_VOLUME_TOGGLE_IMAGE] = Canvas.Add(ImageToggle);
        Image[(int)Imagestate.MUSIC_VOLUME_TOGGLE_AFTERMATH_IMAGE] = Canvas.Add(ImageAftermathToggle);
        foreach (Image Images in Image)
        {
            Images.interactable = true;
        }
    }

    public void UpdatePositionInworld(Vector2 input)
    {
        foreach (Image Images in Image)
        {
            Images.position = input;
        }
    }

    public void Settingvalue(int value)
    {
        numbervalue = value;
        float valuecalculator = ((numbervalue) * 3.15f);
        Image[(int)Imagestate.MUSIC_VOLUME_TOGGLE_AFTERMATH_IMAGE].scale = new Vector2(valuecalculator, 1);
        Image[(int)Imagestate.MUSIC_VOLUME_TOGGLE_IMAGE].position = new Vector2(-1, -1);
    }

    public void update()
    {
        if (Image[(int)Imagestate.MUSIC_VOLUME_IMAGE].Hovered())
        {
            if (Input.GetMouseButton(Input.MouseButtons.LEFT))
            {
                Image[(int)Imagestate.MUSIC_VOLUME_TOGGLE_IMAGE].position = new Vector2(Input.GetMouseX() / Canvas.camera.viewport.size.x, Image[(int)Imagestate.MUSIC_VOLUME_TOGGLE_IMAGE].position.y);
                Image[(int)Imagestate.MUSIC_VOLUME_TOGGLE_AFTERMATH_IMAGE].scale = new Vector2((Image[(int)Imagestate.MUSIC_VOLUME_TOGGLE_IMAGE].position.x - Image[(int)Imagestate.MUSIC_VOLUME_TOGGLE_AFTERMATH_IMAGE].position.x) * 1920, 1);
                LastMousePosition = Input.GetMouseX();
            }
        }

        numbervalue = ((int)(Image[(int)Imagestate.MUSIC_VOLUME_TOGGLE_AFTERMATH_IMAGE].scale.x / 3.15f));
        number.text = numbervalue.ToString();
        //320 max 0 minst


    }

    public void UpdateWithLoadedvalues()
    {
        Image[(int)Imagestate.MUSIC_VOLUME_TOGGLE_IMAGE].position = new Vector2(LastMousePosition / Canvas.camera.viewport.size.x, Image[(int)Imagestate.MUSIC_VOLUME_TOGGLE_IMAGE].position.y);
        Image[(int)Imagestate.MUSIC_VOLUME_TOGGLE_AFTERMATH_IMAGE].scale = new Vector2((Image[(int)Imagestate.MUSIC_VOLUME_TOGGLE_IMAGE].position.x - Image[(int)Imagestate.MUSIC_VOLUME_TOGGLE_AFTERMATH_IMAGE].position.x) * 1920, 1);
        numbervalue = ((int)(Image[(int)Imagestate.MUSIC_VOLUME_TOGGLE_AFTERMATH_IMAGE].scale.x / 3.15f));
        number.text = numbervalue.ToString();

    }
}

public class GUIOptionsMenu : ScriptComponent
{
    public static GUIOptionsMenu instance;
    public enum Textstate
    {
        OPTIONS_TEXT,
        Music_Area_Text,
        sensitivity_TEXT,
        Window_Text,
        BACK_TEXT,
        MASTERVOLUME_TEXT,
        MUSIC_TEXT,
        SFX_TEXT,
        MOVEMENTSENSE_TEXT,
        AIMSENSE_TEXT,
        ANOUNCER_TEXT,
        Fullscreen_TEXT,
        BorderLess_Text,
        FullscreenOn_TEXT,
        BorderLessOn_Text,
        FullscreenOff_TEXT,
        BorderLessOff_Text,
        AnnouncerOn_Text,
        AnnouncerOff_Text,
        NUMSTATES
    }
    public enum ImageBarstate
    {
        MasterVolume_image,
        MUSICBar_IMAGE,
        sfxBar_Image,
        Movement,
        AIM,
        NUMSTATES
    }

    Camera Camera;
    public Canvas Canvas;
    Text[] Text = new Text[(int)Textstate.NUMSTATES];
    WindowManager Wcontroller = new WindowManager();
    public Textstate TextState { set; get; } = Textstate.NUMSTATES;
    public Textstate OldState = Textstate.NUMSTATES;
    public ImageBarstate ImageState { set; get; } = ImageBarstate.NUMSTATES;
    public Vector2 SetOriginText { set; get; }
    public Vector2 SetPositionText { set; get; }
    public Vector2 SetOriginImage { set; get; }
    public Vector2 SetPositionImage { set; get; }
    public bool ActivatedfromExitmenu { set; get; } = false;
    public Vector2 SetScale { set; get; } = Vector2.One;
    Color Unselected = Color.FloralWhite;
    Color Selected = Color.IndianRed;
    public Texture2D ImageBackground { set; get; }
    public Texture2D ImageToggle { set; get; }
    public Texture2D ImageAftermathToggle { set; get; }
    public List<ImageBaradjustment> ImageBar;
    public AudioListener Audio;
    public bool fullscreen;
    public bool borderless;

    public override void OnAwake()
    {
        instance = this;
        Audio = gameObject.GetComponent<AudioListener>();
        ImageBar = new List<ImageBaradjustment>();
        OldState = Textstate.NUMSTATES;
        Camera = gameObject.GetComponent<Camera>();
        AddImagesAndText();
    }

    void voidAnnouncerToggler(bool toggle)
    {
        //dumpa koden här för att sätta på och stänga av använd toggle för av och på. 

        if (toggle)
            Audio.SetVoiceVolume(1);
        else
            Audio.SetVoiceVolume(0);

        string Update;
        Update = toggle.ToString();
        UserSettings.AddOrUpdateAppSetting("Announcer", Update);
    }

    public override void Update()
    {
        //  updatePosOrigin();
        ButtonHovered();
        if (Text[(int)Textstate.BACK_TEXT].Clicked() || (Input.GetKeyDown(Input.Keys.Escape) && ActivatedfromExitmenu == true))
            BackbuttonClicked();

        if (Input.GetKeyDown(Input.Keys.P))
            printPosition();

        foreach (ImageBaradjustment Images in ImageBar)
            Images.update();


        Audio.SetFXVolume((float)ImageBar[(int)ImageBarstate.sfxBar_Image].numbervalue / (float)100);
        Audio.SetMusicVolume((float)ImageBar[(int)ImageBarstate.MUSICBar_IMAGE].numbervalue / (float)100);
        Audio.SetMasterVolume((float)ImageBar[(int)ImageBarstate.MasterVolume_image].numbervalue / (float)100);
    }

    void ButtonHovered()
    {
        if (Text[(int)Textstate.BACK_TEXT].Hovered())
            Text[(int)Textstate.BACK_TEXT].color = Selected;
        else
        {
            Text[(int)Textstate.BACK_TEXT].color = Unselected;
        }

        //borderlessOFF
        if (Text[(int)Textstate.BorderLessOff_Text].Clicked())
        {
            Wcontroller.SetBorderless(false);
            Text[(int)Textstate.BorderLessOff_Text].interactable = false;
            Text[(int)Textstate.BorderLessOn_Text].interactable = true;
        }

        if (Text[(int)Textstate.BorderLessOff_Text].Hovered())
            Text[(int)Textstate.BorderLessOff_Text].color = Selected;
        else if (Text[(int)Textstate.BorderLessOff_Text].interactable != false)
        {
            Text[(int)Textstate.BorderLessOff_Text].color = Unselected;
        }

        //borderlessOn
        if (Text[(int)Textstate.BorderLessOn_Text].Clicked())
        {
            Wcontroller.SetBorderless(true);
            Text[(int)Textstate.BorderLessOn_Text].interactable = false;
            Text[(int)Textstate.BorderLessOff_Text].interactable = true;
        }

        if (Text[(int)Textstate.BorderLessOn_Text].Hovered())
            Text[(int)Textstate.BorderLessOn_Text].color = Selected;
        else if (Text[(int)Textstate.BorderLessOn_Text].interactable != false)
        {
            Text[(int)Textstate.BorderLessOn_Text].color = Unselected;
        }

        //FullscreenOFF
        if (Text[(int)Textstate.FullscreenOff_TEXT].Clicked())
        {
            Wcontroller.SetFullscreen(false);
            Text[(int)Textstate.FullscreenOff_TEXT].interactable = false;
            Text[(int)Textstate.FullscreenOn_TEXT].interactable = true;
        }

        if (Text[(int)Textstate.FullscreenOff_TEXT].Hovered())
            Text[(int)Textstate.FullscreenOff_TEXT].color = Selected;
        else if (Text[(int)Textstate.FullscreenOff_TEXT].interactable != false)
        {
            Text[(int)Textstate.FullscreenOff_TEXT].color = Unselected;
        }

        //FullscreenOn
        if (Text[(int)Textstate.FullscreenOn_TEXT].Clicked())
        {
            Wcontroller.SetFullscreen(true);
            Text[(int)Textstate.FullscreenOn_TEXT].interactable = false;
            Text[(int)Textstate.FullscreenOff_TEXT].interactable = true;
        }

        if (Text[(int)Textstate.FullscreenOn_TEXT].Hovered())
            Text[(int)Textstate.FullscreenOn_TEXT].color = Selected;
        else if (Text[(int)Textstate.FullscreenOn_TEXT].interactable != false)
        {
            Text[(int)Textstate.FullscreenOn_TEXT].color = Unselected;
        }

        //AnnouncerOff_Text
        if (Text[(int)Textstate.AnnouncerOff_Text].Clicked())
        {
            //droppa instancen här toggla av,
            voidAnnouncerToggler(false);
            Text[(int)Textstate.AnnouncerOff_Text].interactable = false;
            Text[(int)Textstate.AnnouncerOn_Text].interactable = true;
        }

        if (Text[(int)Textstate.AnnouncerOff_Text].Hovered())
            Text[(int)Textstate.AnnouncerOff_Text].color = Selected;
        else if (Text[(int)Textstate.AnnouncerOff_Text].interactable != false)
        {
            Text[(int)Textstate.AnnouncerOff_Text].color = Unselected;
        }

        //AnnouncerOn_Text
        if (Text[(int)Textstate.AnnouncerOn_Text].Clicked())
        {
            //droppa instancen här toogla på
            voidAnnouncerToggler(true);
            Text[(int)Textstate.AnnouncerOn_Text].interactable = false;
            Text[(int)Textstate.AnnouncerOff_Text].interactable = true;
        }

        if (Text[(int)Textstate.AnnouncerOn_Text].Hovered())
            Text[(int)Textstate.AnnouncerOn_Text].color = Selected;
        else if (Text[(int)Textstate.AnnouncerOn_Text].interactable != false)
        {
            Text[(int)Textstate.AnnouncerOn_Text].color = Unselected;
        }
    }

    void BackbuttonClicked()
    {
        if (ActivatedfromExitmenu)
        {
            CameraMaster.instance.SetState(CAM_STATE.EXIT_MENU);
            ActivatedfromExitmenu = false;
        }
        else
            CameraMaster.instance.SetState(CAM_STATE.MAIN_MENU);



        //
        string Update;
        Update = Wcontroller.GetFullscreen().ToString();

        UserSettings.AddOrUpdateAppSetting("Fullscreen", Update);
        Update = Wcontroller.GetBorderless().ToString();

        UserSettings.AddOrUpdateAppSetting("Borderless", Update);

        if (ImageBar != null)
        {
            Update = ImageBar[(int)ImageBarstate.MasterVolume_image].numbervalue.ToString();
            UserSettings.AddOrUpdateAppSetting("Master", Update);

            Update = ImageBar[(int)ImageBarstate.Movement].numbervalue.ToString();  // save from this call to Movement LastMousePosition here
            UserSettings.AddOrUpdateAppSetting("Move", Update);
            Update = ImageBar[(int)ImageBarstate.sfxBar_Image].numbervalue.ToString();  // save from this call to sfxBar_Image LastMousePosition here
            UserSettings.AddOrUpdateAppSetting("Sfx", Update);
            Update = ImageBar[(int)ImageBarstate.MUSICBar_IMAGE].numbervalue.ToString();  // save from this call to MUSICBar_IMAGE LastMousePosition here
            UserSettings.AddOrUpdateAppSetting("Music", Update);
            Update = ImageBar[(int)ImageBarstate.AIM].numbervalue.ToString();  // save from this call to aim LastMousePosition here
            UserSettings.AddOrUpdateAppSetting("Aim", Update);
        }


    }

    public void AddImagesAndText()
    {
        Canvas = Camera.AddCanvas();
        #region Text Back
        Text[(int)Textstate.BACK_TEXT] = Canvas.Add("Back");
        Text[(int)Textstate.BACK_TEXT].position = new Vector2(0.01f, 0.85f + 0.05f);
        Text[(int)Textstate.BACK_TEXT].interactable = true;
        #endregion
        #region Text Options
        Text[(int)Textstate.OPTIONS_TEXT] = Canvas.Add("OPTIONS");
        Text[(int)Textstate.OPTIONS_TEXT].scale = new Vector2(1.4f, 1.2f);
        Text[(int)Textstate.OPTIONS_TEXT].position = new Vector2(0, 0.1f);
        #endregion
        #region Text Aim
        Text[(int)Textstate.AIMSENSE_TEXT] = Canvas.Add("Aim");
        Text[(int)Textstate.AIMSENSE_TEXT].position = new Vector2(0.02f, 0.60f);
        #endregion
        #region Text Movement
        Text[(int)Textstate.MOVEMENTSENSE_TEXT] = Canvas.Add("Movement");
        Text[(int)Textstate.MOVEMENTSENSE_TEXT].position = new Vector2(0.02f, 0.55f);
        #endregion
        #region Text Master Volume
        Text[(int)Textstate.MASTERVOLUME_TEXT] = Canvas.Add("MasterVolume");
        Text[(int)Textstate.MASTERVOLUME_TEXT].position = new Vector2(0.02f, 0.27f);
        #endregion
        #region Text Music
        Text[(int)Textstate.MUSIC_TEXT] = Canvas.Add("Music");
        Text[(int)Textstate.MUSIC_TEXT].position = new Vector2(0.02f, 0.32f);
        #endregion
        #region Text SFX
        Text[(int)Textstate.SFX_TEXT] = Canvas.Add("SFX");
        Text[(int)Textstate.SFX_TEXT].position = new Vector2(0.02f, 0.37f);
        #endregion
        #region Text Announcer
        Text[(int)Textstate.ANOUNCER_TEXT] = Canvas.Add("Announcer");
        Text[(int)Textstate.ANOUNCER_TEXT].position = new Vector2(0.02f, 0.42f);
        #endregion
        #region Text Audio
        Text[(int)Textstate.Music_Area_Text] = Canvas.Add("Audio");
        Text[(int)Textstate.Music_Area_Text].scale = new Vector2(0.75f);
        Text[(int)Textstate.Music_Area_Text].position = new Vector2(0.01f, 0.2f);
        #endregion
        #region Text Sensitivity
        Text[(int)Textstate.sensitivity_TEXT] = Canvas.Add("SENSITIVITY");
        Text[(int)Textstate.sensitivity_TEXT].scale = new Vector2(0.75f);
        Text[(int)Textstate.sensitivity_TEXT].position = new Vector2(0.01f, 0.47f);
        #endregion
        #region Text Window
        Text[(int)Textstate.Window_Text] = Canvas.Add("Window");
        Text[(int)Textstate.Window_Text].scale = new Vector2(0.75f);
        Text[(int)Textstate.Window_Text].position = new Vector2(0.01f, 0.64f + 0.05f);
        #endregion
        #region Text Fullscreen
        Text[(int)Textstate.Fullscreen_TEXT] = Canvas.Add("FullScreen");
        Text[(int)Textstate.Fullscreen_TEXT].position = new Vector2(0.02f, 0.72f + 0.05f);
        #endregion
        #region Text Fullscreen Off
        Text[(int)Textstate.FullscreenOff_TEXT] = Canvas.Add("Off");
        Text[(int)Textstate.FullscreenOff_TEXT].interactable = true;
        Text[(int)Textstate.FullscreenOff_TEXT].position = new Vector2(0.17f, 0.72f + 0.05f);
        #endregion
        #region Text Fullscreen On
        Text[(int)Textstate.FullscreenOn_TEXT] = Canvas.Add("On");
        Text[(int)Textstate.FullscreenOn_TEXT].interactable = true;
        Text[(int)Textstate.FullscreenOn_TEXT].position = new Vector2(0.21f, 0.72f + 0.05f);
        #endregion
        #region Text Border Less
        Text[(int)Textstate.BorderLess_Text] = Canvas.Add("BorderLess");
        Text[(int)Textstate.BorderLess_Text].position = new Vector2(0.02f, 0.77f + 0.05f);
        #endregion
        #region Text Border Less Off
        Text[(int)Textstate.BorderLessOff_Text] = Canvas.Add("Off");
        Text[(int)Textstate.BorderLessOff_Text].interactable = true;
        Text[(int)Textstate.BorderLessOff_Text].position = new Vector2(0.17f, 0.77f + 0.05f);
        #endregion
        #region Text Border Less On
        Text[(int)Textstate.BorderLessOn_Text] = Canvas.Add("On");
        Text[(int)Textstate.BorderLessOn_Text].interactable = true;
        Text[(int)Textstate.BorderLessOn_Text].position = new Vector2(0.21f, 0.77f + 0.05f);
        #endregion
        #region Text Off
        Text[(int)Textstate.AnnouncerOff_Text] = Canvas.Add("Off");
        Text[(int)Textstate.AnnouncerOff_Text].interactable = true;
        Text[(int)Textstate.AnnouncerOff_Text].position = new Vector2(0.17f, 0.42f);
        #endregion
        #region Text On
        Text[(int)Textstate.AnnouncerOn_Text] = Canvas.Add("On");
        Text[(int)Textstate.AnnouncerOn_Text].interactable = true;
        Text[(int)Textstate.AnnouncerOn_Text].position = new Vector2(0.21f, 0.42f);
        #endregion
        foreach (Text Texture in Text)
        {
            Texture.depth = 0.9f;
        }
        for (int i = 4; i < Text.Length; i++)
        {
            Text[i].scale = new Vector2(0.5f);
        }
        Texture2D CopyImageBackground = ImageBackground;
        Texture2D CopyImageImageToggle = ImageToggle;
        Texture2D CopyImageImageAftermathToggle = ImageAftermathToggle;

        ImageBar.Add(new ImageBaradjustment(new Vector2(0.1f, 0.27f), Canvas, CopyImageBackground, CopyImageImageToggle, CopyImageImageAftermathToggle));//MasterVolume bar
        ImageBar.Add(new ImageBaradjustment(new Vector2(0.1f, 0.32f), Canvas, CopyImageBackground, CopyImageImageToggle, CopyImageImageAftermathToggle));//music bar
        ImageBar.Add(new ImageBaradjustment(new Vector2(0.1f, 0.37f), Canvas, CopyImageBackground, CopyImageImageToggle, CopyImageImageAftermathToggle));//sfx bar
        ImageBar.Add(new ImageBaradjustment(new Vector2(0.15f, 0.5f + 0.05f), Canvas, CopyImageBackground, CopyImageImageToggle, CopyImageImageAftermathToggle));//movement bar
        ImageBar.Add(new ImageBaradjustment(new Vector2(0.15f, 0.55f + 0.05f), Canvas, CopyImageBackground, CopyImageImageToggle, CopyImageImageAftermathToggle));//Aim bar
        ImageBar[(int)ImageBarstate.AIM].Settingvalue(20);
        ImageBar[(int)ImageBarstate.MasterVolume_image].Settingvalue(100);
        ImageBar[(int)ImageBarstate.Movement].Settingvalue(10);
        ImageBar[(int)ImageBarstate.MUSICBar_IMAGE].Settingvalue(100);
        ImageBar[(int)ImageBarstate.sfxBar_Image].Settingvalue(100);


        string SettingsMasterVolume = UserSettings.GetSetting("Master");
        if (SettingsMasterVolume != null)
        {
            ImageBar[(int)ImageBarstate.MasterVolume_image].Settingvalue(System.Convert.ToInt32(SettingsMasterVolume));
        }
        string settingsMusic = UserSettings.GetSetting("Music");
        if (settingsMusic != null)
        {
            ImageBar[(int)ImageBarstate.MUSICBar_IMAGE].Settingvalue(System.Convert.ToInt32(settingsMusic));
        }
        string settingsSFX = UserSettings.GetSetting("Sfx");
        if (settingsSFX != null)
        {
            ImageBar[(int)ImageBarstate.sfxBar_Image].Settingvalue(System.Convert.ToInt32(settingsSFX));
        }
        string settingsMove = UserSettings.GetSetting("Move");
        if (settingsMove != null)
        {
            ImageBar[(int)ImageBarstate.Movement].Settingvalue(System.Convert.ToInt32(settingsMove));
        }
        string settingsAIM = UserSettings.GetSetting("Aim");
        if (settingsAIM != null)
        {
            ImageBar[(int)ImageBarstate.AIM].Settingvalue(System.Convert.ToInt32(settingsAIM));
        }

        //load onoff fullscreen
        fullscreen = Wcontroller.GetFullscreen();
        borderless = Wcontroller.GetBorderless();
        string settingsFullscreen = UserSettings.GetSetting("Fullscreen");
        if (settingsFullscreen != null)
        {
            fullscreen = System.Convert.ToBoolean(settingsFullscreen);
        }
        string settingsBorderless = UserSettings.GetSetting("Borderless");
        if (settingsBorderless != null)
        {
            borderless = System.Convert.ToBoolean(settingsBorderless);
        }

        string SettingsmasterAnnouncer = UserSettings.GetSetting("Announcer");
        if (SettingsmasterAnnouncer != null)
        {
            bool Announcer = System.Convert.ToBoolean(SettingsmasterAnnouncer);
            voidAnnouncerToggler(Announcer);
            if (Announcer)
            {
                Text[(int)Textstate.AnnouncerOn_Text].interactable = false;
                Text[(int)Textstate.AnnouncerOn_Text].color = Selected;
                Text[(int)Textstate.AnnouncerOff_Text].interactable = true;
            }
            else
            {
                Text[(int)Textstate.AnnouncerOff_Text].interactable = false;
                Text[(int)Textstate.AnnouncerOff_Text].color = Selected;
                Text[(int)Textstate.AnnouncerOn_Text].interactable = true;
            }
        }
        else
        {
            voidAnnouncerToggler(true);
            Text[(int)Textstate.AnnouncerOn_Text].interactable = false;
            Text[(int)Textstate.AnnouncerOn_Text].color = Selected;
            Text[(int)Textstate.AnnouncerOff_Text].interactable = true;
        }

        Wcontroller.SetFullscreen(fullscreen);
        Wcontroller.SetBorderless(borderless);
        if (fullscreen)
        {
            Text[(int)Textstate.FullscreenOn_TEXT].interactable = false;
            Text[(int)Textstate.FullscreenOn_TEXT].color = Selected;
        }
        else
        {
            Text[(int)Textstate.FullscreenOff_TEXT].interactable = false;
            Text[(int)Textstate.FullscreenOff_TEXT].color = Selected;
        }

        if (borderless)
        {
            Text[(int)Textstate.BorderLessOn_Text].interactable = false;
            Text[(int)Textstate.BorderLessOn_Text].color = Selected;
        }
        else
        {
            Text[(int)Textstate.BorderLessOff_Text].interactable = false;
            Text[(int)Textstate.BorderLessOff_Text].color = Selected;
        }

    }

    public void ClearImagesAndText()
    {
        Canvas.Remove(Text[(int)Textstate.BACK_TEXT]);
    }

    public void printPosition()
    {
        Textstate tempstate = Textstate.OPTIONS_TEXT;
        foreach (Text Texture in Text)
        {
            Debug.Log(tempstate.ToString() + "x = " + Texture.position.x + "   y = " + Texture.position.y);
            tempstate++;
        }
    }

    public override void OnDestroy()
    {
        //MORGAN SPARA HÄR
        /*      
                if(Imagebar != null)
                {
                    ImageBar[(int)ImageBarstate.Movement].LastMousePosition;  // save from this call to Movement LastMousePosition here
                    ImageBar[(int)ImageBarstate.sfxBar_Image].LastMousePosition;  // save from this call to sfxBar_Image LastMousePosition here
                    ImageBar[(int)ImageBarstate.MUSICBar_IMAGE].LastMousePosition;  // save from this call to MUSICBar_IMAGE LastMousePosition here
                    ImageBar[(int)ImageBarstate.AIM].LastMousePosition;  // save from this call to aim LastMousePosition here
                }
                  */
        //      ClearImagesAndText();
        string Update;
        Update = Wcontroller.GetFullscreen().ToString();

        UserSettings.AddOrUpdateAppSetting("Fullscreen", Update);
        Update = Wcontroller.GetBorderless().ToString();
 
        UserSettings.AddOrUpdateAppSetting("Borderless", Update);

        if (ImageBar != null)
        {
            Update = ImageBar[(int)ImageBarstate.MasterVolume_image].numbervalue.ToString();
            UserSettings.AddOrUpdateAppSetting("Master", Update);

            Update = ImageBar[(int)ImageBarstate.Movement].numbervalue.ToString();  // save from this call to Movement LastMousePosition here
            UserSettings.AddOrUpdateAppSetting("Move", Update);
            Update = ImageBar[(int)ImageBarstate.sfxBar_Image].numbervalue.ToString();  // save from this call to sfxBar_Image LastMousePosition here
            UserSettings.AddOrUpdateAppSetting("Sfx", Update);
            Update = ImageBar[(int)ImageBarstate.MUSICBar_IMAGE].numbervalue.ToString();  // save from this call to MUSICBar_IMAGE LastMousePosition here
            UserSettings.AddOrUpdateAppSetting("Music", Update);
            Update = ImageBar[(int)ImageBarstate.AIM].numbervalue.ToString();  // save from this call to aim LastMousePosition here
            UserSettings.AddOrUpdateAppSetting("Aim", Update);
        }
    }

    public void updatePosOrigin()
    {
        if (TextState != Textstate.NUMSTATES)
        {
            Text[(int)TextState].origin = SetOriginText;
            Text[(int)TextState].position = SetPositionText;
        }
    }

    public int getAim()
    {
        if (ImageBar[(int)ImageBarstate.AIM].numbervalue > 0)
            return ImageBar[(int)ImageBarstate.AIM].numbervalue;
        else
            return 1;
    }

    public int getMovement()
    {
        if (ImageBar[(int)ImageBarstate.Movement].numbervalue > 0)
            return ImageBar[(int)ImageBarstate.Movement].numbervalue;
        else
            return 1;
    }
}
