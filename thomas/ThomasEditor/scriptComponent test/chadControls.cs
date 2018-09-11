using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ThomasEngine;

namespace ThomasEditor
{
    public class chadControls : ScriptComponent
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

            float xMult = 0;
            float zMult = 0;
            float localSpeed = speed;


            if (Input.GetKey(Input.Keys.W))
                zMult = -1;
            if (Input.GetKey(Input.Keys.S))
                zMult = 1;
            if (Input.GetKey(Input.Keys.A))
                xMult = -1;
            if (Input.GetKey(Input.Keys.D))
                xMult = 1;

            //Avoid double speed diagonally
            if (xMult != 0 && zMult != 0)
            {
                localSpeed *= 0.67f;
            }

            gameObject.transform.position = new Vector3(x + (localSpeed * t * xMult), y, z + (localSpeed * t * zMult));
        }
    }
}
