using ThomasEngine;
using ThomasEngine.Network;
using System.Collections;
using System.Linq;

public class ThomasTrain : Powerup
{
    private ParticleEmitter emitterFire;
    private ParticleEmitter emitterThomasFace;
    private ParticleEmitter emitterSpark;
    public Texture2D fireTexture { get; set; }
    public Texture2D thomasTexture { get; set; }
    public Texture2D sparkTexture { get; set; }

    private SoundComponent soundComponentChargeUp;
    private SoundComponent soundComponentTravel;
    private SoundComponent soundComponentExplosion;

    public AudioClip soundClipChargeUp { get; set; }
    public AudioClip soundClipTravel { get; set; }
    public AudioClip soundClipExplosion { get; set; }

    public float ExplosionRadius { get; set; } = 10.0f;
    public float ExplosionForce { get; set; } = 300.0f;

    private float soundcooldown;
    private bool playChargeUpSound;


    public override void OnAwake()
    {
        base.OnAwake();
        emitterFire = gameObject.AddComponent<ParticleEmitter>();
        emitterThomasFace = gameObject.AddComponent<ParticleEmitter>();
        emitterSpark = gameObject.AddComponent<ParticleEmitter>();

        soundComponentChargeUp = gameObject.AddComponent<SoundComponent>();
        soundComponentChargeUp.Looping = false;
        soundComponentChargeUp.Is3D = true;
        soundComponentTravel = gameObject.AddComponent<SoundComponent>();
        soundComponentTravel.Looping = false;
        soundComponentTravel.Is3D = true;
        soundComponentExplosion = gameObject.AddComponent<SoundComponent>();
        soundComponentExplosion.Looping = false;
        soundComponentExplosion.Is3D = true;
        
        soundcooldown = 0.0f;

        m_throwable = true; // change depending on power-up
        emitterFire.Texture = fireTexture;
        emitterFire.BlendState = ParticleEmitter.BLEND_STATES.ADDITIVE;
        emitterFire.MinSize = 4.0f;
        emitterFire.MaxSize = 6.0f;
        emitterFire.EndSize = 0.0f;
        emitterFire.MinLifeTime = 0.1f;
        emitterFire.MaxLifeTime = 1.7f;
        emitterFire.EmissionRate = 10;
        emitterFire.MinRotationSpeed = -2.0f;
        emitterFire.MaxRotationSpeed = 2.0f;
        emitterFire.MinSpeed = 0.5f;
        emitterFire.MaxSpeed = 1.0f;
        emitterFire.EndSpeed = -6.0f;
        emitterFire.DistanceFromSphereCenter = 0.0f;
        emitterFire.SpawnAtEdge = true;
        emitterFire.Radius = 5.2f;

        emitterThomasFace.Texture = thomasTexture;
        emitterThomasFace.MinSize = 5.0f;
        emitterThomasFace.MaxSize = 5.0f;
        emitterThomasFace.EndSize = 3.0f;
        emitterThomasFace.MaxLifeTime = 1.5f;
        emitterThomasFace.MinLifeTime = 1.5f;
        emitterThomasFace.MinRotationSpeed = 5.0f;
        emitterThomasFace.MaxRotationSpeed = 5.0f;
        emitterThomasFace.MaxSpeed = 0;
        emitterThomasFace.MinSpeed = 0;
        emitterThomasFace.EndSpeed = 0;

        emitterSpark.Texture = sparkTexture;
        emitterSpark.MinSize = 0.6f;
        emitterSpark.MaxSize = 1.4f;
        emitterSpark.EndSize = 0.02f;
        emitterSpark.MaxLifeTime = 0.5f;
        emitterSpark.MinLifeTime = 0.5f;
        emitterSpark.MinRotationSpeed = 5.0f;
        emitterSpark.MaxRotationSpeed = 5.0f;
        emitterSpark.MaxSpeed = 20;
        emitterSpark.MinSpeed = 10;
        emitterSpark.EndSpeed = 0;
        emitterSpark.SpawnAtEdge = true;
        emitterSpark.Radius = 6.7f;

        soundComponentChargeUp.Looping = false;

        soundComponentTravel.Looping = false;

        soundComponentChargeUp.clip = soundClipChargeUp;
        soundComponentTravel.clip = soundClipTravel;
        soundComponentExplosion.clip = soundClipExplosion;

        playChargeUpSound = true;
    }


    public override void Update()
    {
        base.Update();
        soundcooldown -= Time.DeltaTime;
    }

    public override void Cleanup()
    {
        base.Cleanup();
        playChargeUpSound = false;
        soundComponentChargeUp.Stop();
    }
    

    public override void ChargeEffect()
    {
        base.ChargeEffect();
        if (soundcooldown < 0.0f)
        {
            soundcooldown = 1.0f;
            soundComponentChargeUp.Play();
        }
    }


    public override void OnThrow()
    {
        soundComponentTravel.Play();
        Debug.Log("throw");
    }

    // if this is a throwable power-up this function will be called
    public override void Throw(Vector3 camPos, Vector3 force)
    {
        base.Throw(camPos + Vector3.Normalize(force)*2, force);
        

        m_rigidBody.UseGravity = false;
        transform.scale *= 8.0f;
    }
   

    // this function will be called upon powerup use / collision after trown
    public override void OnActivate()
    {
        //Make sure powerups can only be activated once!
        if (activated)
            return;
        activated = true;
        // boom particles, Gustav do your magic, sprinkla lite magic till boisen
        Explosion();

        ChadControls localChad = MatchSystem.instance.LocalChad;
        if (localChad)
        {
            float distance = Vector3.Distance(localChad.transform.position, transform.position);
            if (distance < ExplosionRadius)
            {
                Vector3 forceDir = localChad.transform.position - transform.position;
                forceDir.Normalize();
                forceDir.y += 3.0f;
                float distForce = ExplosionRadius - distance;
                localChad.ActivateRagdoll(2.0f, distForce * forceDir * ExplosionForce);
            }
        }
    }

    private void Explosion()
    {
        //hide the train.
        StopAllCoroutines();
        gameObject.transform.scale = Vector3.One;
        soundComponentTravel.Stop();

        emitterThomasFace.EmitOneShot(1);
        emitterFire.EmitOneShot(40);
        emitterSpark.EmitOneShot(200);

        soundComponentExplosion.PlayOneShot();

        StartCoroutine(KillTrain());
    }

    private IEnumerator KillTrain()
    {
        yield return null;//wait one frame to emit particles
        Remove();
    }
}
