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
    public uint ChainLength { get; set; } 
    protected uint m_traceBoneIndex;            // Index for lookAt bone
    protected RenderSkinnedComponent m_rC;      // Render component used as animation src
    IK_FABRIK_Constraint m_iK;

    public IKConstraint()
        : base()
    {
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
        m_iK = new IK_FABRIK_Constraint(ChainLength);
        m_iK.apply(gameObject, m_traceBoneIndex);
        m_iK.Weight = 1.0f;
        m_iK.OrientationWeight = 1.0f;
    }

    public override void Start()
    {
        base.Start();
    }

    public override void OnDisable()
    {
        m_iK.disable();
    }

    public override void OnDestroy()
    {
        base.OnDestroy();
    }

    public override void Update()
    {
        if (Target != null)
        {
            m_iK.Target = Target.transform.localPosition;
            m_iK.Orientation = Target.transform.localRotation;
        }
    }
}
