using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ThomasEngine;
using ThomasEngine.Network;
using ThomasEngine.Script;


public class Chadimations : NetworkComponent
{
    public enum STATE
    {
        IDLE,
        WALKING,
        STRAFING_LEFT,
        STRAFING_RIGHT,
        BACKWARDS,
        RUNNING,
        TURNING_LEFT,
        TURNING_RIGHT,
        THROWING,
        DIVING,
        RAGDOLL,

        NUM_STATES
    };

    Dictionary<STATE, PlaybackNode> PlaybackNodes = new Dictionary<STATE, PlaybackNode>();
    List<PlaybackHandle> PlaybackHandles;
    public List<Animation> Animations { get; set; } = new List<Animation>();

    public Animation Idle { get; set; }
    public Animation Walking { get; set; }
    public Animation StrafingLeft { get; set; }
    public Animation StrafingRight { get; set; }
    public Animation Backwards { get; set; }
    public Animation Running { get; set; }
    public Animation TurningLeft { get; set; }
    public Animation TurningRight { get; set; }
    public Animation Throwing { get; set; }
    public Animation Diving { get; set; }


    BlendNode MasterBlendNode;
    WeightHandle MasterWeightHandle;

    LookAtConstraint lookAt;

    public RenderSkinnedComponent skin { get; set; }
    //WeightHandle weight;
    //float timer;

    public override void Start()
    {
        if (skin != null)
        {
            PlaybackNodes.Add(STATE.IDLE, new PlaybackNode(skin.model, Idle, true));
            PlaybackNodes.Add(STATE.WALKING, new PlaybackNode(skin.model, Walking, true));
            PlaybackNodes.Add(STATE.STRAFING_LEFT, new PlaybackNode(skin.model, StrafingLeft, true));
            PlaybackNodes.Add(STATE.STRAFING_RIGHT, new PlaybackNode(skin.model, StrafingRight, true));
            PlaybackNodes.Add(STATE.BACKWARDS, new PlaybackNode(skin.model, Backwards, true));
            PlaybackNodes.Add(STATE.RUNNING, new PlaybackNode(skin.model, Running, true));
            PlaybackNodes.Add(STATE.TURNING_LEFT, new PlaybackNode(skin.model, TurningLeft, true));
            PlaybackNodes.Add(STATE.TURNING_RIGHT, new PlaybackNode(skin.model, TurningRight, true));
            PlaybackNodes.Add(STATE.THROWING, new PlaybackNode(skin.model, Throwing, true));
            PlaybackNodes.Add(STATE.DIVING, new PlaybackNode(skin.model, Diving, true));

            // IdleHandle = IdleNode.getTimeHandle();
            // RunningHandle = RunningNode.getTimeHandle();

            MasterBlendNode = new BlendNode(skin.model);
            foreach (var node in PlaybackNodes)
                MasterBlendNode.appendNode(node.Value);

            // Do last
            MasterWeightHandle = MasterBlendNode.generateWeightHandle();
            MasterWeightHandle.setWeight(0, new WeightTripple(1f));
            skin.setBlendTreeNode(MasterBlendNode);
            //IdleHandle.Play();
        }
    }

    public void SetAnimations(Dictionary<STATE, float> weights)
    {
        for (uint i = 0; i < PlaybackNodes.Count; ++i)
        {
            float weight = 0;
            foreach (var stateWeight in weights)
            {
                if ((STATE)i == stateWeight.Key)
                {
                    weight = stateWeight.Value;
                    break;
                }
            }
            MasterWeightHandle.setWeight(i, new WeightTripple(weight));
        }
    }

}