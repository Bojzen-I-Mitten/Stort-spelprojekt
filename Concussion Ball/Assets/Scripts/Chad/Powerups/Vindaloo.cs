using ThomasEngine;
using ThomasEngine.Network;
using System.Collections;
using System.Linq;

public class Vindaloo : Powerup
{
    ChadControls ObjectOwner = null;

    public Texture2D fireTexture { get; set; }
    public Texture2D smokeTexture { get; set; }
    public Texture2D gravelTexture { get; set; }

    public AudioClip VindalooExplosionSound { get; set; }

    private ParticleEmitter emitterFire;
    private ParticleEmitter emitterSmoke;
    private ParticleEmitter emitterGravel;

    private SoundComponent ExplosionSound;
    private float _DespawnTimer;

    public float ExplosionRadius { get; set; } = 8.0f;
    public float ExplosionForce;


    public override void OnAwake()
    {
        base.OnAwake();

        m_throwable = true; // change depending on power-up
        MovementSpeedModifier = 0.7f;
        ExplosionRadius = 8.0f;
        ExplosionForce = 60.0f;
        BaseThrowForce = 15.0f;
        MaxThrowForce = 25.0f;
        ThrowForce = BaseThrowForce;
        _DespawnTimer = 0.0f;

        ExplosionSound = gameObject.AddComponent<SoundComponent>();
        ExplosionSound.Type = SoundComponent.SoundType.Effect;
        ExplosionSound.Clip = VindalooExplosionSound;
        ExplosionSound.Looping = false;
        ExplosionSound.Is3D = true;
        ExplosionSound.MaxDistance = 10000;
        ExplosionSound.MinDistance = 20;

        Rigidbody rb = gameObject.GetComponent<Rigidbody>();
        rb.CcdMotionThreshold = 1e-7f;
        rb.CcdSweptSphereRadius = 0.1f;

        #region big meme particle emitter bois

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
        emitterSmoke.EndSpeed = 3.0f;
        emitterSmoke.DistanceFromSphereCenter = 0.7f;
        emitterSmoke.Radius = 1.7f;
        emitterGravel.SpawnAtEdge = false;
        #endregion
    }

    public override void Update()
    {
        base.Update();

        // Despawn if Vindaloo has not hit anyone in 30 seconds
        if (_DespawnTimer > 30)
        {
            _DespawnTimer = 0.0f;
            base.Activate();
        }
        else if (_DespawnTimer > 0)
            _DespawnTimer += Time.DeltaTime;
        
    }

    // if this is a throwable power-up this function will be called
    public override void Throw(Vector3 camPos, Vector3 direction)
    {
        base.Throw(camPos, Vector3.Normalize(direction) * ThrowForce);

        _DespawnTimer += Time.DeltaTime;
    }

    public override void SaveObjectOwner(ChadControls chad)
    {
        ObjectOwner = chad;
    }


    // this function will be called upon powerup use / collision after trown
    public override void OnActivate()
    {
        //Make sure powerups can only be activated once!
        if (activated)
            return;
        activated = true;
        // boom particles, Gustav do your magic, sprinkla lite magic till boisen
        

        ChadControls localChad = MatchSystem.instance.LocalChad;

        TEAM_TYPE playerTeam = MatchSystem.instance.GetPlayerTeam(ObjectOwner.gameObject);
        TEAM_TYPE otherPlayerTeam = MatchSystem.instance.GetPlayerTeam(localChad.gameObject);
        if (localChad && otherPlayerTeam != playerTeam)
        {
            float distance = Vector3.Distance(localChad.transform.position, transform.position);
            if (distance < ExplosionRadius)
            {
                Vector3 forceDir = localChad.transform.position - transform.position;
                forceDir.Normalize();
                forceDir.y += 3.0f;

                float distForce = ExplosionRadius - distance;
                Vector3 force = forceDir * ExplosionForce * distForce;
                Ragdoll.ImpactParams param = new Ragdoll.ImpactParams(gameObject.transform.position, force, 0.0f);
                param.bodyPartFactor[(int)Ragdoll.BODYPART.SPINE] = 0.88f;
                localChad.ActivateRagdoll(2.0f, param);
                AnnouncerSoundManager.Instance.Announce(ANNOUNCEMENT_TYPE.EXPLODED);
            }
        }

        Explosion();
    }

    private void Explosion()
    {
        // Play the vindaloo explosion sound
        ExplosionSound.Play();

        emitterFire.EmitOneShot(35);
        emitterGravel.EmitOneShot(20);
        emitterSmoke.EmitOneShot(50);

        StartCoroutine(RemoveNextFrame());
        
        _DespawnTimer = 0.0f;
        //Remove();
    }

    private IEnumerator RemoveNextFrame()
    {
        yield return null;

        Remove();
    }

    public override void Reset()
    {
        base.Reset();
        _DespawnTimer = 0.0f;
    }
}
