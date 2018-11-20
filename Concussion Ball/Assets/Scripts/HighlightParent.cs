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
    private bool hasParent;
    private bool canBePickedUp;

    public override void Start()
    {
        hasParent = false;
        canBePickedUp = false;

        gameObject.transform.localPosition = new Vector3(0,0,0);
        gameObject.transform.localRotation = new Quaternion(0, 0, 0, 0);
        gameObject.transform.localScale = new Vector3(1.001f, 1.001f, 1.001f);

        Shader s = Shader.Find("HighlightObject");
        highLightMat = new Material(s);
        highLightMat.SetColor("color", color);
        //highLightMat->SetRenderQueue(2001);
        
        RenderComponent rc = gameObject.AddComponent<RenderComponent>();
        rc.model = gameObject.transform.parent.gameObject.GetComponent<RenderComponent>().model;
        transform.scale *= 5.0f;

        rc.material = highLightMat;
    }

    public override void Update()
    {
        //Check if there even is a ball
        if(gameObject.transform.parent.parent.gameObject)
        {
            // check if ball can be picked up
            canBePickedUp = gameObject.transform.parent.parent.gameObject.GetComponent<Ball>().PickupCollider.enabled;
            
            // if ball had no owner, but has just been picked up
            if (!hasParent && !canBePickedUp)// && gameObject.transform.parent.parent.parent.parent.parent.gameObject)
            {
                Debug.Log("Ball picked up");
                hasParent = true;
                transform.scale /= 5.0f;

                Debug.Log(gameObject.transform.parent.parent.parent.parent.gameObject.Name);
                Debug.Log(gameObject.transform.parent.parent.parent.parent.gameObject.GetComponent<RenderComponent>().material.GetColor("color"));
                // time to update them colors
                highLightMat.SetColor("color", gameObject.transform.parent.parent.parent.parent.gameObject.GetComponent<RenderComponent>().material.GetColor("color"));
                // highLightMat.SetColor("color", gameObject.transform.parent.parent.gameObject.GetComponent<RenderComponent>().material.GetColor("color"));
            }
            // if ball was owned by parent, but has just been dropped
            else if (hasParent && canBePickedUp)
            {
                Debug.Log("Ball dropped/thrown");
                hasParent = false;
                transform.scale *= 5.0f;

                // time to update them colors
                highLightMat.SetColor("color", Color.White);
            }
        }
    }
}

