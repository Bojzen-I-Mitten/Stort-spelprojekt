using ThomasEngine;
using ThomasEngine.Network;
using System;
using System.Collections;

public class ToySoldier : Powerup
{
    ChadControls ObjectOwner = null;

    // Coroutines
    IEnumerator ScaleClock = null;

    private float ScaleCountdown;
    private Vector3 OriginalScale;

    // Tweaking constants
    private float ScaleDuration = 5.0f;
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

                //OriginalScale = collisionChad.transform.scale;

                //ConfigureScale(ScaleDecreaseFactor, ref collisionChad);
                //ScaleClock = ScaleTimerRoutine(ScaleDuration, collisionChad);
                //StartCoroutine(ScaleClock);
                //gameObject.transform.scale = new Vector3(0);

                // Scale down cube to 0 then disable when the requested corot has ended
                StartCoroutine(RemoveCube());
            }
        }
    }

    private void ConfigureScale(float scaleFactor, ref ChadControls targetChad)
    {
        targetChad.transform.scale *= scaleFactor;
    }

    #region Coroutines
    IEnumerator ScaleTimerRoutine(float seconds, ChadControls targetChad)
    {
        ScaleCountdown = seconds;

        while (ScaleCountdown > 0)
        {
            Debug.Log("Scale affector remaining: " + ScaleCountdown);
            yield return new WaitForSeconds(1.0f);
            ScaleCountdown--;
        }

        // Set back original scale when timer has expired
        // TODO: Make this better with a scale up over a short amount of time instead of instant growth
        targetChad.transform.scale = OriginalScale;
        ScaleCountdown = ScaleDuration;
    }

    private IEnumerator RemoveCube()
    {
        yield return null;
        Remove();
    }
    #endregion
}