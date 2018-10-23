using System;
using System.Collections;
using LiteNetLib;
using LiteNetLib.Utils;
using ThomasEngine;
using ThomasEngine.Network;
public class Ball : PickupObjects
{
    private float accumulator;
    private float chargeupTime;

    private ParticleEmitter emitterElectricity1;
    private ParticleEmitter emitterElectricity2;
    private ParticleEmitter emitterElectricity3;
    private ParticleEmitter emitterSmoke;
    private ParticleEmitter emitterFire;
    public Texture2D electricityTex1 { get; set; }
    public Texture2D electricityTex2 { get; set; }
    public Texture2D electricityTex3 { get; set; }
    public Texture2D smokeTex { get; set; }
    public Texture2D fireTex { get; set; }

    private Rigidbody rigidbody;
    private RenderComponent renderComponent;
    //public bool PickedUp { get { if (rigidbody != null) return !rigidbody.enabled; else return false; } set { if (rigidbody != null) rigidbody.enabled = !value; } }
    public float chargeTimeCurrent;
    private float chargeTimeMax;
    private float electricityIntensifyerThreshold;
    private float fireIntensityreThreshold;

    public override void Start()
    {
        chargeTimeMax = 4.0f;
        chargeTimeCurrent = 0.0f;
        rigidbody = gameObject.GetComponent<Rigidbody>();

        renderComponent = gameObject.GetComponent<RenderComponent>();
        renderComponent.material.SetColor("color", new Color(0, 0, 255));

        emitterElectricity1 = gameObject.AddComponent<ParticleEmitter>();
        emitterElectricity2 = gameObject.AddComponent<ParticleEmitter>();
        emitterElectricity3 = gameObject.AddComponent<ParticleEmitter>();
        emitterSmoke = gameObject.AddComponent<ParticleEmitter>();
        emitterFire = gameObject.AddComponent<ParticleEmitter>();

        emitterElectricity1.Texture = electricityTex1;
        emitterElectricity2.Texture = electricityTex2;
        emitterElectricity3.Texture = electricityTex3;
        emitterSmoke.Texture = smokeTex;
        emitterFire.Texture = fireTex;

        ResetFireEmitters();
        ResetElectricityEmitters();
        fireIntensityreThreshold = 0.6f;
        electricityIntensifyerThreshold = 0.6f;


        MultiplyWithIntensity((float)(0.5f), emitterElectricity1);
        MultiplyWithIntensity((float)(0.5f), emitterElectricity2);
        MultiplyWithIntensity((float)(0.5f), emitterElectricity3);
    }

    private void ResetFireEmitters()
    {
        emitterSmoke.MinSize = 0.1f;
        emitterSmoke.MaxSize = 0.3f;
        emitterSmoke.EndSize = 0.7f;
        emitterSmoke.MinLifeTime = 1.0f;
        emitterSmoke.MaxLifeTime = 2.2f;
        emitterSmoke.EmissionRate = 100;
        emitterSmoke.MinRotationSpeed = -2.0f;
        emitterSmoke.MaxRotationSpeed = 2.0f;
        emitterSmoke.MinSpeed = 0.2f;
        emitterSmoke.MaxSpeed = 0.5f;
        emitterSmoke.EndSpeed = 0.0f;
        emitterSmoke.DistanceFromSphereCenter = 0;
        emitterSmoke.Radius = 0.7f;

        emitterFire.MinSize = 0.1f;
        emitterFire.MaxSize = 0.5f;
        emitterFire.EndSize = 0.0f;
        emitterFire.MinLifeTime = 0.5f;
        emitterFire.MaxLifeTime = 1.2f;
        emitterFire.EmissionRate = 10;
        emitterFire.MinRotationSpeed = -2.0f;
        emitterFire.MaxRotationSpeed = 2.0f;
        emitterFire.MinSpeed = 0.2f;
        emitterFire.MaxSpeed = 0.5f;
        emitterFire.EndSpeed = 0.0f;
        emitterFire.Gravity = -1;
        emitterFire.DistanceFromSphereCenter = 0;
        emitterFire.Radius = 0.7f;


    }

