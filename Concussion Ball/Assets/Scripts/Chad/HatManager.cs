using System.Collections.Generic;
using System.ComponentModel;
using ThomasEngine;

public class HatManager : ScriptComponent
{

    [Newtonsoft.Json.JsonObject]
    [TypeConverter(typeof(ExpandableObjectConverter))]
    public class Hat
    {
        public Model hatModel { get; set; } = null;
        public Vector3 position { get; set; } = Vector3.Zero;
        public Vector3 rotation { get; set; } = Vector3.Zero;
        public Vector3 scale { get; set; } = Vector3.One;
        public Texture2D texture { get; set; } = Texture2D.whiteTexture;
        public Texture2D specular { get; set; } = Texture2D.blackTexture;

        [Newtonsoft.Json.JsonIgnore]
        public Material material;
        public Hat()
        {
        }
    }

    public List<Hat> Hats { get; set; } = new List<Hat>();

    public static HatManager Instance;
    public override void OnAwake()
    {
        Instance = this;
        foreach (var hat in Hats)
        {
            hat.material = new Material(Material.StandardMaterial);
            hat.material.SetColor("color", Color.White);
            hat.material.SetTexture2D("DiffuseTexture", hat.texture);
            if(hat.specular != null)
                hat.material.SetTexture2D("SpecularTexture", hat.specular);
        }
    }

    public override void OnDestroy()
    {
        Instance = null;
    }

    public Hat GetHat(int index)
    {
        if (index < Hats.Count)
            return Hats[index];
        else
            return null;
    }
    
    public override void Update()
    {

    }
}
