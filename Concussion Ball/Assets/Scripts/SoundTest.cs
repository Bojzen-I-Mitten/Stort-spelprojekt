using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ThomasEngine;

public class SoundTest : ScriptComponent
{
    public AudioClip Sound { get; set; }
    private SoundComponent ExplosionSound;

    public override void Start()
    {
        ExplosionSound = gameObject.AddComponent<SoundComponent>();
        ExplosionSound.Type = SoundComponent.SoundType.Effect;
        ExplosionSound.Clip = Sound;
        ExplosionSound.Looping = true;
        ExplosionSound.Is3D = true;
        ExplosionSound.Volume = 0.5f;

        // 3D options
        ExplosionSound.MaxDistance = 100;
        ExplosionSound.MinDistance = 3;
        ExplosionSound.SpreadAngle = 0;
        ExplosionSound.Play();
    }

    public override void Update()
    {
        if(Input.GetKeyDown(Input.Keys.W))
        {
            ExplosionSound.Stop();
        }
    }
}