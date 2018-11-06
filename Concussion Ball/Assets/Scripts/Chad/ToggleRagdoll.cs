using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ThomasEngine;

public class ToggleRagdoll : ScriptComponent
{
    public Ragdoll ragdoll { set; get; }
    public override void Start()
    {

    }

    public override void Update()
    {
        if (Input.GetKeyDown(Input.Keys.Space))
        {
            if (ragdoll.RagdollEnabled == false)
                ragdoll.EnableRagdoll();
            else
                ragdoll.DisableRagdoll();
        }
    }
}
