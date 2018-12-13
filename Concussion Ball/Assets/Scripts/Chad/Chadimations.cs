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

public class Chadimations : NetworkComponent, ThomasEngine.IPropertyChanged
{
    public enum PlayType
    {
        None = 0,
        Once = 1,
        Loop = 2
    };
    [Newtonsoft.Json.JsonObject]
    [TypeConverter(typeof(ExpandableObjectConverter))]
    public class AnimationNode
    {
        public Animation Animation { get; set; }
        public Vector3 Position { get; set; }
        public PlayType PlayType { get; set; }
        public AnimationParams param { get; set; }
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

        public AnimationNode()
        {
            param = new AnimationParams();
        }
    }
    public void PropertyDataChanged(Type type, object data)
    {
        if(type == typeof(Dictionary<ChadControls.STATE, Dictionary<int, AnimationNode>>))
        {
            var val = data as Dictionary<ChadControls.STATE, Dictionary<int, AnimationNode>>;
            Animations = val;
        }
    }
    /* Animation state, losely related to State of chad. However each chad state can have an animation state as well (synced over net)
     * This allows chad state to relate to multiple instances without caring over RPC calls.
     * Solution is pretty shitty but \^(-)^/ 
     * Each chad state also controls a BlendNode containing the animations with a non-synced weight control (RPC req.)
    */
    public class AnimationState
    {
        public BlendNode node;      // Blend node owning
        public int state = -1;      // Internal animation state of the playback (if any is defined)
    }
    private ChadControls Chad = null;
    private AnimationMixer mixer;
    /* Chad direction input information
    */
    private Vector3 Direction { get { return Chad.Direction; } }
    /* Fetch state from chad controller
     */
    private ChadControls.STATE ChadState { get { return Chad.State; } }
    /* State of the animation system in last frame.
    */
    private ChadControls.STATE ChadAnimState = ChadControls.STATE.NUM_STATES;
    /* Serialization property
    */
    [JsonProperty(ObjectCreationHandling = ObjectCreationHandling.Replace)]
    public Dictionary<ChadControls.STATE, Dictionary<int, AnimationNode>> Animations
    {

        get
        {
            Dictionary<ChadControls.STATE, Dictionary<int, AnimationNode>> dict = new Dictionary<ChadControls.STATE, Dictionary<int, AnimationNode>>();

            for (int state = 0; state < m_animations.Count; state++)
            {
                Dictionary<int, AnimationNode> node = new Dictionary<int, AnimationNode>();
                for (int i = 0; i < m_animations[state].Count; i++)
                {
                    node.Add(i, m_animations[state][i]);
                }
                dict.Add((ChadControls.STATE)state, node);
            }
            return dict;
        }
        set
        {
            foreach (var list_pair in value)
            {
                List<AnimationNode> list = new List<AnimationNode>(list_pair.Value.Count);
                foreach (var pair in list_pair.Value)
                    list.Add(null);
                foreach (var pair in list_pair.Value)
                {
                    if(pair.Key < list_pair.Value.Count && pair.Key >= 0)   // Assert size
                        list[pair.Key] = pair.Value;
                }
                m_animations[(int)list_pair.Key] = list;
            }
        }
    }
    
    private List<List<AnimationNode>> m_animations;
    private List<AnimationState> AnimState;


    public Chadimations()
    {
        m_animations = new List<List<AnimationNode>>();
        AnimState = new List<AnimationState>((int)ChadControls.STATE.NUM_STATES);
        for (int i = 0; i < (int)ChadControls.STATE.NUM_STATES; i++)
        {
            m_animations.Add(new List<AnimationNode>());
        }
    }

    private WeightHandle getWeightHandle(ChadControls.STATE state)
    {
        var node = AnimState[(int)state].node;
        return node == null ? null : node.getWeightHandle();
    }

