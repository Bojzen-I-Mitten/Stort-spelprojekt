using ThomasEngine;

public class Ball : ScriptComponent
{

    Rigidbody rb;

    public override void Start()
    {
        rb = gameObject.GetComponent<Rigidbody>();
    }

    public override void Update()
    {
        if(Input.GetKeyDown(Input.Keys.Space) && !rb.enabled)
        {
            transform.parent = null;
            rb.enabled = true;
        }
    }


    public override void OnCollisionEnter(GameObject collider)
    {
        if (rb.enabled && Input.GetKey(Input.Keys.LeftShift))
        {
            rb.enabled = false;
            transform.localPosition = new Vector3(0, 2, 0);
            transform.parent = collider.transform;
        }

    }
}
