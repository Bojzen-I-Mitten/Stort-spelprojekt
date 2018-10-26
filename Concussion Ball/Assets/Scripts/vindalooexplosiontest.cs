using System;
using System.Collections;
using LiteNetLib;
using LiteNetLib.Utils;
using ThomasEngine;
using ThomasEngine.Network;
public class vindalooexplosiontest : NetworkComponent
{
    public Texture2D fireTexture { get; set; }
    public Texture2D fire2Texture { get; set; }
    public Texture2D smokeTexture { get; set; }
    public Texture2D gravelTexture { get; set; }
    private ParticleEmitter emitterFire;
    private ParticleEmitter emitterFire2;
    private ParticleEmitter emitterSmoke;
    private ParticleEmitter emitterGravel;
    private float cooldown;
    public override void Start()
    {
        emitterFire = gameObject.AddComponent<ParticleEmitter>();
        emitterFire.MinSize = 1.2f;
        emitterFire.MaxSize = 1.99f;
        emitterFire.EndSize = 1.6f;
        emitterFire.MinLifeTime = 0.1f;
        emitterFire.MaxLifeTime = 0.2f;
        emitterFire.EmissionRate = 100;
        emitterFire.MinRotationSpeed = 0.0f;
        emitterFire.MaxRotationSpeed = 4.0f;
        emitterFire.MinSpeed = 5.5f;
        emitterFire.MaxSpeed = 10.0f;
        emitterFire.EndSpeed = 20.0f;
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
        emitterGravel.MinRotationSpeed = -6.0f;
        emitterGravel.MaxRotationSpeed = 6.0f;
        emitterGravel.MinSpeed = 13.5f;
        emitterGravel.MaxSpeed = 18.0f;
        emitterGravel.EndSpeed = 0.0f;
        emitterGravel.DistanceFromSphereCenter = 0.3f;
        emitterGravel.Radius = 0.2f;
        emitterGravel.Gravity = 4.0f;
        emitterGravel.SpawnAtEdge = false;
        emitterGravel.Texture = gravelTexture;

        emitterFire2 = gameObject.AddComponent<ParticleEmitter>();
        emitterFire2.MinSize = 0.1f;
        emitterFire2.MaxSize = 0.2f;
        emitterFire2.EndSize = 0.05f;
        emitterFire2.MinLifeTime = 0.4f;
        emitterFire2.MaxLifeTime = 1.3f;
        emitterFire2.EmissionRate = 100;
        emitterFire2.MinRotationSpeed = -2.0f;
        emitterFire2.MaxRotationSpeed = 3.0f;
        emitterFire2.MinSpeed = 13.5f;
        emitterFire2.MaxSpeed = 18.0f;
        emitterFire2.EndSpeed = 0.0f;
        emitterFire2.DistanceFromSphereCenter = 0.3f;
        emitterFire2.Radius = 0.2f;
        emitterFire2.Gravity = 8.0f;
        emitterFire2.SpawnAtEdge = false;
        emitterFire2.Texture = fire2Texture;

        emitterSmoke = gameObject.AddComponent<ParticleEmitter>();
        
        emitterSmoke.Texture = smokeTexture;

        emitterSmoke.MinSize = 0.5f;
        emitterSmoke.MaxSize = 0.7f;
        emitterSmoke.EndSize = 1.3f;
        emitterSmoke.MinLifeTime = 0.7f;
        emitterSmoke.MaxLifeTime = 2.6f;
        emitterSmoke.EmissionRate = 100;
        emitterSmoke.MinRotationSpeed = -2.0f;
        emitterSmoke.MaxRotationSpeed = 2.0f;
        emitterSmoke.MinSpeed = 0.6f;
        emitterSmoke.MaxSpeed = 1.5f;
        emitterSmoke.EndSpeed = 0.0f;
        emitterSmoke.DistanceFromSphereCenter = 0.7f;
        emitterSmoke.Radius = 1.7f;
        emitterGravel.SpawnAtEdge = false;

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
                emitterFire2.EmitOneShot(45);
                emitterGravel.EmitOneShot(20);
                emitterSmoke.EmitOneShot(60);
                cooldown = 2.0f;
            }
        }
    }
}