    public override void OnAwake()
    {
        base.OnAwake();
        // Fetch chad
        Chad = gameObject.GetComponent<ChadControls>();
        if (!Chad)
            throw new InvalidOperationException("No chad found in object.");
        if (m_animations.Count == 0)
            throw new InvalidOperationException("No animation information found in component.");
        if (m_animations[0].Count == 0 || m_animations[0][0].Animation == null)
            throw new InvalidOperationException("No Chadding default animation found.");

        for (int state = 0; state < (int)ChadControls.STATE.NUM_STATES; state++)
        {
            AnimationState value = new AnimationState();
            AnimState.Add(value);
        }
    }
    public override void Start()
    {
        // Initiate
        mixer = new AnimationMixer(gameObject);
        // Generate nodes
        for (int state = 0; state < (int)ChadControls.STATE.NUM_STATES; state++)
        {
            var list = m_animations[state];
            if (state == (int)ChadControls.STATE.RAGDOLL)
            { } // No Ragdoll animations
            else
            {
                // Gen. state blend node
                var node = new BlendNode(mixer.Skin.model); ;
                AnimState[state].node = node;
                // Gen. individual playback nodes
                for (int i = 0; i < list.Count; ++i)
                {
                    if (list[i].Animation == null)
                    {
                        Debug.LogWarning("Animation was null in state: " + state + ", at index: " + i);
                    }
                    else
                        list[i].Node = node.appendNode(list[i].Animation, list[i].PlayType == PlayType.Loop);
                }
            }
        }
        Reset();
        if (CheckAnimList(ChadControls.STATE.THROWING))
            throw new InvalidOperationException("Throwing animations not found.");
    }
    public void Reset()
    {
        // Set default playback state.
        SetBlendState(ChadControls.STATE.CHADING, AnimationMixer.BlendIn.Instant);
        for (int i = 0; i < (int)ChadControls.STATE.NUM_STATES; i++)
            AnimState[i].state = -1;
    }
    private bool CheckAnimList(ChadControls.STATE State)
    {
        List<AnimationNode> list = m_animations[(int)State];
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
        ChadAnimState = state;
        // Currently only overwrite current state (no mixed state)
        AnimationMixer.MixInfo info;
        info.mixTime = 1.0f;
        info.mixWeight = 1.0f;
        info.mode = mode;
        // Apply
        mixer.mixIn(AnimState[(int)state].node, info);
    }
    /* Run update logic on the throwing state
     */
    private void UpdateThrowingState()
    {
        BlendNode blend = AnimState[(int)ChadControls.STATE.THROWING].node;
        var nodes = m_animations[(int)ChadControls.STATE.THROWING];

    }
    /* Run update logic on the general state(s)
     */
    private void UpdateAnimationState(ChadControls.STATE State)
    {
        BlendNode blend = AnimState[(int)State].node;
        List<AnimationNode> nodes = m_animations[(int)State];

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
        if (ChadAnimState != State)
        {
            var PrevState = ChadAnimState;
            SetBlendState(State);
            // Leave animation state of prev.
            AnimState[(int)PrevState].state = -1;
        }
        // Update state logic
        switch (State)
        {
            case ChadControls.STATE.THROWING:
                {
                    UpdateThrowingState();
                }
                break;
            case ChadControls.STATE.CHADING:
                {
                    UpdateAnimationState(State);
                }
                break;
            case ChadControls.STATE.DIVING:
            case ChadControls.STATE.RAGDOLL:
            default:
                break;
        }
        // Update weight mixer
        mixer.Update(Time.DeltaTime);
    }
    public void EnterState(ChadControls.STATE chadState, int animState)
    {
        AnimationState A = AnimState[(int)chadState];
        if (A.state == animState)
            return;
        List<AnimationNode> nodes = m_animations[(int)chadState];
        // Reset other states
        for(int i = 0; i < (int)ChadControls.STATE.NUM_STATES; i++)
            AnimState[i].state = -1;
        // Apply new
        A.state = animState;
        // Enter animation state logic
        switch (chadState)
        {
            case ChadControls.STATE.THROWING:
                {
                    if (animState == 0)
                        PlayThrowCharge();
                    else if (animState == 1)
                        PlayThrowRelease();
                    else
                        ResetThrow();
                }
                break;
            case ChadControls.STATE.DIVING:
                {
                    // Tackle!
                    nodes[0].Node.getTimeHandle().Play();
                    A.node.getWeightHandle().setWeight(0, 1.0f);
                }
                break;
            case ChadControls.STATE.RAGDOLL:
                {

                }
                break;
            case ChadControls.STATE.CHADING:
            default:
                {
                    nodes[0].Node.getTimeHandle().ResetTime();
                }
                break;
        }
    }
    public int ReadState(ChadControls.STATE chadState)
    {
        return AnimState[(int)chadState].state;
    }
    private void EnterThrowState(int newState)
    {

    }
    private void PlayThrowCharge()
    {
        var nodes = m_animations[(int)ChadControls.STATE.THROWING];
        // Charge anim control : node[0]
        nodes[0].Node.getTimeHandle().Play();
        if (Chad.PickedUpObject) // Charge throw anim
            AnimationSpeed(nodes[0], 2.6f / Chad.PickedUpObject.chargeTimeMax);
        else
            AnimationSpeed(nodes[0], 2.4f);
        // Set weights
        var handle = getWeightHandle(ChadControls.STATE.THROWING);
        handle.setWeight(0, 1.0f);
        handle.setWeight(1, 0.0f);
    }
    private void PlayThrowRelease()
    {
        var nodes = m_animations[(int)ChadControls.STATE.THROWING];
        // Throw release anim control : node[1]
        AnimationSpeed(nodes[1], 2.4f);
        nodes[1].Node.getTimeHandle().Play();
        // Set weights
        var handle = getWeightHandle(ChadControls.STATE.THROWING);
        handle.setWeight(0, 0.0f);
        handle.setWeight(1, 1.0f);
    }
    private void ResetThrow()
    {
        // Nothing req. to do
    }

    public void SetAnimationWeight(uint index, float weight)
    {
        var State = ChadState;
        AnimationNode node = m_animations[(int)State][(int)index];
        // Reset
        node.Node.getTimeHandle().ResetTime();

        // Debug.Log("Manually setting weight of animation number: " + index + " to: "+ weight);
        getWeightHandle(State).setWeight(index, weight);
    }

    public void ResetTimer(ChadControls.STATE state, uint index)
    {
        AnimState[(int)state].node.ResetPlayback();
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