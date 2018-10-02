using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ThomasEngine;

namespace ThomasEditor
{
    public class Collider : ScriptComponent
    {
        [Description("Test description")]
        public Vector3 torque { get; set; }

        public override void Start()
        {
        }

        public override void Update()
        {
        }
    }
}


