using System;
using System.Collections;
using LiteNetLib;
using LiteNetLib.Utils;
using ThomasEngine;
using ThomasEngine.Network;

public class PickupableObject : NetworkComponent
{
    public Rigidbody m_rigidBody;

    public Transform PickupOffset { get; set; } = null;
    public float MovementSpeedModifier { get; set; } = 1.0f;
    public bool DropOnRagdoll { get; set; } = true;

    public bool m_throwable = false;
    private float chargeTimeCurrent;
    public float chargeTimeMax { get; set; } = 4.0f;
    public Collider PickupCollider { get; set; }
    [Newtonsoft.Json.JsonIgnore]
    public bool charging { get { return chargeTimeCurrent > 0.00001f; } }

    public ChadControls _Chad;
    private RenderComponent m_renderComponent;

    public override void Awake()
    {
        m_rigidBody = gameObject.GetComponent<Rigidbody>();
        m_renderComponent = gameObject.GetComponent<RenderComponent>();
        chargeTimeCurrent = 0.0f;
        if (!PickupCollider)
            Debug.LogError("Pickup collider empty");
    }

    public override void Update()
    {
        if (charging)
        {
            ChargeEffect();
        }
    }

    public void SetChargeTime(float other)
    {
        chargeTimeCurrent = other;
    }

    public float GetChargeTime()
    {
        return chargeTimeCurrent;
    }

    virtual public void ChargeEffect()
    {

    }

    virtual public void OnThrow()
    {

    }

    virtual public void Throw(Vector3 camPos, Vector3 force)
    {
        Vector3 pos = camPos;
        Drop();
        transform.position = pos;
        transform.LookAt(transform.position + Vector3.Normalize(force));
        m_rigidBody.Position = transform.position;
        m_rigidBody.Rotation = transform.rotation;
        StartCoroutine(ThrowRoutine(force));
        OnThrow();
        SendRPC("OnThrow");
    }

    public IEnumerator ThrowRoutine(Vector3 force)
    {
        m_rigidBody.AttachedCollider.isTrigger = true;
        yield return null;
        m_rigidBody.AddForce(force, Rigidbody.ForceMode.Impulse);
        yield return new WaitForSeconds(0.1f);
        m_rigidBody.AttachedCollider.isTrigger = false;
    }


    virtual public void StopEmitting()
    {

    }

    virtual public void Cleanup()
    {
        chargeTimeCurrent = 0;
    }

    public void Drop()
    {
        if(PickupCollider.enabled == false)
        {
            RPCDrop();
            SendRPC("RPCDrop");
        }

    }

    public virtual void OnDrop()
    {

    }

    public void RPCDrop()
    {
        if (PickupCollider.enabled == false)
        {
            
            m_rigidBody.enabled = true;
            
            gameObject.GetComponent<NetworkTransform>().SyncMode = NetworkTransform.TransformSyncMode.SyncRigidbody;

            transform.SetParent(null, true);
            if (_Chad)
            {
                _Chad.PickedUpObject = null;
                _Chad = null;
            }
            OnDrop();
            StopEmitting();
            Cleanup();
        }
    }

    //Never call this method without also calling RPC.
    virtual public void OnActivate()
    {
    }

    public void Activate()
    {
        OnActivate();
        SendRPC("OnActivate");
    }

    virtual public void Pickup(ChadControls chad, Transform hand)
    {

        if (!m_rigidBody)
            m_rigidBody = gameObject.GetComponent<Rigidbody>();

        m_rigidBody.enabled = false;

        transform.parent = hand;
        transform.localPosition = Vector3.Zero;
        transform.localRotation = Quaternion.Identity;
        if (PickupOffset)
        {
            transform.localPosition = PickupOffset.localPosition;
            transform.localRotation = PickupOffset.localRotation;
        }

            
        chad.PickedUpObject = this;
        
        _Chad = chad;
        PickupCollider.enabled = false;
        gameObject.GetComponent<NetworkTransform>().SyncMode = NetworkTransform.TransformSyncMode.SyncNone;
        
    }

   public override bool OnWrite(NetDataWriter writer, bool initialState)
    {
        writer.Put(chargeTimeCurrent);
        if (initialState)
        {
            writer.Put(PickupCollider.enabled);
            int syncMode = (int)gameObject.GetComponent<NetworkTransform>().SyncMode;
            writer.Put(syncMode);
        }
        return true;
    }

    public override void OnRead(NetPacketReader reader, bool initialState)
    {
        if (isOwner)
        {
            reader.GetFloat();
            if (initialState)
            {
                reader.GetBool();
                reader.GetBool();
                reader.GetInt();
            }
            return;
        }
        chargeTimeCurrent = reader.GetFloat();
        if (initialState)
        {

            PickupCollider.enabled = reader.GetBool();
            NetworkTransform.TransformSyncMode syncMode = (NetworkTransform.TransformSyncMode)reader.GetInt();
            gameObject.GetComponent<NetworkTransform>().SyncMode = syncMode;
        }

    }


    public override void OnLostOwnership()
    {


    }

    virtual public void Reset()
    {
        RPCDrop();
        transform.scale = Vector3.One;
        chargeTimeCurrent = 0.0f;
        _Chad = null;
        PickupCollider.enabled = true;
        m_rigidBody.IgnoreNextTransformUpdate();
    }
}