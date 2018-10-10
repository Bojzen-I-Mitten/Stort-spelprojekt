using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ThomasEngine;

namespace ThomasEditor
{
    public class Ragdoll : ScriptComponent
    {
        public Vector3 Position { get; set; }
        public string Spine { get; set; }
        public string Hips { get; set; }
        public string Neck { get; set; }
        public bool AllobjectKinectic { get; set; }
        public override void Start()
        {
            RenderSkinnedComponent renderskinnedcomponent = gameObject.GetComponent<RenderSkinnedComponent>();
            uint boneindex = 0;

            //Hips
            GameObject G_Hips = new GameObject("Hips");
            G_Hips.transform.SetParent(gameObject.transform);
            BoneTransformComponent B_Hips = G_Hips.AddComponent<BoneTransformComponent>();
            B_Hips.BoneName = Hips;
            B_Hips.AnimatedObject = gameObject;
            renderskinnedcomponent.FetchBoneIndex(Utility.hash(Hips), out boneindex);
            G_Hips.transform.position = renderskinnedcomponent.GetBoneMatrix((int)boneindex).Translation;
            SphereCollider spherecolliderhips = G_Hips.AddComponent<SphereCollider>();
            spherecolliderhips.radius = 0.2f;
            Rigidbody rigidbodyhips = G_Hips.AddComponent<Rigidbody>();
            rigidbodyhips.IsKinematic = AllobjectKinectic;

            //Spine

            GameObject G_Spine = new GameObject("Spine");
            G_Spine.transform.SetParent(gameObject.transform);
            BoneTransformComponent B_Spine = G_Spine.AddComponent<BoneTransformComponent>();
            B_Spine.BoneName = Spine;
            B_Spine.AnimatedObject = gameObject;
            renderskinnedcomponent.FetchBoneIndex(Utility.hash(Spine), out boneindex);
            BoxCollider boxcolliderSpine = G_Spine.AddComponent<BoxCollider>();
            G_Spine.transform.position = renderskinnedcomponent.GetBoneMatrix((int)boneindex).Translation;
            boxcolliderSpine.center = calculatePosbetweenTwoSkeletons(Spine, Neck,renderskinnedcomponent); //renderskinnedcomponent.GetBoneMatrix((int)boneindex).Translation;
            boxcolliderSpine.size = new Vector3(0.2f,0.2f,0.2f);
            Rigidbody rigidbodySpine = G_Spine.AddComponent<Rigidbody>();
            rigidbodySpine.IsKinematic = AllobjectKinectic;


/*
            renderskinnedcomponent.FetchBoneIndex(Utility.hash(Spine),out boneindex);
            Vector3 bone1 = B_Hips.transform.localPosition;
            Vector3 bone2 = renderskinnedcomponent.GetBoneMatrix((int)boneindex).Translation;
            Vector3 up = bone2 - bone1;
            float length = up.Length();
            if (length != 0)
                up /= length; // Normalize
            Vector3 Pos = bone1 + up * (length * 0.5f);           
*/

        }

       Vector3 calculatePosbetweenTwoSkeletons(string BoneName1,string BoneName2, RenderSkinnedComponent renderskinnedcomponent)
        {
            uint boneindex = 0;
            renderskinnedcomponent.FetchBoneIndex(Utility.hash(BoneName1), out boneindex);
            Vector3 bone1 = renderskinnedcomponent.GetBoneMatrix((int)boneindex).Translation;
            renderskinnedcomponent.FetchBoneIndex(Utility.hash(BoneName2), out boneindex);
            Vector3 bone2 = renderskinnedcomponent.GetBoneMatrix((int)boneindex).Translation;
            Vector3 up = bone2 - bone1;
            float length = up.Length();
            if (length != 0)
                up /= length; // Normalize
            Vector3 Pos =  up * (length * 0.5f);
            return Pos;
        }

       
    void initSkeleton(RenderSkinnedComponent Model)
        {
         
        }
 
        public override void Update()
        {
        }
        public override void OnEnable()
        {

        }
        public override void Awake()
        {
        
        }
        public override void OnDisable()
        {
           
        }
    

    }
}
