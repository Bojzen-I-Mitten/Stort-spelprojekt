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
    public Vector3 OrientAngle { get; set; }            // Orienatation around each axis

    private int maskGround;                 // Mask index for tracing ground
    protected uint traceBoneIndex;            // Index for raytraced bone
    protected RenderSkinnedComponent rC;    // Render component used as animation src
    const int MAX_SAMPLES = 5;              // Number of samples (hardcoded)
    /* Trace information
     */
    int FoundSamples = 0;
    private Vector3[] Points = new Vector3[MAX_SAMPLES];
    private Vector3[] Src_Points = new Vector3[MAX_SAMPLES];
    /* Traced params
    */
    private Vector3 center, normal, forward;

    /* Access to trace result
    */

    [Browsable(false)]
    [Newtonsoft.Json.JsonIgnore]
    public Vector3 Center { get { return center; } }
    [Browsable(false)]
    [Newtonsoft.Json.JsonIgnore]
    public Vector3 Forward { get { return forward; } }
    [Browsable(false)]
    [Newtonsoft.Json.JsonIgnore]
    public Vector3 Z { get { return -forward; } }
    [Browsable(false)]
    [Newtonsoft.Json.JsonIgnore]
    public Vector3 Up { get { return normal; } }
    [Browsable(false)]
    [Newtonsoft.Json.JsonIgnore]
    public Vector3 Right { get { return Vector3.Cross(normal, Z); } }
    [Browsable(false)]
    [Newtonsoft.Json.JsonIgnore]
    public Quaternion Orient { get { return MathEngine.CreateRotation(Right, normal, Z); } }

    public GroundOffset() 
        : base()
    {
    }

    public override void OnAwake()
    {
        maskGround = Physics.GetCollisionGroupBit("Ground");
    }

    public override void OnEnable()
    {
        rC = gameObject.GetComponent<RenderSkinnedComponent>();
        uint index;
        if (rC.FetchBoneIndex(BoneName, out index))
        {
            traceBoneIndex = index;
        }
        else
            traceBoneIndex = 0;
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
        Matrix m = rC.GetLocalBoneMatrix(traceBoneIndex);
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
        FoundSamples = 0;
        for (int i = 0; i < MAX_SAMPLES; i++)
        {
            if (Physics.Raycast(Src_Points[i], Vector3.Down, out res, 0.3f + OffsetCast, maskGround))
                Points[FoundSamples++] = res.point;
        }

        if(!Utility.PlaneFromPointsY(Points, FoundSamples, out center, out normal))
        {
            center = m.Translation;
            normal = m.Forward;
            normal.Normalize();
        }
        forward = m.Up;
        forward = forward - Vector3.Dot(normal, forward) * normal;
        forward.Normalize();
        normal.Normalize();
        // Orient in to local space
        Matrix inv = Matrix.Invert(world);
        center = Vector3.Transform(center, inv);
        // Custom orientation of basis
        inv = MathEngine.CreateRotationXYZ(Right, Up, Z, OrientAngle) * inv;
        forward = Vector3.TransformNormal(forward, inv);
        normal = Vector3.TransformNormal(normal, inv);
    }

    public override void OnDrawGizmos()
    {
        //Gizmos.SetColor(Color.Green);
        //Gizmos.DrawRay(center, normal, OffsetCast + OffsetSide);
        //Gizmos.SetColor(Color.Blue);
        //Gizmos.DrawRay(center, forward, OffsetForward);
        Quaternion q = Orient;
        Gizmos.DrawQuat(ref q, ref center, 0.05f);

        Matrix orientInv = MathEngine.CreateRotationXYZ(Right, Up, Z, -OrientAngle);
        Vector3 forward = Vector3.TransformNormal(this.forward, orientInv);
        Vector3 normal = Vector3.TransformNormal(this.normal, orientInv);

        Vector3 p0 = center + forward * OffsetForward + Right * OffsetSide;
        Vector3 p1 = center + forward * OffsetForward - Right * OffsetSide;
        Vector3 p2 = center - forward * OffsetForward - Right * OffsetSide;
        Vector3 p3 = center - forward * OffsetForward + Right * OffsetSide;
        Gizmos.DrawLine(p0, p1);
        Gizmos.DrawLine(p1, p2);
        Gizmos.DrawLine(p2, p3);
        Gizmos.DrawLine(p3, p0);
        //Gizmos.DrawBoundingBox()

        Gizmos.SetMatrix(Matrix.Identity);  // In world space
        Gizmos.SetColor(Color.Red);
        for (int i = 0; i < FoundSamples; i++)
            Gizmos.DrawLine(Src_Points[i], Points[i]);
    }

}

