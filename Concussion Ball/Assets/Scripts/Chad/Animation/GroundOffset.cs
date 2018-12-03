using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ThomasEngine;
using ThomasEngine.Script;

public class GroundOffset : ScriptComponent
{

    public GroundOffset() 
        : base()
    {
    }

    public override void OnAwake()
    {
        var skin = gameObject.GetComponent<RenderSkinnedComponent>();
        skin.setBlendTreeNode
    }

    public override void OnEnable()
    {
    }

    public override void Start()
    {
    }

    public override void OnDisable()
    {
    }

    public override void OnDestroy()
    {
    }

    public override void Update()
    {
    }

}

