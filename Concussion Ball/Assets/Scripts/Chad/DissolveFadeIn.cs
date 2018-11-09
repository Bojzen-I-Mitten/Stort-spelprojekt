using System.Collections;
using ThomasEngine;
public class DissolveFadeIn : ScriptComponent
{
    public float fadeInTime { get; set; } = 1.0f;
    Material m;
    public override void Awake()
    {
        m = gameObject.GetComponent<RenderComponent>()?.material;

    }

    public override void OnEnable()
    {
        m.SetFloat("dissolveAmount", 1.0f);
        m.SetFloat("dissolveOffset", 0.0f);
        StartCoroutine(FadeIn(fadeInTime));
    }

    public override void OnDisable()
    {
        StopAllCoroutines();
    }

    IEnumerator FadeIn(float time)
    {

        m.SetFloat("dissolveOffset", Time.ElapsedTime);
        float totalFadeTime = time;
        float timeLeft = time;

        while (timeLeft > 0)
        {

            timeLeft -= Time.DeltaTime;
            float dissolveAmount = timeLeft / totalFadeTime;
            m.SetFloat("dissolveAmount", dissolveAmount);
            yield return null;
        }
    }

}
