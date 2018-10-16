using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ThomasEngine;

public class GUITest : ScriptComponent
{
    public Texture2D Sprite { get; set; }
    public Font TextFont { get; set; }
    private Vector4 OriginalColor;
    private Vector4 RedColor;
    Camera Camera;
    

    public override void Start()
    {
        Camera = gameObject.GetComponent<Camera>();
        Camera.AddImage("Chad", Sprite, new Vector2(200, 30), new Vector2(0.5f, 0.5f), true);
        Camera.AddText("Hi", "Hi, test 1 2 1 2", new Vector2(200, 100));

        OriginalColor = new Vector4(1.0f, 1.0f, 1.0f, 1.0f);
        RedColor = new Vector4(0.0f, 0.0f, 1.0f, 1.0f);
    }

    public override void Update()
    {
        if (Camera.OnImageClicked("Chad"))
        {
            Debug.Log(Input.GetLastKey());

            //Camera.SetImageColor("Chad", RedColor);
        }
        //else
        //{
        //    Camera.SetImageColor("Chad", OriginalColor);
        //}
    }
}
