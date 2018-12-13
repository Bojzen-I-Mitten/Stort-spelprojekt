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
        private enum State
        {
            Enabled,
            Disabled
        }
        private static readonly uint NUM_LINK = 4;
        //[Browsable(false)]
        //[Newtonsoft.Json.JsonIgnore]

        //public string IKTargetBone { get; set; }    // Name of the bone ray's are traced from
        //public string IKResolveBoneName { get; set; }
        //private uint ikBoneIndex;                   // Index for raytraced bone
        private IK_FABRIK_Constraint IK { get; set; }
        private State state = State.Disabled;
        private float ikTimeRemaining = 0;
        private float ikTimeBlending = 0;
        private float ikFromWeight = 0;
        private float ikTargetWeight = 0;
        private float ikOrientWeight = 0;
        public float IKBlendInTime { get; set; } = 0.33f;
        public float IKBlendOutTime { get; set; } = 0.33f;
        public float MaxDistanceOffset { get; set; } = 0.2f;            // Offset from bone -> target IK is blended in
        public float MaxIKChainDistanceOffset { get; set; } = 0.2f;     // Offset from max chain length IK is blended in
        public Vector3 BoneTargetOffset { get; set; }                   // Target offset after src is found
        public float BendAngleMinFadeOut { get; set; } = 20;            // Degree angle for which foot will be considered 'not placed down'
        private Vector3 ikTarget;                                       // Target stored between frames
        private float ikWeight = 0.0f;
        
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
            IK.Weight = 1.0f; ikTargetWeight = 0.0f; ikFromWeight = 0.0f;
            IK.OrientationWeight = 1.0f; ikOrientWeight = 0.0f;
            ikOrientWeight = 0.0f;
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
        private float blendInFactor()
        {
            ikTimeRemaining = Math.Max(0.0f, ikTimeRemaining  - Time.DeltaTime);
            return MathHelper.Lerp(ikTargetWeight, ikFromWeight, ikTimeRemaining / Math.Max(0.01f, ikTimeBlending));   // Lerp, if values are equal minimal diff. has no effect!
        }

        protected override Matrix fetchTransform()
        {
            if (IK.Weight < 0.001f)
                return base.fetchTransform();
            else
                return IK.LastPoseTransform;
        }

        public override void Update()
        {
            base.Update();
            Matrix inv = Matrix.Invert(gameObject.transform.world);
            Quaternion orient = Orient * MathEngine.ExtractRotation(inv);
            Vector3 target = Vector3.Transform(Target, inv);
            target += Vector3.Transform(BoneTargetOffset, orient);
            ikTarget = target;

            Matrix mFoot = fetchTransform();
            float angle = Math.Abs(MathHelper.ToDegrees((float)Math.Acos(Vector3.Dot(mFoot.Up, Vector3.Up))) - 90);
            
            float distanceToRoot = CalcDistance((int)IK.RootBoneIndex);
            if (!m_sampleSuccess ||                                                 // Verify enough samples found
                Distance > MaxDistanceOffset ||                                     // Verify target point is close enough
                distanceToRoot > IK.BoneChainLength - MaxIKChainDistanceOffset ||   // Verify target point is close enough
                angle > BendAngleMinFadeOut)
            {
                if (state == State.Enabled)
                {
                    // Fade out
                    ikFromWeight = ikWeight;
                    ikTargetWeight = 0;
                    ikOrientWeight = 0;
                    ikTimeRemaining = IKBlendOutTime;
                    ikTimeBlending = IKBlendOutTime;
                    state = State.Disabled;
                }
            }
            else
            {
                if (state == State.Disabled)
                {
                    // Fade out
                    ikFromWeight = ikWeight;
                    ikTargetWeight = 1;
                    ikOrientWeight = 1;
                    ikTimeRemaining = IKBlendInTime;
                    ikTimeBlending = IKBlendInTime;
                    state = State.Enabled;
                }
            }

            Vector3 s, t;
            Quaternion r;
            if(mFoot.Decompose(out s, out r, out t)) { }

            ikWeight = blendInFactor();
            IK.Target = Vector3.Lerp(t, target, ikWeight);
            IK.Orientation = Quaternion.Slerp(r, orient, ikWeight);
            //IK.Weight = ikWeight;// blendInFactor(ikWeight, ikTargetWeight, IKBlendFactor);
            //IK.OrientationWeight = ikWeight; // blendInFactor(IK.OrientationWeight, ikOrientWeight, IKBlendFactor);

            if (tick++ > 5)
            {
                tick = 0;
                Debug.Log(ikWeight);
            }
        }
        private int tick = 0;

        public override void OnDrawGizmos()
        {
            base.OnDrawGizmos();
            Gizmos.SetMatrix(gameObject.transform.world);
            Gizmos.SetColor(Color.Yellow);
            Gizmos.DrawRing(ikTarget, Vector3.Up, 0.02f);
        }
    }
}
