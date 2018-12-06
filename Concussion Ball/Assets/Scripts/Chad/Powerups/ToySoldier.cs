using ThomasEngine;
using ThomasEngine.Network;
using System;
using System.Collections;

public class ToySoldier : Powerup
{
    ChadControls ObjectOwner = null;

    // Tweaking constant
    private float ScaleDecreaseFactor = 0.5f;

    public override void OnAwake()
    {
        base.OnAwake();

        BaseThrowForce = 15.0f;
        MaxThrowForce = 25.0f;
        ThrowForce = BaseThrowForce;
        m_throwable = true; // Change depending on power-up
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
                Debug.Log("Activated");
                activated = true;
                collisionChad.ToySoliderAffected = true;              
                colliderObject.transform.scale *= ScaleDecreaseFactor;

                // Remove powerup
                StartCoroutine(RemoveCube());
            }
        }
    }

    private IEnumerator RemoveCube()
    {
        yield return null;
        Remove();
    }
}