using ThomasEngine;

public class ThrowStrengthVisualizer : ScriptComponent
{
    public override void Start()
    {
        
    }

    public void SetStrength(float strength)
    {
        transform.localScale = new Vector3(0.1f * strength, 0.2f, 0.1f);
    }

    public override void Update()
    {
        
    }
}
