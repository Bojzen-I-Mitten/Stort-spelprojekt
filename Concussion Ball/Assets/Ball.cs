using ThomasEngine;
using ThomasEngine.Network;
public class Ball : NetworkComponent
{

    Rigidbody rb;
    public float force { get; set; } = 5.0f;
    GameObject playerThatHasBall;
    public float currentForce = 0;
    private ThrowStrengthVisualizer visualizer;
    public override void Start()
    {
        rb = gameObject.GetComponent<Rigidbody>();
        visualizer = Object.GetObjectsOfType<ThrowStrengthVisualizer>()[0];
    }

    public override void Update()
    {
        if (true)
        {
            if (Input.GetKey(Input.Keys.Space) && !rb.enabled)
            {
                currentForce += force * Time.DeltaTime;
                visualizer.SetStrength(currentForce);
            }

            if (Input.GetKeyUp(Input.Keys.Space) && !rb.enabled)
            {
                transform.parent = null;
                rb.enabled = true;
                rb.ApplyCentralImpulseForce(playerThatHasBall.transform.forward * currentForce);
                playerThatHasBall = null;
                currentForce = 0.0f;
                visualizer.SetStrength(0);
            }
        }

    }


    public override void OnCollisionEnter(GameObject collider)
    {
        if (rb.enabled)
        {
            playerThatHasBall = collider;
            rb.enabled = false;
            transform.parent = collider.transform;
            transform.localPosition = new Vector3(0, 2, 0);
        }

    }
}
