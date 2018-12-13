using System;
using System.Collections;
using LiteNetLib;
using LiteNetLib.Utils;
using ThomasEngine;
using ThomasEngine.Network;
public class Ball : PickupableObject
{
    public Vector3 SpawnPoint { get; set; } = new Vector3(0, 10, 0);

    private ParticleEmitter emitterElectricity1;
    private ParticleEmitter emitterElectricity2;
    private ParticleEmitter emitterElectricity3;
    private ParticleEmitter emitterSmoke;
    private ParticleEmitter emitterFire;

    private LightComponent pointLight;

    private SoundComponent soundFireThrow;
    private SoundComponent soundChargeBall;
    private bool begunCharge;

    private float ThrowForce66Procent;
    private float electricityIntensifyerThreshold;


    public override void Start()
    {
        base.Start();
        m_throwable = true;
        DropOnRagdoll = true;
        MovementSpeedModifier = 0.65f;
        chargeTimeMax = 2.0f;
        BaseThrowForce = 12.0f;
        MaxThrowForce = 18.0f;
        ThrowForce66Procent = BaseThrowForce + (MaxThrowForce - BaseThrowForce) * 0.66f;
        Debug.Log(ThrowForce66Procent);
        ThrowForce = BaseThrowForce;

        soundFireThrow = gameObject.AddComponent<SoundComponent>();
        soundFireThrow.Clip = (AudioClip)Resources.LoadThomasPath("%THOMAS_ASSETS%/Sounds/ThrowBall.wav");
        soundFireThrow.Looping = false;
        soundFireThrow.Type = SoundComponent.SoundType.Effect;
        soundFireThrow.MaxDistance = 45;
        soundFireThrow.MinDistance = 10;
        soundFireThrow.Is3D = true;


        soundChargeBall = gameObject.AddComponent<SoundComponent>();
        soundChargeBall.Clip = (AudioClip)Resources.LoadThomasPath("%THOMAS_ASSETS%/Sounds/ElectricSparks.mp3");
        soundChargeBall.Looping = false;
        soundChargeBall.Type = SoundComponent.SoundType.Effect;
        soundChargeBall.Volume = 0.01f;
        soundChargeBall.MaxDistance = 10;
        soundChargeBall.MinDistance = 3;
        soundChargeBall.Is3D = true;
        begunCharge = false;

        pointLight = gameObject.AddComponent<LightComponent>();
        pointLight.Type = LightComponent.LIGHT_TYPES.POINT;
        pointLight.Intensity = 0;
        pointLight.DiffuseColor = Color.OrangeRed;

        #region Init emitters
        emitterElectricity1 = gameObject.AddComponent<ParticleEmitter>();
        emitterElectricity2 = gameObject.AddComponent<ParticleEmitter>();
        emitterElectricity3 = gameObject.AddComponent<ParticleEmitter>();
        emitterSmoke = gameObject.AddComponent<ParticleEmitter>();
        emitterFire = gameObject.AddComponent<ParticleEmitter>();

        emitterElectricity1.Texture = (Texture2D)Resources.LoadThomasPath("%THOMAS_ASSETS%/Particles/el1.png");
        emitterElectricity2.Texture = (Texture2D)Resources.LoadThomasPath("%THOMAS_ASSETS%/Particles/el2.png");
        emitterElectricity3.Texture = (Texture2D)Resources.LoadThomasPath("%THOMAS_ASSETS%/Particles/el3.png");
        emitterSmoke.Texture = (Texture2D)Resources.LoadThomasPath("%THOMAS_ASSETS%/Particles/SmokeParticle.png");
        emitterFire.Texture =  (Texture2D)Resources.LoadThomasPath("%THOMAS_ASSETS%/Particles/fire.png");//fireTex;

        ResetFireEmitters();
        ResetElectricityEmitters();

        
        #endregion

        Rigidbody rb = gameObject.GetComponent<Rigidbody>();
        rb.CcdMotionThreshold = 1e-7f;
        rb.CcdSweptSphereRadius = 0.1f;
    }
    #region Particle handling
    private void ResetFireEmitters()
    {
        emitterSmoke.MinSize = 0.1f;
        emitterSmoke.MaxSize = 0.2f;
        emitterSmoke.EndSize = 0.5f;
        emitterSmoke.MinLifeTime = 1.5f;
        emitterSmoke.MaxLifeTime = 2.2f;
        emitterSmoke.EmissionRate = 40;
        emitterSmoke.MinRotationSpeed = -2.0f;
        emitterSmoke.MaxRotationSpeed = 2.0f;
        emitterSmoke.MinSpeed = 0.2f;
        emitterSmoke.MaxSpeed = 0.5f;
        emitterSmoke.EndSpeed = 0.0f;
        emitterSmoke.DistanceFromSphereCenter = 0;
        emitterSmoke.Gravity = -0.3f;
        emitterSmoke.Radius = 0.7f;

        emitterFire.BlendState = ParticleEmitter.BLEND_STATES.ADDITIVE;
        emitterFire.MinSize = 0.1f;
        emitterFire.MaxSize = 0.5f;
        emitterFire.EndSize = 0.0f;
        emitterFire.MinLifeTime = 0.5f;
        emitterFire.MaxLifeTime = 1.2f;
        emitterFire.EmissionRate = 30;
        emitterFire.MinRotationSpeed = -4.0f;
        emitterFire.MaxRotationSpeed = 4.0f;
        emitterFire.MinSpeed = 0.2f;
        emitterFire.MaxSpeed = 0.5f;
        emitterFire.EndSpeed = 0.0f;
        emitterFire.Gravity = -1;
        emitterFire.DistanceFromSphereCenter = 0;
        emitterFire.Radius = 0.7f;

        emitterFire.Emit = false;
        emitterSmoke.Emit = false;

        pointLight.Intensity = 0;
    }

