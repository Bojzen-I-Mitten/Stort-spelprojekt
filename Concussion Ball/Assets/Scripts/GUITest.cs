using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ThomasEngine;

public class GUITest : ScriptComponent
{
    public Texture2D Sprite { get; set; }
    //public Font TextFont { get; set; }
    private Vector4 OriginalColor;
    private Vector4 RedColor;
    Camera Camera;
    string IPText;
    
    public override void Start()
    {
        Camera = gameObject.GetComponent<Camera>();
        Camera.AddImage("Chad", Sprite, new Vector2(200, 30), new Vector2(0.5f, 0.5f), true);
        Camera.AddText("Test", "Text", new Vector2(200, 100));

        OriginalColor = new Vector4(1.0f, 1.0f, 1.0f, 1.0f);
        RedColor = new Vector4(0.0f, 0.0f, 1.0f, 1.0f);
        IPText = "";
    }

    public override void Update()
    {
        // DNumpad characters, Numpad characters and period
        if((Input.GetLastKey() >= 48 && Input.GetLastKey() <= 57) || (Input.GetLastKey() >= 96 && Input.GetLastKey() <= 105) || 
            Input.GetLastKey() == 190)
        {
            IPText += Input.ConvertKeyToString(Input.GetLastKey());
        }
        else if(Input.GetLastKey() == 8)
        {
            if(IPText.Length > 0)
            {
                IPText = IPText.Remove(IPText.Length - 1);
            }
        }

        Camera.SetText("Test", IPText);
    }
}
