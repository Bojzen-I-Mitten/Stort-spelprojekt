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
        private List<FeetIK> m_IK;
        private PositionConstraint m_point;
        protected RenderSkinnedComponent m_rC;
        uint m_hipBone;

        public string HipBone { get; set; }
        public float Distance { get { return m_point.Distance; } set { m_point.Distance = value; } }

        public HipAdjustment()
            : base()
        {
            m_point = new PositionConstraint();
        }


        public override void OnAwake()
        {
            m_rC = gameObject.GetComponent<RenderSkinnedComponent>();
            m_IK = gameObject.GetComponents<FeetIK>();
            if (m_IK.Count == 0)
                throw new InvalidOperationException("Missing FeetIK scripts");
            uint index;
            if (m_rC.FetchBoneIndex(HipBone, out index))
            {
                m_hipBone = index;
            }
            else
                m_hipBone = 0;
        }

        public override void OnEnable()
        {
            base.OnEnable();
        }

        public override void Start()
        {
            base.Start();
        }

        public override void OnDisable()
        {
            base.OnDisable();
        }

        public override void OnDestroy()
        {
            base.OnDestroy();
        }

        public override void Update()
        {
            Vector3 avg = Vector3.Zero;
            foreach(var p in m_IK)
            {
                avg += p.IKTarget;
            }
            avg /= m_IK.Count;

            m_point.Position = avg;
        }
    }
}
