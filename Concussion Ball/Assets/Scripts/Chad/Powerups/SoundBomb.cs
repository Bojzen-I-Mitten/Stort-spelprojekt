using ThomasEngine;
using ThomasEngine.Network;
using System.Collections;
using System.Linq;
using System;
using LiteNetLib.Utils;

public class SoundBomb : Powerup
{
    ChadControls ObjectOwner = null;
    Collider _FirstCollider;

    public LightComponent PointBoi { get; set; }

    //public AudioClip VindalooExplosionSound { get; set; }
    //private SoundComponent ExplosionSound;

    public float ExplosionRadius;
    public float ExplosionForce;

    private float _JumpTimer;
    private float _DanceDuration;
    private float _Hue;
    private bool _SpawnedLight;

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
        _Hue = 0.0f;
        _SpawnedLight = false;
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
                _Hue = _JumpTimer % 1.0f;
                PointBoi.DiffuseColor = HSLColor(_Hue);

                _JumpTimer += Time.DeltaTime;

                // Jump animation
                if (_JumpTimer > 1.0f)
                {
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
            else
            {
                if (PointBoi)
                {
                    _SpawnedLight = false;
                    PointBoi.enabled = false;
                }
            }
        }
        else
        {
            if (_SpawnedLight)
            {
                PointBoi.enabled = true;
            }
            else
            {
                PointBoi.enabled = false;
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

        if (!colliderPlayer && _JumpTimer == 0 && isOwner)
        {
            if (PointBoi)
            {
                _SpawnedLight = true;
                PointBoi.enabled = true;
            }
            m_rigidBody.Friction = 100.0f;
            m_rigidBody.LinearVelocity = Vector3.Zero;
            //PickupCollider.enabled = true; // for testing

            _JumpTimer += Time.DeltaTime;

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
        ChadControls localChad = MatchSystem.instance.LocalChad;

        TEAM_TYPE playerTeam = MatchSystem.instance.GetPlayerTeam(ObjectOwner.gameObject);
        TEAM_TYPE otherPlayerTeam = MatchSystem.instance.GetPlayerTeam(localChad.gameObject);

        float timer = 0.0f;

        while (timer < _DanceDuration)
        {
            timer += Time.DeltaTime;

            if (localChad /*&& otherPlayerTeam != playerTeam*/)
            {
                float distance = Vector3.Distance(localChad.transform.position, transform.position);
                if (distance < ExplosionRadius)
                {
                    localChad.Direction = Vector3.Zero;
                    localChad.rBody.LinearVelocity = Vector3.Zero;
                    localChad.State = ChadControls.STATE.DANCING;
                }
            }

            yield return null;
        }
        localChad.State = ChadControls.STATE.CHADING;
        Explosion();

        if (PointBoi)
        {
            _SpawnedLight = false;
            PointBoi.enabled = false;
        }
    }


    // COLOR CALCS FROM ALBIN
    private float HueFromRGB(Color rgba)
    {
        Vector3 rgb = rgba.ToVector3() / 255.0f;
        float hue = 0;

        float _min = Math.Min(Math.Min(rgb.x, rgb.y), rgb.z);
        float _max = Math.Max(Math.Max(rgb.x, rgb.y), rgb.z);
        float _delta = _max - _min;
        if (_delta != 0)
        {
            if (rgb.x == _max)
                hue = (rgb.y - rgb.z) / _delta;
            else if (rgb.y == _max)
                hue = 2.0f + (rgb.z - rgb.x) / _delta;
            else if (rgb.z == _max)
                hue = 4.0f + (rgb.x - rgb.y) / _delta;
        }
        return hue;
    }

    private Color HSLColor(double hue)
    {
        double saturation = 1.0d;
        double luminosity = 0.5d;

        byte r, g, b;

        double t1, t2;
        double th = hue;

        t2 = (luminosity + saturation) - (luminosity * saturation);
        t1 = 2d * luminosity - t2;

        double tr, tg, tb;
        tr = th + (1.0d / 3.0d);
        tg = th;
        tb = th - (1.0d / 3.0d);

        tr = ColorCalc(tr, t1, t2);
        tg = ColorCalc(tg, t1, t2);
        tb = ColorCalc(tb, t1, t2);
        r = (byte)Math.Round(tr * 255d);
        g = (byte)Math.Round(tg * 255d);
        b = (byte)Math.Round(tb * 255d);
        return new Color(r, g, b);
    }

    private double ColorCalc(double c, double t1, double t2)
    {
        if (c < 0) c += 1d;
        if (c > 1) c -= 1d;
        if (6.0d * c < 1.0d) return t1 + (t2 - t1) * 6.0d * c;
        if (2.0d * c < 1.0d) return t2;
        if (3.0d * c < 2.0d) return t1 + (t2 - t1) * (2.0d / 3.0d - c) * 6.0d;
        return t1;
    }

    public override bool OnWrite(NetDataWriter writer, bool initialState)
    {
        base.OnWrite(writer, initialState);

        writer.Put(_SpawnedLight);

        return true;
    }

    public override void OnRead(NetDataReader reader, bool initialState)
    {
        base.OnRead(reader, initialState);

        _SpawnedLight = reader.GetBool();
    }
}
