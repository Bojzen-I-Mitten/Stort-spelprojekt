using ThomasEngine;
using ThomasEngine.Network;
using System;
using System.Collections;

public class MysteriousCube : Powerup
{
    ChadControls ObjectOwner = null;

    // Coroutines
    IEnumerator MovementClock = null;
    IEnumerator ScaleClock = null;
    IEnumerator RagdollImmunityClock = null;

    private float MovementCountdown;
    private float ScaleCountdown;
    private float RagdollCountdown;
    private float OriginalAcceleration;
    private float OriginalBaseSpeed;
    private float OriginalMaxSpeed;
    private float OriginalRagdollTimer;
    private Vector3 OriginalScale;

    // Tweaking constant
    private float MovementSpeedupDuration = 5.0f;
    private float MovementSpeedDecreaseFactor = 0.5f;
    private float MovementSpeedUpFactor = 2.0f;
    private float ScaleDuration = 5.0f;
    private float ScaleDecreaseFactor = 0.5f;
    private float PushBackForce = 100.0f;
    private float RagDollImunityDuration = 7.0f; 

    public override void OnAwake()
    {
        base.OnAwake();

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
        // Make sure powerups can only be activated once!
        if (activated)
        {
            return;
        }

        activated = true; 
        ChadControls localChad = MatchSystem.instance.LocalChad;

        if (localChad)
        {
            // Choose random power-up 
            System.Random rnd = new System.Random();
            int number = rnd.Next(1, 6);

            // Might need a very small distance to check against or this will modify all chads?

            // The power-up effect only affects the collision object
            if(number == 1)
            {
                // Increase movement speed -> Speed Demon
                OriginalBaseSpeed = localChad.BaseSpeed;
                OriginalMaxSpeed = localChad.MaxSpeed;
                OriginalAcceleration = localChad.Acceleration;

                ConfigureMovementSpeed(MovementSpeedUpFactor, ref localChad);
                MovementClock = MovementTimerRoutine(MovementSpeedupDuration, localChad);
                StartCoroutine(MovementClock);
            }
            else if(number == 2)
            {
                // Ragdoll immunity -> Unstoppable
                OriginalRagdollTimer = localChad.MinimumRagdollTimer;

                RagdollImmunityModifier(ref localChad);
            }
            else if (number == 3)
            {
                // TODO: Increase throw distance
            }
            else if (number == 4)
            {
                // Decrease movement speed -> Never Lucky
                OriginalBaseSpeed = localChad.BaseSpeed;
                OriginalMaxSpeed = localChad.MaxSpeed;
                OriginalAcceleration = localChad.Acceleration;

                ConfigureMovementSpeed(MovementSpeedDecreaseFactor, ref localChad);
                MovementClock = MovementTimerRoutine(MovementSpeedupDuration, localChad);
                StartCoroutine(MovementClock);
            }
            else if (number == 5)
            {
                // Scale down -> Midget
                OriginalScale = localChad.transform.scale;

                ConfigureScale(ScaleDecreaseFactor, ref localChad);
                ScaleClock = ScaleTimerRoutine(ScaleDuration, localChad);
                StartCoroutine(ScaleClock);
            }
            else
            {
                // Push player backwards in the impact direction -> Push Back
                PushBackModifier(ref localChad);
            }
        }
    }

    #region Modifiers
    private void ConfigureMovementSpeed(float movementFactor, ref ChadControls targetChad)
    {
        targetChad.BaseSpeed *= movementFactor;
        targetChad.MaxSpeed *= movementFactor;
        targetChad.Acceleration *= movementFactor;
    }

    private void ConfigureScale(float scaleFactor, ref ChadControls targetChad)
    {
        targetChad.transform.scale *= scaleFactor;
    }

    private void PushBackModifier(ref ChadControls targetChad)
    {
        Vector3 forceDir = targetChad.transform.position - transform.position;
        forceDir.Normalize();
        Vector3 force = forceDir * PushBackForce;

        targetChad.gameObject.GetComponent<Rigidbody>().AddForce(force, Rigidbody.ForceMode.Impulse);
    }

    private void RagdollImmunityModifier(ref ChadControls targetChad)
    {
        targetChad.MinimumRagdollTimer = RagDollImunityDuration;
    }
    #endregion

    #region Coroutines
    IEnumerator MovementTimerRoutine(float seconds, ChadControls targetChad)
    {
        MovementCountdown = seconds;

        while (MovementCountdown > 0)
        {
            Debug.Log("Movement affector remaining: " + MovementCountdown);
            yield return new WaitForSeconds(1.0f);
            MovementCountdown--;
        }

        // Set back original movement values when timer has expired
        targetChad.BaseSpeed = OriginalBaseSpeed;
        targetChad.MaxSpeed = OriginalMaxSpeed;
        targetChad.Acceleration = OriginalAcceleration;
        MovementCountdown = MovementSpeedupDuration;
    }

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

    IEnumerator RagdollTimerRoutine(float seconds, ChadControls targetChad)
    {
        RagdollCountdown = seconds;

        while (RagdollCountdown > 0)
        {
            Debug.Log("Ragdoll immunity affector remaining: " + RagdollCountdown);
            yield return new WaitForSeconds(1.0f);
            RagdollCountdown--;
        }

        // Set back original immunity when timer has expired
        targetChad.MinimumRagdollTimer = OriginalRagdollTimer;
        RagdollCountdown = RagDollImunityDuration;
    }
    #endregion
}