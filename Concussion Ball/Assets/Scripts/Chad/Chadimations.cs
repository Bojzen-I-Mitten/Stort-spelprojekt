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
    private bool _Throwing = false;
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

                        WeightHandle newWeightHandle = BlendNodes[state.Key].generateWeightHandle();
                        WeightHandles.Add(state.Key, newWeightHandle);
                    }
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
                if (node != null)
                {
                    if (State != ChadControls.STATE.THROWING) // set throwing weights from chadControls instead
                    {

                        WeightHandles[State].setWeight(i, node.GetWeight(Direction));
                        node.Node.getTimeHandle().SetSpeed(1);
                    }
                    else if (!_Throwing && i == 1) // not throwing and index handling throwing anim
                    {
                        WeightHandles[State].setWeight(i, 0); // set throwing anim weight to 0
                        node.Node.getTimeHandle().SetSpeed(2.2f);
                    }
                    else if (i == 1) // throw anim
                        node.Node.getTimeHandle().SetSpeed(2.2f);
                    else if (Chad.PickedUpObject) // charge throw anim
                        node.Node.getTimeHandle().SetSpeed(2.6f / Chad.PickedUpObject.chargeTimeMax);
                }
                else
                {
                    Debug.Log("Animation not found for: " + State + i);
                }
            }
        }
    }

    public void SetAnimationWeight(uint index, float weight)
    {
        if (index == 1 && weight == 1)
        {
            _Throwing = true;
            
        }
        else
            _Throwing = false;

        if (BlendNodes.ContainsKey(State))
            BlendNodes[State].ResetPlayback();
        
        // Debug.Log("Manually setting weight of animation numer: " + index + " to: "+ weight);
        if(WeightHandles.ContainsKey(State))
            WeightHandles[State].setWeight(index, weight);
    }

    public void ResetTimer(ChadControls.STATE state, uint index)
    {
        BlendNodes[state].ResetPlayback();
    }

    public void RPCResetTimer(uint index)
    {
        ResetTimer(State, index);
        SendRPC("ResetTimer", (int)index);
    }
} 