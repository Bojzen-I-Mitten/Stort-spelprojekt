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
    private Rigidbody Target
    {
        get
        {
            if (Ball._Chad)
                return Ball._Chad.rBody;
            else
                return Ball.m_rigidBody;
        }
    }

    public override void Start()
    {
    }

    public override void Update()
    {
        if (MatchSystem.instance && MatchSystem.instance.ReplaySystem.Replaying)
        {
            Vector3 forward = Vector3.Normalize(Target.LinearVelocity);
            Vector3 TargetPosition = Target.Position + new Vector3(0, 1.8f, 0) - forward * ChadCam.instance.CameraOffset;
            if (Ball._Chad)
                Debug.Log("Following Chad");
            else
                Debug.Log("Following Ball");
                Debug.Log("Current pos: " + transform.position);
            Debug.Log("Target pos: " + TargetPosition);
            Debug.Log("DeltaTime: " + Time.ActualDeltaTime);
            transform.position = Vector3.Lerp(transform.position, TargetPosition, Time.ActualDeltaTime);

            Vector3 TargetLookAt = Target.Position + new Vector3(0, 1.8f, 0)/* + Target.forward*/;
            TargetLookAt = Vector3.Lerp(transform.position + transform.forward, TargetLookAt, Time.ActualDeltaTime);
            transform.LookAt(TargetLookAt);
        }
    }

    public override void OnEnable()
    {
        if (MatchSystem.instance && MatchSystem.instance.ReplaySystem.Replaying)
        {
            if (ChadCam.instance)
            {
                Vector3 forward = Vector3.Normalize(Target.LinearVelocity);
                float x = Random.Range(-5, 5);
                float y = Random.Range(5, 15);
                float z = Random.Range(-5, 5);
                transform.position = /*new Vector3(x, y, z);*/Target.Position - forward * ChadCam.instance.CameraOffset;
                transform.LookAt(Target.transform.position);
            }
        }
    }
}
