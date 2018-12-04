using ThomasEngine;
using ThomasEngine.Network;
using System.Collections;
using System.Linq;

public class Banana : Powerup
{    
    // Public
    public GameObject BananaFull { get; set; }
    public GameObject BananaEaten {get; set; }
    public AudioClip BananaSlipSound { get; set; }

    public float DespawnTime;

     // Private
    private SoundComponent SlipSound;
    private Collider _FirstCollider;
    private ChadControls ObjectOwner = null;

    private bool _BananaTriggered;
    private float _BananaTimer;
    private float _Force;

    public override void OnAwake()
    {
        base.OnAwake();

        m_throwable = true; // change depending on power-up
        MovementSpeedModifier = 1.0f;

        BaseThrowForce = 15.0f;
        MaxThrowForce = 25.0f;
        ThrowForce = BaseThrowForce;
        m_rigidBody.Friction = 100.0f;

        _FirstCollider = null;
        DespawnTime = 60.0f;
        _BananaTriggered = false;
        _BananaTimer = 0.0f;


        if(BananaFull && BananaEaten)
        {
            BananaFull.SetActive(true);
            BananaEaten.SetActive(false);
        }

        SlipSound = gameObject.AddComponent<SoundComponent>();
        SlipSound.Type = SoundComponent.SoundType.Effect;
        SlipSound.Clip = BananaSlipSound;
        SlipSound.Looping = false;
        SlipSound.Is3D = true;
    }

    public override void Update()
    {
        base.Update();

        if (_BananaTimer > DespawnTime)
        {
            //Debug.Log("Banana not triggered for: " + DespawnTime + " seconds.");
            // Despawn
            base.OnCollisionEnter(_FirstCollider);
        }
        else if (_BananaTimer > 0)
        {
            _BananaTimer += Time.DeltaTime;

            // If Banana falls on side, reset
            if (m_rigidBody.LinearVelocity.y < 0.1f && (gameObject.transform.rotation.x > 0.4f || gameObject.transform.rotation.z > 0.4f))
            {
                Debug.Log("Banana fell on side");
                gameObject.transform.rotation = Quaternion.CreateFromYawPitchRoll(0,0,0);
            }
        }
        else
        {
            if (BananaFull && BananaEaten)
            {
                BananaFull.SetActive(true);
                BananaEaten.SetActive(false);
            }
        }
    }

    // if this is a throwable power-up this function will be called
    public override void Throw(Vector3 camPos, Vector3 direction)
    {
        _BananaTimer += Time.DeltaTime;
        if (BananaFull && BananaEaten)
        {
            BananaFull.SetActive(false);
            BananaEaten.SetActive(true);
        }
        base.Throw(camPos, Vector3.Normalize(direction) * ThrowForce);
    }

    public override void OnCollisionEnter(Collider collider)
    {
        _FirstCollider = collider;
        //Check if colliding with a player
        ChadControls otherChad = collider.gameObject.GetComponent<ChadControls>();
        if (otherChad)
        {
            base.OnCollisionEnter(collider);
        }
        else
        {
            // colliding with static object 
            m_rigidBody.Friction = 100.0f;
            //m_rigidBody.IsKinematic = true;
            m_rigidBody.LinearVelocity = Vector3.Zero;
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
        Debug.Log("Entered activate");
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
            param.bodyPartFactor[(int)Ragdoll.BODYPART.RIGHT_LOWER_LEG] = 10.0f;
            param.bodyPartFactor[(int)Ragdoll.BODYPART.LEFT_LOWER_LEG] = 5.0f;
            param.bodyPartFactor[(int)Ragdoll.BODYPART.HEAD] = -2.0f;
            param.force = otherChad.transform.forward * otherChad.CurrentVelocity.y * 50.0f;
            otherChad.ActivateRagdoll(4.0f, param);
            SlipSound.Play();
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
        if (BananaFull && BananaEaten)
        {
            BananaFull.SetActive(true);
            BananaEaten.SetActive(false);
        }
        Remove();
    }
}
