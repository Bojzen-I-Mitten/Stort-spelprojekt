using ThomasEngine;
using ThomasEngine.Network;
public class RpcTest : ScriptComponent
{
    RenderTexture rt;
    Canvas canvas;
    public override void OnAwake()
    {
        
        canvas = new Canvas(new Viewport(0, 0, 1024, 1024));
        rt = new RenderTexture(1024, 1024);
        Text t = canvas.Add("Test");
        t.color = Color.Red;
        Debug.Log(t.text);
        rt.WriteCanvas(canvas);
        
    }

    public override void Start()
    {
        Material mat = gameObject.GetComponent<RenderComponent>().material;
        mat.SetTexture2D("DiffuseTexture", rt);
    }

}
