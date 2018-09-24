using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ThomasEngine;

namespace ThomasEditor
{
    public class Collider : ScriptComponent
    {
        public Vector3 torque { get; set; }

        public override void Start()
        {           
        }

        public override void Update()
        {
            gameObject.GetComponent<Rigidbody>().AddTorque(torque);
        }
    }
}


