using ThomasEngine;

public class RayTest : ScriptComponent
{
    Ray r;
    public override void Start()
    {
        
    }

    public override void Update()
    {
        
        RaycastHit hit;
        if(Physics.Raycast(r, out hit))
        {
            Debug.Log(hit.collider);
        }
    }

    public override void OnDrawGizmos()
    {
        r = new Ray(transform.position, transform.forward);
        Gizmos.SetMatrix(Matrix.Identity);
        Gizmos.SetColor(Color.Green);
        Gizmos.DrawRay(ref r);
    }
}
