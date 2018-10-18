using LiteNetLib;
using LiteNetLib.Utils;
using ThomasEngine;
using ThomasEngine.Network;
public class Ball : NetworkComponent
{
    Rigidbody rb;
    RenderComponent rc;
    public bool PickedUp { get { if (rb) return !rb.enabled; else return false; } set { if (rb) rb.enabled = !value; } }
    private float accumulator;
    private float chargeupTime;

    private float interp = 0;

    public override void Start()
    {
        chargeupTime = 4.0f;
        accumulator = 0.0f;
        rb = gameObject.GetComponent<Rigidbody>();
        rc = gameObject.GetComponent<RenderComponent>();


        rc.material.SetColor("color", new Color(0, 0, 255));
        //visualizer = Object.GetObjectsOfType<ThrowStrengthVisualizer>()[0];
    }

    public override void Update()
    {
    }

    public void Drop()
    {
        RPCDrop();
        SendRPC("RpcDrop");
    }

    public void ChargeColor()
    {
        //Debug.Log("Red: " + rc.material.GetColor("color").r);

        accumulator += Time.DeltaTime;
        interp = accumulator / chargeupTime;

        float value = interp;

        Color newColor = new Color(value, 0.0f, (1.0f - interp));

        rc.material.SetColor("color", newColor);

    }

    public void RPCDrop()
    {
        if (PickedUp)
        {
            gameObject.GetComponent<NetworkTransform>().SyncMode = NetworkTransform.TransformSyncMode.SyncRigidbody;
            PickedUp = false;
            transform.parent = null;
        }
    }

    public void Throw(Vector3 force)
    {
        if (PickedUp)
        {
            Drop();
            transform.position = transform.position + Vector3.Normalize(force) * 2;
            rb.AddForce(force, Rigidbody.ForceMode.Impulse);

            rc.material.SetColor("color", new Color(0, 0, 255));

            // Reset values
            accumulator = 0;
            interp = 0.0f;
        }

    }

    
    public void Pickup(GameObject gobj, Transform hand)
    {
        gameObject.GetComponent<NetworkTransform>().SyncMode = NetworkTransform.TransformSyncMode.SyncNone;
        PickedUp = true;
        transform.parent = hand;
        transform.localPosition = Vector3.Zero;
    }

    public override void OnRead(NetPacketReader reader, bool initialState)
    {
        //if(isOwner)
        //{
        //    reader.GetBool();
        //    return;
        //}
       // rb.enabled = reader.GetBool();
    }

    public override bool OnWrite(NetDataWriter writer, bool initialState)
    {
        //writer.Put(rb.enabled);
        return true;
    }
}
