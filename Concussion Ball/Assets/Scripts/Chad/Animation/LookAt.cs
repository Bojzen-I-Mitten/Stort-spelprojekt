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
public class LookAt : ScriptComponent
{
    public string BoneName { get; set; }        // Name of the bone constraint is added to
    public GameObject Target { get; set; }      // LookAt target
    protected uint m_traceBoneIndex;            // Index for lookAt bone
    protected RenderSkinnedComponent m_rC;      // Render component used as animation src
    LookAtConstraint m_lK;

    public LookAt()
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
        m_lK = new LookAtConstraint(LookAtConstraint.AxisConstraint.AxisXYZ);
        m_lK.apply(gameObject, m_traceBoneIndex);
        m_lK.Weight = 1.0f;
    }

    public override void Start()
    {
        base.Start();
    }

    public override void OnDisable()
    {
        m_lK.disable();
    }

    public override void OnDestroy()
    {
        base.OnDestroy();
    }

    public override void Update()
    {
        if(Target != null)
            m_lK.Target = Target.transform.localPosition;
    }
}
