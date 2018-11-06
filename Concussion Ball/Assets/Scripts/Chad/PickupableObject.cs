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
    public bool m_pickupable = true;

    public float chargeTimeCurrent;
    public float chargeTimeMax { get; set; } = 4.0f;

    private ChadControls _Chad;
    private RenderComponent m_renderComponent;

    public bool PickedUp = false;

    public override void Awake()
    {
        m_rigidBody = gameObject.GetComponent<Rigidbody>();
        m_renderComponent = gameObject.GetComponent<RenderComponent>();
        chargeTimeCurrent = 0.0f;
    }

    public override void Update()
    {
        Debug.Log("TEST");
    }

    virtual public void ChargeEffect()
    {

    }

    virtual public void Throw(Vector3 camPos, Vector3 force)
    {
        if (PickedUp)
        {
            Vector3 pos = camPos;
            Drop();
            StartCoroutine(ThrowRoutine());
            transform.position = pos;
            transform.LookAt(transform.position + Vector3.Normalize(force));
            m_rigidBody.AddForce(force, Rigidbody.ForceMode.Impulse);
        }
    }

    public IEnumerator ThrowRoutine()
    {
        gameObject.GetComponent<Collider>().isTrigger = true;
        yield return new WaitForSeconds(0.1f);
        gameObject.GetComponent<Collider>().isTrigger = false;
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
        if(PickedUp)
        {
            RPCDrop();
            SendRPC("RPCDrop");
        }

    }

    public void RPCDrop()
    {
        if (PickedUp)
        {
            PickedUp = false;
            m_rigidBody.IsKinematic = false;
            m_rigidBody.enabled = true;
            
            gameObject.GetComponent<NetworkTransform>().SyncMode = NetworkTransform.TransformSyncMode.SyncRigidbody;

            Debug.Log("drop");
            transform.SetParent(null, true);
            if (_Chad)
            {
                _Chad.PickedUpObject = null;
                _Chad = null;
            }
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
        if(m_pickupable && !PickedUp)
        {

            if (!m_rigidBody)
                m_rigidBody = gameObject.GetComponent<Rigidbody>();

            Debug.Log("pickup");
            m_rigidBody.IsKinematic = false;
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
            PickedUp = true;
            m_pickupable = false;
            _Chad = chad;
        }
    }


    virtual public void Reset()
    {
        RPCDrop();
        chargeTimeCurrent = 0.0f;
        PickedUp = false;
        m_pickupable = true;
        transform.localRotation = Quaternion.Identity;
        m_rigidBody.enabled = true;
        _Chad = null;
    }
}