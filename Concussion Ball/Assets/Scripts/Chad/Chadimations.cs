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
    public bool Throwing = false;
    private ChadControls Chad = null;
    private AnimationMixer mixer;
    /* Chad direction input information
    */
    private Vector3 Direction   {   get {   return Chad.Direction;  }   }
    /* Fetch state from chad controller
     */
    private ChadControls.STATE ChadState    {   get   { return Chad.State;  }    }
    /* State of the animation system in last frame.
    */
    private ChadControls.STATE AnimState = ChadControls.STATE.NUM_STATES;

    public Dictionary<ChadControls.STATE, List<AnimationNode>> Animations { get; set; } = new Dictionary<ChadControls.STATE, List<AnimationNode>>();
    public Dictionary<ChadControls.STATE, BlendNode> BlendNodes = new Dictionary<ChadControls.STATE, BlendNode>();

    private WeightHandle getWeightHandle(ChadControls.STATE state)
    {
        var node = BlendNodes[state];
        return node == null ? null : node.getWeightHandle();
    }
    

    public override void Start()
    {
        // Fetch chad
        Chad = gameObject.GetComponent<ChadControls>();
        if (!Chad)
            throw new InvalidOperationException("No chad found in object.");
        if(Animations.Count == 0)
            throw new InvalidOperationException("No animation information found in component.");
        
        // Initiate
        mixer = new AnimationMixer(gameObject);
        // Generate nodes
        foreach (var state in Animations)
        {
            if (state.Key == ChadControls.STATE.RAGDOLL)
                continue; // No Ragdoll animations
            // Gen. state blend node
            BlendNode newBlendNode = new BlendNode(mixer.Skin.model);
            // Gen. individual playback nodes
            for (int i = 0; i < state.Value.Count; ++i)
            {
                if (state.Key == ChadControls.STATE.THROWING)
                    state.Value[i].Node = newBlendNode.appendNode(state.Value[i].Animation, false);
                else
                    state.Value[i].Node = newBlendNode.appendNode(state.Value[i].Animation, true);
            }
            BlendNodes.Add(state.Key, newBlendNode);
        }
        // Set default playback state.
        SetBlendState(ChadControls.STATE.CHADING, AnimationMixer.BlendIn.Instant);
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
        SetBlendState(state, AnimationMixer.BlendIn.Overwrite);
    }
    private void SetBlendState(ChadControls.STATE state, AnimationMixer.BlendIn mode)
    {
        AnimState = state;
        // Currently only overwrite current state (no mixed state)
        AnimationMixer.MixInfo info;
        info.mixTime = 0.5f;
        info.mixWeight = 1.0f;
        info.mode = mode;
        // Apply
        mixer.mixIn(BlendNodes[state], info);
    }
    /* Run update logic on the throwing state
     */
    private void UpdateThrowingState()
    {
        BlendNode blend = BlendNodes[ChadControls.STATE.THROWING];
        var nodes = Animations[ChadControls.STATE.THROWING];
        if (CheckAnimList(nodes, ChadControls.STATE.THROWING))
            return;

        // Charge anim control : node[0]
        if (Chad.PickedUpObject) // charge throw anim
            AnimationSpeed(nodes[0], 2.6f / Chad.PickedUpObject.chargeTimeMax);

        // Release anim control : node[1]
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
        // Update weight mixer
        mixer.Update(Time.DeltaTime);
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