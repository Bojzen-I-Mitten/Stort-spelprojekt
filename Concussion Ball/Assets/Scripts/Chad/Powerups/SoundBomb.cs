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
    public Texture2D _NoteTex1 { get; set; }
    public Texture2D _NoteTex2 { get; set; }
    public Texture2D _ShockWaveTex { get; set; }


    //public AudioClip VindalooExplosionSound { get; set; }
    //private SoundComponent ExplosionSound;

    public float ExplosionRadius;
    public float ExplosionForce;

    private ParticleEmitter _Note1;
    private ParticleEmitter _Note2;
    private ParticleEmitter _ShockWave;

    private float _JumpTimer;
    private float _DanceDuration;
    private float _Hue;
    private bool _SpawnedLight;
    private bool _Landed;

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
        _Landed = false;
        //ExplosionSound = gameObject.AddComponent<SoundComponent>();
        //ExplosionSound.Type = SoundComponent.SoundType.Effect;
        //ExplosionSound.Clip = VindalooExplosionSound;
        //ExplosionSound.Looping = false;
        //ExplosionSound.Is3D = true;

        #region big meme particle emitter bois
        _Note1 = gameObject.AddComponent<ParticleEmitter>();
        if (_NoteTex1 != null)
            _Note1.Texture = _NoteTex1;
        _Note1.MinSize = 0.2f;
        _Note1.MaxSize = 0.5f;
        _Note1.EndSize = 0.5f;
        _Note1.MinLifeTime = 1.0f;
        _Note1.MaxLifeTime = 1.0f;
        _Note1.EmissionRate = 6;
        _Note1.MinRotationSpeed = 0.0f;
        _Note1.MaxRotationSpeed = 0.0f;
        _Note1.MinSpeed = 0.0f;
        _Note1.MaxSpeed = 0.0f;
        _Note1.EndSpeed = 0.0f;
        _Note1.DistanceFromSphereCenter = 0;
        _Note1.Radius = 3.0f;
        _Note1.Gravity = 0.0f;
        _Note1.SpawnAtEdge = true;
        _Note1.BlendState = ParticleEmitter.BLEND_STATES.ALPHA;

        _Note2 = gameObject.AddComponent<ParticleEmitter>();
        if (_NoteTex2 != null)
            _Note2.Texture = _NoteTex2;
        _Note2.MinSize = 0.2f;
        _Note2.MaxSize = 0.5f;
        _Note2.EndSize = 0.5f;
        _Note2.MinLifeTime = 1.0f;
        _Note2.MaxLifeTime = 1.0f;
        _Note2.EmissionRate = 6;
        _Note2.MinRotationSpeed = 0.0f;
        _Note2.MaxRotationSpeed = 0.0f;
        _Note2.MinSpeed = 0.0f;
        _Note2.MaxSpeed = 0.0f;
        _Note2.EndSpeed = 0.0f;
        _Note2.DistanceFromSphereCenter = 0;
        _Note2.Radius = 3.0f;
        _Note2.Gravity = 0.0f;
        _Note2.SpawnAtEdge = true;
        _Note2.BlendState = ParticleEmitter.BLEND_STATES.ALPHA;

        _ShockWave = gameObject.AddComponent<ParticleEmitter>();
        if (_ShockWaveTex != null)
            _ShockWave.Texture = _ShockWaveTex;
        _ShockWave.MinSize = 0.0f;
        _ShockWave.MaxSize = 0.0f;
        _ShockWave.EndSize = 4.0f;
        _ShockWave.MinLifeTime = 1.0f;
        _ShockWave.MaxLifeTime = 1.0f;
        _ShockWave.EmissionRate = 1;
        _ShockWave.MinRotationSpeed = 0.0f;
        _ShockWave.MaxRotationSpeed = 0.0f;
        _ShockWave.MinSpeed = 0.0f;
        _ShockWave.MaxSpeed = 0.0f;
        _ShockWave.EndSpeed = 0.0f;
        _ShockWave.DistanceFromSphereCenter = 0;
        _ShockWave.Radius = 1.0f;
        _ShockWave.Gravity = 0.0f;
        _ShockWave.SpawnAtEdge = false;
        _ShockWave.BlendState = ParticleEmitter.BLEND_STATES.ALPHA;
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
                    _JumpTimer = Time.DeltaTime; // reset
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
            if (_Landed)
            {
                Debug.Log("Landed, applying force");
                m_rigidBody.LinearVelocity = Vector3.Zero;
                m_rigidBody.AddForce(new Vector3(0, 250, 0));
                _Landed = false;
                if(_ShockWave)
                    _ShockWave.EmitOneShot(1);
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
            _Landed = true;
            if (PointBoi)
            {
                _SpawnedLight = true;
                PointBoi.enabled = true;
            }
            m_rigidBody.Friction = 100.0f;
            m_rigidBody.LinearVelocity = Vector3.Zero;
            m_rigidBody.FreezeRotation = new Vector3(0, 0, 0); // freeze ya'll's music box
            if (_Note1 && _Note2)
            {
                _Note1.Emit = true;
                _Note2.Emit = true;
            }
            _JumpTimer += Time.DeltaTime;

            base.OnCollisionEnter(collider);
        }
        else if (_JumpTimer > 0.1f)
        {
            _Landed = true;
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
        if (_Note1 && _Note2)
        {
            _Note1.Emit = false;
            _Note2.Emit = false;
        }
    }

    public override void Reset()
    {
        base.Reset();

        if (PointBoi)
        {
            _SpawnedLight = false;
            PointBoi.enabled = false;
        }
        if (_Note1 && _Note2)
        {
            _Note1.Emit = false;
            _Note2.Emit = false;
        }
        _Landed = false;
        _JumpTimer = 0.0f;
    }

    // COLOR CALCS FROM ALBIN
    #region ALBIN_COLOR
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
    #endregion

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
