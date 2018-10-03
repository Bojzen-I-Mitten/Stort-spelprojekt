using LiteNetLib;
using LiteNetLib.Utils;
using ThomasEngine;
using ThomasEngine.Network;
public class Ball : NetworkComponent
{
    Rigidbody rb;
    RenderComponent rc;
    bool pickedUp { get { return !rb.enabled; } set { rb.enabled = !value; } }
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
        }
        
    }

    public void ChargeColor(float throwForce)
    {
        //Debug.Log("Red: " + rc.material.GetColor("color").r);

        accumulator += Time.DeltaTime;
        interp = accumulator / chargeupTime;

        float value = interp;

        Color newColor = new Color(value, 0.0f, (1.0f-interp));
        
        rc.material.SetColor("color", newColor);

    }

    public void Throw(Vector3 force)
    {
        if (pickedUp)
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
        rb.enabled = false;
        transform.parent = hand;
        transform.localPosition = Vector3.Zero;
    }

    public override void OnLostOwnership()
    {
        rb.enabled = false;
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
