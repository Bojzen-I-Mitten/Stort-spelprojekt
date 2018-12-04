using System;
using System.Collections;
using ThomasEngine;

public class GUIOptionsMenu : ScriptComponent
{
    public enum Textstate
    {
        OPTIONS_TEXT,
        BACK_TEXT,
        MUSIC_TEXT,
        SFX_TEXT,
        MOVEMENTSENSE_TEXT,
        AIMSENSE_TEXT,
        NUMSTATES
    }
    Camera Camera;
    public Canvas Canvas;
    Text[] Text = new Text[(int)Textstate.NUMSTATES];
    public Textstate State { set; get; } = Textstate.BACK_TEXT;
    public Textstate OldState = Textstate.NUMSTATES;
    public Font Font           { set; get; }
    public Vector2 SetOrigin   { set; get; }
    public Vector2 SetPosition { set; get; }
    public Vector2 SetScale { set; get; } = Vector2.One;
    Color Unselected = Color.FloralWhite;
    Color Selected = Color.IndianRed;
    public override void Start()
    {
        Camera = gameObject.GetComponent<Camera>();
        AddImagesAndText();
        OldState = Textstate.NUMSTATES; 
    }
    public override void Update()
    {
        updatePosOrigin();
        ButtonHovered();
        if (Text[(int)Textstate.BACK_TEXT].Clicked())
            BackbuttonClicked();

        if (Input.GetKeyDown(Input.Keys.P))
            printPosition();

    }
    void ButtonHovered()
    {
        for(int i=0;i<(int)Textstate.NUMSTATES;i++)
        { 
            if (Text[i].Hovered())
                Text[i].color = Selected;
            else
                Text[i].color = Unselected;
        }
    }
    void BackbuttonClicked()
    {
        CameraMaster.instance.State = CAM_STATE.MAIN_MENU;
    }
    public void AddImagesAndText()
    {
        Canvas = Camera.AddCanvas();
        Text[(int)Textstate.BACK_TEXT] = Canvas.Add("Back"); Text[(int)Textstate.BACK_TEXT].position = new Vector2(0.05f, 0.65f); Text[(int)Textstate.BACK_TEXT].interactable = true;
        Text[(int)Textstate.OPTIONS_TEXT] = Canvas.Add("OPTIONS"); Text[(int)Textstate.OPTIONS_TEXT].scale = new Vector2(1.4f, 1.2f); Text[(int)Textstate.OPTIONS_TEXT].position = new Vector2(0.05f, 0.1f);
        Text[(int)Textstate.AIMSENSE_TEXT] = Canvas.Add("AIM"); Text[(int)Textstate.AIMSENSE_TEXT].position = new Vector2(0.05f, 0.55f);
        Text[(int)Textstate.MOVEMENTSENSE_TEXT] = Canvas.Add("Movement"); Text[(int)Textstate.MOVEMENTSENSE_TEXT].position = new Vector2(0.05f, 0.45f);
        Text[(int)Textstate.MUSIC_TEXT] = Canvas.Add("Music"); Text[(int)Textstate.MUSIC_TEXT].position = new Vector2(0.05f, 0.25f);
        Text[(int)Textstate.SFX_TEXT] = Canvas.Add("SFX"); Text[(int)Textstate.SFX_TEXT].position = new Vector2(0.05f, 0.35f);
        for (int i = 0; i < (int)Textstate.NUMSTATES; i++)
        {
            Text[i].depth = 0.9f;
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
  //      ClearImagesAndText();
    }
    public void updatePosOrigin()
    {
        if(State!= OldState)
        {
           SetPosition = Text[(int)State].position;

            OldState = State;
//            Debug.Log(SetPosition.x+" "+SetPosition.y);
        }
        else
        {
         //   Text[(int)State].scale = SetScale;
            Text[(int)State].origin = SetOrigin;
            Text[(int)State].position = SetPosition;
        }
    }
}
