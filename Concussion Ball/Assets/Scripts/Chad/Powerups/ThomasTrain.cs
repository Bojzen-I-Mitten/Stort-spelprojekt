using ThomasEngine;
using ThomasEngine.Network;
using System.Collections;
using System.Linq;

public class ThomasTrain : Powerup
{
    ChadControls ObjectOwner = null;

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
    public float ExplosionForce { get; set; }

    private float soundcooldown;
    private bool hasPlayedChargeSound;
    private float _DespawnTimer; 

    public override void OnAwake()
    {
        base.OnAwake();
        BaseThrowForce = 20.0f;
        MaxThrowForce = 36.0f;
        ThrowForce = BaseThrowForce;
        ExplosionForce = 60.0f;
        _DespawnTimer = 0.0f;

        soundComponentChargeUp = gameObject.AddComponent<SoundComponent>();
        soundComponentChargeUp.Type = SoundComponent.SoundType.Effect;
        soundComponentChargeUp.Looping = false;
        soundComponentChargeUp.Is3D = true;
        soundComponentChargeUp.MinDistance = 0.1f;

        soundComponentTravel = gameObject.AddComponent<SoundComponent>();
        soundComponentTravel.Type = SoundComponent.SoundType.Effect;
        soundComponentTravel.Looping = false;
        soundComponentTravel.Is3D = true;

        soundComponentExplosion = gameObject.AddComponent<SoundComponent>();
        soundComponentExplosion.Type = SoundComponent.SoundType.Effect;
        soundComponentExplosion.Looping = false;
        soundComponentExplosion.Is3D = true;

        soundComponentExplosion.MaxDistance = 10000;
        soundComponentExplosion.MinDistance = 20;

        soundcooldown = 0.0f;

        soundComponentChargeUp.Clip = soundClipChargeUp;
        soundComponentTravel.Clip = soundClipTravel;
        soundComponentExplosion.Clip = soundClipExplosion;

        m_throwable = true; // change depending on power-up

        #region emitters
        emitterFire = gameObject.AddComponent<ParticleEmitter>();
        emitterThomasFace = gameObject.AddComponent<ParticleEmitter>();
        emitterSpark = gameObject.AddComponent<ParticleEmitter>();
        
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
        #endregion
    }


    public override void Update()
    {
        base.Update();

        soundcooldown -= Time.DeltaTime;
        if (hasPlayedChargeSound)
        {
            if (!charging)
            {
                if (soundcooldown < 0.0f)
                {
                    hasPlayedChargeSound = false;
                }
            }
        }

        // Despawn if Train has not hit anyone in 30 seconds
        if (_DespawnTimer > 30)
            base.Activate();
        else if (_DespawnTimer > 0)
            _DespawnTimer += Time.DeltaTime;
    }

    public override void OnCollisionStay(Collider collider)
    {
        base.OnCollisionEnter(collider);
    }

    public override void SaveObjectOwner(ChadControls chad)
    {
        ObjectOwner = chad;
    }

    public override void Cleanup()
    {
        base.Cleanup();
        soundComponentChargeUp.Stop();
    }
    

    public override void ChargeEffect()
    {
        base.ChargeEffect();
        if (!hasPlayedChargeSound)
        {
            soundComponentChargeUp.Play();
            hasPlayedChargeSound = true;
            soundcooldown = 5.0f;
        }
    }


    public override void OnThrow()
    {
        soundComponentTravel.Play();
    }

    // if this is a throwable power-up this function will be called
    public override void Throw(Vector3 camPos, Vector3 direction)
    {
        base.Throw(camPos, Vector3.Normalize(direction) * ThrowForce);

        _DespawnTimer += Time.DeltaTime;
        m_rigidBody.UseGravity = false;
        transform.scale *= 8.0f;
    }

    //public override void OnCollisionEnter(Collider collider)
    //{
    //    //Check if colliding with a player
    //    ChadControls otherChad = collider.gameObject.GetComponent<ChadControls>();
    //    if (!otherChad)
    //    {
    //        base.OnCollisionEnter(collider);
    //    }
    //    else
    //    {
    //        ChadControls localChad = MatchSystem.instance.LocalChad;

    //        TEAM_TYPE playerTeam = MatchSystem.instance.GetPlayerTeam(ObjectOwner.gameObject);
    //        TEAM_TYPE otherPlayerTeam = MatchSystem.instance.GetPlayerTeam(collider.gameObject);

    //        if (localChad && (otherPlayerTeam != playerTeam))
    //            base.OnCollisionEnter(collider);
    //    }
    //}

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
                localChad.ActivateRagdoll(2.0f, param);
                AnnouncerSoundManager.Instance.Announce(ANNOUNCEMENT_TYPE.EXPLODED);
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

        soundComponentExplosion.Play();

        StartCoroutine(KillTrain());

        _DespawnTimer = 0.0f;
    }

    private IEnumerator KillTrain()
    {
        yield return null;//wait one frame to emit particles
        Remove();
    }
}
