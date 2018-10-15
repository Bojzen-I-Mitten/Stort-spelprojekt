using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ThomasEngine;

public class GUITest : ScriptComponent
{
    public Texture2D Sprite { get; set; }

    public override void Start()
    {
       
    }

    public override void Update()
    {
        GUI.AddImage("Chad", Sprite, new Vector2(200, 30), new Vector2(0.5f, 0.5f), true);

        if(GUI.OnImageClicked("Chad"))
        {
            Debug.Log("Clicked Chad");
        }
    }
}
