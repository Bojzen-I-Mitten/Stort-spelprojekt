using ThomasEngine;

public class TeamGoal : ScriptComponent
{
    public TEAM_TYPE Team { get; set; }
    public override void Start()
    {
        BoxCollider c = gameObject.AddComponent<BoxCollider>();
        c.isTrigger = true;
        c.size = new Vector3(0.5f,0.5f,0.5f);
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
            GUI.SetMatrix(transform.world);
            GUI.SetColor(t.Color * 0.5f);
            GUI.DrawBoundingBox(Vector3.Zero, new Vector3(0.5f, 0.5f, 0.5f));
        }
    }
}
