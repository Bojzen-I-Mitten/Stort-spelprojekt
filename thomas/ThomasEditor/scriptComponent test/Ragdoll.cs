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
        public string Spine { get; set; }
        public string Hips { get; set; }
        public Vector3 displaydata { get; set; }
        public override void Start()
        {
          
           
            //Hips
            GameObject G_Hips = new GameObject("Hips");
            G_Hips.transform.SetParent(gameObject.transform);
            
            BoneTransformComponent B_Hips = G_Hips.AddComponent<BoneTransformComponent>();
            B_Hips.BoneName = Hips;
            B_Hips.AnimatedObject = gameObject;




            //Spine



            RenderSkinnedComponent renderskinnedcomponent =  gameObject.GetComponent<RenderSkinnedComponent>();
            uint boneindex=0;
            renderskinnedcomponent.FetchBoneIndex(Utility.hash(Spine),out boneindex);
            Vector3 bone1 = B_Hips.transform.localPosition;
            Vector3 bone2 = renderskinnedcomponent.GetBoneMatrix((int)boneindex).Translation;
            Vector3 up = bone2 - bone1;
            float length = up.Length();
            if (length != 0)
                up /= length; // Normalize
            Vector3 Pos = bone1 + up * (length * 0.5f);

            G_Hips.transform.position = Pos;
            G_Hips.AddComponent<BoxCollider>();
            G_Hips.AddComponent<Rigidbody>();

            //    Matrix test = gameObject.GetComponent<RenderSkinnedComponent>().GetBoneMatrix(2);
            /*     Vector3 test1;
                 Quaternion test2;
                 Vector3 test3;
                 test.Decompose(out test1, out test2, out test3);
                 displaydata = test3;

                 Vector3 bone1 =  B_Hips.transform.localPosition;
                 Vector3 bone2 = displaydata;
                 Vector3 up = bone2 - bone1;
                 float length = up.Length();
                 if (length != 0)
                     up /= length; // Normalize
                 Vector3 Pos = bone1 + up * (length * 0.5f);

         */

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
