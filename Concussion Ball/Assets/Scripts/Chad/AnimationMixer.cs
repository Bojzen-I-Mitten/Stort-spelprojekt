using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.ComponentModel;
using ThomasEngine;
using ThomasEngine.Script;

    [Newtonsoft.Json.JsonObject]
    [TypeConverter(typeof(ExpandableObjectConverter))]
    public class AnimationParams
    {
        public List<Vector2> LeftFootGround { get; set; } = new List<Vector2>();
        public List<Vector2> RightFootGround { get; set; } = new List<Vector2>();

        public AnimationParams() { }
}
/*
 * Basic animation mixer for chad
 */
public class AnimationMixer
{
    public enum BlendIn
    {
        Additive,
        Overwrite,
        Instant     // No blend in to the node. Applies it
    }
    public struct MixInfo
    {
        public float mixTime;
        public float mixWeight;
        public BlendIn mode;

        public void clamp()
        {
            mixWeight = MathHelper.Clamp(mixWeight, 0.0f, 1.0f);
        }
    }
    public struct NodeBlendState
    {
        public float totalTime;     //  Time to reach weight factor
        public float time;          //  Time left to reach weight factor
        public float fromWeight;    //  Blended from
        public float weight;        //  Target weight
    }

    public RenderSkinnedComponent Skin { private set; get; }
    private BlendNode Master;
    private WeightHandle Weights;
    /* Interpolate mixing factors for each node. 
    */
    private NodeBlendState[] BlendTargets = new NodeBlendState[BlendNode.MAX_ANIMATION_BLEND_NODE];

    public AnimationMixer(GameObject gameObject)
    {
        Skin = gameObject.GetComponent<RenderSkinnedComponent>();
        if (!Skin)
            throw new InvalidOperationException("No skinning component found in object.");
        // Create and apply master blend node
        Master = new BlendNode(Skin.model, BlendNode.MAX_ANIMATION_BLEND_NODE);
        Weights = Master.generateWeightHandle();
        Skin.setBlendTreeNode(Master);
    }

    private uint emptySlot()
    {
        for (uint i = 0; i < BlendNode.MAX_ANIMATION_BLEND_NODE; i++)
        {
            if(Weights.getWeight(i).AbsoluteSum() == 0.0f)
                return i;
        }
        return 0; // Solve some other way?
    }
    /* Find index of node with largest blend factor
     */ 
    private uint largestFactor()
    {
        uint index = 0;
        float fac = Weights.getWeight(0).AbsoluteSum();
        for (uint i = 1; i < BlendNode.MAX_ANIMATION_BLEND_NODE; i++)
        {
            float f = Weights.getWeight(i).AbsoluteSum();
            if (f > fac)
            {
                index = i;
                fac = f;
            }
        }
        return index; // Solve some other way?
    }

    private void reset(uint remainer, float remainder)
    {
        for (uint i = 0; i < BlendNode.MAX_ANIMATION_BLEND_NODE; i++)
        {
            setBlendFac(i, 0.0f, 0.0f);
            Weights.setWeight(i, 0.0f);
        }
        if (remainer < BlendNode.MAX_ANIMATION_BLEND_NODE)
        {
            setBlendFac(remainer, 0.0f, 0.0f);
            Weights.setWeight(remainer, remainder);
        }
    }

    public void mixIn(BlendNode node, MixInfo info)
    {
        info.clamp();
        // Find available index
        uint slot = emptySlot();
        Master.setNode(node, slot);
        // Relate blend in to other factors
        switch (info.mode)
        {
            case BlendIn.Additive:
                {
                    // Blend in node to the mix factor
                    setBlendFac(slot, info.mixTime, info.mixWeight);
                }
                break;
            case BlendIn.Instant:
                {
                    reset(slot, info.mixWeight);
                }
                break;
            case BlendIn.Overwrite:
            default:
                {
                    uint other = largestFactor();
                    // Blend out all other weights
                    for (uint i = 0; i < BlendNode.MAX_ANIMATION_BLEND_NODE; i++)
                        setBlendFac(i, info.mixTime, 0.0f);
                    // Blend in node, and weight to other
                    setBlendFac(slot, info.mixTime, info.mixWeight);
                    setBlendFac(other, info.mixTime, 1.0f - info.mixWeight);
                }
                break;
        }
    }

    private void setBlendFac(uint i, float time, float weight)
    {
        BlendTargets[i].totalTime = time;
        BlendTargets[i].time = time;
        BlendTargets[i].fromWeight = Weights.getWeight(i).Average();
        BlendTargets[i].weight = weight;
    }

    public void Update(float dT)
    {
        // Update blending factors
        for (uint i = 0; i < BlendNode.MAX_ANIMATION_BLEND_NODE; i++)
        {
            if (BlendTargets[i].time > 0.0f) // Assert node is blended
            {
                BlendTargets[i].time = Math.Max(0.0f, BlendTargets[i].time - dT);
                float w = MathHelper.Lerp(BlendTargets[i].weight, BlendTargets[i].fromWeight, BlendTargets[i].time / BlendTargets[i].totalTime);
                Weights.setWeight(i, w);
            }
        }
    }
}
