using ThomasEngine;

public class MoveBall : ScriptComponent
{
    public int testVar { get; set; }

    public override void Start()
    {
        // m.GenerateBones(gameObject);
    }

    public override void Update()
    {
        gameObject.transform.position = new Vector3(0, 5, 0);
      
        //t += Time.DeltaTime;
        //gameObject.transform.position = new Vector3(0, (float)posCurve.GetYFromX((t * 6.5) % 5), 0);
        //gameObject.transform.scale = new Vector3((float)scaleCurve.GetYFromX((t * 6.5) % 5), 1, (float)scaleCurve.GetYFromX((t * 6.5) % 5));
    }
}
