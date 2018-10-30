using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ThomasEngine;
using ThomasEngine.Network;
using ThomasEngine.Script;


public class Chadimations : NetworkComponent
{
    public class AnimationNode
    {
        public Animation Animation { get; set; }
        public Vector3 Position { get; set; }

        public float GetWeight(Vector3 target)
        {
            //Vector3 active = new Vector3(Position.x != 0 ? 1 : 0, Position.y != 0 ? 1 : 0, Position.z != 0 ? 1 : 0);
            //Vector3 distanceVector = target - Position;
            //distanceVector = Vector3.Multiply(distanceVector, active);
            //float distance = distanceVector.Length();
            float distance = Vector3.Distance(target, Position); //Buggy animations
            return 1.0f - MathHelper.Clamp(distance, 0.0f, 1.0f);
        }
    }

    private ChadControls Chad = null;
    private Vector3 Direction
    {
        get
        {
            if (Chad != null)
                return Chad.Direction;
            return new Vector3(0, 0, 0);
        }
    }
    private ChadControls.STATE State
    {
        get
        {
            if (Chad != null)
                return Chad.State;
            return 0;
        }
    }

    public Dictionary<ChadControls.STATE, List<AnimationNode>> Animations { get; set; } = new Dictionary<ChadControls.STATE, List<AnimationNode>>();
    public Dictionary<ChadControls.STATE, BlendNode> BlendNodes = new Dictionary<ChadControls.STATE, BlendNode>();
    public Dictionary<ChadControls.STATE, WeightHandle> WeightHandles = new Dictionary<ChadControls.STATE, WeightHandle>();

    public RenderSkinnedComponent Skin { get; set; }

    public override void Start()
    {
        Chad = gameObject.GetComponent<ChadControls>();
        if (Skin != null)
        {
            if (Animations.Count > 0)
            {
                foreach (var state in Animations)
                {
                    BlendNode newBlendNode = new BlendNode(Skin.model);
                    foreach (var node in state.Value)
                    {
                        newBlendNode.appendNode(node.Animation, true);
                    }
                    BlendNodes.Add(state.Key, newBlendNode);

                    WeightHandle newWeightHandle = BlendNodes[state.Key].generateWeightHandle();
                    WeightHandles.Add(state.Key, newWeightHandle);
                }

                WeightHandles[ChadControls.STATE.CHADING].setWeight(0, new WeightTripple(1f));
                Skin.setBlendTreeNode(BlendNodes[ChadControls.STATE.CHADING]);
            }
        }
    }

    public override void Update()
    {
        if (BlendNodes.ContainsKey(State))
        {
            Skin.setBlendTreeNode(BlendNodes[State]);
            for (uint i = 0; i < Animations[State].Count; i++)
            {
                AnimationNode node = Animations[State][(int)i];
                WeightHandles[State].setWeight(i, node.GetWeight(Direction));
            }
        }
    }
}