using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ThomasEngine;

public class CameraMove : ScriptComponent
{
    public override void Start()
    {
    }

    public override void Update()
    {
        if (Input.GetKey(Input.Keys.W))
        {
            transform.position += new Vector3(0, 0, -0.5f);
        }
        if (Input.GetKey(Input.Keys.S))
        {
            transform.position += new Vector3(0, 0, 0.5f);
        }
        if (Input.GetKey(Input.Keys.A))
        {
            transform.position += new Vector3(-0.5f, 0, 0);
        }
        if (Input.GetKey(Input.Keys.D))
        {
            transform.position += new Vector3(0.5f, 0, 0);
        }
    }
}