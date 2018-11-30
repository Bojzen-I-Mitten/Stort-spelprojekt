using ThomasEngine;

//[ExecuteInEditor]
public class Forcefield : ScriptComponent
{
    public TEAM_TYPE Team { get; set; }
    Material mat;
    public override void Start()
    {
        mat = gameObject.GetComponent<RenderComponent>().material;
        mat.SetRenderQueue(4000);
    }

    public override void Update()
    {
        
        if(MatchSystem.instance?.Ball != null)
        {
            Color c = new Color(MatchSystem.instance.Teams[Team].Color, 0.4f);
            mat.SetColor("color", c);
            mat.SetVector("ballPosition", new Vector4(MatchSystem.instance.Ball.transform.position, 1.0f));
        }
    }
}
