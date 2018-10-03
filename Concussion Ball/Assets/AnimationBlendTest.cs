using ThomasEngine;
using ThomasEngine.Script;
using System;

public class AnimationBlendTest : ScriptComponent
{
    BlendNode root;
    PlaybackNode from;
    PlaybackNode to;
    LookAtConstraint lookAt;

    RenderSkinnedComponent skinn;
    WeightHandle weight;
    float timer;

    public Animation fromAnim { get; set; }
    public Animation toAnim { get; set; }
    public override void Start()
    {
        skinn = gameObject.GetComponent<RenderSkinnedComponent>();

        root = new BlendNode(skinn.model);
        from = new PlaybackNode(skinn.model, fromAnim);
        to = new PlaybackNode(skinn.model, toAnim);
        root.appendNode(from);
        root.appendNode(to);
        weight = root.generateWeightHandle();
        skinn.setBlendTreeNode(root);

        // LookAt constraint
        lookAt = new LookAtConstraint();
        uint boneIndex;
        if (skinn.FetchBoneIndex(Utility.hash("mixamorig:Head"), out boneIndex))
            lookAt.apply(skinn, boneIndex);
    }

    public override void Update()
    {
        unsafe
        {
            //weight.m_WeightData[0] = 
            timer += Time.DeltaTime;
            float t = (float)(0.5 * Math.Sin(timer) + 0.5);
            WeightTripple w = WeightTripple.fromWeight(t);
            WeightTripple w2 = WeightTripple.fromWeight(1 - t);
            weight.setWeight(0, w);
            weight.setWeight(1, w2);
        }
    }
}
