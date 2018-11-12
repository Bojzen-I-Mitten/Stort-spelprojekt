using System;
using System.Collections;
using LiteNetLib;
using LiteNetLib.Utils;
using ThomasEngine;
using ThomasEngine.Network;

public class HighlightParent : NetworkComponent
{
    private Color color = Color.White;// { get; set; }
    private Material highLightMat;

    public override void Start()
    {
        gameObject.transform.localPosition = new Vector3(0,0,0);
        gameObject.transform.localRotation = new Quaternion(0, 0, 0, 0);
        gameObject.transform.localScale = new Vector3(1.001f, 1.001f, 1.001f);

        Shader s = Shader.Find("HighlightObject");
        highLightMat = new Material(s);
        highLightMat.SetColor("color", color);
        //highLightMat->SetRenderQueue(2001);
        
        RenderComponent rc = gameObject.AddComponent<RenderComponent>();
        rc.model = gameObject.transform.parent.gameObject.GetComponent<RenderComponent>().model;
        transform.scale *= 1.1f;

        rc.material = highLightMat;
    }

    public override void Update()
    {
        
    }
}

