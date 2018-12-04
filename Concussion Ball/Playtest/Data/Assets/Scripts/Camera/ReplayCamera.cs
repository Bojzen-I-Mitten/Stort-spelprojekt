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

    public override void Start()
    {
    }

    public override void Update()
    {
        if (MatchSystem.instance && MatchSystem.instance.ReplaySystem.Replaying)
        {
            if (Ball._Chad)
            {
                transform.LookAt(Ball._Chad.transform.position);
            } else
                transform.LookAt(Ball.transform.position);
        }

    }
}
