using ThomasEngine;
using ThomasEngine.Network;
using System.Collections;
using System.Linq;

public class SoundBomb : Powerup
{
    ChadControls ObjectOwner = null;
    Collider _FirstCollider;
    //public AudioClip VindalooExplosionSound { get; set; }
    //private SoundComponent ExplosionSound;

    public float ExplosionRadius;
    public float ExplosionForce;

    private float _JumpTimer;
    private float _DanceDuration;

    public override void OnAwake()
    {
        base.OnAwake();

        m_throwable = true; // change depending on power-up
        MovementSpeedModifier = 0.65f;
        ExplosionRadius = 5.0f;
        ExplosionForce = 60.0f;
        BaseThrowForce = 12.0f;
        MaxThrowForce = 18.0f;
        ThrowForce = BaseThrowForce;
        _FirstCollider = null;
        m_rigidBody.Friction = 100.0f;
        _JumpTimer = 0.0f;
        _DanceDuration = 5.0f;

        //ExplosionSound = gameObject.AddComponent<SoundComponent>();
        //ExplosionSound.Type = SoundComponent.SoundType.Effect;
        //ExplosionSound.Clip = VindalooExplosionSound;
        //ExplosionSound.Looping = false;
        //ExplosionSound.Is3D = true;

        #region big meme particle emitter bois

        //emitterFire = gameObject.AddComponent<ParticleEmitter>();
        //emitterFire.MinSize = 1.2f;
        //emitterFire.MaxSize = 1.99f;
        //emitterFire.EndSize = 1.6f;
        //emitterFire.MinLifeTime = 0.1f;
        //emitterFire.MaxLifeTime = 0.2f;
        //emitterFire.EmissionRate = 100;
        //emitterFire.MinRotationSpeed = 0.0f;
        //emitterFire.MaxRotationSpeed = 4.0f;
        //emitterFire.MinSpeed = 5.5f;
        //emitterFire.MaxSpeed = 10.0f;
        //emitterFire.EndSpeed = 20.0f;
        //emitterFire.DistanceFromSphereCenter = 0;
        //emitterFire.Radius = 1.0f;
        //emitterFire.Gravity = 0.0f;
        //emitterFire.SpawnAtEdge = false;
        //emitterFire.Texture = fireTexture;
        //emitterFire.BlendState = ParticleEmitter.BLEND_STATES.ADDITIVE;

        //emitterGravel = gameObject.AddComponent<ParticleEmitter>();
        //emitterGravel.MinSize = 0.1f;
        //emitterGravel.MaxSize = 0.2f;
        //emitterGravel.EndSize = 0.05f;
        //emitterGravel.MinLifeTime = 0.4f;
        //emitterGravel.MaxLifeTime = 1.3f;
        //emitterGravel.EmissionRate = 100;
        //emitterGravel.MinRotationSpeed = -6.0f;
        //emitterGravel.MaxRotationSpeed = 6.0f;
        //emitterGravel.MinSpeed = 13.5f;
        //emitterGravel.MaxSpeed = 18.0f;
        //emitterGravel.EndSpeed = 0.0f;
        //emitterGravel.DistanceFromSphereCenter = 0.3f;
        //emitterGravel.Radius = 0.2f;
        //emitterGravel.Gravity = 4.0f;
        //emitterGravel.SpawnAtEdge = false;
        //emitterGravel.Texture = gravelTexture;

        //emitterFire2 = gameObject.AddComponent<ParticleEmitter>();
        //emitterFire2.MinSize = 0.1f;
        //emitterFire2.MaxSize = 0.2f;
        //emitterFire2.EndSize = 0.05f;
        //emitterFire2.MinLifeTime = 0.4f;
        //emitterFire2.MaxLifeTime = 1.3f;
        //emitterFire2.EmissionRate = 100;
        //emitterFire2.MinRotationSpeed = -2.0f;
        //emitterFire2.MaxRotationSpeed = 3.0f;
        //emitterFire2.MinSpeed = 13.5f;
        //emitterFire2.MaxSpeed = 18.0f;
        //emitterFire2.EndSpeed = 0.0f;
        //emitterFire2.DistanceFromSphereCenter = 0.3f;
        //emitterFire2.Radius = 0.2f;
        //emitterFire2.Gravity = 8.0f;
        //emitterFire2.SpawnAtEdge = false;
        //emitterFire2.Texture = fire2Texture;

        //emitterSmoke = gameObject.AddComponent<ParticleEmitter>();

        //emitterSmoke.Texture = smokeTexture;

        //emitterSmoke.MinSize = 0.5f;
        //emitterSmoke.MaxSize = 0.7f;
        //emitterSmoke.EndSize = 1.3f;
        //emitterSmoke.MinLifeTime = 0.7f;
        //emitterSmoke.MaxLifeTime = 2.6f;
        //emitterSmoke.EmissionRate = 100;
        //emitterSmoke.MinRotationSpeed = -2.0f;
        //emitterSmoke.MaxRotationSpeed = 2.0f;
        //emitterSmoke.MinSpeed = 0.6f;
        //emitterSmoke.MaxSpeed = 1.5f;
        //emitterSmoke.EndSpeed = 3.0f;
        //emitterSmoke.DistanceFromSphereCenter = 0.7f;
        //emitterSmoke.Radius = 1.7f;
        //emitterGravel.SpawnAtEdge = false;
        #endregion
    }

