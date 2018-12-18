using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ThomasEngine;
using ThomasEngine.Script;

namespace Concussion_Ball.Assets.Scripts
{
    public class HipAdjustment : ScriptComponent
    {
        private PositionConstraint m_point;
        protected RenderSkinnedComponent m_rC;
        private List<FeetIK> m_IK;
        public string HipBone { get; set; }
        public float Distance { get; set; }
        public float ClampOffset { get; set; } = 0.2f;
        public float PoleDistance { get; set; } = 1.0f;
        public float PoleForwardBias { get; set; } = 2f;
        public Vector3 PoleOffset { get; set; }

        private uint hipBoneIndex;
        private Vector3 targetPoint;
        private FeetIK feetA;
        private FeetIK feetB;
        private GameObject poleA;
        private GameObject poleB;

        public HipAdjustment()
            : base()
        {
            m_point = new PositionConstraint();
        }

        public override void OnAwake()
        {
            m_rC = gameObject.GetComponent<RenderSkinnedComponent>();
            m_IK = gameObject.GetComponents<FeetIK>();
            if (m_IK.Count < 2)
                throw new InvalidOperationException("Missing FeetIK scripts");
            feetA = m_IK[0];
            feetB = m_IK[1];
            // Create pole targets
            poleA = new GameObject("PoleA");
            poleB = new GameObject("PoleB");
            poleA.transform.SetParent(gameObject.transform);
            poleB.transform.SetParent(gameObject.transform);
            feetA.PoleTarget = poleA;
            feetB.PoleTarget = poleB;
        }

        public override void OnEnable()
        {
            base.OnEnable();
            uint index;
            if (!m_rC.FetchBoneIndex(HipBone, out index))
                index = 0;
            hipBoneIndex = index;
            m_point.apply(m_rC, index);
        }

        public override void Start()
        {
            base.Start();
        }

        public override void OnDisable()
        {
            m_point.disable();
        }

        public override void OnDestroy()
        {
            base.OnDestroy();
        }
        private static int printTimer = 0;

        public override void Update()
        {
            m_point.Distance = Math.Max(Distance, 0.3f);
            /* Calculate hip adjustment
            */
            Vector3 avg = Vector3.Zero;
            float minY = float.MaxValue;
            float count = 0.0f;
            foreach(var p in m_IK)
            {
                if (!p.IsActive)
                    continue;
                avg += p.IKTarget;
                minY = Math.Min(minY, p.IKTarget.y);
                count++;
            }
            Vector3 target = Vector3.Zero;
            if (count > 0.0f)
            {
                // Set height to lowest point
                avg /= m_IK.Count == 0.0f ? 1.0f : count;
                target.y = MathHelper.Clamp(minY, -ClampOffset, ClampOffset);
            }
            // Smooth target traversal
            float minimalDistanceTraversal = 0.05f;
            Vector3 diff = target - targetPoint;
            float distance = diff.Length();
            float targetSmooth = Math.Max(0.0f, distance - minimalDistanceTraversal) / (Distance * 2.0f);
            targetSmooth = Math.Min(1.0f - minimalDistanceTraversal, targetSmooth * targetSmooth) + minimalDistanceTraversal;
            // Track prev. target
            targetPoint = targetPoint + diff * targetSmooth;
            // Apply new target
            m_point.Position = targetPoint;


            /* Calculate pole targets
             */
             // Foot A
            Matrix hipRoot = m_rC.GetLocalBoneMatrix(feetA.IK.RootBoneIndex);
            Vector3 v = Vector3.Normalize(hipRoot.Forward) * PoleForwardBias +  feetA.LocalBoneForward;
            v.y = 0; v.Normalize();     // Project on xz
            v += PoleOffset;
            //v.z -= ForwardPoleBias;
            //v.Normalize();
            if (Vector3.Dot(Vector3.Forward, v) < 0.0f)
                v.z = -v.z;
            poleA.transform.localPosition = hipRoot.Translation + v * PoleDistance;
            // Foot B
            hipRoot = m_rC.GetLocalBoneMatrix(feetB.IK.RootBoneIndex);
            v = Vector3.Normalize(hipRoot.Forward) * PoleForwardBias + feetB.LocalBoneForward;
            v.y = 0; v.Normalize();     // Project on xz
            v += PoleOffset;
            //v.z -= ForwardPoleBias;
            //v.Normalize();
            if (Vector3.Dot(Vector3.Forward, v) < 0.0f)
                v.z = -v.z;
            poleB.transform.localPosition = hipRoot.Translation + v * PoleDistance;
        }
    }
}
