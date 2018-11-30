using ThomasEngine;
using ThomasEngine.Network;
public class RpcTest : ScriptComponent
{
    RenderTexture rt = null;
    Canvas canvas;
    Text t = null;
    public Font chadFont { get; set; }
    string _test = "woop";
    public string test {
        get { return _test; }
        set
        {
            _test = value;
            if(rt != null && t != null)
            {
                t.text = value;
                rt.WriteCanvas(canvas);
            }
        }
    }
    public Vector2 pos
    {
        get
        {
            if (t != null)
                return t.position;
            return Vector2.Zero;
        }

        set
        {
            if (t != null)
            {
                t.position = value;
                rt.WriteCanvas(canvas);
            }
                
        }
    }

    public override void OnAwake()
    {
        
        canvas = new Canvas(new Viewport(0, 0, 1024, 1024));
        rt = new RenderTexture(1024, 1024);
        t = canvas.Add(test);
        t.color = Color.Red;
        t.font = chadFont;
        t.scale = new Vector2(5);
        t.origin = new Vector2(0.5f, 0.5f);
        t.position = new Vector2(0.5f, 0.5f);
        rt.WriteCanvas(canvas);

        Material mat = gameObject.GetComponent<RenderComponent>().material;
        mat.SetTexture2D("diffuseTex", rt);
    }

    public override void Start()
    {

    }

}