    public override void Update()
    {
        base.Update();

        if (isOwner)
        {
            if (_JumpTimer > 0)
            {
                _JumpTimer += Time.DeltaTime;

                // Jump animation
                if (_JumpTimer > 1.0f)
                {
                    Debug.Log("Jumping, timer: " + _JumpTimer);
                    m_rigidBody.LinearVelocity = Vector3.Zero;
                    m_rigidBody.AddForce(new Vector3(0, 100, 0));

                    // splash some particles Gustav @gustav @ijäzy

                    _JumpTimer = Time.DeltaTime;
                }

                // If Sound Bomb falls on side, reset
                if (m_rigidBody.LinearVelocity.y < 0.1f && (gameObject.transform.rotation.x > 0.4f || gameObject.transform.rotation.z > 0.4f))
                {
                    gameObject.transform.rotation = Quaternion.CreateFromYawPitchRoll(0, 0, 0);
                }
            }
        }
    }

    // if this is a throwable power-up this function will be called
    public override void Throw(Vector3 camPos, Vector3 direction)
    {
        base.Throw(camPos, Vector3.Normalize(direction) * ThrowForce);
    }

    public override void OnCollisionEnter(Collider collider)
    {
        _FirstCollider = collider;

        ChadControls colliderPlayer = _FirstCollider.gameObject.GetComponent<ChadControls>();

        if(!colliderPlayer && _JumpTimer == 0 && isOwner)
        {
            //Debug.Log("Colliding with object that is not ya boi");
            m_rigidBody.Friction = 100.0f;
            m_rigidBody.LinearVelocity = Vector3.Zero;
            //PickupCollider.enabled = true; // for testing

            _JumpTimer += Time.DeltaTime;

            // Test
            //OnActivate();
            base.OnCollisionEnter(collider);
        }


    }

    public override void SaveObjectOwner(ChadControls chad)
    {
        ObjectOwner = chad;
    }

    // this function will be called upon powerup use / collision after trown
    public override void OnActivate()
    {
        Debug.Log("SoundBomb activated boi");
        //Make sure powerups can only be activated once!
        if (activated)
            return;
        activated = true;
        // boom particles, Gustav do your magic, sprinkla lite magic till boisen

        StartCoroutine(BlastingMusic());   
    }

    private void Explosion()
    {
        // Play the vindaloo explosion sound
        //ExplosionSound.Play();

        //emitterFire.EmitOneShot(25);
        //emitterFire2.EmitOneShot(45);
        //emitterGravel.EmitOneShot(20);
        //emitterSmoke.EmitOneShot(50);

        StartCoroutine(RemoveNextFrame());
        //Remove();
    }
    private IEnumerator RemoveNextFrame()
    {
        yield return null;

        Remove();
    }

    private IEnumerator BlastingMusic()
    {
        // Start blasting 
        //Debug.Log("Looking for players to blast");
        ChadControls localChad = MatchSystem.instance.LocalChad;

        TEAM_TYPE playerTeam = MatchSystem.instance.GetPlayerTeam(ObjectOwner.gameObject);
        TEAM_TYPE otherPlayerTeam = MatchSystem.instance.GetPlayerTeam(localChad.gameObject);

        float timer = 0.0f;

        while (timer < _DanceDuration)
        {
            timer += Time.DeltaTime;

            if (localChad && otherPlayerTeam != playerTeam)
            {
                float distance = Vector3.Distance(localChad.transform.position, transform.position);
                if (distance < ExplosionRadius)
                {
                    localChad.rBody.LinearVelocity = Vector3.Zero;
                }
            }

            yield return null;
        }
        Explosion();
    }
}
