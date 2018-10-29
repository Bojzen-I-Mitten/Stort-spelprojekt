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
    protected bool m_pickedUp { get { if (m_rigidBody != null) return !m_rigidBody.enabled; else return false; } set { if (m_rigidBody != null) m_rigidBody.enabled = !value; } }

    public override void Start()
    {
        m_rigidBody = gameObject.GetComponent<Rigidbody>();
        m_renderComponent = gameObject.GetComponent<RenderComponent>();

        chargeTimeCurrent = 0.0f;
    }

    public override void Update()
    {
        Debug.Log("TEST");
    }

    public bool GetPickedUp()
    {
        return m_pickedUp;
    }

    virtual public void ChargeEffect()
    {

    }

    virtual public void Throw(Vector3 force)
    {
        if (m_pickedUp)
        {
            Vector3 pos = transform.position;
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
        RPCDrop();
        SendRPC("RPCDrop");
    }

    public void RPCDrop()
    {
        if (m_pickedUp)
        {
            Vector3 scale = transform.localScale;
            gameObject.GetComponent<NetworkTransform>().SyncMode = NetworkTransform.TransformSyncMode.SyncRigidbody;
            m_pickedUp = false;
            transform.SetParent(null, true);
            transform.localScale = scale;
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
        if(m_pickupable)
        {
            if(!this.gameObject.GetComponent<Ball>())
                m_pickupable = false;

            if (!m_rigidBody)
                m_rigidBody = gameObject.GetComponent<Rigidbody>();

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
            _Chad = chad;
        }
    }

    public override void OnLostOwnership()
    {
        m_rigidBody.enabled = false;
    }

    public override void OnRead(NetPacketReader reader, bool initialState)
    {
    }

    public override bool OnWrite(NetDataWriter writer, bool initialState)
    {
        return true;
    }
}