using System;
using System.Collections;
using System.ComponentModel;
using System.Linq;
using ThomasEngine;
using ThomasEngine.Network;

public class GUIScoreScreen : ScriptComponent
{
    public static GUIScoreScreen Instance = null;
    Camera cam;
    Canvas Canvas;
    public bool ToggleBool = true;
    public override void Awake()
    {
        if (!Instance)
            Instance = this;
        else
        {
            Destroy(this);
            return;
        }
        ToggleBool = true;
    }
    public override void Start()
    {
       
        ToggleBool = true;
    }
    public override void Update()
    {
       
    }
    public void DisplayBar(Vector2 OnOff)
    {

    }
   public void Toggle(bool OnOff)
    {
        ToggleBool = OnOff;
        GUIPlayerScore.Instance.lastupdate();

        if (!OnOff)
        GUIPlayerScore.Instance.Toggle = false;
    }
    public bool getToggleBool()
    {
        return ToggleBool;
    }
}
