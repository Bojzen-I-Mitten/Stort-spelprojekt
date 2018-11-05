using System;
using System.Collections;
using LiteNetLib;
using LiteNetLib.Utils;
using ThomasEngine;
using ThomasEngine.Network;

class ConfettiEmitter : NetworkComponent
{
    private ParticleEmitter emitterConfetti1;
    private ParticleEmitter emitterConfetti2;
    private ParticleEmitter emitterConfetti3;

    public Texture2D praticleTexture1 { get; set; }
    public Texture2D praticleTexture2 { get; set; }
    public Texture2D praticleTexture3 { get; set; }

    public override void Start()
    {
        emitterConfetti1 = gameObject.AddComponent<ParticleEmitter>();
        emitterConfetti2 = gameObject.AddComponent<ParticleEmitter>();
        emitterConfetti3 = gameObject.AddComponent<ParticleEmitter>();

        emitterConfetti1.MinSize = 0.03f;
        emitterConfetti1.MaxSize = 0.15f;
        emitterConfetti1.EndSize = 0.15f;
        emitterConfetti1.MinLifeTime = 1.0f;
        emitterConfetti1.MaxLifeTime = 1.5f;
        emitterConfetti1.EmissionRate = 100;
        emitterConfetti1.MinRotationSpeed = -50.0f;
        emitterConfetti1.MaxRotationSpeed = 50.0f;
        emitterConfetti1.MinSpeed = 7.5f;
        emitterConfetti1.MaxSpeed = 15.0f;
        emitterConfetti1.EndSpeed = 0.0f;
        emitterConfetti1.DistanceFromSphereCenter = 20;
        emitterConfetti1.Radius = 0.4f;
        emitterConfetti1.Gravity = 5.0f;
        emitterConfetti1.SpawnAtEdge = true;

        emitterConfetti2.MinSize = 0.03f;
        emitterConfetti2.MaxSize = 0.15f;
        emitterConfetti2.EndSize = 0.15f;
        emitterConfetti2.MinLifeTime = 0.8f;
        emitterConfetti2.MaxLifeTime = 1.3f;
        emitterConfetti2.EmissionRate = 125;
        emitterConfetti2.MinRotationSpeed = -25.0f;
        emitterConfetti2.MaxRotationSpeed = 25.0f;
        emitterConfetti2.MinSpeed = 12.0f;
        emitterConfetti2.MaxSpeed = 20.0f;
        emitterConfetti2.EndSpeed = 0.0f;
        emitterConfetti2.DistanceFromSphereCenter = 8;
        emitterConfetti2.Radius = 1.0f;
        emitterConfetti2.Gravity = 5.0f;
        emitterConfetti2.SpawnAtEdge = false;
    }

    public IEnumerator EmitForDuration(float duration)
    {
        emitterConfetti1.Emit = true;
        emitterConfetti2.Emit = true;

        yield return new WaitForSeconds(5.0f);

        emitterConfetti1.Emit = false;
        emitterConfetti2.Emit = false;
    }
}

