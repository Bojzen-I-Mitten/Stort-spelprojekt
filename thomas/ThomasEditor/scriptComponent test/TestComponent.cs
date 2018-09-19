using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ThomasEngine;
namespace ThomasEditor
{
    public class TestComponent : ScriptComponent
    {
        public Curve posCurve {get; set;}
        public Curve scaleCurve { get; set; }
        public int testVar { get; set; }
        public AudioClip audioClip { get; set; }
        public Material mat { get; set; }
        public Model m { get; set; }
        public GameObject coolPrefab { get; set; }
        public RenderComponent otherObjectsRender { get; set; }

        float t = 0;

        public override void Start()
        {
            
            // m.GenerateBones(gameObject);
          //  GameObject.Instantiate(coolPrefab, new Vector3(10, 10, 10), new Quaternion());
            //otherObjectsRender.Destroy();

           // List<Component> c = ThomasEngine.Object.GetObjectsOfType<Component>();
        }

        public override void Update()
        {
         //   GUI.ImguiStringUpdate(t.ToString(),new Vector2(100,0),new Vector2(0,0));
            t += Time.DeltaTime;
            //  gameObject.transform.position = new Vector3(0, (float)posCurve.GetYFromX((t*6.5) % 5), 0);
            //  gameObject.transform.scale = new Vector3((float)scaleCurve.GetYFromX((t*6.5) % 5), 1, (float)scaleCurve.GetYFromX((t*6.5) % 5));
            gameObject.transform.position = gameObject.transform.position + new Vector3(0.1f, 0.0f, 0.0f);
        }
    }
}