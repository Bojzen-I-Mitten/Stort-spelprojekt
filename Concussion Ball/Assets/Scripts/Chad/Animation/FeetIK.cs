using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.ComponentModel;
using ThomasEngine;
using ThomasEngine.Script;


namespace Concussion_Ball.Assets.Scripts
{
    public class FeetIK : GroundOffset
    {
        private static readonly uint NUM_LINK = 4;
        //[Browsable(false)]
        //[Newtonsoft.Json.JsonIgnore]

        //public string IKTargetBone { get; set; }    // Name of the bone ray's are traced from
        //public string IKResolveBoneName { get; set; }
        //private uint ikBoneIndex;                   // Index for raytraced bone
        private IK_FABRIK_Constraint IK { get; set; }
        private float ikTargetWeight = 0;
        private float ikOrientWeight = 0;
        public float IKBlendFactor { get; set; } = 0.5f;        // Factor determining how fast IK is blended in when activated
        public float MaxDistanceOffset { get; set; } = 0.2f;    // Offset from max chain length IK is blended in
        
        public IK_FABRIK_Constraint.JointParams[] Joints
        {
            get { return IK.Joints; }
            set
            {
                IK.Joints = value;
            }
        }

        //public IK_FABRIK_Constraint.JointParams JointRoot { get { return arr[0]; } set { arr[0] = value; IK.setJoint(0, ref value); } }
        //public IK_FABRIK_Constraint.JointParams JointLeg { get { return arr[1]; } set { arr[1] = value; IK.setJoint(1, ref value); } }
        //public IK_FABRIK_Constraint.JointParams JointAnkle { get { return arr[2]; } set { arr[2] = value; IK.setJoint(2, ref value); } }
        //public IK_FABRIK_Constraint.JointParams JointFoot { get { return arr[3]; } set { arr[3] = value; IK.setJoint(3, ref value); } }

        public FeetIK()
            : base()
        {
            IK = new IK_FABRIK_Constraint(NUM_LINK);
        }

        public override void OnAwake()
        {
            base.OnAwake();
        }

        public override void OnEnable()
        {
            base.OnEnable();
            //uint index;
            //if (rC.FetchBoneIndex(IKTargetBone, out index))
            //{
            //    ikBoneIndex = index;
            //}
            //else
            //    ikBoneIndex = 0;
            IK.apply(m_rC, m_traceBoneIndex);
            IK.Weight = ikTargetWeight;
            IK.OrientationWeight = ikOrientWeight;
        }

        public override void Start()
        {
            base.Start();
        }

        public override void OnDisable()
        {
            base.OnDisable();
            IK.disable();
        }

        public override void OnDestroy()
        {
            base.OnDestroy();
        }

        private static float blendInFactor(float weight, float targetWeight, float blendFactor)
        {
            float diff = Math.Abs(targetWeight - weight) * Time.DeltaTime * blendFactor;    // Find diff. adjusted over time and mixed with blend in factor
            diff = MathHelper.Clamp(diff, 0.05f, 1.0f);                                     // Clamp to [0.1,1], assert not large delta/blend factor and minimal mix in
            weight = MathHelper.Lerp(weight, targetWeight, diff);                           // Lerp, if values are equal minimal diff. has no effect!
            return weight;
        }

        public override void Update()
        {
            base.Update();
            
            if (!m_sampleSuccess ||                                   // Verify enough samples found
                Distance > IK.BoneChainLength - MaxDistanceOffset)    // Verify target point is close enough
            {
                ikTargetWeight = 0;
                ikOrientWeight = 0;
            }
            else
            {
                ikTargetWeight = 1;
                ikOrientWeight = 1;
                if (Distance < 0.05f)
                    LockSearch = true;
            }

            IK.Target = Target;
            IK.Orientation = Orient;
            IK.Weight = blendInFactor(IK.Weight, ikTargetWeight, IKBlendFactor);
            IK.OrientationWeight = blendInFactor(IK.OrientationWeight, ikOrientWeight, IKBlendFactor);
        }
    }
}
