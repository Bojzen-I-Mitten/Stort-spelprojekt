using System;
using System.Collections;
using LiteNetLib;
using LiteNetLib.Utils;
using ThomasEngine;
using ThomasEngine.Network;

public class PickupObjects : NetworkComponent
{
    private Rigidbody m_rigidBody;
    private RenderComponent m_renderComponent;

    public bool m_pickedUp { get { if (m_rigidBody != null) return !m_rigidBody.enabled; else return false; } set { if (m_rigidBody != null) m_rigidBody.enabled = !value; } }

    /*
        Stöd för throw 
    */

    public override void Start()
    {
        m_rigidBody = gameObject.GetComponent<Rigidbody>();
        m_renderComponent = gameObject.GetComponent<RenderComponent>();
    }

    public override void Update()
    { 
    }

    //public void RPCDrop()
    //{
    //    if (m_pickedUp)
    //    {
    //        gameObject.GetComponent<NetworkTransform>().SyncMode = NetworkTransform.TransformSyncMode.SyncRigidbody;
    //        m_pickedUp = false;
    //        transform.parent = null;
    //    }
    //}

    //public void Pickup(GameObject gobj, Transform hand)
    //{
    //    m_rigidBody.enabled = false;
    //    transform.parent = hand;
    //    transform.localPosition = Vector3.Zero;
    //}

    //public override void OnLostOwnership()
    //{
    //    m_rigidBody.enabled = false;
    //}
}
