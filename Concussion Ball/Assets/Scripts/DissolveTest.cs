using System.Collections;
using ThomasEngine;
public class DissolveTest : ScriptComponent
{
    Material m;
    public override void Start()
    {
        m = gameObject.GetComponent<RenderComponent>()?.material;
        m.SetFloat("dissolveAmount", 1.0f);
        m.SetFloat("dissolveOffset", 0.0f);
    }

    public override void Update()
    {
        if (Input.GetKeyDown(Input.Keys.O))
            StartCoroutine(FadeIn(3.0f));

    }

    IEnumerator FadeIn(float time)
    {

        m.SetFloat("dissolveOffset", Time.ElapsedTime);
        float totalFadeTime = time;
        float timeLeft = time;

        while(timeLeft > 0)
        {
            
            timeLeft -= Time.DeltaTime;
            float dissolveAmount = timeLeft / totalFadeTime;
            m.SetFloat("dissolveAmount", dissolveAmount);
            yield return null;
        }

        while (timeLeft < totalFadeTime)
        {
            timeLeft += Time.DeltaTime;
            float dissolveAmount = timeLeft / totalFadeTime;
            m.SetFloat("dissolveAmount", dissolveAmount);
            yield return null;
        }


    }

}
