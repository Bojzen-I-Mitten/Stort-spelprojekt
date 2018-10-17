using ThomasEngine;

public class TeamSpawn : ScriptComponent
{
    public TEAM_TYPE Team { get; set; }
    public override void Start()
    {
        MatchSystem.instance.FindTeam(Team).SetSpawnArea(this);
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
            GUI.SetColor(t.Color);
            GUI.DrawBoundingBox(Vector3.Zero, Vector3.One);
        }

    }
}