    private void ResetElectricityEmitters()
    {
        electricityIntensifyerThreshold = 0.6f;

        emitterElectricity1.MinSize = 0.1f;
        emitterElectricity1.MaxSize = 0.3f;
        emitterElectricity1.EndSize = 1.0f;
        emitterElectricity1.MinLifeTime = 0.2f;
        emitterElectricity1.MaxLifeTime = 0.1f;
        emitterElectricity1.EmissionRate = 10;
        emitterElectricity1.MinRotationSpeed = 0.1f;
        emitterElectricity1.MaxRotationSpeed = 2.0f;
        emitterElectricity1.MinSpeed = 0.1f;
        emitterElectricity1.MaxSpeed = 0.4f;
        emitterElectricity1.EndSpeed = 1.3f;
        emitterElectricity1.DistanceFromSphereCenter = 0;
        emitterElectricity1.Radius = 0.5f;
        emitterElectricity1.SpawnAtEdge = true;
        
        emitterElectricity2.MinSize = 0.25f;
        emitterElectricity2.MaxSize = 0.5f;
        emitterElectricity2.EndSize = 0.05f;
        emitterElectricity2.MinLifeTime = 0.7f;
        emitterElectricity2.MaxLifeTime = 0.3f;
        emitterElectricity2.EmissionRate = 5;
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
        emitterElectricity3.MaxLifeTime = 0.1f;
        emitterElectricity3.EmissionRate = 25;
        emitterElectricity3.MinRotationSpeed = -2.1f;
        emitterElectricity3.MaxRotationSpeed = -1.0f;
        emitterElectricity3.MinSpeed = 1.1f;
        emitterElectricity3.MaxSpeed = 1.4f;
        emitterElectricity3.EndSpeed = 1.3f;
        emitterElectricity3.DistanceFromSphereCenter = 0;
        emitterElectricity3.Radius = 0.7f;
        emitterElectricity3.SpawnAtEdge = true;
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

    void EmitterExplosion()
    {
        MultiplyWithIntensity((float)(2.5f), emitterElectricity1);
        MultiplyWithIntensity((float)(2.5f), emitterElectricity2);
        MultiplyWithIntensity((float)(2.5f), emitterElectricity3);
        emitterElectricity1.EmitOneShot(20);
        emitterElectricity2.EmitOneShot(15);
        emitterElectricity3.EmitOneShot(8);
        emitterFire.EmitOneShot(10);

        MultiplyWithIntensity(2.0f, emitterFire);
        emitterFire.Emit = true;
        emitterSmoke.Emit = true;
    }

    public void TimeSinceThrown(float time)
    {
        if (time > fireIntensityreThreshold)
        {
            fireIntensityreThreshold = 9999999999.0f;
            MultiplyWithIntensity(0.5f, emitterFire);
            MultiplyWithIntensity(0.5f, emitterSmoke);
        }

        if (time > 1.4f)
        {
            ResetFireEmitters();
            emitterFire.Emit = false;
            emitterSmoke.Emit = false;
            ResetElectricityEmitters();
        }
    }

    public override void Update()
    {
    }

    public void Drop()
    {
        RPCDrop();
        SendRPC("RPCDrop");
        
    }

    public void ChargeColor()
    {
        emitterElectricity1.Emit = true;
        emitterElectricity2.Emit = true;
        emitterElectricity3.Emit = true;
        
        float interp = MathHelper.Min(chargeTimeCurrent / chargeTimeMax, 1.0f);

        Color newColor = new Color(interp, 0.0f, (1.0f-interp));
        renderComponent.material.SetColor("color", newColor);

        if (interp > 0.8f)
        {
            emitterFire.Emit = true;
        }
        if (interp > electricityIntensifyerThreshold)
        {
            MultiplyWithIntensity((float)(2.0f - electricityIntensifyerThreshold), emitterElectricity1);
            MultiplyWithIntensity((float)(2.0f - electricityIntensifyerThreshold), emitterElectricity2);
            MultiplyWithIntensity((float)(2.0f - electricityIntensifyerThreshold), emitterElectricity3);
            electricityIntensifyerThreshold += 0.3f;
        }
        
    }

    public void StopEmitting()
    {
        StartCoroutine(StopEmission());
    }

    private IEnumerator StopEmission()
    {
        float timer = 3;
        while(timer > 0)
        {
            timer -= Time.DeltaTime;
            yield return null;
        }

        emitterElectricity1.Emit = false;
        emitterElectricity2.Emit = false;
        emitterElectricity3.Emit = false;
        emitterFire.Emit = false;
        emitterSmoke.Emit = false;
    }

    public void Throw(Vector3 force)
    {
        if (m_pickedUp)
        {
            Drop();
            transform.position = transform.position + Vector3.Normalize(force) * 2;
            rigidbody.AddForce(force, Rigidbody.ForceMode.Impulse);

            Cleanup();
            fireIntensityreThreshold = 0.6f;
            emitterFire.Emit = true;
            emitterSmoke.Emit = true;
            EmitterExplosion();
            
            
        }
    }

    public void Cleanup()
    {
        renderComponent.material.SetColor("color", new Color(0, 0, 255));

        emitterElectricity1.Emit = false;
        emitterElectricity2.Emit = false;
        emitterElectricity3.Emit = false;

        emitterFire.Emit = false;
        emitterSmoke.Emit = false;
        ResetFireEmitters();
        // Reset values
        chargeTimeCurrent = 0;
        ResetElectricityEmitters();
    }

    
    public void Pickup(GameObject gobj, Transform hand)
    {
        rigidbody.enabled = false;
        transform.parent = hand;
        transform.localPosition = Vector3.Zero;
    }

    public override void OnLostOwnership()
    {
        rigidbody.enabled = false;
    }

    public override void OnRead(NetPacketReader reader, bool initialState)
    {
        float chargeTime = reader.GetFloat();
        if (chargeTime > 0)
        {
            ChargeColor();
        }
        else if(chargeTime == 0 && chargeTimeCurrent > 0)
        {
            StopEmitting();
        }
            
    }

    public override bool OnWrite(NetDataWriter writer, bool initialState)
    {
        writer.Put(chargeTimeCurrent);
        return true;
    }
}
