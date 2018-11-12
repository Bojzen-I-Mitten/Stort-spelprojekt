using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ThomasEngine;

public class SoundTest : ScriptComponent
{
    //public Transform Listener { get; set; }

    SoundComponent Gun;

    public override void Start()
    {
        Gun = gameObject.GetComponent<SoundComponent>();
        //Gun.Play();
    }

    public override void Update()
    {
        if(Input.GetKeyDown(Input.Keys.W))
        {
            Gun.Play();
        }

        if (Input.GetKeyDown(Input.Keys.E))
        {
            Gun.Pause(false);
        }
    }
}