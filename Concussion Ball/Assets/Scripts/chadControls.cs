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
        public float force { get; set; } = 5;

        Rigidbody rBody;

        public override void Start()
        {
            rBody = gameObject.GetComponent<Rigidbody>();
        }

        public override void Update()
        {
            //Jumping
            if (Input.GetKeyDown(Input.Keys.Space) && transform.position.y < 1.0f)
            {
                rBody.ApplyCentralImpulseForce(new Vector3(0.0f, force, 0.0f));   
            }   
            //WASD movement
            if (Input.GetKey(Input.Keys.S))
            {
                transform.position -= transform.forward * speed * Time.DeltaTime;
                if (Input.GetKey(Input.Keys.A))
                    transform.RotateByAxis(new Vector3(0.0f, 1.0f, 0.0f), -0.5f * Time.DeltaTime);
                if (Input.GetKey(Input.Keys.D))
                    transform.RotateByAxis(new Vector3(0.0f, 1.0f, 0.0f), 0.5f * Time.DeltaTime);
            }
            if (Input.GetKey(Input.Keys.W))
            {
                transform.position += transform.forward * speed * Time.DeltaTime;
                if (Input.GetKey(Input.Keys.A))
                    transform.RotateByAxis(new Vector3(0.0f, 1.0f, 0.0f), 0.5f * Time.DeltaTime);
                if (Input.GetKey(Input.Keys.D))
                    transform.RotateByAxis(new Vector3(0.0f, 1.0f, 0.0f), -0.5f * Time.DeltaTime);
            }
            //else
            //{ 
            //    if (Input.GetKey(Input.Keys.A))
            //        transform.RotateByAxis(new Vector3(0.0f, 1.0f, 0.0f), 0.5f * Time.DeltaTime);
            //    if (Input.GetKey(Input.Keys.D))
            //        transform.RotateByAxis(new Vector3(0.0f, 1.0f, 0.0f), -0.5f * Time.DeltaTime);
            //}

            //Avoid tumbling TEST??
            //Quaternion test = Quaternion.CreateFromYawPitchRoll(transform.rotation.y, 0.0f, 0.0f);
            //transform.rotation = test;
        }
    }
}
