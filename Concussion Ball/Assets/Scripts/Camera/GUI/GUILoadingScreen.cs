using System;
using System.Collections;
using ThomasEngine;
using LiteNetLib;

public class GUILoadingScreen : ScriptComponent
{
    Camera Camera;

    public Canvas Canvas;
    public Texture2D Background { get; set; }

    Image BG;

    public override void OnAwake()
    {
        Camera = gameObject.GetComponent<Camera>();
        Canvas = Camera.AddCanvas();
        if (Background != null)
        {
            BG = Canvas.Add(Background);
        }
    }

    public override void Update()
    {

    }
}
