using ThomasEngine;


public class SkeletonSyncer : ScriptComponent
{
    public int bone { get; set; }
    public RenderSkinnedComponent skeleton {get;set;}
    public override void Start()
    {
        
    }

    public override void Update()
    {
        if (skeleton)
        {
            transform.world = skeleton.GetBoneMatrix(bone);
            transform.scale = new Vector3(1, 1, 1);
        }
            
    }
}
