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

    protected bool activated = false;
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
        if (spawner && PickedUp)
        {
            float test = (float)Math.Sin(Time.ElapsedTime);

            transform.position = spawner.transform.position + (new Vector3(0, test, 0))/10.0f;
            transform.localEulerAngles += (new Vector3(0, 360.0f/5.0f, 0)) * Time.DeltaTime;
        }

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
        if (isOwner)
        {
            if (!m_pickupable && !PickedUp)
            {
                if (!activated)
                {
                    Activate();
                    activated = true;
                }
            }
        }

    }


    public override bool OnWrite(NetDataWriter writer, bool initialState)
    {
        base.OnWrite(writer, initialState);
        if (initialState)
        {
            if (spawner)
                writer.Put(spawner.ID);
            else
                writer.Put(-1);
        }


        return true;
    }

    public override void OnRead(NetPacketReader reader, bool initialState)
    {
        base.OnRead(reader, initialState);

        if (initialState)
        {
            int spawnerID = reader.GetInt();
            if ((!spawner && spawnerID != -1) || (spawner && spawner.ID != spawnerID))
            {
                spawner = MatchSystem.instance.Scene.FindNetworkObject(spawnerID)?.gameObject.GetComponent<PowerupSpawner>();
            }
        }


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
}
