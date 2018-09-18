using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ThomasEngine;

namespace ThomasEditor
{
    public class ChadControls : ScriptComponent
    {
        public int speed { get; set; } = 5;
        public float turnSpeed { get; set; } = 3;

        public override void Start()
        {
        }

        public override void Update()
        {
            if (Input.GetKey(Input.Keys.W))
                transform.position += transform.forward * speed * Time.DeltaTime;
            if (Input.GetKey(Input.Keys.S))
                transform.position -= transform.forward * speed * Time.DeltaTime;
            if (Input.GetKey(Input.Keys.A))
                transform.RotateByAxis(new Vector3(0.0f, 1.0f, 0.0f), turnSpeed * Time.DeltaTime);
            if (Input.GetKey(Input.Keys.D))
                transform.RotateByAxis(new Vector3(0.0f, 1.0f, 0.0f), -turnSpeed * Time.DeltaTime);
        }
    }
}
