using ThomasEngine;

public class ParticleComponentTest : ScriptComponent
{
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
    private float cooldown;
    private float intensity;

    public override void Start()
    {
        cooldown = -0.1f;
        intensity = 2.0f;

        emitterElectricity1 = gameObject.AddComponent<ParticleEmitter>();
        emitterElectricity2 = gameObject.AddComponent<ParticleEmitter>();
        emitterElectricity3 = gameObject.AddComponent<ParticleEmitter>();
        emitterSmoke = gameObject.AddComponent<ParticleEmitter>();
        emitterFire = gameObject.AddComponent<ParticleEmitter>();
        

        emitterElectricity1.Texture = electricityTex1;
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

        emitterElectricity2.Texture = electricityTex2;
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

        emitterElectricity3.Texture = electricityTex3;
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

        emitterSmoke.Texture = smokeTex;
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


        emitterFire.Texture = fireTex;
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

    private void Intensify(float intentisty, ParticleEmitter emitter)
    {
        emitter.MinSize *= intentisty;
        emitter.MaxSize *= intentisty;
        emitter.EndSize *= intentisty;
        emitter.MinLifeTime *= intentisty;
        emitter.MaxLifeTime *= intentisty;
        emitter.EmissionRate = (uint)MathHelper.Max(((float) emitterElectricity3.EmissionRate * intentisty), 0.0f);
        emitter.MinRotationSpeed *= intentisty;
        emitter.MaxRotationSpeed *= intentisty;
        emitter.MinSpeed *= intentisty;
        emitter.MaxSpeed *= intentisty;
        emitter.EndSpeed *= intentisty;
        emitter.Radius *= intentisty;
    }

    private void Dampen(float intentisty, ParticleEmitter emitter)
    {
        emitter.MinSize /= intentisty;
        emitter.MaxSize /= intentisty;
        emitter.EndSize /= intentisty;
        emitter.MinLifeTime /= intentisty;
        emitter.MaxLifeTime /= intentisty;
        emitter.EmissionRate = (uint)MathHelper.Max(((float)emitterElectricity3.EmissionRate / intentisty), 0.0f);
        emitter.MinRotationSpeed /= intentisty;
        emitter.MaxRotationSpeed /= intentisty;
        emitter.MinSpeed /= intentisty;
        emitter.MaxSpeed /= intentisty;
        emitter.EndSpeed /= intentisty;
        emitter.Radius /= intentisty;
    }

    public override void Update()
    {
        if (cooldown < 0)
        {
            if (Input.GetKey(Input.Keys.L))
            {
                Intensify(intensity, emitterElectricity1);
                Intensify(intensity, emitterElectricity2);
                Intensify(intensity, emitterElectricity3);
                cooldown = 2;
            }
            if (Input.GetKey(Input.Keys.J))
            {
                Dampen(intensity, emitterElectricity1);
                Dampen(intensity, emitterElectricity2);
                Dampen(intensity, emitterElectricity3);
                cooldown = 2;
                
            }
        }
        if (Input.GetKey(Input.Keys.I))
        {
            emitterElectricity1.Emit = true;
            emitterElectricity2.Emit = true;
            emitterElectricity3.Emit = true;
        }
        if (Input.GetKey(Input.Keys.O))
        {
            emitterElectricity1.Emit = false;
            emitterElectricity2.Emit = false;
            emitterElectricity3.Emit = false;
            emitterSmoke.Emit = false;
            emitterFire.Emit = false;
        }
        if (Input.GetKey(Input.Keys.K) && cooldown < 0.0f)
        {
            emitterElectricity1.EmitOneShot(20);
            emitterElectricity2.EmitOneShot(15);
            emitterElectricity3.EmitOneShot(8);
            emitterFire.EmitOneShot(15);

            emitterElectricity1.Emit = false;
            emitterElectricity2.Emit = false;
            emitterElectricity3.Emit = false;
            emitterSmoke.Emit = true;
            emitterFire.Emit = true;
            cooldown = 0.2f;
        }
        cooldown -= Time.DeltaTime;
    }
}
