using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ThomasEngine;
using ThomasEngine.Network;
using ThomasEngine.Script;
using Newtonsoft.Json;

public class Chadimations : NetworkComponent
{
    public class AnimationNode
    {
        public Animation Animation { get; set; }
        public Vector3 Position { get; set; }
        /* PlaybackHandle node related to the anim
        */
        [JsonIgnoreAttribute]
        [BrowsableAttribute(false)]
        public PlaybackNode Node { get; set; }

        [JsonIgnoreAttribute]
        [BrowsableAttribute(false)]
        public float Weight;

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
    public bool Throwing = false;
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
    private ChadControls.STATE m_blendState = ChadControls.STATE.NUM_STATES;
    private ChadControls.STATE ChadState
    {
        get
        {
            if (Chad != null)
                return Chad.State;
            return 0;
        }
    }
    private ChadControls.STATE AnimState;

    public Dictionary<ChadControls.STATE, List<AnimationNode>> Animations { get; set; } = new Dictionary<ChadControls.STATE, List<AnimationNode>>();
    public Dictionary<ChadControls.STATE, BlendNode> BlendNodes = new Dictionary<ChadControls.STATE, BlendNode>();

    private WeightHandle getWeightHandle(ChadControls.STATE state)
    {
        var node = BlendNodes[state];
        return node == null ? null : node.getWeightHandle();
    }
    
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
                    if (state.Key != ChadControls.STATE.RAGDOLL) // No Ragdoll animations
                    {
                        BlendNode newBlendNode = new BlendNode(Skin.model);
                        for (int i = 0; i < state.Value.Count; ++i)
                        {
                            if (state.Key == ChadControls.STATE.THROWING)
                                state.Value[i].Node = newBlendNode.appendNode(state.Value[i].Animation, false);
                            else
                                state.Value[i].Node = newBlendNode.appendNode(state.Value[i].Animation, true);
                        }
                        BlendNodes.Add(state.Key, newBlendNode);
                    }
                }

                getWeightHandle(ChadControls.STATE.CHADING).setWeight(0, new WeightTripple(1f));
                Skin.setBlendTreeNode(BlendNodes[ChadControls.STATE.CHADING]);
                AnimState = ChadControls.STATE.CHADING;
            }
        }
    }
    private bool CheckAnimList(List<AnimationNode> list, ChadControls.STATE State)
    {
        bool invalid = false;
        for (uint i = 0; i < list.Count; i++)
        {
            AnimationNode node = list[(int)i];
            if (node == null)
            {
                Debug.Log("Animation not found for: " + State + i);
                invalid = true;
            }
        }
        return invalid;
    }
    /* Switch animated state
    */
    private void SetBlendState(ChadControls.STATE state)
    {
        AnimState = state;
        Skin.setBlendTreeNode(BlendNodes[state]);

    }
    /* Run update logic on the throwing state
     */
    private void UpdateThrowingState()
    {
        BlendNode blend = BlendNodes[ChadControls.STATE.THROWING];
        var nodes = Animations[ChadControls.STATE.THROWING];
        if (CheckAnimList(nodes, ChadControls.STATE.THROWING))
            return;

        // Charge anim control
        if (Chad.PickedUpObject) // charge throw anim
            AnimationSpeed(nodes[0], 2.6f / Chad.PickedUpObject.chargeTimeMax);

        // Pickup anim control
        if(Throwing)
        {
            AnimationSpeed(nodes[1], 2.4f);
        }
        else
        {
            blend.getWeightHandle().setWeight(1, 0);// set throwing anim weight to 0
            //node.Node.getTimeHandle().SetSpeed(2.2f);
            AnimationSpeed(nodes[1], 2.4f);

        }
    }
    /* Run update logic on the general state(s)
     */
    private void UpdateAnimationState(ChadControls.STATE State)
    {
        BlendNode blend = BlendNodes[State];
        List<AnimationNode> nodes = Animations[State];
        if (CheckAnimList(nodes, ChadControls.STATE.THROWING))
            return;

        for (uint i = 0; i < nodes.Count; i++)
        {
            AnimationNode node = nodes[(int)i];
            if (node == null)
            {
                Debug.Log("Animation not found for: " + State + i);
                return;
            }
            blend.getWeightHandle().setWeight(i, node.GetWeight(Direction));
            //node.Node.getTimeHandle().SetSpeed(1);
            AnimationSpeed(node, 1);
        }
    }
    
    public override void Update()
    {
        var State = ChadState;
        if (BlendNodes.ContainsKey(ChadState))
        {
            if (AnimState != State)
                SetBlendState(State);
            switch (State)
            {
                case ChadControls.STATE.THROWING:
                    {
                        UpdateThrowingState();
                    }
                    break;
                case ChadControls.STATE.DIVING:
                case ChadControls.STATE.RAGDOLL:
                case ChadControls.STATE.CHADING:
                default:
                    {
                        UpdateAnimationState(State);
                    }
                    break;
            }
        }
    }


    public void SetAnimationWeight(uint index, float weight)
    {
        if (index == 1 && weight == 1)
        {
            Throwing = true;
            
        }
        else
            Throwing = false;
        var State = ChadState;
        if (BlendNodes.ContainsKey(State))
        {
            // Reset
            BlendNodes[State].ResetPlayback();

            // Debug.Log("Manually setting weight of animation numer: " + index + " to: "+ weight);
            getWeightHandle(State).setWeight(index, weight);
        }
    }

    public void ResetTimer(ChadControls.STATE state, uint index)
    {
        BlendNodes[state].ResetPlayback();
    }

    public void RPCResetTimer(uint index)
    {
        ResetTimer(ChadState, index);
        SendRPC("ResetTimer", (int)index);
    }

    private void RPCAnimationSpeed(AnimationNode node, float amount)
    {
        AnimationSpeed(node, amount);
        SendRPC("AnimationSpeed", (AnimationNode)node, (float)amount);
    }

    private void AnimationSpeed(AnimationNode node, float amount)
    {
        node.Node.getTimeHandle().SetSpeed(amount);
    }
} 