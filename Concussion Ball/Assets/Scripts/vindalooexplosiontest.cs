using System;
using System.Collections;
using LiteNetLib;
using LiteNetLib.Utils;
using ThomasEngine;
using ThomasEngine.Network;
public class vindalooexplosiontest : NetworkComponent
{
    public Texture2D fireTexture { get; set; }
    public Texture2D smokeTexture { get; set; }
    public Texture2D gravelTexture { get; set; }
    private ParticleEmitter emitterFire;
    private ParticleEmitter emitterSmoke;
    private ParticleEmitter emitterGravel;
    private float cooldown;
    public override void Start()
    {
        emitterFire = gameObject.AddComponent<ParticleEmitter>();
        emitterFire.MinSize = 0.3f;
        emitterFire.MaxSize = 0.6f;
        emitterFire.EndSize = 1.3f;
        emitterFire.MinLifeTime = 0.1f;
        emitterFire.MaxLifeTime = 0.2f;
        emitterFire.EmissionRate = 100;
        emitterFire.MinRotationSpeed = 0.0f;
        emitterFire.MaxRotationSpeed = 4.0f;
        emitterFire.MinSpeed = 5.5f;
        emitterFire.MaxSpeed = 15.0f;
        emitterFire.EndSpeed = 0.0f;
        emitterFire.DistanceFromSphereCenter = 0;
        emitterFire.Radius = 1.0f;
        emitterFire.Gravity = 0.0f;
        emitterFire.SpawnAtEdge = false;
        emitterFire.Texture = fireTexture;
        emitterFire.BlendState = ParticleEmitter.BLEND_STATES.ADDITIVE;

        emitterGravel = gameObject.AddComponent<ParticleEmitter>();
        emitterGravel.MinSize = 0.1f;
        emitterGravel.MaxSize = 0.2f;
        emitterGravel.EndSize = 0.05f;
        emitterGravel.MinLifeTime = 0.4f;
        emitterGravel.MaxLifeTime = 1.3f;
        emitterGravel.EmissionRate = 100;
        emitterGravel.MinRotationSpeed = -2.0f;
        emitterGravel.MaxRotationSpeed = 3.0f;
        emitterGravel.MinSpeed = 13.5f;
        emitterGravel.MaxSpeed = 18.0f;
        emitterGravel.EndSpeed = 0.0f;
        emitterGravel.DistanceFromSphereCenter = 0.3f;
        emitterGravel.Radius = 0.2f;
        emitterGravel.Gravity = 8.0f;
        emitterGravel.SpawnAtEdge = false;
        emitterGravel.Texture = gravelTexture;

        emitterSmoke = gameObject.AddComponent<ParticleEmitter>();
        
        emitterSmoke.Texture = smokeTexture;

        emitterSmoke.MinSize = 0.6f;
        emitterSmoke.MaxSize = 0.8f;
        emitterSmoke.EndSize = 1.3f;
        emitterSmoke.MinLifeTime = 1.3f;
        emitterSmoke.MaxLifeTime = 2.2f;
        emitterSmoke.EmissionRate = 100;
        emitterSmoke.MinRotationSpeed = -2.0f;
        emitterSmoke.MaxRotationSpeed = 2.0f;
        emitterSmoke.MinSpeed = 1.2f;
        emitterSmoke.MaxSpeed = 2.5f;
        emitterSmoke.EndSpeed = 0.0f;
        emitterSmoke.DistanceFromSphereCenter = 3;
        emitterSmoke.Radius = 1.7f;
        emitterGravel.SpawnAtEdge = true;

        cooldown = 0.0f;
    }

    public override void Update()
    {
        cooldown -= Time.DeltaTime;
        if (Input.GetKey(Input.Keys.J))
        {
            if (cooldown < 0.0f)
            {
                emitterFire.EmitOneShot(25);
                emitterGravel.EmitOneShot(50);
                emitterSmoke.EmitOneShot(40);
                cooldown = 2.0f;
            }
        }
    }
}

