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
        //public GameObject aDude;
        public int speed { get; set; }

        Camera chadCamera;
        float t = 0;

        public override void Start()
        {
            chadCamera = this.gameObject.GetComponent<Camera>();
        }

        public override void Update()
        {
            t = Time.DeltaTime;

            

            if (Input.GetKey(Input.Keys.W))
            {
                gameObject.transform.position = new Vector3(gameObject.transform.position.x + gameObject.transform.forward.x * t * speed, gameObject.transform.position.y + gameObject.transform.forward.y * t * speed, gameObject.transform.position.z + gameObject.transform.forward.z * t * speed);
            }
            if (Input.GetKey(Input.Keys.S))
                gameObject.transform.position = new Vector3(gameObject.transform.position.x - gameObject.transform.forward.x * t * speed, gameObject.transform.position.y - gameObject.transform.forward.y * t * speed, gameObject.transform.position.z - gameObject.transform.forward.z * t * speed);
            if (Input.GetKey(Input.Keys.A))
            {
                chadCamera.gameObject.transform.RotateByAxis(new Vector3(0.0f, 1.0f, 0.0f), 0.01f);
                //gameObject.transform.position = new Vector3(gameObject.transform.position.x - gameObject.transform.right.x * t * speed, gameObject.transform.position.y - gameObject.transform.right.y * t * speed, gameObject.transform.position.z - gameObject.transform.right.z * t * speed);

            }
            if (Input.GetKey(Input.Keys.D))
            {
                chadCamera.gameObject.transform.RotateByAxis(new Vector3(0.0f, 1.0f, 0.0f), -0.01f);
                //gameObject.transform.position = new Vector3(gameObject.transform.position.x + gameObject.transform.right.x * t * speed, gameObject.transform.position.y + gameObject.transform.right.y * t * speed, gameObject.transform.position.z + gameObject.transform.right.z * t * speed);
            }
        }
    }
}
