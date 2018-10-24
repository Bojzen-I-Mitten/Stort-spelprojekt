using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ThomasEngine;

public class SoundTest : ScriptComponent
{
    public Transform Listener { get; set; }

    SoundComponent Gun;

    public override void Start()
    {
        Gun = gameObject.GetComponent<SoundComponent>();
        Gun.Play();
        //Gun.Apply3D(Listener.position, gameObject.transform.position);
    }

    public override void Update()
    {
        //Gun.Update3D(Listener.position, gameObject.transform.position);
    }
}