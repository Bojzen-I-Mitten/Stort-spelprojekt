using ThomasEngine;
using ThomasEngine.Network;
using System.Collections;
using System.Linq;

public class ThomasTrain : Powerup
{
    private ParticleEmitter emitterFire;
    private ParticleEmitter emitterThomasFace;
    private ParticleEmitter emitterDank;
    public Texture2D fireTexture { get; set; }
    public Texture2D thomasTexture { get; set; }
    public Texture2D dankTexture { get; set; }

    private SoundComponent soundComponentChargeUp;
    private SoundComponent soundComponentTravel;
    private SoundComponent soundComponentExplosion;

    public AudioClip soundClipChargeUp { get; set; }
    public AudioClip soundClipTravel { get; set; }
    public AudioClip soundClipExplosion { get; set; }

    public float ExplosionRadius { get; set; } = 5.0f;
    public float ExplosionForce { get; set; } = 200.0f;

    private bool playChargeUpSound;

    public override void Start()
    {
        base.Start();

        m_throwable = true; // change depending on power-up
        emitterFire = gameObject.AddComponent<ParticleEmitter>();
        emitterThomasFace = gameObject.AddComponent<ParticleEmitter>();
        emitterDank = gameObject.AddComponent<ParticleEmitter>();

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

        emitterDank.Texture = dankTexture;
        emitterDank.MinSize = 0.6f;
        emitterDank.MaxSize = 1.4f;
        emitterDank.EndSize = 0.02f;
        emitterDank.MaxLifeTime = 0.5f;
        emitterDank.MinLifeTime = 0.5f;
        emitterDank.MinRotationSpeed = 5.0f;
        emitterDank.MaxRotationSpeed = 5.0f;
        emitterDank.MaxSpeed = 20;
        emitterDank.MinSpeed = 10;
        emitterDank.EndSpeed = 0;
        emitterDank.SpawnAtEdge = true;
        emitterDank.Radius = 6.7f;

        soundComponentChargeUp = gameObject.AddComponent<SoundComponent>();
        soundComponentChargeUp.Looping = false;
        soundComponentTravel = gameObject.AddComponent<SoundComponent>();
        soundComponentTravel.Looping = false;
        soundComponentExplosion = gameObject.AddComponent<SoundComponent>();
        soundComponentChargeUp.clip = soundClipChargeUp;
        soundComponentTravel.clip = soundClipTravel;
        soundComponentExplosion.clip = soundClipExplosion;

        playChargeUpSound = true;

    }

    public override void Update()
    {
        base.Update();
    }

    public override void Cleanup()
    {
        playChargeUpSound = true;
        soundComponentChargeUp.Stop();
    }

    public override void ChargeEffect()
    {
        if (playChargeUpSound)
        {
            soundComponentChargeUp.Play();
            playChargeUpSound = false;
        }
    }
    // if this is a throwable power-up this function will be called
    public override void Throw(Vector3 camPos, Vector3 force)
    {
        base.Throw(camPos, force * 5.0f);
        soundComponentTravel.Play();

        m_rigidBody.UseGravity = false;
        StartCoroutine(Scale());
    }

    IEnumerator Scale()
    {
        transform.scale *= 10.0f;
        yield return new WaitForSeconds(0.1f);
        float t = 2.0f;
        while (t > 0.0f)
        {
            transform.scale += new Vector3(1.0f) * Time.DeltaTime;
            t -= Time.DeltaTime;
            yield return null;
        }
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

        // loop through players and check distance from explosion source

        var players = NetworkManager.instance.Scene.Players.Values.ToList();
        players.ForEach(player =>
        {
            float distance = Vector3.Distance(player.transform.position, transform.position);
            if (distance < ExplosionRadius)
            {
                Vector3 forceDir = player.transform.position - transform.position;
                forceDir.y += 3.0f;

                // ragdoll and knock-back
                player.gameObject.GetComponent<ChadControls>().PublicStartRagdoll(5.0f, forceDir * ExplosionForce);
            }
        });



    }

    private void Explosion()
    {
        //hide the train.
        StopAllCoroutines();
        m_rigidBody.enabled = false;
        gameObject.transform.scale = Vector3.Zero;
        soundComponentTravel.Stop();

        emitterThomasFace.EmitOneShot(1);
        emitterFire.EmitOneShot(40);
        emitterDank.EmitOneShot(200);

        soundComponentExplosion.PlayOneShot();

        StartCoroutine(KillTrain());
    }

    private IEnumerator KillTrain()
    {
        yield return null;//wait one frame to emit particles

        // despawn gameobject
        Remove();
    }
}
