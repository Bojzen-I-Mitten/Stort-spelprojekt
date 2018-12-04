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
        //[Browsable(false)]
        //[Newtonsoft.Json.JsonIgnore]

        //public string IKTargetBone { get; set; }    // Name of the bone ray's are traced from
        //public string IKResolveBoneName { get; set; }
        //private uint ikBoneIndex;                   // Index for raytraced bone
        private IK_FABRIK_Constraint IK { get; set; }


        public FeetIK()
            : base()
        {
            IK = new IK_FABRIK_Constraint(4);
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
            IK.apply(rC, traceBoneIndex);
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

        public override void Update()
        {
            base.Update();
            IK.Target = Center;
            IK.Orientation = Orient;
        }
    }
}
