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

    public override void Start()
    {
        cooldown = -0.1f;

        emitterElectricity1 = gameObject.AddComponent<ParticleEmitter>();
        emitterElectricity2 = gameObject.AddComponent<ParticleEmitter>();
        emitterElectricity3 = gameObject.AddComponent<ParticleEmitter>();
        emitterSmoke = gameObject.AddComponent<ParticleEmitter>();
        emitterFire = gameObject.AddComponent<ParticleEmitter>();
        

        emitterElectricity1.Texture = electricityTex1;
        emitterElectricity1.MinSize = 0.2f;
        emitterElectricity1.MaxSize = 0.6f;
        emitterElectricity1.EndSize = 2.0f;
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
        emitterElectricity2.MinSize = 0.5f;
        emitterElectricity2.MaxSize = 1.0f;
        emitterElectricity2.EndSize = 0.1f;
        emitterElectricity2.MinLifeTime = 0.7f;
        emitterElectricity2.MaxLifeTime = 0.3f;
        emitterElectricity2.EmissionRate = 5;
        emitterElectricity2.MinRotationSpeed = 2.1f;
        emitterElectricity2.MaxRotationSpeed = -2.0f;
        emitterElectricity2.MinSpeed = 1.1f;
        emitterElectricity2.MaxSpeed = 3.4f;
        emitterElectricity2.EndSpeed = -2.3f;
        emitterElectricity2.DistanceFromSphereCenter = 0;
        emitterElectricity2.Radius = 1.0f;

        emitterElectricity3.Texture = electricityTex3;
        emitterElectricity3.MinSize = 0.7f;
        emitterElectricity3.MaxSize = 1.3f;
        emitterElectricity3.EndSize = 1.0f;
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

    private void UpdateIntensity(float intentisty)
    {
        emitterElectricity3.MinSize *= intentisty;
        emitterElectricity3.MaxSize *= intentisty;
        emitterElectricity3.EndSize *= intentisty;
        emitterElectricity3.MinLifeTime *= intentisty;
        emitterElectricity3.MaxLifeTime *= intentisty;
        emitterElectricity3.EmissionRate = (uint)MathHelper.Max(((float) emitterElectricity3.EmissionRate * intentisty), 0.0f);
        emitterElectricity3.MinRotationSpeed *= intentisty;
        emitterElectricity3.MaxRotationSpeed *= intentisty;
        emitterElectricity3.MinSpeed *= intentisty;
        emitterElectricity3.MaxSpeed *= intentisty;
        emitterElectricity3.EndSpeed *= intentisty;
        emitterElectricity3.Radius *= intentisty;
    }

    public override void Update()
    {
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
            emitterElectricity1.EmitOneShot(10);
            emitterElectricity2.EmitOneShot(10);
            emitterElectricity3.EmitOneShot(5);
            emitterFire.EmitOneShot(15);
            emitterSmoke.Emit = true;
            emitterFire.Emit = true;
            cooldown = 0.2f;
        }
        cooldown -= Time.DeltaTime;
    }
}
