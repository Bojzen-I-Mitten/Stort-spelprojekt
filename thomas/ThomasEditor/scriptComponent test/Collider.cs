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
        private GameObject collider;

        public override void Start()
        {
            collider = new GameObject("Something");
        }

        public override void Update()
        {
        }

        public override void OnCollisionEnter()
        {
            if (gameObject.GetComponent<Rigidbody>().HasCollided(collider))
            {
                //Debug.Log(collider.Name);

                //if (collider.Name == "Ball")
                //{
                //    Debug.Log("Collided with ball");
                //}
            }
        }
    }
}


