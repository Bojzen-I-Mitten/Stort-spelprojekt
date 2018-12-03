using ThomasEngine;
using ThomasEngine.Network;
using System.Collections;
using System.Linq;

public class Banana : Powerup
{
    ChadControls ObjectOwner = null;

    public float ExplosionRadius { get; set; } = 8.0f;
    public float ExplosionForce;
    public float DespawnTime;

    private bool _BananaTriggered;
    private float _BananaTimer;

    public override void OnAwake()
    {
        base.OnAwake();

        m_throwable = true; // change depending on power-up
        MovementSpeedModifier = 1.0f;
        
        ExplosionForce = 60.0f;
        BaseThrowForce = 15.0f;
        MaxThrowForce = 25.0f;
        ThrowForce = BaseThrowForce;
        m_rigidBody.Friction = 100.0f;

        DespawnTime = 6.0f;
        _BananaTriggered = false;
        _BananaTimer = 0.0f;


        //ExplosionSound = gameObject.AddComponent<SoundComponent>();
        //ExplosionSound.Type = SoundComponent.SoundType.Effect;
        //ExplosionSound.Clip = VindalooExplosionSound;
        //ExplosionSound.Looping = false;
        //ExplosionSound.Is3D = true;
    }

    public override void Update()
    {
        base.Update();

        if (_BananaTimer > DespawnTime)
        {
            //Debug.Log("Banana not triggered for: " + DespawnTime + " seconds.");
            //Activate();
            // Despawn
        }
        else if (_BananaTimer > 0)
            _BananaTimer += Time.DeltaTime;
    }

    // if this is a throwable power-up this function will be called
    public override void Throw(Vector3 camPos, Vector3 direction)
    {
        _BananaTimer += Time.DeltaTime;
        base.Throw(camPos, Vector3.Normalize(direction) * ThrowForce);
    }

    public override void OnCollisionEnter(Collider collider)
    {
        //Check if colliding with a player
        ChadControls otherChad = collider.gameObject.GetComponent<ChadControls>();
        if (otherChad)
        {
            //TEAM_TYPE playerTeam = MatchSystem.instance.GetPlayerTeam(ObjectOwner.gameObject);
            //TEAM_TYPE otherPlayerTeam = MatchSystem.instance.GetPlayerTeam(otherChad.gameObject);

            //if (playerTeam != otherPlayerTeam)
            //    base.OnCollisionEnter(collider);
            base.OnCollisionEnter(collider);
        }
        else
        {
            // colliding with static object 
            Debug.Log("Banana collision with static object: " + collider.gameObject.Name);

            m_rigidBody.Friction = 100.0f;
            //PickupCollider.enabled = true; // for testing
            
        }

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

        ChadControls otherChad = colliderObject.GetComponent<ChadControls>();
        if(otherChad)
        {
            // rustle his jimmies
            Ragdoll.ImpactParams param = new Ragdoll.ImpactParams(gameObject.transform.position, Vector3.Zero, 0.0f);
            param.bodyPartFactor[(int)Ragdoll.BODYPART.SPINE] = 0.88f;
            param.force = otherChad.transform.forward * 500.0f;
            param.bodyPartFactor[(int)Ragdoll.BODYPART.RIGHT_LOWER_LEG] = 10.0f;
            otherChad.ActivateRagdoll(4.0f, param);
        }
        
        Explosion();
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
}
