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
        public override void Start()
        {
        }

        public override void Update()
        {
        }

        public override void OnCollisionEnter(GameObject collider)
        {
            //Debug.Log(collider.Name);     
        }
    }
}


