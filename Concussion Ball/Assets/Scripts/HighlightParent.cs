using System;
using System.Collections;
using LiteNetLib;
using LiteNetLib.Utils;
using ThomasEngine;
using ThomasEngine.Network;

public class HighlightParent : ScriptComponent
{
    private Color Color = Color.White;// { get; set; }
    private Ball Ball;
    private Material highLightMat;
    public Vector3 scale { get; set; } = new Vector3(1.1f);

    public override void Start()
    {
        gameObject.transform.localPosition = new Vector3(0,0,0);
        gameObject.transform.localRotation = new Quaternion(0, 0, 0, 0);
        gameObject.transform.localScale = new Vector3(1.001f, 1.001f, 1.001f);

        Shader s = Shader.Find("HighlightObject");
        highLightMat = new Material(s);
        highLightMat.SetColor("color", Color);
        //highLightMat->SetRenderQueue(2001);
        
        RenderComponent rc = gameObject.AddComponent<RenderComponent>();
        rc.model = gameObject.transform.parent.gameObject.GetComponent<RenderComponent>().model;
        //transform.scale *= 5.0f;

        rc.material = highLightMat;

        Ball = transform.parent.transform.parent.gameObject.GetComponent<Ball>();
    }

    public override void Update()
    {
        if(Ball._Chad == MatchSystem.instance.LocalChad)
        {
            gameObject.transform.localScale = new Vector3(0, 0, 0);
        }
        else
        {
            if (Ball?._Chad)
                Color = Ball._Chad.NetPlayer.Team.Color;
            else
                Color = Color.White;
            highLightMat.SetColor("color", Color);
            gameObject.transform.localScale = new Vector3(2.001f, 2.001f, 2.001f);
        }
    }
}

