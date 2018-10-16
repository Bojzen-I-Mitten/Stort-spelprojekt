using ThomasEngine;

public class TeamGoal : ScriptComponent
{
    public TEAM_TYPE Team { get; set; }
    public override void Start()
    {
        MatchSystem.instance.FindTeam(Team).SetGoalArea(this);
    }

    public override void Update()
    {

    }

    public override void OnDrawGizmos()
    {
        GUI.SetMatrix(transform.world);
        GUI.SetColor(Color.Blue);
        GUI.DrawCube(transform.position, transform.scale);
    }
}
