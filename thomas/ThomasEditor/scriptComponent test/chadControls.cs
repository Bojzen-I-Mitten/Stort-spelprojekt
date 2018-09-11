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

        float t = 0;

        public override void Start()
        {
        }

        public override void Update()
        {
            t = Time.DeltaTime;
            float x = gameObject.transform.position.x;
            float y = gameObject.transform.position.y;
            float z = gameObject.transform.position.z;

            Vector3 test = gameObject.transform.forward;
            float test2 = gameObject.transform.forward.z + t * speed;
            
            if (Input.GetKey(Input.Keys.W))
                gameObject.transform.position = new Vector3(gameObject.transform.position.x + gameObject.transform.forward.x * t * speed, gameObject.transform.position.y + gameObject.transform.forward.y * t*speed, gameObject.transform.position.z+ gameObject.transform.forward.z * t*speed);
            if (Input.GetKey(Input.Keys.S))
                gameObject.transform.position = new Vector3(gameObject.transform.position.x - gameObject.transform.forward.x * t * speed, gameObject.transform.position.y - gameObject.transform.forward.y * t * speed, gameObject.transform.position.z - gameObject.transform.forward.z * t * speed);
            if (Input.GetKey(Input.Keys.A))
                gameObject.transform.position = new Vector3(gameObject.transform.position.x - gameObject.transform.right.x * t * speed, gameObject.transform.position.y - gameObject.transform.right.y * t * speed, gameObject.transform.position.z - gameObject.transform.right.z * t * speed);
            if (Input.GetKey(Input.Keys.D))
                gameObject.transform.position = new Vector3(gameObject.transform.position.x + gameObject.transform.right.x * t * speed, gameObject.transform.position.y + gameObject.transform.right.y * t * speed, gameObject.transform.position.z + gameObject.transform.right.z * t * speed);
        }
    }
}
