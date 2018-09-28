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
            rb.AddForce(force, Rigidbody.ForceMode.Impulse);
        }
    }
    
    public void Pickup(GameObject chad)
    {

        rb.enabled = false;
        transform.parent = chad.transform;
        transform.localPosition = new Vector3(0, 3, 3);
        pickedUp = true;
    }
}
