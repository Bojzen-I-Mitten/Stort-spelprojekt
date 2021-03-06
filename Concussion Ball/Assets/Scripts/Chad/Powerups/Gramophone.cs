using ThomasEngine;
using ThomasEngine.Network;
using System.Collections;
//using System.Collections.Generic;
//using System.ComponentModel;
using System.Linq;
using System;
using LiteNetLib.Utils;

public class Gramophone : Powerup
{
    ChadControls ObjectOwner = null;
    Collider _FirstCollider;

    public LightComponent PointBoi { get; set; }

    public AudioClip GramophoneSound { get; set; }
    private SoundComponent GramophoneClip;

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
    private float _Timer;

    private float _DespawnTimer;
    private float _DespawnTime;

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
        _Timer = 0.0f;

        _DespawnTimer = 0.0f;
        _DespawnTime = 30.0f;

        GramophoneClip = gameObject.AddComponent<SoundComponent>();
        GramophoneClip.Type = SoundComponent.SoundType.Effect;
        GramophoneClip.Clip = GramophoneSound;
        GramophoneClip.Looping = false;
        GramophoneClip.Is3D = true;

        Rigidbody rb = gameObject.GetComponent<Rigidbody>();
        rb.CcdMotionThreshold = 1e-7f;
        rb.CcdSweptSphereRadius = 0.1f;

        #region big meme particle emitter bois
        _Note1 = gameObject.AddComponent<ParticleEmitter>();
        
        _Note1.Texture = (Texture2D)Resources.LoadThomasPath("%THOMAS_ASSETS%/Particles/note1_particle.png"); 
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
      
        _Note2.Texture = (Texture2D)Resources.LoadThomasPath("%THOMAS_ASSETS%/Particles/note2_particle.png"); 
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
        
        _ShockWave.Texture = (Texture2D)Resources.LoadThomasPath("%THOMAS_ASSETS%/Particles/shockwave_particle.png");
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

            // Despawn if thrown off edge 
            if (_DespawnTimer > 0)
            {
                _DespawnTimer += Time.DeltaTime;
                if (_DespawnTimer > _DespawnTime)
                {
                    base.Activate();
                    _DespawnTimer = 0.0f;
                }
            }

            if (_Landed)
            {
                m_rigidBody.LinearVelocity = Vector3.Zero;
                m_rigidBody.LinearVelocity = new Vector3(0, 3, 0); //works better than forces
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
            if (_Landed)
            {
                m_rigidBody.LinearVelocity = Vector3.Zero;
                m_rigidBody.LinearVelocity = new Vector3(0, 3, 0); //works better than forces
                _Landed = false;
                if (_ShockWave)
                    _ShockWave.EmitOneShot(1);
            }
        }
    }

    // if this is a throwable power-up this function will be called
    public override void Throw(Vector3 camPos, Vector3 direction)
    {
        base.Throw(camPos, Vector3.Normalize(direction) * ThrowForce);

        _DespawnTimer += Time.DeltaTime;
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
            
            _JumpTimer += Time.DeltaTime;

            base.OnCollisionEnter(collider);
        }
        else if (_JumpTimer > 0)
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

        Debug.Log("ACTIVATE");
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
        //List<ChadControls> affectedChads = new List<ChadControls>();

        if (_Note1 && _Note2)
        {
            _Note1.Emit = true;
            _Note2.Emit = true;
        }
        if (GramophoneClip)
        {
            GramophoneClip.Play();
        }

        // Start blasting 
        ChadControls localChad = MatchSystem.instance.LocalChad;

        TEAM_TYPE playerTeam = MatchSystem.instance.GetPlayerTeam(ObjectOwner.gameObject);
        TEAM_TYPE otherPlayerTeam = MatchSystem.instance.GetPlayerTeam(localChad.gameObject);

        while (_Timer < _DanceDuration)
        {
            _Timer += Time.DeltaTime;

            if (localChad)
            {
                float distance = Vector3.Distance(localChad.transform.position, transform.position);
                if (distance < ExplosionRadius && localChad.State != ChadControls.STATE.RAGDOLL && localChad.State != ChadControls.STATE.DANCING)
                {
                    localChad.Direction = Vector3.Zero;
                    localChad.rBody.LinearVelocity = new Vector3(0, localChad.rBody.LinearVelocity.y, 0); ;
                    localChad.CurrentVelocity = Vector2.Zero;
                    localChad.State = ChadControls.STATE.DANCING;
                    ChadHud.Instance.DeactivateAimHUD();
                    localChad.ChargeTime = 0;
                    //affectedChads.Add(localChad);
                }
                else if (distance > ExplosionRadius && localChad.State == ChadControls.STATE.DANCING)
                {
                    //foreach (var player in affectedChads)
                    //{
                    //    if (player == localChad)
                    //    {
                    //        localChad.State = ChadControls.STATE.CHADING;
                    //        affectedChads.Remove(player); // buggy?
                    //    }
                    //}
                    localChad.State = ChadControls.STATE.CHADING;
                }
            }

            yield return null;
        }
        GramophoneClip.Stop();

        // Reset all dancing Chads, the party is over
        //foreach (var player in affectedChads)
        //{
        //    if (player == localChad && localChad.State == ChadControls.STATE.DANCING)
        //    {
        //        localChad.State = ChadControls.STATE.CHADING;
        //        //affectedChads.Remove(player); // buggy?
        //    }
        //}
        if (localChad.State == ChadControls.STATE.DANCING)
            localChad.State = ChadControls.STATE.CHADING;
        //affectedChads.Clear();
        Explosion();
        
  
    }

    public override void Reset()
    {
        base.Reset();

        StopCoroutine(BlastingMusic());
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
        _Timer = 0.0f;

        _DespawnTimer = 0.0f;
        _DespawnTime = 30.0f;
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
        writer.Put(_Landed);
        writer.Put(_JumpTimer);
        writer.Put(_Timer);

        return true;
    }

    public override void OnRead(NetDataReader reader, bool initialState)
    {
        base.OnRead(reader, initialState);

        if(isOwner)
        {
            reader.GetBool();
            reader.GetBool();
            reader.GetFloat();
            reader.GetFloat();
            return;
        }
        _SpawnedLight = reader.GetBool();
        _Landed = reader.GetBool();
        _JumpTimer = reader.GetFloat();
        _Timer = reader.GetFloat();
    }

}
