using System;
using System.Collections;
using System.Collections.Generic;
using ThomasEngine;
using ThomasEngine.Script;

public class Forcefield : ScriptComponent
{
    public TEAM_TYPE Team { get; set; }
    Material mat;
    float maxBallDist = 3.0f;
    IEnumerator impactRoutine = null;
    List<Forcefield> forceFields;
    public override void Start()
    {
        mat = gameObject.GetComponent<RenderComponent>().material;
        mat.SetRenderQueue(4000);
        forceFields = new List<Forcefield>(ScriptUtility.GetComponentsOfType<Forcefield>());
    }

    public override void Update()
    {
        
        if(MatchSystem.instance?.Ball != null)
        {
            Color c = new Color(MatchSystem.instance.Teams[Team].Color, 0.4f);
            mat.SetColor("color", c);
            if (impactRoutine == null)
            {
                mat.SetVector("ballPosition", new Vector4(MatchSystem.instance.Ball.transform.position, maxBallDist));
                mat.SetFloat("hole", 0.0f);
            }
                
            
        }
    }

    public override void OnCollisionEnter(Collider collider)
    {
        if(collider.gameObject.GetComponent<PickupableObject>())
        {
            forceFields.ForEach((field) => field.StartImpact(collider.transform.position));
        }
    }

    public void StartImpact(Vector3 pos)
    {
        if (impactRoutine == null)
        {
            impactRoutine = ForceFieldImpact(pos);
            StartCoroutine(impactRoutine);
        }
    }

    private IEnumerator ForceFieldImpact(Vector3 pos)
    {
        float time = 0.0f;
        Vector3 impactPos = pos;
        while (time < 1.0f)
        {
            time += Time.DeltaTime;
            float dist = time * 30.0f;
            mat.SetFloat("hole", 0.5f + time*0.5f);
            mat.SetVector("ballPosition", new Vector4(impactPos, maxBallDist + dist));
            yield return null;
        }
        //while (time > 0.0f)
        //{
        //    time -= Time.DeltaTime*2.0f;
        //    float dist = time * 30.0f;
        //    mat.SetVector("ballPosition", new Vector4(impactPos, maxBallDist + dist));
        //    yield return null;
        //}
        impactRoutine = null;
    }
}
