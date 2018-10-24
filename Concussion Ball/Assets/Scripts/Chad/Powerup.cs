using System;
using System.Collections;
using LiteNetLib;
using LiteNetLib.Utils;
using ThomasEngine;
using ThomasEngine.Network;

public class Powerup : PickupableObject
{
    /* Powerup charge particle emitters
       Powerup charge particle textures*/

    protected RenderComponent m_renderComponent; // in case we want to alter material with charge up
    
    public override void Start()
    {
        base.Start();

        m_rigidBody.IsKinematic = true;
        m_renderComponent = gameObject.GetComponent<RenderComponent>();

        #region Init emitters
        // emitter bois
        #endregion
    }

    public override void Update()
    {   
    }

    override public void ChargeEffect()
    {
        /*do particle bois and stuff*/
    }

    override public void StopEmitting()
    {
        StartCoroutine(StopEmission());
    }

    private IEnumerator StopEmission()
    {
        float timer = 3;
        while (timer > 0)
        {
            timer -= Time.DeltaTime;
            yield return null;
        }

        // set emits to false
    }

    override public void Cleanup()
    {

    }

    public override void OnCollisionEnter(Collider collider)
    {
        if (!m_pickupable)
        {
            OnActivate();
        }
    }
}
