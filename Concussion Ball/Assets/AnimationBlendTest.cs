using ThomasEngine;
using ThomasEngine.Script;

public class AnimationBlendTest : ScriptComponent
{

    public Animation fromAnim { get; set; }
    public Animation toAnim { get; set; }
    RenderSkinnedComponent skinn;
    public override void Start()
    {
        skinn = gameObject.GetComponent<RenderSkinnedComponent>();

        BlendNode node = new BlendNode(skinn.model);
        node.appendNode(fromAnim, false);
        node.appendNode(toAnim, false);
        node.appendNode(fromAnim, false);
        node.appendNode(toAnim, false);
        node.appendNode(fromAnim, false);
        node.appendNode(toAnim, false);
        node.generateLinearMixer(2.0f);
        skinn.setBlendTreeNode(node);
    }

    public override void Update()
    {
        
    }
}
