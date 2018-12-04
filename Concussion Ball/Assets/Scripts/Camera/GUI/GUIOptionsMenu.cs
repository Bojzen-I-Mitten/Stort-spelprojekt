using System;
using System.Collections;
using ThomasEngine;

public class GUIOptionsMenu : ScriptComponent
{
    Camera Camera;
    public Canvas Canvas;
    Image BackButtonImage;
    public Texture2D BackButton;

    public override void Start()
    {
        Camera = gameObject.GetComponent<Camera>();
        AddImagesAndText();
    }
    public override void Update()
    {
        if(BackButtonImage.Clicked())
            BackbuttonClicked();        
    }
    void BackbuttonClicked()
    {
        CameraMaster.instance.State = CAM_STATE.MAIN_MENU;
    }
    public void AddImagesAndText()
    {
        Canvas = Camera.AddCanvas();
        BackButtonImage = Canvas.Add(BackButton);
    }
    public void ClearImagesAndText()
    {
        Canvas.Remove(BackButtonImage);
    }
    public override void OnDestroy()
    {
        ClearImagesAndText();
    }
}
