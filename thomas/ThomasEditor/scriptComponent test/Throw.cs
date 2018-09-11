using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ThomasEngine;

namespace ThomasEditor
{
    public class Throw : ScriptComponent
    {
        public Vector3 force { get; set; }

        public override void Start()
        {
        }

        public override void Update()
        {
            gameObject.GetComponent<Rigidbody>().ApplyCentralForce(force);
        }
    }
}


