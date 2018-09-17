using ThomasEngine;

public class ThrowBall : ScriptComponent
{
    public Vector3 force { get; set; }

    public override void Start()
    {
        
    }

    public override void Update()
    {
        gameObject.GetComponent<Rigidbody>().ApplyCentralForce(force);
    }
}
