using ThomasEngine;

public class ParticleComponentTest : ScriptComponent
{
    private ParticleEmitter emitter;
    public Texture2D tex { get; set; }
    private float cooldown;

    public override void Start()
    {
        emitter = gameObject.AddComponent<ParticleEmitter>();
        emitter.MinSize = 0.5f;
        emitter.MaxSize = 1.5f;
        emitter.EndSize = 0.05f;
        emitter.MinLifeTime = 0.5f;
        emitter.MaxLifeTime = 1.01f;
        emitter.MinRotationSpeed = 2.0f;
        emitter.MaxRotationSpeed = 5.0f;
        emitter.MinSpeed = 2.0f;
        emitter.MaxSpeed = 3.0f;
        emitter.EndSpeed = 0.1f;
        emitter.DistanceFromSphereCenter = 3;
        emitter.Radius = 1.5f;

        cooldown = -0.1f;
        emitter.Texture = tex;
    }

    public override void Update()
    {
        if (Input.GetKey(Input.Keys.I))
        {
            emitter.Emit = true;
        }
        if (Input.GetKey(Input.Keys.O))
        {
            emitter.Emit = false;
        }
        if (Input.GetKey(Input.Keys.K) && cooldown < 0.0f)
        {
            emitter.EmitOneShot(100);
            cooldown = 0.2f;
        }
        cooldown -= Time.DeltaTime;
    }
}
