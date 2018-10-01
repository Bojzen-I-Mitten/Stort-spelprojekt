using ThomasEngine;
using ThomasEngine.Script;

public class AnimationBlendTest : ScriptComponent
{
    BlendNode root;
    RenderSkinnedComponent skinn;

    public Animation fromAnim { get; set; }
    public Animation toAnim { get; set; }
    public override void Start()
    {
        skinn = gameObject.GetComponent<RenderSkinnedComponent>();

        root = new BlendNode(skinn.model);
        root.appendNode(fromAnim, false);
        root.appendNode(toAnim, false);
        root.generateLinearMixer(2.0f);
        skinn.setBlendTreeNode(root);
    }

    public override void Update()
    {
        
    }
}
