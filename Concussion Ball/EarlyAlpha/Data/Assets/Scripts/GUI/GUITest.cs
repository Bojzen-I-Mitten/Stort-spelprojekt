using System.Collections;
using ThomasEngine;

public class GUITest : ScriptComponent
{
    public Texture2D Background { get; set; }

    Camera Camera;
    ScriptComponent JoinHost;
    GUISelectTeam SelectTeam;

    Canvas canvas;

    Image BG;

    public override void Start()
    {
        Camera = gameObject.GetComponent<Camera>();
        JoinHost = gameObject.GetComponent<GUIJoinHost>();
        SelectTeam = gameObject.GetComponent<GUISelectTeam>();
        SelectTeam.enabled = false;

        canvas = Camera.AddCanvas();
        if (Background != null)
        {
            BG = canvas.Add(Background);
            BG.interactable = true;
        }
    }

    public override void Update()
    {
        if (!JoinHost.enabled)
            SelectTeam.enabled = true;
        //StartCoroutine(LoadSelectTeam());
    }

    IEnumerator LoadSelectTeam()
    {
        yield return new WaitForSecondsRealtime(2);
        SelectTeam.enabled = true;
    }
}
