using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ThomasEngine;

public class SoundTest : ScriptComponent
{
    SoundComponent Gun;

    public override void Start()
    {
        Gun = gameObject.GetComponent<SoundComponent>();
        Gun.Play();
    }

    public override void Update()
    {
    }
}