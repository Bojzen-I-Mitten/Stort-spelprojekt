using ThomasEngine;
using ThomasEngine.Network;
using System;
using System.Collections;
using LiteNetLib;

public class ToySoldier : Powerup
{
    ChadControls ObjectOwner = null;
    public Material ToySoldierMaterial { get; set; }
    private float _DespawnTimer;

    public override void OnAwake()
    {
        base.OnAwake();

        BaseThrowForce = 15.0f;
        MovementSpeedModifier = 0.7f;
        MaxThrowForce = 25.0f;
        ThrowForce = BaseThrowForce;
        m_throwable = true; // Change depending on power-up
        gameObject.GetComponent<Rigidbody>().Friction = 2.5f;
        _DespawnTimer = 0.0f;
    }

    public override void Update()
    {
        base.Update();

        // Despawn if Vindaloo has not hit anyone in 30 seconds
        if (_DespawnTimer > 30)
            base.Activate();
        else if (_DespawnTimer > 0)
            _DespawnTimer += Time.DeltaTime;
    }

    // If this is a throwable power-up this function will be called
    public override void Throw(Vector3 camPos, Vector3 direction)
    {
        base.Throw(camPos, Vector3.Normalize(direction) * ThrowForce);

        _DespawnTimer += Time.DeltaTime;
    }

    public override void SaveObjectOwner(ChadControls chad)
    {
        ObjectOwner = chad;
    }

    // This function will be called upon powerup use / collision after trown
    public override void OnActivate()
    {
        if (activated)
            return;

        PickupCollider.enabled = true;
        pickedUp = false;

        // Make sure powerups can only be activated once!
        if (colliderObject != null)
        {
            ChadControls collisionChad = colliderObject.GetComponent<ChadControls>();

            if (collisionChad != null && collisionChad != ObjectOwner)
            {
                NetPeer peer = null;
                foreach (var player in MatchSystem.instance.Scene.Players)
                {
                    if(player.Value == collisionChad.gameObject.GetComponent<NetworkIdentity>())
                    {
                        peer = player.Key;
                        break;
                    }
                }

                if(peer != null)
                {
                   // Network
                    MatchSystem.instance.SendRPC(peer, ID, "RPCIGotHit");
                }

                // Local changes
                RenderSkinnedComponent render = collisionChad.gameObject.GetComponent<RenderSkinnedComponent>();
                render.SetMaterial(0, ToySoldierMaterial);
                render.SetMaterial(1, ToySoldierMaterial);
                render.SetMaterial(2, ToySoldierMaterial);

                // Remove powerup
                PickupCollider.enabled = false;
                pickedUp = true;
                activated = true;
                StartCoroutine(RemoveToySoldier());
                _DespawnTimer = 0.0f;
            }
        }
    }

    public void RPCIGotHit(int ID)
    {
        ChadControls localChad = MatchSystem.instance.LocalChad;

        // Scale and movement decrease
        localChad.ToySoldierAffected = true;
        localChad.transform.scale *= 0.5f;
        localChad.Acceleration *= 0.5f;
        localChad.BaseSpeed *= 0.5f;
        localChad.MaxSpeed *= 0.5f;

        // Collider
        CapsuleCollider capsule = localChad.gameObject.GetComponent<CapsuleCollider>();
        capsule.center = new Vector3(0, 0.32f, 0);
        capsule.height *= 0.5f;
        capsule.radius *= 0.5f;
        localChad.rBody.Mass *= 0.5f;
        localChad.FirstJumpForce *= 0.3f;
        localChad.SecondJumpForce *= 0.3f;

        // Material
        RenderSkinnedComponent render = localChad.gameObject.GetComponent<RenderSkinnedComponent>();
        render.SetMaterial(0, ToySoldierMaterial);
        render.SetMaterial(1, ToySoldierMaterial);
        render.SetMaterial(2, ToySoldierMaterial);

        PickupCollider.enabled = false;
        pickedUp = true;
        activated = true;
        StartCoroutine(RemoveToySoldier());
        _DespawnTimer = 0.0f;
    }

    private IEnumerator RemoveToySoldier()
    {
        yield return null;
        Remove();
    }
}