using ThomasEngine;
using ThomasEngine.Script;
using System;

public class AnimationBlendTest : ScriptComponent
{
    BlendNode root;
    RenderSkinnedComponent skinn;
    WeightHandle weight;
    float timer;

    public Animation fromAnim { get; set; }
    public Animation toAnim { get; set; }
    public override void Start()
    {
        skinn = gameObject.GetComponent<RenderSkinnedComponent>();

        root = new BlendNode(skinn.model);
        root.appendNode(fromAnim, false);
        root.appendNode(toAnim, false);
        weight = root.generateWeightHandle();
        skinn.setBlendTreeNode(root);
    }

    public override void Update()
    {
        unsafe
        {
            //weight.m_WeightData[0] = 
            timer += Time.DeltaTime;
            float t = (float)(0.5 * Math.Sin(timer) + 0.5);
            WeightTripple w;
            w.m_scale = t;
            w.m_rot = t;
            w.m_translation = t;
            weight.setWeight(0, w);

            t = 1 - t;
            w.m_scale = t;
            w.m_rot = t;
            w.m_translation = t;
            weight.setWeight(1, w);
        }
    }
}
