using ThomasEngine;

public class ReplayCamera : ScriptComponent
{
    [Newtonsoft.Json.JsonIgnore]
    Ball _Ball;
    Ball Ball
    {
        get
        {
            if (!_Ball)
                _Ball = MatchSystem.instance?.Ball?.GetComponent<Ball>();
            return _Ball;
        }
    }

    [Newtonsoft.Json.JsonIgnore]
    private Transform Target
    {
        get
        {
            if (Ball._Chad)
                return Ball._Chad.transform;
            else
                return Ball.transform;
        }
    }

    public override void Start()
    {
        enabled = false;
    }

    public override void Update()
    {
        if (MatchSystem.instance && MatchSystem.instance.ReplaySystem.Replaying)
        {
                Vector3 TargetPosition = Target.position + new Vector3(0, 1.8f, 0) - Target.forward * ChadCam.instance.CameraOffset;
                transform.position = Vector3.Lerp(transform.position, TargetPosition, Time.DeltaTime);

                Vector3 TargetLookAt = Target.position + new Vector3(0, 1.8f, 0)/* + Target.forward*/;
                TargetLookAt = Vector3.Lerp(transform.position + transform.forward, TargetLookAt, Time.DeltaTime);
                transform.LookAt(TargetLookAt);
        }

    }

    public override void OnEnable()
    {
        if (MatchSystem.instance && MatchSystem.instance.ReplaySystem.Replaying)
        {
            if (ChadCam.instance)
            {
                float x = Random.Range(-5, 5);
                float y = Random.Range(5, 15);
                float z = Random.Range(-5, 5);
                transform.position = new Vector3(x, y, z);//Target.position - Target.forward * ChadCam.instance.CameraOffset;
                transform.LookAt(Target.transform.position);
            }
        }
    }
}
