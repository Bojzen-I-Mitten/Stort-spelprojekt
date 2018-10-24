using System;
using System.Collections;
using LiteNetLib;
using LiteNetLib.Utils;
using ThomasEngine;
using ThomasEngine.Network;

public class PickupableObject : NetworkComponent
{
    public Rigidbody m_rigidBody;

    public bool m_throwable = false;
    public bool m_pickupable = true;

    public float chargeTimeCurrent;
    public float chargeTimeMax { get; set; } = 4.0f;

    private ChadControls _Chad;
    private RenderComponent m_renderComponent;
    private bool m_pickedUp { get { if (m_rigidBody != null) return !m_rigidBody.enabled; else return false; } set { if (m_rigidBody != null) m_rigidBody.enabled = !value; } }

    public override void Start()
    {
        m_rigidBody = gameObject.GetComponent<Rigidbody>();
        m_renderComponent = gameObject.GetComponent<RenderComponent>();

        chargeTimeCurrent = 0.0f;
    }

    public override void Update()
    {
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
            Drop();
            transform.position = transform.position + Vector3.Normalize(force) * 2;
            m_rigidBody.AddForce(force, Rigidbody.ForceMode.Impulse);
        }
    }

    virtual public void OnActivate()
    {
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
            gameObject.GetComponent<NetworkTransform>().SyncMode = NetworkTransform.TransformSyncMode.SyncRigidbody;
            m_pickedUp = false;
            transform.parent = null;
            if (_Chad)
            {
                _Chad.PickedUpObject = null;
                _Chad = null;
            }
        }
    }

    public void Pickup(ChadControls chad, Transform hand)
    {
        if(m_pickupable)
        {
            if(this.GetType() == typeof(Powerup))
                m_pickupable = false;

            m_rigidBody.IsKinematic = false;

            m_rigidBody.enabled = false;
            transform.parent = hand;
            transform.localPosition = Vector3.Zero;
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