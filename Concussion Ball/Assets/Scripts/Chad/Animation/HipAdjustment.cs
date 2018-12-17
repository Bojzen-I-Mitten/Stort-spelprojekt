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

        public HipAdjustment()
            : base()
        {
        }


        public override void OnAwake()
        {
            base.OnAwake();
            m_IK = gameObject.GetComponents<FeetIK>();
            if (m_IK.Count == 0)
                throw new InvalidOperationException("Missing FeetIK scripts");
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
            base.Update();
        }
    }
}
