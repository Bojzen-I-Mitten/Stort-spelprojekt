﻿using System;
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
                    BlendNode newBlendNode = new BlendNode(Skin.model);
                    for(int i = 0; i < state.Value.Count; ++i)
                    {
                        if(state.Key == ChadControls.STATE.THROWING && i == 1)
                            newBlendNode.appendNode(state.Value[i].Animation, false);
                        else
                            newBlendNode.appendNode(state.Value[i].Animation, true);
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
                if (State != ChadControls.STATE.THROWING) // set throwing weights from chadControls instead
                {
                    AnimationNode node = Animations[State][(int)i];
                    WeightHandles[State].setWeight(i, node.GetWeight(Direction));
                }
                else if (!_Throwing && i == 1) // shouldn't be needed, but it is, rewrite
                    WeightHandles[State].setWeight(i, 0);
            }
        }
    }

    public void SetAnimationWeight(uint index, float weight)
    {
        if (index == 1 && weight == 1)
        {
            _Throwing = true;
            BlendNodes[State].ResetPlayback();
        }
        else
            _Throwing = false;

        // Debug.Log("Manually setting weight of animation numer: " + index + " to: "+ weight);
        WeightHandles[State].setWeight(index, weight);
    }
} 