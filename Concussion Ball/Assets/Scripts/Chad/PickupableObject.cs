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
    public float MovementSpeedModifier = 1.0f;//{ get; set; } = 1.0f;
    public bool DropOnRagdoll { get; set; } = true;

    public bool m_throwable = false;
    private float chargeTimeCurrent;
    public float chargeTimeMax;// { get; set; } = 4.0f;
    public float BaseThrowForce = 0;
    public float MaxThrowForce = 0;
    public float ThrowForce = 0;

    public bool pickedUp = false;

    private SoundComponent soundThrow;

    public Collider PickupCollider { get; set; }
    [Newtonsoft.Json.JsonIgnore]
    public bool charging { get { return chargeTimeCurrent > 0.00001f; } }

    public ChadControls _Chad;
    private RenderComponent m_renderComponent;

    public override void OnAwake()
    {
        m_rigidBody = gameObject.GetComponent<Rigidbody>();
        m_renderComponent = gameObject.GetComponent<RenderComponent>();
        chargeTimeCurrent = 0.0f;
        chargeTimeMax = 2.0f;
        if (!PickupCollider)
            Debug.LogError("Pickup collider empty");

        soundThrow = gameObject.AddComponent<SoundComponent>();
        soundThrow.Clip = (AudioClip)Resources.LoadThomasPath("%THOMAS_ASSETS%/Sounds/ThrowGeneral.wav");
        soundThrow.Looping = false;
        soundThrow.Type = SoundComponent.SoundType.Effect;
        soundThrow.MaxDistance = 30;
        soundThrow.MinDistance = 10;
        soundThrow.Is3D = true;

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

    virtual public void Throw(Vector3 camPos, Vector3 direction)
    {
        Vector3 pos = camPos;
        Drop();
        transform.position = pos;
        transform.LookAt(transform.position + Vector3.Normalize(direction));
        m_rigidBody.Position = transform.position;
        m_rigidBody.Rotation = transform.rotation;
        soundThrow.Play();
        StartCoroutine(ThrowRoutine(direction));
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
        RPCDrop();
        SendRPC("RPCDrop");
    }

    public virtual void OnDrop()
    {

    }

    public void RPCDrop()
    {
        if (pickedUp)
        {
            //Debug.Log("Drop!");
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
            pickedUp = false;
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

    public override void OnDisconnect()
    {
        RPCDrop();
    }

    virtual public void SaveObjectOwner(ChadControls chad)
    {

    }

    virtual public void Pickup(ChadControls chad, Transform hand)
    {
        RPCDrop();
        SaveObjectOwner(chad);
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
        pickedUp = true;
    }

    public override bool OnWrite(NetDataWriter writer, bool initialState)
    {
        writer.Put(pickedUp);
        writer.Put(PickupCollider.enabled);
        writer.Put(chargeTimeCurrent);
        return true;
    }

    public override void OnRead(NetDataReader reader, bool initialState)
    {
        if (isOwner)
        {
            reader.GetBool();
            reader.GetBool();
            reader.GetFloat();
            return;
        }
        pickedUp = reader.GetBool();
        PickupCollider.enabled = reader.GetBool();
        chargeTimeCurrent = reader.GetFloat();

    }


    public override void OnLostOwnership()
    {
        if (_Chad == MatchSystem.instance.LocalChad || pickedUp || transform.parent != null)
        {
            Drop();
        }

    }

    virtual public void Disable()
    {
        pickedUp = false;
        PickupCollider.enabled = false;
        m_rigidBody.enabled = false;
        gameObject.GetComponent<NetworkTransform>().SyncMode = NetworkTransform.TransformSyncMode.SyncNone;
        gameObject.SetActive(false);
    }

    virtual public void Reset()
    {
        RPCDrop();
        transform.scale = Vector3.One;
        chargeTimeCurrent = 0.0f;
        _Chad = null;
        PickupCollider.enabled = false;
        PickupCollider.enabled = true;
        gameObject.GetComponent<NetworkTransform>().SyncMode = NetworkTransform.TransformSyncMode.SyncRigidbody;
    }
}