    private void ResetElectricityEmitters()
    {
        electricityIntensifyerThreshold = 0.3f;

        emitterElectricity1.MinSize = 0.1f;
        emitterElectricity1.MaxSize = 0.3f;
        emitterElectricity1.EndSize = 0.8f;
        emitterElectricity1.MinLifeTime = 0.03f;
        emitterElectricity1.MaxLifeTime = 0.08f;
        emitterElectricity1.EmissionRate = 10;
        emitterElectricity1.MinRotationSpeed = 0.1f;
        emitterElectricity1.MaxRotationSpeed = 1.0f;
        emitterElectricity1.MinSpeed = 0.1f;
        emitterElectricity1.MaxSpeed = 0.4f;
        emitterElectricity1.EndSpeed = 1.3f;
        emitterElectricity1.DistanceFromSphereCenter = 0;
        emitterElectricity1.Radius = 0.5f;
        emitterElectricity1.SpawnAtEdge = true;

        emitterElectricity2.MinSize = 0.25f;
        emitterElectricity2.MaxSize = 0.5f;
        emitterElectricity2.EndSize = 0.05f;
        emitterElectricity2.MinLifeTime = 0.03f;
        emitterElectricity2.MaxLifeTime = 0.1f;
        emitterElectricity2.EmissionRate = 10;
        emitterElectricity2.MinRotationSpeed = 2.1f;
        emitterElectricity2.MaxRotationSpeed = -2.0f;
        emitterElectricity2.MinSpeed = 0.8f;
        emitterElectricity2.MaxSpeed = 2.4f;
        emitterElectricity2.EndSpeed = -2.3f;
        emitterElectricity2.DistanceFromSphereCenter = 0;
        emitterElectricity2.Radius = 1.0f;

        emitterElectricity3.MinSize = 0.4f;
        emitterElectricity3.MaxSize = 1.0f;
        emitterElectricity3.EndSize = 0.7f;
        emitterElectricity3.MinLifeTime = 0.01f;
        emitterElectricity3.MaxLifeTime = 0.05f;
        emitterElectricity3.EmissionRate = 25;
        emitterElectricity3.MinRotationSpeed = -2.1f;
        emitterElectricity3.MaxRotationSpeed = -1.0f;
        emitterElectricity3.MinSpeed = 1.1f;
        emitterElectricity3.MaxSpeed = 1.4f;
        emitterElectricity3.EndSpeed = 1.3f;
        emitterElectricity3.DistanceFromSphereCenter = 0;
        emitterElectricity3.Radius = 0.7f;
        emitterElectricity3.SpawnAtEdge = true;

        MultiplyWithIntensity((float)(0.3f), emitterElectricity1);
        MultiplyWithIntensity((float)(0.3f), emitterElectricity2);
        MultiplyWithIntensity((float)(0.3f), emitterElectricity3);

        emitterElectricity1.Emit = false;
        emitterElectricity2.Emit = false;
        emitterElectricity3.Emit = false;

    }

