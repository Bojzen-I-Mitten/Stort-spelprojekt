//#define PER_FRAME_HAT_SYNC

using ThomasEngine;

public class Hatter : ScriptComponent
{
    BoneSyncComponent boneSyncer;
    RenderComponent HatRenderer;

    HatManager.Hat hat;

    public override void OnAwake()
    {
        GameObject HatObject = new GameObject("Hat");
        HatObject.transform.SetParent(transform);
        HatRenderer = HatObject.AddComponent<RenderComponent>();

        boneSyncer = HatObject.AddComponent<BoneSyncComponent>();
        boneSyncer.AnimatedObject = gameObject;
        boneSyncer.Mode = BoneSyncComponent.BoneSyncMode.Local;
        boneSyncer.BoneName = "mixamorig:Head";
    }

    public bool SetHat(int hatIndex)
    {
        hat = HatManager.Instance?.GetHat(hatIndex);
        if (hat != null)
        {
            HatRenderer.model = hat.hatModel;
            HatRenderer.material = hat.material;
            if(HatRenderer.materials.Length == 2)
                HatRenderer.SetMaterial(1, hat.material);
            HatRenderer.transform.eulerAngles = hat.rotation;
            boneSyncer.Offset = Matrix.CreateScale(hat.scale) * Matrix.CreateWorld(hat.position, HatRenderer.transform.forward, HatRenderer.transform.up);
            return true;
        }
        else
            return false;
    }

    public override void Update()
    {
#if (PER_FRAME_HAT_SYNC)
        if (hat != null)
        {

            HatRenderer.transform.eulerAngles = hat.rotation;
            boneSyncer.Offset = Matrix.CreateScale(hat.scale) * Matrix.CreateWorld(hat.position, HatRenderer.transform.forward, HatRenderer.transform.up);
        }

#endif
    }

    public int GetHatCount()
    {
        return HatManager.Instance.Hats.Count;
    }
}
