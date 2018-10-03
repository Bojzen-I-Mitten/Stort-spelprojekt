using LiteNetLib;
using LiteNetLib.Utils;
using ThomasEngine;
using ThomasEngine.Network;
public class Ball : NetworkComponent
{
    Rigidbody rb;
    bool pickedUp = false;

    public override void Start()
    {
        rb = gameObject.GetComponent<Rigidbody>();
        //visualizer = Object.GetObjectsOfType<ThrowStrengthVisualizer>()[0];
    }

    public override void Update()
    {
        if (transform.parent == null)
        {
            Drop();
        }
            
    }

    public void Drop()
    {
        if (pickedUp)
        {
            rb.enabled = true;
            transform.parent = null;
            pickedUp = false;
        }
        
    }

    public void Throw(Vector3 force)
    {
        if (pickedUp)
        {
            Drop();
            transform.position = transform.position + Vector3.Normalize(force) * 2;
            rb.AddForce(force, Rigidbody.ForceMode.Impulse);
        }
    }
    
    public void Pickup(GameObject gobj, Transform hand)
    {
        rb.enabled = false;
        transform.parent = hand;
        transform.localPosition = Vector3.Zero;
        pickedUp = true;
    }

    public override void OnLostOwnership()
    {
        rb.enabled = false;
        pickedUp = true;
    }

    public override void OnRead(NetPacketReader reader, bool initialState)
    {
        if(isOwner)
        {
            reader.GetBool();
            return;
        }
        rb.enabled = reader.GetBool();
    }

    public override bool OnWrite(NetDataWriter writer, bool initialState)
    {
        writer.Put(rb.enabled);
        return true;
    }
}
