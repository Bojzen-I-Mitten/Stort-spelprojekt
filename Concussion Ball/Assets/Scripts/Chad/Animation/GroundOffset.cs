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


    private int maskGround;
    private uint boneIndex;
    private RenderSkinnedComponent rC;
    private Vector3[] Points = new Vector3[4];
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
        // Cast ray: Forward
        Vector3[] dir = 
        {
            m.Translation + m.Forward * OffsetForward,
            m.Translation + m.Backward * OffsetForward,
            m.Translation + m.Right * OffsetSide,
            m.Translation + m.Left * OffsetSide
        };
        for (int i = 0; i < 4; i++)
        {
            if (!Physics.Raycast(dir[i], Vector3.Down, out res, 0.3f, maskGround))
                Points[i] = m.Translation;
            else
                Points[i] = res.point;
        }

        if(!Utility.PlaneFromPoints(Points, out center, out normal))
        {
            center = m.Translation;
            normal = m.Up;
        }
    }

    public override void OnDrawGizmos()
    {
        Gizmos.SetColor(Color.Red);
        for (int i = 0; i < 4; i++)
            Gizmos.DrawRay(Points[i], Vector3.Down, OffsetForward);
        Gizmos.SetColor(Color.Green);
        Gizmos.DrawRay(center, normal, OffsetForward);
        //Gizmos.DrawBoundingBox()
    }

}

