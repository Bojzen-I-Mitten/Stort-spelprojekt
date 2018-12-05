using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.ComponentModel;
using ThomasEngine;
using ThomasEngine.Script;

public class GroundOffset : ScriptComponent
{
    
    public string BoneName { get; set; }                // Name of the bone ray's are traced from
    public float OffsetForward { get; set; } = 0.2f;    // Sample offsets along foot forward
    public float OffsetSide { get; set; } = 0.1f;       // Sample offset on foot side
    public float OffsetCast { get; set; } = 0.3f;       // Raycast offset along Y
    public float RayCastDistance { get; set; } = 0.3f;  // Maximum raytrace distance
    public Vector3 OrientAngle { get; set; }            // Orienatation around each axis

    private int m_maskGround;                   // Mask index for tracing ground
    protected uint m_traceBoneIndex;            // Index for raytraced bone
    protected RenderSkinnedComponent m_rC;      // Render component used as animation src
    const int MAX_SAMPLES = 5;                  // Number of samples (hardcoded)
    /* Trace information
     */
    private int m_foundSamples = 0;
    private Vector3[] Points = new Vector3[MAX_SAMPLES];
    private Vector3[] Src_Points = new Vector3[MAX_SAMPLES];
    protected bool m_sampleSuccess;
    /* Traced params
    */
    private Vector3 m_center, m_normal, m_forward;

    /* Access to trace result
    */

    [Browsable(false)]
    [Newtonsoft.Json.JsonIgnore]
    public Vector3 Center { get { return m_center; } }
    [Browsable(false)]
    [Newtonsoft.Json.JsonIgnore]
    public Vector3 Forward { get { return m_forward; } }
    [Browsable(false)]
    [Newtonsoft.Json.JsonIgnore]
    public Vector3 Z { get { return -m_forward; } }
    [Browsable(false)]
    [Newtonsoft.Json.JsonIgnore]
    public Vector3 Up { get { return m_normal; } }
    [Browsable(false)]
    [Newtonsoft.Json.JsonIgnore]
    public Vector3 Right { get { return Vector3.Cross(m_normal, Z); } }
    [Browsable(false)]
    [Newtonsoft.Json.JsonIgnore]
    public Quaternion Orient { get { return MathEngine.CreateRotation(Right, m_normal, Z); } }
    [Browsable(false)]
    [Newtonsoft.Json.JsonIgnore]
    public int FoundSamples { get { return m_foundSamples; } }

    public GroundOffset() 
        : base()
    {
    }

    public override void OnAwake()
    {
        m_maskGround = Physics.GetCollisionGroupBit("Ground");
    }

    public override void OnEnable()
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

    public override void Start()
    {
    }

    public override void OnDisable()
    {
    }

    public override void OnDestroy()
    {
    }
    

    private Vector3 Average(Vector3[] points, int num)
    {
        Vector3 sum = Vector3.Zero;
        for(int i = 0; i < num; i++)
        {
            sum += points[i];
        }
        return sum / num;
    }
    

    public override void Update()
    {
        Matrix m = m_rC.GetLocalBoneMatrix(m_traceBoneIndex);
        Matrix world = gameObject.transform.world;
        m = m * world;
        // Result output
        RaycastHit res;
        // Cast rays from 'foot':
        int s = 0;
        Src_Points[s++] = m.Translation + Vector3.Up * OffsetCast;                                // Center
        Src_Points[s++] = m.Translation + m.Up * OffsetForward + Vector3.Up * OffsetCast;         // Toe
        Src_Points[s++] = m.Translation + m.Down * OffsetForward + Vector3.Up * OffsetCast;       // Heel
        Src_Points[s++] = m.Translation + m.Right * OffsetSide + Vector3.Up * OffsetCast;         // Left side
        Src_Points[s++] = m.Translation + m.Left * OffsetSide + Vector3.Up * OffsetCast;          // Right side
        m_foundSamples = 0;
        for (int i = 0; i < MAX_SAMPLES; i++)
        {
            if (Physics.Raycast(Src_Points[i], Vector3.Down, out res, RayCastDistance + OffsetCast, m_maskGround))
                Points[m_foundSamples++] = res.point;
        }

        m_sampleSuccess = Utility.PlaneFromPointsY(Points, m_foundSamples, out m_center, out m_normal);
        if (!m_sampleSuccess)
        {
            m_center = m.Translation;
            m_normal = m.Forward;
            m_normal.Normalize();
        }
        m_forward = m.Up;
        m_forward = m_forward - Vector3.Dot(m_normal, m_forward) * m_normal;
        m_forward.Normalize();
        m_normal.Normalize();
        // Orient in to local space
        Matrix inv = Matrix.Invert(world);
        m_center = Vector3.Transform(m_center, inv);
        // Custom orientation of basis
        inv = MathEngine.CreateRotationXYZ(Right, Up, Z, OrientAngle) * inv;
        m_forward = Vector3.TransformNormal(m_forward, inv);
        m_normal = Vector3.TransformNormal(m_normal, inv);
    }

    public override void OnDrawGizmos()
    {
        //Gizmos.SetColor(Color.Green);
        //Gizmos.DrawRay(center, normal, OffsetCast + OffsetSide);
        //Gizmos.SetColor(Color.Blue);
        //Gizmos.DrawRay(center, forward, OffsetForward);
        Quaternion q = Orient;
        Gizmos.DrawQuat(ref q, ref m_center, 0.05f);

        Matrix orientInv = MathEngine.CreateRotationXYZ(Right, Up, Z, -OrientAngle);
        Vector3 forward = Vector3.TransformNormal(this.m_forward, orientInv);
        Vector3 normal = Vector3.TransformNormal(this.m_normal, orientInv);

        Vector3 p0 = m_center + forward * OffsetForward + Right * OffsetSide;
        Vector3 p1 = m_center + forward * OffsetForward - Right * OffsetSide;
        Vector3 p2 = m_center - forward * OffsetForward - Right * OffsetSide;
        Vector3 p3 = m_center - forward * OffsetForward + Right * OffsetSide;
        Gizmos.DrawLine(p0, p1);
        Gizmos.DrawLine(p1, p2);
        Gizmos.DrawLine(p2, p3);
        Gizmos.DrawLine(p3, p0);
        //Gizmos.DrawBoundingBox()

        Gizmos.SetMatrix(Matrix.Identity);  // In world space
        Gizmos.SetColor(Color.Red);
        for (int i = 0; i < m_foundSamples; i++)
            Gizmos.DrawLine(Src_Points[i], Points[i]);
    }

}

