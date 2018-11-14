using System.Collections.Generic;
using System.ComponentModel;
using ThomasEngine;
using ThomasEngine.Script;

public class AnimationBlendTree : ScriptComponent
{
    public class AnimationNode
    {
        public Animation animation { get; set; }
        public Vector2 position { get; set; }

        public float GetWeight(Vector2 target)
        {
            float distance = Vector2.Distance(target, position);
            return 1.0f - MathHelper.Clamp(distance, 0.0f, 1.0f);
        }
    }

    WeightHandle weight;

    public RenderSkinnedComponent skinn { get; set; }
    public List<AnimationNode> nodes { get; set; } = new List<AnimationNode>();
    public Vector2 currentPos { get; set; }

    public override void Start()
    {
        //Blabla initial setup
       
        BlendNode blend = new BlendNode(skinn.model);

        foreach (var node in nodes)
            blend.appendNode(node.animation, true);
        weight = blend.generateWeightHandle();
        skinn.setBlendTreeNode(blend);

    }

    public override void Update()
    {
        //Ugly move code
        currentPos = Vector2.Clamp(currentPos, -Vector2.One, Vector2.One);
        transform.position += new Vector3(currentPos.x, 0, -currentPos.y) * Time.DeltaTime;


        //Weight update
        for(uint i=0; i < nodes.Count; i++)
        {
            AnimationNode node = nodes[(int)i];
            weight.setWeight(i, new WeightTripple(node.GetWeight(currentPos)));
        }
    }
}
