using ThomasEngine;

public class CollisionTest : ScriptComponent
{
    public override void Start()
    {
        
    }

    public override void Update()
    {
        
    }

    public override void OnCollisionEnter(Collider collider)
    {
        Debug.Log("enter!");
    }
    public override void OnCollisionExit(Collider collider)
    {
        Debug.Log("exit!");
    }


}
