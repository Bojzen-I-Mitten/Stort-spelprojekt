using ThomasEngine;

public class InputTest : ScriptComponent
{
    public override void Start()
    {
        
    }

    public override void Update()
    {
        if (Input.GetKeyDown(Input.Keys.W))
        {
            Debug.Log("Hello World!");
        }
    }
}
