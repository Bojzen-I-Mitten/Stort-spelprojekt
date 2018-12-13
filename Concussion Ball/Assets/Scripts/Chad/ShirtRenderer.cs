using ThomasEngine;
public class ShirtRenderer : ScriptComponent
{
    RenderTexture rt = null;
    Canvas canvas;
    Text nameText = null;
    Text teamText = null;
    Text backNumber = null;
    Text shoulderNumber = null;
    Text shoulderNumber2 = null;
    Text frontNumber = null;
    Image shirtLogo = null;
    Material mat = null;
    public Font chadFont { get; set; }
    public Texture2D shoulderTexture { get; set; }
    public Texture2D logo { get; set; }
    public Texture2D logo2 { get; set; }
    string _PlayerName = "";
    string _TeamName = "";
    public string PlayerName
    {
        get { return _PlayerName; }
        set
        {
            
            if (value != _PlayerName && nameText != null)
            {
                nameText.text = value;
                UpdateTextScale();
                rt.WriteCanvas(canvas);
                
            }
            _PlayerName = value;
        }
    }


    public string TeamName
    {
        get { return _TeamName; }
        set
        {
            
            if (_TeamName != value && teamText != null)
            {
                teamText.text = value;
                UpdateTextScale();
                rt.WriteCanvas(canvas);

            }
            _TeamName = value;
        }
    }

    Color _color = Color.White;
    public Color Color
    {
        get { return _color; }
        set
        {
            if(_color != value && mat != null)
            {
                mat.SetColor("color", value);
                _color = value;
            }
            
        }
    }

    string _number;
    public string number
    {
        get { return _number; }
        set
        {
            
            if (_number != value && backNumber != null)
            {
                backNumber.text = value.ToString();
                frontNumber.text = value.ToString();
                shoulderNumber.text = value.ToString();
                shoulderNumber2.text = value.ToString();
                rt.WriteCanvas(canvas);
            }
            _number = value;
        }
    }

    TEAM_TYPE _Team = TEAM_TYPE.TEAM_1;
    public TEAM_TYPE Team
    {
        get
        {
            return _Team;
        }
        set
        {
            if(_Team != value && shirtLogo != null)
            {
                if (_Team == TEAM_TYPE.TEAM_1)
                    shirtLogo.texture = logo;

                else
                    shirtLogo.texture = logo2;

                rt.WriteCanvas(canvas);
            }
            _Team = value;
        }
    }


    public override void OnAwake()
    {

        rt = new RenderTexture(1024, 1024);

        canvas = new Canvas(new Viewport(0, 0, 1024, 1024));


        Image shoulders = canvas.Add(shoulderTexture);

        shoulders.rotation = MathHelper.ToRadians(180);
        shoulders.origin = new Vector2(0.5f);

        shoulders.position = new Vector2(851 / 1024.0f, 142 / 1024.0f);


        Image shoulders2 = canvas.Add(shoulderTexture);
        shoulders2.rotation = MathHelper.ToRadians(-90);
        shoulders2.origin = new Vector2(0.5f);

        shoulders2.position = new Vector2(880 / 1024.0f, 455 / 1024.0f);


        shirtLogo = canvas.Add(logo);

        shirtLogo.rotation = MathHelper.ToRadians(180);
        shirtLogo.origin = new Vector2(0.5f);

        shirtLogo.position = new Vector2(900 / 1024.0f, 920 / 1024.0f);

        shirtLogo.scale = new Vector2(0.5f);

        backNumber = canvas.Add(number.ToString());
        backNumber.origin = new Vector2(0.5f);
        backNumber.font = chadFont;
        backNumber.color = Color.Black;

        shoulderNumber = canvas.Add(number.ToString());
        shoulderNumber.rotation = MathHelper.ToRadians(-90.0f);
        shoulderNumber.origin = new Vector2(0.5f);
        shoulderNumber.color = Color.Black;
        shoulderNumber.font = chadFont;

        shoulderNumber2 = canvas.Add(number.ToString());
        shoulderNumber2.flip = new Vector2(0, 1);
        shoulderNumber2.origin = new Vector2(0.5f);
        shoulderNumber2.font = chadFont;
        shoulderNumber.color = Color.Black;

        frontNumber = canvas.Add(number.ToString());
        frontNumber.rotation = MathHelper.ToRadians(180.0f);
        frontNumber.origin = new Vector2(0.5f);
        frontNumber.font = chadFont;
        frontNumber.color = Color.Black;


        backNumber.position = new Vector2(350/1024.0f, 480/1024.0f);
        backNumber.scale = new Vector2(0.35f / backNumber.size.x, 0.35f / backNumber.size.x);

        shoulderNumber.position = new Vector2(860 / 1024.0f, 145/1024.0f);
        shoulderNumber.scale = new Vector2(0.12f / shoulderNumber.size.x, 0.12f / shoulderNumber.size.x);
        shoulderNumber.color = Color.Black;

        shoulderNumber2.position = new Vector2(900 / 1024.0f, 460 / 1024.0f);
        shoulderNumber2.scale = new Vector2(0.12f / shoulderNumber2.size.x, 0.12f / shoulderNumber2.size.x);
        shoulderNumber2.color = Color.Black;

        frontNumber.position = new Vector2(715 / 1024.0f, 850 / 1024.0f);
        frontNumber.scale = new Vector2(0.10f / frontNumber.size.x, 0.10f / frontNumber.size.x);


        nameText = canvas.Add(PlayerName);
        nameText.origin = new Vector2(0.5f);
        nameText.color = Color.Black;
        nameText.font = chadFont;

        teamText = canvas.Add(TeamName);
        teamText.origin = new Vector2(0.5f);
        teamText.color = Color.Black;
        teamText.font = chadFont;
        teamText.rotation = MathHelper.ToRadians(180);

        UpdateTextScale();
        rt.WriteCanvas(canvas);

        mat = gameObject.GetComponent<RenderComponent>().CreateMaterialInstance("Chad66");
        mat.SetTexture2D("shirtPattern", rt);
        mat.SetColor("color", _color);
    }

    public void Reset()
    {
        gameObject.GetComponent<RenderSkinnedComponent>().SetMaterial(1, mat);
    }

    void UpdateTextScale()
    {
        nameText.scale = Vector2.One;
        nameText.position = new Vector2(370.0f / 1024.0f, 175 / 1024.0f);
        float scale = MathHelper.Min(0.5f / nameText.size.x, 2.0f);
        nameText.scale = new Vector2(scale);

        teamText.scale = Vector2.One;
        teamText.position = new Vector2(710 / 1024.0f, 915 / 1024.0f);
        float scale2 = MathHelper.Min(0.15f / teamText.size.x, 0.4f);
        teamText.scale = new Vector2(scale2);

    }

    public override void Start()
    {

    }


    public override void OnDestroy()
    {
        if(rt != null)
            rt.Dispose();
        base.OnDestroy();
    }

}