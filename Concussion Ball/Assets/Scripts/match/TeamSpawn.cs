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
        GUI.SetMatrix(transform.world);
        GUI.SetColor(Color.Green);
        GUI.DrawCube(transform.position, transform.scale);
    }
}
