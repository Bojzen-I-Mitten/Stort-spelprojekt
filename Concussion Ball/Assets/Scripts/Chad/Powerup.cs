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
    public PowerupSpawner spawner;
    public GameObject colliderObject;


    protected bool activated = false;
    public override void OnAwake()
    {
        base.OnAwake();
        m_renderComponent = gameObject.GetComponent<RenderComponent>();
        chargeTimeMax = 0.5f;
        colliderObject = null;
    }

    public override void OnEnable()
    {
        base.OnEnable();
        activated = false;
        m_rigidBody.enabled = false;
    }

    public override void Update()
    {
        base.Update();
        if (spawner)
        {
            float test = (float)Math.Sin(Time.ElapsedTime);

            transform.position = spawner.transform.position + (new Vector3(0, test, 0))/10.0f;
            transform.localEulerAngles += (new Vector3(0, 360.0f/5.0f, 0)) * Time.DeltaTime;
        }

    }

    override public void ChargeEffect()
    {
        base.ChargeEffect();
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



    public override void Pickup(ChadControls chad, Transform hand)
    {
        
        base.Pickup(chad, hand);

        if (spawner)
        {
            spawner.Free();
            spawner = null;
            Debug.Log("Cleared spawner");
        }
            
    }

    public override void OnCollisionEnter(Collider collider)
    {
        if (collider)
            colliderObject = collider.gameObject;

        if (isOwner)
        {
            if (PickupCollider.enabled == false && m_rigidBody.enabled && pickedUp == false)
            {
                if (!activated)
                {
                    Debug.Log("boom!");
                    Activate();
                    
                }
            }
        }

    }


    public override bool OnWrite(NetDataWriter writer, bool initialState)
    {
        base.OnWrite(writer, initialState);
        if (spawner)
            writer.Put(spawner.ID);
        else
            writer.Put(-1);

        writer.Put(activated);
        return true;
    }

    public override void OnRead(NetDataReader reader, bool initialState)
    {
        base.OnRead(reader, initialState);

        int spawnerID = reader.GetInt();
        if ((!spawner && spawnerID != -1) || (spawner && spawner.ID != spawnerID))
        {
            Reset();
            spawner = MatchSystem.instance.Scene.FindNetworkObject(spawnerID)?.gameObject.GetComponent<PowerupSpawner>();    
        }
        if (spawnerID == -1)
            spawner = null;

        bool newActivation = reader.GetBool();
        if (!activated && newActivation)
        {
            OnActivate();
        }
        activated = newActivation;

    }


    public void RPCRemove()
    {
        RPCDrop();
        if (spawner)
        {
            if (spawner.isOwner)
                spawner.Free();
            spawner = null;
        }
        MatchSystem.instance.PowerupManager.RecyclePowerup(this);
    }

    public void Remove()
    {

        RPCRemove();
        SendRPC("RPCRemove");
    }

    public override void Disable()
    {
        base.Disable();
        activated = false;
    }

    public override void Reset()
    {
        base.Reset();
        spawner = null;
        m_rigidBody.enabled = false;
        activated = false;
    }
}
