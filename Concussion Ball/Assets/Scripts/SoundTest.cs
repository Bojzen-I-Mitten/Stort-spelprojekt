using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ThomasEngine;

public class SoundTest : ScriptComponent
{
    SoundComponent Sound;

    public override void Start()
    {
        Sound = gameObject.GetComponent<SoundComponent>();
        Sound.PlayOneShot(Sound.clip.Name, Sound.Volume);
        //Sound.Play();
    }

    public override void Update()
    {
        //if(Input.GetKey(Input.Keys.W))
        //{
        //    //Debug.Log("W");
        //    Sound.PlayOneShot(Sound.clip.Name, Sound.Volume);
        //}

    }
}