    private void MultiplyWithIntensity(float intensity, ParticleEmitter emitter)
    {
        emitter.MinSize *= intensity;
        emitter.MaxSize *= intensity;
        emitter.EndSize *= intensity;
        emitter.MinLifeTime *= intensity;
        emitter.MaxLifeTime *= intensity;
        emitter.EmissionRate = (uint)MathHelper.Max(((float)emitterElectricity3.EmissionRate * intensity), 0.0f);
        emitter.MinRotationSpeed *= intensity;
        emitter.MaxRotationSpeed *= intensity;
        emitter.MinSpeed *= intensity;
        emitter.MaxSpeed *= intensity;
        emitter.EndSpeed *= intensity;
        emitter.Radius *= intensity;
    }

    override public void StopEmitting()
    {
        StartCoroutine(StopEmission());
    }

    private IEnumerator StopEmission()
    {
        float timer = 3;
        while (timer > 0)
        {
            timer -= Time.DeltaTime;
            yield return null;
        }

        
        ResetElectricityEmitters();
        ResetFireEmitters();
    }
    #endregion

    public override void Update()
    {
        base.Update();
        if (transform.position.y < -5)
            Reset();

        if (charging)
        {
            if (!begunCharge)
            {
                begunCharge = true;
                soundChargeBall.Play();
            }
        }
        else
        {
            begunCharge = false;
            soundChargeBall.Stop();
        }
    }

    public override void Throw(Vector3 camPos, Vector3 direction)
    {
        RPCParticles();
        SendRPC("RPCParticles");
        direction.y += 0.2f;
        if (ThrowForce > ThrowForce66Procent)
            soundFireThrow.Play();
        base.Throw(camPos, Vector3.Normalize(direction) * ThrowForce);
    }

    public void RPCParticles()
    {
        if (ThrowForce > MaxThrowForce - 0.01f)
        {
            //emitterElectricity1.EmitOneShot(10);
            //emitterElectricity2.EmitOneShot(10);
            //emitterElectricity3.EmitOneShot(10);
            //emitterFire.EmitOneShot(25);


        }
        if (ThrowForce > ThrowForce66Procent)
        {
            
            emitterFire.Emit = true;
            emitterSmoke.Emit = true;

        }
        
    }
    

    override public void ChargeEffect()
    {
        float interp = MathHelper.Min(GetChargeTime() / chargeTimeMax, 1.0f);
        if (interp > 0)
        {
            
            //emitterElectricity1.Emit = true;
            //emitterElectricity2.Emit = true;
            //emitterElectricity3.Emit = true;

            pointLight.Intensity = interp;


            if (interp > 0.7f)
            {
                emitterSmoke.Emit = true;
            }
            if (interp > 0.99f)
            {
                emitterFire.Emit = true;
            }

            if (interp > electricityIntensifyerThreshold)
            {
                MultiplyWithIntensity(1.25f, emitterElectricity1);
                MultiplyWithIntensity(1.25f, emitterElectricity2);
                MultiplyWithIntensity(1.25f, emitterElectricity3);

                electricityIntensifyerThreshold += 0.1f;
            }
        }
    }

    override public void Cleanup()
    {
        base.Cleanup();

        StartCoroutine(CleanTimer());
    }

    public override void OnDrop()
    {
        base.OnDrop();
        soundChargeBall.Stop();
        begunCharge = false;
        StartCoroutine(PickupDelay());
    }
    IEnumerator PickupDelay()
    {
        PickupCollider.enabled = false;
        yield return new WaitForSecondsRealtime(0.5f);
        PickupCollider.enabled = true;
    }

    public override void Pickup(ChadControls chad, Transform hand)
    {
        base.Pickup(chad, hand);
    }


    public override void Disable()
    {
        base.Disable();
    }

    IEnumerator ReturnToMiddle()
    {
        yield return new WaitForSecondsRealtime(0.3f);
        m_rigidBody.Position = Vector3.Zero;
        m_rigidBody.Rotation = Quaternion.Identity;
        m_rigidBody.LinearVelocity = Vector3.Zero;
        m_rigidBody.AngularVelocity = Vector3.Zero;
    }
    public override void Reset()
    {
        base.Reset();
        StartCoroutine(ReturnToMiddle());
    }

    private IEnumerator CleanTimer()
    {
        yield return new WaitForSeconds(2.0f);
        
        ResetFireEmitters();
        ResetElectricityEmitters();
    }
}