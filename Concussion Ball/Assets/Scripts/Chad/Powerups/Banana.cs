using ThomasEngine;
using ThomasEngine.Network;
using System.Collections;
using System.Linq;
using LiteNetLib.Utils;

public class Banana : Powerup
{    
    // Public
    public GameObject BananaFull { get; set; }
    public GameObject BananaEaten {get; set; }
    public AudioClip BananaSlipSound { get; set; }

    public float DespawnTime;

     // Private
    private SoundComponent SlipSound;
    private Collider _LastCollider;
    private ChadControls ObjectOwner = null;

    private float _BananaTimer;

    public override void OnAwake()
    {
        base.OnAwake();

        m_throwable = true; // change depending on power-up
        MovementSpeedModifier = 1.0f;

        BaseThrowForce = 15.0f;
        MaxThrowForce = 25.0f;
        ThrowForce = BaseThrowForce;
        m_rigidBody.Friction = 100.0f;

        _LastCollider = null;
        DespawnTime = 60.0f;
        _BananaTimer = 0.0f;


        SetModels(true, false);

        SlipSound = gameObject.AddComponent<SoundComponent>();
        SlipSound.Type = SoundComponent.SoundType.Effect;
        SlipSound.Clip = BananaSlipSound;
        SlipSound.Looping = false;
        SlipSound.Is3D = true;
    }

    public override void Update()
    {
        base.Update();

        if (isOwner)
        {
            if (_BananaTimer > DespawnTime)
            {
                //Debug.Log("Banana not triggered for: " + DespawnTime + " seconds.");
                // Despawn
                base.OnCollisionEnter(_LastCollider);
            }
            else if (_BananaTimer > 0)
            {
                _BananaTimer += Time.DeltaTime;

                // If Banana falls on side, reset
                if (m_rigidBody.LinearVelocity.y < 0.1f && (gameObject.transform.rotation.x > 0.4f || gameObject.transform.rotation.z > 0.4f))
                {
                    //Debug.Log("Banana fell on side");
                    gameObject.transform.rotation = Quaternion.CreateFromYawPitchRoll(0, 0, 0);
                }
            }
            else
            {
                SetModels(true, false);
            }
        }

    }

    // if this is a throwable power-up this function will be called
    public override void Throw(Vector3 camPos, Vector3 direction)
    {
        _BananaTimer += Time.DeltaTime;
        SetModels(false, true);
        base.Throw(camPos, Vector3.Normalize(direction) * ThrowForce);
    }

    public override void OnCollisionEnter(Collider collider)
    {
        _LastCollider = collider;
        //Check if colliding with a player
        ChadControls otherChad = collider.gameObject.GetComponent<ChadControls>();
        if (otherChad && _BananaTimer > 1)
        {
            base.OnCollisionEnter(collider);
        }
        else
        {
            // colliding with static object 
            m_rigidBody.Friction = 100.0f;
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
        //Make sure powerups can only be activated once!
        if (activated)
            return;
        activated = true;

        //ChadControls otherChad = colliderObject.GetComponent<ChadControls>();
        ChadControls otherChad = _LastCollider.gameObject.GetComponent<ChadControls>();
        if (otherChad)
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
        SetModels(true, false);
        _BananaTimer = 0.0f;
        Remove();
    }

    public void SetModels(bool full, bool peel)
    {
        if (BananaFull && BananaEaten)
        {
            BananaFull.SetActive(full);
            BananaEaten.SetActive(peel);
        }
    }

    //private void SetModels(bool full, bool peel)
    //{
    //    RPCSetModels(full, peel);
    //    SendRPC("RPCSetModels", (bool)full, (bool)peel);
    //}

    public override bool OnWrite(NetDataWriter writer, bool initialState)
    {
        base.OnWrite(writer, initialState);

        writer.Put(BananaFull.GetActive());
        writer.Put(BananaEaten.GetActive());

        return true;
    }

    public override void OnRead(NetDataReader reader, bool initialState)
    {
        base.OnRead(reader, initialState);

        BananaFull.SetActive(reader.GetBool());
        BananaEaten.SetActive(reader.GetBool());
    }
}
