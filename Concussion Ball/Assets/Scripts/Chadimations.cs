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
        public Animation animation { get; set; }
        public Vector3 position { get; set; }

        public float GetWeight(Vector3 target)
        {
            float distance = Vector3.Distance(target, position);
            return 1.0f - MathHelper.Clamp(distance, 0.0f, 1.0f);
        }
    }

    private ChadStateMachine ChadSM = null;
    private Vector3 Direction
    {
        get
        {
            if (ChadSM != null)
                return ChadSM.Direction;
            return new Vector3(0, 0, 0);
        }
    }
    private ChadStateMachine.CHAD_STATE State
    {
        get
        {
            if (ChadSM != null)
                return ChadSM._State;
            return 0;
        }
    }

    Dictionary<ChadStateMachine.CHAD_STATE, List<AnimationNode>> Animations { get; set; } = new Dictionary<ChadStateMachine.CHAD_STATE, List<AnimationNode>>();
    Dictionary<ChadStateMachine.CHAD_STATE, BlendNode> BlendNodes = new Dictionary<ChadStateMachine.CHAD_STATE, BlendNode>();
    Dictionary<ChadStateMachine.CHAD_STATE, WeightHandle> WeightHandles = new Dictionary<ChadStateMachine.CHAD_STATE, WeightHandle>();

    public RenderSkinnedComponent Skin { get; set; }

    public override void Start()
    {
        ChadSM = gameObject.GetComponent<ChadStateMachine>();
        if (Skin != null)
        {
            foreach (var state in Animations)
            {
                BlendNode newBlendNode = new BlendNode(Skin.model);
                foreach (var node in state.Value)
                {
                    newBlendNode.appendNode(node.animation, true);
                }
                BlendNodes.Add(state.Key, newBlendNode);

                WeightHandle newWeightHandle = BlendNodes[state.Key].generateWeightHandle();
                WeightHandles.Add(state.Key, newWeightHandle);
            }

            WeightHandles[ChadStateMachine.CHAD_STATE.CHADING].setWeight(0, new WeightTripple(1f));
        }
    }

    public override void Update()
    {
        //Weight update
        for (uint i = 0; i < Animations[State].Count; i++)
        {
            AnimationNode node = Animations[State][(int)i];
            WeightHandles[State].setWeight(i, new WeightTripple(node.GetWeight(Direction)));
        }
    }
}