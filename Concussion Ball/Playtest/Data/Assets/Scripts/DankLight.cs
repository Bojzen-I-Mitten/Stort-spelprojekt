using ThomasEngine;

public class DankLight : ScriptComponent
{

    public Curve LightCurve { get; set; }
    LightComponent light;
    float timeStack = 0;
    public override void Start()
    {
        light = gameObject.GetComponent<LightComponent>();
    }

    public override void Update()
    {
        timeStack = (timeStack + Time.DeltaTime) % 1.0f;
        if (light)
        {
            light.Intensity = (float)LightCurve.GetYFromX((double)timeStack);
        }
    }
}
