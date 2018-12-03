using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ThomasEngine;
using ThomasEngine.Script;

public class GroundOffset : ScriptComponent
{
    
    public string BoneName { get; set; }
    public float OffsetForward { get; set; } = 0.1f;    // Sample offsets along foot forward
    public float OffsetSide { get; set; } = 0.1f;       // Sample offset on foot side
    public float OffsetCast { get; set; } = 0.1f;       // Raycast offset along Y


    private int maskGround;
    private uint boneIndex;
    private RenderSkinnedComponent rC;
    const int MAX_SAMPLES = 5;
    int FoundSamples = 0;
    private Vector3[] Points = new Vector3[MAX_SAMPLES];
    private Vector3[] Src_Points = new Vector3[MAX_SAMPLES];
    private Vector3 center, normal;

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
            boneIndex = index;
        }
        else
            boneIndex = 0;
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
        Matrix m = rC.GetBoneMatrix(boneIndex);
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
        }
    }

    public override void OnDrawGizmos()
    {
        Gizmos.SetMatrix(Matrix.Identity);
        Gizmos.SetColor(Color.Red);
        for (int i = 0; i < FoundSamples; i++)
            Gizmos.DrawLine(Src_Points[i], Points[i]);
        Gizmos.SetColor(Color.Green);
        Gizmos.DrawRay(center, normal, OffsetCast + OffsetSide);
        //Gizmos.DrawBoundingBox()
    }

}

