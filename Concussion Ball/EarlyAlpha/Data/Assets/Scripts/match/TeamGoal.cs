using ThomasEngine;
using ThomasEngine.Network;
public class TeamGoal : ScriptComponent
{
    public TEAM_TYPE Team { get; set; }
    public override void Start()
    {
        BoxCollider c = gameObject.AddComponent<BoxCollider>();
        c.isTrigger = true;
        c.size = new Vector3(0.5f, 0.5f, 0.5f);
        //MatchSystem.instance.FindTeam(Team).SetGoalArea(this);
    }

    public override void Update()
    {

    }

    public override void OnDrawGizmos()
    {
        Team t = MatchSystem.instance?.FindTeam(Team);
        if (t != null)
        {
            Gizmos.SetMatrix(transform.world);
            Gizmos.SetColor(t.Color * 0.5f);
            Gizmos.DrawBoundingBox(Vector3.Zero, new Vector3(0.5f, 0.5f, 0.5f));
        }
    }

    public override void OnTriggerEnter(Collider collider)
    {
        if (MatchSystem.instance)
        {
            if (collider.gameObject == MatchSystem.instance.Ball)
            {
                if (MatchSystem.instance.Ball.GetComponent<NetworkIdentity>().Owner)
                {
                    TEAM_TYPE teamThatScored = MatchSystem.instance.GetOpposingTeam(Team);
                    MatchSystem.instance.OnGoal(teamThatScored);
                }
            }
        }
        
    }
}
