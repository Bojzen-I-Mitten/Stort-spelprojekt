using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.ComponentModel;
using ThomasEngine;
using ThomasEngine.Script;

/* Look at constraint, allows bones to be rotated toward a target point
 */
public class IKConstraint : ScriptComponent
{
    public string BoneName { get; set; }        // Name of the bone constraint is added to
    public GameObject Target { get; set; }      // LookAt target
    public uint ChainLength
    {
        get { return IK.NumLinks; }
        set
        {
            if(enabled)
            {
                Debug.LogWarning("Chain length change not supported during runtime.");
                return;
            }
            var joint = IK.Joints;
            IK = new IK_FABRIK_Constraint(value);
            IK.Joints = joint;
        }
    }
    protected uint m_traceBoneIndex;            // Index for lookAt bone
    protected RenderSkinnedComponent m_rC;      // Render component used as animation src
    IK_FABRIK_Constraint IK;


    public IK_FABRIK_Constraint.JointParams[] Joints
    {
        get { return IK.Joints; }
        set
        {
            IK.Joints = value;
        }
    }

    public IKConstraint()
        : base()
    {
        IK = new IK_FABRIK_Constraint(0);
    }


    public override void OnAwake()
    {
        m_rC = gameObject.GetComponent<RenderSkinnedComponent>();
        uint index;
        if (m_rC.FetchBoneIndex(BoneName, out index))
        {
            m_traceBoneIndex = index;
        }
        else
            m_traceBoneIndex = 0;
    }

    public override void OnEnable()
    {
        IK.apply(gameObject, m_traceBoneIndex);
        IK.Weight = 1.0f;
        IK.OrientationWeight = 1.0f;
    }

    public override void Start()
    {
        base.Start();
    }

    public override void OnDisable()
    {
        IK.disable();
    }

    public override void OnDestroy()
    {
        base.OnDestroy();
    }

    public override void Update()
    {
        if (Target != null)
        {
            IK.Target = Target.transform.localPosition;
            IK.Orientation = Target.transform.localRotation;
        }
    }
}
