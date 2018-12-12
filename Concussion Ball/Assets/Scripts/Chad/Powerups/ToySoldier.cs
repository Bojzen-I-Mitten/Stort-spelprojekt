using ThomasEngine;
using ThomasEngine.Network;
using System;
using System.Collections;
using LiteNetLib;

public class ToySoldier : Powerup
{
    ChadControls ObjectOwner = null;
    public Material ToySoldierMaterial { get; set; }

    private ChadControls collisionChad;
    private float _DespawnTimer;
    private float _DespawnTime;

    public override void OnAwake()
    {
        base.OnAwake();

        BaseThrowForce = 15.0f;
        MovementSpeedModifier = 0.7f;
        MaxThrowForce = 25.0f;
        ThrowForce = BaseThrowForce;
        m_throwable = true; // Change depending on power-up
        gameObject.GetComponent<Rigidbody>().Friction = 2.5f;

        collisionChad = null;

        _DespawnTime = 30.0f;
        _DespawnTimer = 0.0f;
    }

    public override void Update()
    {
        base.Update();

        // Check for despawn
        if (isOwner)
        {
            if (_DespawnTimer > 0)
            {
                _DespawnTimer += Time.DeltaTime;
                if (_DespawnTimer > _DespawnTime)
                {
                    // Nothing was hit
                    _DespawnTimer = 0.0f;
                    StartCoroutine(RemoveToySoldier());
                }
            }
        }   
    }

    // If this is a throwable power-up this function will be called
    public override void Throw(Vector3 camPos, Vector3 direction)
    {
        base.Throw(camPos, Vector3.Normalize(direction) * ThrowForce);

        if(isOwner)
        {
            _DespawnTimer = Time.DeltaTime;
            PickupCollider.enabled = false;
        }
    }

    public override void SaveObjectOwner(ChadControls chad)
    {
        ObjectOwner = chad;
    }

    public override void OnCollisionEnter(Collider collider)
    {
        ChadControls otherChad = collider.gameObject.GetComponent<ChadControls>();
        

        ChadControls localChad = MatchSystem.instance.LocalChad;
        // Direct Hit with a Chad
        if (otherChad)
        {
            if (otherChad == localChad)
            {
                TEAM_TYPE colliderTeam = MatchSystem.instance.GetPlayerTeam(localChad.gameObject);
                TEAM_TYPE throwerTeam = MatchSystem.instance.GetPlayerTeam(ObjectOwner.gameObject);
                if (colliderTeam != throwerTeam)
                {
                    localChad.ToySoldierAffected = true;
                    collisionChad = localChad;
                    Activate();
                }
            } 
        }
        else if (isOwner)
        {
            PickupCollider.enabled = true;
            pickedUp = false;
            _DespawnTimer = 0.0f;
        }         
    }

    // This function will be called upon powerup use / collision after trown
    public override void OnActivate()
    {
        if (activated)
            return;

        ChadControls localChad = MatchSystem.instance.LocalChad;            

        if (localChad.ToySoldierAffected)
        {
            RenderSkinnedComponent render = localChad.gameObject.GetComponent<RenderSkinnedComponent>();
            localChad.ChadSecondMaterial = render.GetMaterial(1);

            RPCSetTiny();

            localChad.Acceleration *= 0.5f;
            localChad.BaseSpeed *= 0.5f;
            localChad.MaxSpeed *= 0.5f;
            localChad.transform.scale *= 0.5f;

            
            localChad.FirstJumpForce *= 0.3f;
            localChad.SecondJumpForce *= 0.3f;

            activated = true;
        }
        StartCoroutine(RemoveToySoldier());
    }

    private IEnumerator RemoveToySoldier()
    {
        yield return null;
        Remove();
    }

    public override void Reset()
    {
        base.Reset();

        _DespawnTime = 30.0f;
        _DespawnTimer = 0.0f;

        ObjectOwner = null;
    }

    public void RPCSetTiny()
    {
        SendRPC("SetTiny");
        SetTiny();
    }

    private void SetTiny()
    {
        //foreach(var player in MatchSystem.instance.Scene.Players)
        //{
        //    if (player.Value == collisionChad.gameObject.GetComponent<NetworkIdentity>())
        //    {
        //        RenderSkinnedComponent render = 
        //    }
        //}
        RenderSkinnedComponent render = collisionChad.gameObject.GetComponent<RenderSkinnedComponent>();
        render.SetMaterial(0, ToySoldierMaterial);
        render.SetMaterial(1, ToySoldierMaterial);
        render.SetMaterial(2, ToySoldierMaterial);

        collisionChad.rBody.Mass *= 0.5f;

        CapsuleCollider capsule = collisionChad.gameObject.GetComponent<CapsuleCollider>();
        capsule.center = new Vector3(0, 0.32f, 0);
        capsule.height *= 0.5f;
        capsule.radius *= 0.5f;
    }
}