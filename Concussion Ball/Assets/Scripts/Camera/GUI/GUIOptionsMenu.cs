using System;
using System.Collections;
using ThomasEngine;

public class GUIOptionsMenu : ScriptComponent
{
    public enum Textstate
    {
        BACK_TEXTURE,
        NUMSTATES
    }
    Camera Camera;
    public Canvas Canvas;
    Text[] Text = new Text[(int)Textstate.NUMSTATES];
    public Textstate State { set; get; } = Textstate.BACK_TEXTURE;
    public Textstate OldState = Textstate.NUMSTATES;
    public Font Font { set; get; }
    public Vector2 SetOrigin{set;get;}
    public Vector2 SetPosition { set; get; }
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
        if (Text[(int)Textstate.BACK_TEXTURE].Hovered())
        {
            Text[(int)Textstate.BACK_TEXTURE].color = Selected;
        }
        else
            Text[(int)Textstate.BACK_TEXTURE].color = Unselected;

        if (Text[(int)Textstate.BACK_TEXTURE].Clicked())
            BackbuttonClicked();      
    }
    
    void BackbuttonClicked()
    {
        CameraMaster.instance.State = CAM_STATE.MAIN_MENU;
    }
    public void AddImagesAndText()
    {
        Canvas = Camera.AddCanvas();
        Text[(int)Textstate.BACK_TEXTURE] = Canvas.Add("Back");
        Text[(int)Textstate.BACK_TEXTURE].position = new Vector2(0.1f, 0.2f);
        foreach (Text Texture in Text)
        {
            Texture.interactable = true;
            Texture.font = Font;
            Texture.scale = Vector2.One;
        }
    }
    public void ClearImagesAndText()
    {
        Canvas.Remove(Text[(int)Textstate.BACK_TEXTURE]);
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
//          Debug.Log("oldstateUpdated");
        }
        else
        { 
            Text[(int)State].origin = SetOrigin;
            Text[(int)State].position = SetPosition;
        }
    }
}
