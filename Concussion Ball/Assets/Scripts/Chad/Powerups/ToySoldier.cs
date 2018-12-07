using ThomasEngine;
using ThomasEngine.Network;
using System;
using System.Collections;
using LiteNetLib;

public class ToySoldier : Powerup
{
    ChadControls ObjectOwner = null;
    public Material ToySoldierMaterial { get; set; }

    public override void OnAwake()
    {
        base.OnAwake();

        BaseThrowForce = 15.0f;
        MaxThrowForce = 25.0f;
        ThrowForce = BaseThrowForce;
        m_throwable = true; // Change depending on power-up
        gameObject.GetComponent<Rigidbody>().Friction = 2.0f;
    }

    public override void Update()
    {
        base.Update();
    }

    // If this is a throwable power-up this function will be called
    public override void Throw(Vector3 camPos, Vector3 direction)
    {
        base.Throw(camPos, Vector3.Normalize(direction) * ThrowForce);
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
                    MatchSystem.instance.SendRPC(peer, ID, "RPCIGotHit");
                }

                // Remove powerup
                PickupCollider.enabled = false;
                pickedUp = true;
                activated = true;
                StartCoroutine(RemoveCube());
            }
        }
    }

    public void RPCIGotHit()
    {
        ChadControls localChad = MatchSystem.instance.LocalChad;

        // Scale and movement decrease
        // The scale up effect with the collider look's a bit weird
        // Chad can still sprint with this powerup
        // Collider can go through some walls when thrown
        // Object can spin when thrown to the ground in unrealistic manner (like on powerup spawn)
        localChad.ToySoldierAffected = true;
        localChad.transform.scale *= 0.5f;
        localChad.Acceleration *= 0.5f;
        localChad.BaseSpeed *= 0.5f;
        localChad.MaxSpeed *= 0.5f;

        // Collider
        CapsuleCollider capsule = localChad.gameObject.GetComponent<CapsuleCollider>();
        capsule.center = new Vector3(0, 0.3f, 0);
        capsule.height *= 0.5f;
        capsule.radius *= 0.5f;
        localChad.rBody.Mass *= 0.5f;

        // Material
        RenderSkinnedComponent render = localChad.gameObject.GetComponent<RenderSkinnedComponent>();
        render.SetMaterial(0, ToySoldierMaterial);
        render.SetMaterial(1, ToySoldierMaterial);
        render.SetMaterial(2, ToySoldierMaterial);
    }

    private IEnumerator RemoveCube()
    {
        yield return null;
        Remove();
    }
}