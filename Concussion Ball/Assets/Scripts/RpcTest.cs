using ThomasEngine;
using ThomasEngine.Network;
public class RpcTest : ScriptComponent
{
    RenderTexture rt = null;
    Canvas canvas;
    Text nameText = null;
    Text numberText = null;
    Text numberText2 = null;
    Text numberText3 = null;
    public Font chadFont { get; set; }

    public Texture2D symbol { get; set; }


    string _test = "";

    public string name {
        get { return _test; }
        set
        {
            _test = value;
            if(nameText != null)
            {
                nameText.text = value;
                nameText.scale = Vector2.One;
                nameText.position = new Vector2(0.21f, nameText.size.y - 0.1f);
                nameText.scale = new Vector2(0.25f / nameText.size.x, 0.2f / nameText.size.y);
                rt.WriteCanvas(canvas);
            }
        }
    }
    string _number;
    public string number
    {
        get { return _number; }
        set
        {
            _number = value;
            if (numberText != null)
            {
                numberText.text = value.ToString();
                numberText2.text = value.ToString();
                numberText3.text = value.ToString();
                rt.WriteCanvas(canvas);
            }
        }
    }



    public override void OnAwake()
    {
        
       
        rt = new RenderTexture(512, 512);

        canvas = new Canvas(new Viewport(0, 0, 512, 512));

        numberText = canvas.Add(number.ToString());
        numberText.origin = new Vector2(0.5f);
        numberText.font = chadFont;
        numberText.position = new Vector2(0.2f, numberText.size.y + 0.2f);
        numberText.scale = new Vector2(0.25f / numberText.size.x, 0.5f / numberText.size.y);
        numberText.color = Color.Black;

        numberText2 = canvas.Add(number.ToString());
        numberText2.rotation = MathHelper.ToRadians(-90.0f);
        numberText2.origin = new Vector2(0.5f);
        numberText2.font = chadFont;
        numberText2.position = new Vector2(0.51f, numberText2.size.y - 0.15f);
        numberText2.scale = new Vector2(0.12f / numberText2.size.x, 0.08f / numberText2.size.y);
        numberText2.color = Color.Black;

        numberText3 = canvas.Add(number.ToString());
        numberText3.rotation = MathHelper.ToRadians(180.0f);
        numberText3.origin = new Vector2(0.5f);
        numberText3.font = chadFont;
        numberText3.position = new Vector2(0.43f, numberText3.size.y + 0.56f);
        numberText3.scale = new Vector2(0.08f / numberText3.size.x, 0.16f / numberText3.size.y);
        numberText3.color = Color.Black;


        nameText = canvas.Add(name);
        nameText.origin = new Vector2(0.5f);
        nameText.color = Color.Black;
        nameText.font = chadFont;
        nameText.position = new Vector2(0.21f, nameText.size.y - 0.1f);
        nameText.scale = new Vector2(0.25f / nameText.size.x, 0.2f / nameText.size.y);


        Image image = canvas.Add(symbol);
        image.origin = new Vector2(0.5f);
        image.rotation = MathHelper.ToRadians(180.0f);
        image.position = new Vector2(0.54f, 0.93f);
        image.scale = new Vector2(0.05f / image.size.x, 0.05f / image.size.y);

        rt.WriteCanvas(canvas);

        


        Material mat = gameObject.GetComponent<RenderComponent>().material;
        mat.SetTexture2D("shirtPattern", rt);
    }

    public override void Start()
    {

    }

}
