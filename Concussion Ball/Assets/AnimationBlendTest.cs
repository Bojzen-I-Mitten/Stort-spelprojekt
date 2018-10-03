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
        from = new PlaybackNode(skinn.model, fromAnim, true);
        to = new PlaybackNode(skinn.model, toAnim, true);
        root.appendNode(from);
        root.appendNode(to);
        weight = root.generateWeightHandle();
        skinn.setBlendTreeNode(root);

        // LookAt constraint
        lookAt = new LookAtConstraint(LookAtConstraint.AxisConstraint.AxisXYZ);
        lookAt.Weight = 1.0f;
        uint boneIndex = 0;
        if (skinn.FetchBoneIndex(Utility.hash("mixamorig:Head"), out boneIndex))
            lookAt.apply(skinn, boneIndex);
    }

    public override void Update()
    {
        unsafe
        {
            //weight.m_WeightData[0] = 
            timer += Time.DeltaTime;
            float curve = (float)Math.Sin(timer);
            float t = 0.5f * curve + 0.5f;
            WeightTripple w = WeightTripple.fromWeight(1);
            WeightTripple w2 = WeightTripple.fromWeight(1);
            weight.setWeight(0, w);
            weight.setWeight(1, w2);
            lookAt.Target =  new Vector3(curve, 1.5f, -1f);
        }
    }
}
