using System;
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
        Image[(int)Imagestate.MUSIC_VOLUME_IMAGE].scale = new Vector2(0.5f,1);
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
    public void start(Vector2 input, Canvas Canvas, Texture2D ImageBackground, Texture2D ImageToggle, Texture2D ImageAftermathToggle)
    {
        this.Canvas = Canvas;
        Image[(int)Imagestate.MUSIC_VOLUME_IMAGE] = Canvas.Add(ImageBackground);
        Image[(int)Imagestate.MUSIC_VOLUME_TOGGLE_IMAGE] = Canvas.Add(ImageToggle);
        Image[(int)Imagestate.MUSIC_VOLUME_TOGGLE_AFTERMATH_IMAGE] = Canvas.Add(ImageAftermathToggle);
        foreach (Image Images in Image)
        {
            Images.interactable = true;
            Images.position = input;
        }
    }
    public void UpdatePositionInworld(Vector2 input)
    {
        foreach (Image Images in Image)
        {
            Images.position = input;
        }
    }

    public void update()
    {
        if (Image[(int)Imagestate.MUSIC_VOLUME_IMAGE].Hovered())
        { 
            if(Input.GetMouseButton(Input.MouseButtons.LEFT))
            { 
                Image[(int)Imagestate.MUSIC_VOLUME_TOGGLE_IMAGE].position = new Vector2(Input.GetMouseX() / Canvas.camera.viewport.size.x, Image[(int)Imagestate.MUSIC_VOLUME_TOGGLE_IMAGE].position.y);
                Image[(int)Imagestate.MUSIC_VOLUME_TOGGLE_AFTERMATH_IMAGE].scale = new Vector2((Image[(int)Imagestate.MUSIC_VOLUME_TOGGLE_IMAGE].position.x - Image[(int)Imagestate.MUSIC_VOLUME_TOGGLE_AFTERMATH_IMAGE].position.x) * 1920, 1);
                LastMousePosition = Input.GetMouseX();
            }
        }
        //    if (Input.GetKeyDown(Input.Keys.X))
        //   Debug.Log(Image[(int)Imagestate.MUSIC_VOLUME_TOGGLE_AFTERMATH_IMAGE].scale.x);
       
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
        MUSIC_TEXT,
        SFX_TEXT,
        MOVEMENTSENSE_TEXT,
        AIMSENSE_TEXT,
        Fullscreen_TEXT,
        BorderLess_Text,
        FullscreenOn_TEXT,
        BorderLessOn_Text,
        FullscreenOff_TEXT,
        BorderLessOff_Text,
        NUMSTATES
    }
    public enum ImageBarstate
    {
        MUSICBar_IMAGE,
        sfxBar_Image,
        Movement,
        AIM,
        NUMSTATES
    }


    Camera Camera;
   public Canvas Canvas;
    Text[] Text = new Text[(int)Textstate.NUMSTATES];
    WindowController Wcontroller = new WindowController();
    public Textstate TextState { set; get; } = Textstate.NUMSTATES;
    public Textstate OldState = Textstate.NUMSTATES;
    public ImageBarstate ImageState { set; get; } = ImageBarstate.NUMSTATES;
    public Font Font{ set; get; }
    public Vector2 SetOriginText   { set; get; }
    public Vector2 SetPositionText { set; get; }
    public Vector2 SetOriginImage { set; get; }
    public Vector2 SetPositionImage { set; get; }
    public bool ActivatedfromExitmenu { set; get; } = false;
    public Vector2 SetScale { set; get; } = Vector2.One;
    Color Unselected = Color.FloralWhite;
    Color Selected = Color.IndianRed;
    public Texture2D ImageBackground { set; get; }
    public Texture2D ImageToggle { set; get; }
    public Texture2D ImageAftermathToggle { set; get;}
    public List<ImageBaradjustment> ImageBar;
    public AudioListener Audio;

    public override void OnAwake()
    {
        instance = this;
    }

    public override void Start()
    {
        Audio = gameObject.GetComponent<AudioListener>();
        ImageBar = new List<ImageBaradjustment>();
        Camera = gameObject.GetComponent<Camera>();
        AddImagesAndText();
        OldState = Textstate.NUMSTATES; 
    }
    public override void Update()
    {
      //  updatePosOrigin();
        ButtonHovered();
        if (Text[(int)Textstate.BACK_TEXT].Clicked() ||  (Input.GetKeyDown(Input.Keys.Escape) && ActivatedfromExitmenu ==true))
            BackbuttonClicked();

        if (Input.GetKeyDown(Input.Keys.P))
            printPosition();

        foreach (ImageBaradjustment Images in ImageBar)
            Images.update();


        Audio.SetFXVolume((float)ImageBar[(int)ImageBarstate.sfxBar_Image].numbervalue / (float)100);
        Audio.SetMusicVolume((float)ImageBar[(int)ImageBarstate.MUSICBar_IMAGE].numbervalue / (float)100);


   

        /*    if (Input.GetKeyDown(Input.Keys.X))
            {
                Debug.Log(ImageBar[(int)ImageBarstate.MUSICBar_IMAGE].LastMousePosition);
                Debug.Log(ImageBar[(int)ImageBarstate.Movement].LastMousePosition);
            }
            /*        if (Input.GetKeyDown(Input.Keys.Y))
                        Debug.Log(Image[(int)ImageBarstate.MUSIC_VOLUME_TOGGLE_AFTERMATH_IMAGE].scale.x);*/
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
      //      Debug.Log("button clicked C#");
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
      //      Debug.Log("button clicked C#");
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
       //     Debug.Log("button clicked C#");
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
         //   Debug.Log("button clicked C#");
            Text[(int)Textstate.FullscreenOn_TEXT].interactable = false;
            Text[(int)Textstate.FullscreenOff_TEXT].interactable = true;
        }

        if (Text[(int)Textstate.FullscreenOn_TEXT].Hovered())
            Text[(int)Textstate.FullscreenOn_TEXT].color = Selected;
        else if (Text[(int)Textstate.FullscreenOn_TEXT].interactable != false)
        {
            Text[(int)Textstate.FullscreenOn_TEXT].color = Unselected;
        }

    }
    void BackbuttonClicked()
    {
        if(ActivatedfromExitmenu)
        {
            CameraMaster.instance.State = CAM_STATE.GAME;
            Input.SetMouseMode(Input.MouseMode.POSITION_RELATIVE);
            ActivatedfromExitmenu = false;
        }
        else
            CameraMaster.instance.State = CAM_STATE.MAIN_MENU;

        
    }
    public void AddImagesAndText()
    {
        Canvas = Camera.AddCanvas();
        Text[(int)Textstate.BACK_TEXT] = Canvas.Add("Back"); Text[(int)Textstate.BACK_TEXT].position = new Vector2(0.01f, 0.85f); Text[(int)Textstate.BACK_TEXT].interactable = true;
        Text[(int)Textstate.OPTIONS_TEXT] = Canvas.Add("OPTIONS"); Text[(int)Textstate.OPTIONS_TEXT].scale = new Vector2(1.4f, 1.2f); Text[(int)Textstate.OPTIONS_TEXT].position = new Vector2(0, 0.1f);
        Text[(int)Textstate.AIMSENSE_TEXT] = Canvas.Add("Aim"); Text[(int)Textstate.AIMSENSE_TEXT].position = new Vector2(0.02f, 0.55f);
        Text[(int)Textstate.MOVEMENTSENSE_TEXT] = Canvas.Add("Movement"); Text[(int)Textstate.MOVEMENTSENSE_TEXT].position = new Vector2(0.02f, 0.5f);
        Text[(int)Textstate.MUSIC_TEXT] = Canvas.Add("Music"); Text[(int)Textstate.MUSIC_TEXT].position = new Vector2(0.02f, 0.27f);
        Text[(int)Textstate.SFX_TEXT] = Canvas.Add("SFX"); Text[(int)Textstate.SFX_TEXT].position = new Vector2(0.02f, 0.32f);
        Text[(int)Textstate.Music_Area_Text] = Canvas.Add("Audio"); Text[1].scale = new Vector2(0.75f); Text[(int)Textstate.Music_Area_Text].position = new Vector2(0.01f, 0.2f);
        Text[(int)Textstate.sensitivity_TEXT] = Canvas.Add("SENSITIVITY"); Text[2].scale = new Vector2(0.75f); Text[(int)Textstate.sensitivity_TEXT].position = new Vector2(0.01f, 0.42f);

        Text[(int)Textstate.Window_Text] = Canvas.Add("Window"); Text[(int)Textstate.Window_Text].scale = new Vector2(0.75f); Text[(int)Textstate.Window_Text].position = new Vector2(0.01f, 0.64f);
        ///////////////////////////////////////////////////////////////////
        Text[(int)Textstate.Fullscreen_TEXT] = Canvas.Add("FullScreen"); Text[(int)Textstate.Fullscreen_TEXT].position = new Vector2(0.02f, 0.72f);
        Text[(int)Textstate.FullscreenOff_TEXT] = Canvas.Add("Off"); Text[(int)Textstate.FullscreenOff_TEXT].interactable = true; Text[(int)Textstate.FullscreenOff_TEXT].position = new Vector2(0.17f, 0.72f);
        Text[(int)Textstate.FullscreenOn_TEXT] = Canvas.Add("On"); Text[(int)Textstate.FullscreenOn_TEXT].interactable = true; Text[(int)Textstate.FullscreenOn_TEXT].position = new Vector2(0.21f, 0.72f);
        ///////////////////////////////////////////////////////////////////
        Text[(int)Textstate.BorderLess_Text] = Canvas.Add("BorderLess");  Text[(int)Textstate.BorderLess_Text].position = new Vector2(0.02f, 0.77f);
        Text[(int)Textstate.BorderLessOff_Text] = Canvas.Add("Off"); Text[(int)Textstate.BorderLessOff_Text].interactable = true; Text[(int)Textstate.BorderLessOff_Text].position = new Vector2(0.17f, 0.77f);
        Text[(int)Textstate.BorderLessOn_Text] = Canvas.Add("On"); Text[(int)Textstate.BorderLessOn_Text].interactable = true; Text[(int)Textstate.BorderLessOn_Text].position = new Vector2(0.21f, 0.77f);

        foreach (Text Texture in Text)
        {
            Texture.depth = 0.9f;
        }
        for(int i=4;i<Text.Length;i++)
        {
            Text[i].scale = new Vector2(0.5f);
        }
        Texture2D CopyImageBackground = ImageBackground; Texture2D CopyImageImageToggle = ImageToggle; Texture2D CopyImageImageAftermathToggle = ImageAftermathToggle;
       
        ImageBar.Add(new ImageBaradjustment(new Vector2(0.1f,0.27f), Canvas, CopyImageBackground, CopyImageImageToggle, CopyImageImageAftermathToggle));//music bar
        ImageBar.Add(new ImageBaradjustment(new Vector2(0.1f, 0.32f), Canvas, CopyImageBackground, CopyImageImageToggle, CopyImageImageAftermathToggle));//sfx bar
        ImageBar.Add(new ImageBaradjustment(new Vector2(0.15f, 0.5f), Canvas, CopyImageBackground, CopyImageImageToggle, CopyImageImageAftermathToggle));//movement bar
        ImageBar.Add(new ImageBaradjustment(new Vector2(0.15f, 0.55f), Canvas, CopyImageBackground, CopyImageImageToggle, CopyImageImageAftermathToggle));//Aim bar
                                                                                                                                                          //ImageBar.Add(new ImageBaradjustment(Canvas));
        ImageBar[(int)ImageBarstate.MUSICBar_IMAGE].LastMousePosition = 373;// load LastMousePosition here
        ImageBar[(int)ImageBarstate.Movement].LastMousePosition = 460; // load LastMousePosition here
        ImageBar[(int)ImageBarstate.sfxBar_Image].LastMousePosition = 373;// load LastMousePosition here
        ImageBar[(int)ImageBarstate.AIM].LastMousePosition = 460; // load LastMousePosition here
        ImageBar[(int)ImageBarstate.Movement].UpdateWithLoadedvalues();
        ImageBar[(int)ImageBarstate.MUSICBar_IMAGE].UpdateWithLoadedvalues();
        ImageBar[(int)ImageBarstate.AIM].UpdateWithLoadedvalues();
        ImageBar[(int)ImageBarstate.sfxBar_Image].UpdateWithLoadedvalues();
        ChadCam.instance.CameraSensitivity_x = ImageBar[(int)ImageBarstate.Movement].numbervalue;
        ChadCam.instance.CameraSensitivity_y = ImageBar[(int)ImageBarstate.Movement].numbervalue;

        //load onoff fullscreen
        bool fullscreen = Wcontroller.getBorderless();
        bool borderless = Wcontroller.getFullscreen();
        Wcontroller.SetFullscreen(fullscreen);
        Wcontroller.SetBorderless(borderless);
        if(fullscreen)
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
          */                                                                //      ClearImagesAndText();
    }
    public void updatePosOrigin()
    {
        /*     if(TextState!= OldState)
             {
                SetPositionText = Text[(int)TextState].position;

                 OldState = State;
     //            Debug.Log(SetPositionText.x+" "+SetPositionText.y);
             }
             else
             {*/
        //   Text[(int)TextState].scale = SetScale;
        if(TextState != Textstate.NUMSTATES)
        { 
             Text[(int)TextState].origin = SetOriginText;
             Text[(int)TextState].position = SetPositionText;
        }

   //     if (ImageState != ImageBarstate.NUMSTATES)
    //        ImageBar[(int)ImageState].UpdatePositionInworld(SetPositionImage);

        // }
        //   if (ImageState != ImageBarstate.NUMSTATES)
        //      ImageBar
        //     ImageBar[(int)ImageState].UpdatePositionInworld = SetPositionImage;
        //      Image[(int)ImageBarstate].position = SetPositionImage;
        // }
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
