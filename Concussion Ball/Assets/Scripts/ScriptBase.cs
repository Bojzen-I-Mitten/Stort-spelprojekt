using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.ComponentModel;
using ThomasEngine;
using ThomasEngine.Script;

public class ScriptBase : ScriptComponent
{
    //[Browsable(false)]
    //[Newtonsoft.Json.JsonIgnore]

    public ScriptBase()
        : base()
    {
    }


    public override void OnAwake()
    {
        base.OnAwake();
    }

    public override void OnEnable()
    {
        base.OnEnable();
    }

    public override void Start()
    {
        base.Start();
    }

    public override void OnDisable()
    {
        base.OnDisable();
    }

    public override void OnDestroy()
    {
        base.OnDestroy();
    }

    public override void Update()
    {
        base.Update();
    }
}
