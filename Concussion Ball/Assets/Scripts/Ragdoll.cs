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
        public string Neck { get; set; }
        public string Head { get; set; }
        public string UpperLeftArm { get; set; }
        public string LowerLeftArm { get; set; }
        public string LowerLeftHand { get; set; }
        public string LeftAxel { get; set; }
        public bool AllobjectKinectic { get; set; }
        GameObject G_Hips;
        GameObject G_Spine;
        GameObject G_Head;
        GameObject G_LeftArm;
        GameObject G_LeftUnderArm;
        public override void Start()
        {

            RenderSkinnedComponent renderskinnedcomponent = gameObject.GetComponent<RenderSkinnedComponent>();
            uint boneindex = 0;

            //Hips
            G_Hips = new GameObject("Hips");
            G_Hips.transform.SetParent(gameObject.transform);
            BoneTransformComponent B_Hips = G_Hips.AddComponent<BoneTransformComponent>();
            B_Hips.BoneName = Hips;
            B_Hips.AnimatedObject = gameObject;
            renderskinnedcomponent.FetchBoneIndex(Utility.hash(Hips), out boneindex);
            G_Hips.transform.local_world = Matrix.CreateScale(100) * renderskinnedcomponent.GetLocalBoneMatrix((int)boneindex);
            G_Hips.transform.localPosition = new Vector3(G_Hips.transform.localPosition.x * 100, G_Hips.transform.localPosition.y * 100, G_Hips.transform.localPosition.z * 100);
            SphereCollider spherecolliderhips = G_Hips.AddComponent<SphereCollider>();
            spherecolliderhips.radius = 0.2f*100;
            Rigidbody rigidbodyhips = G_Hips.AddComponent<Rigidbody>();
            rigidbodyhips.IsKinematic = AllobjectKinectic;

            //Spine
            G_Spine = new GameObject("Spine");
            G_Spine.transform.SetParent(gameObject.transform);
            BoneTransformComponent B_Spine = G_Spine.AddComponent<BoneTransformComponent>();
            B_Spine.BoneName = Spine;
            B_Spine.AnimatedObject = gameObject;
            renderskinnedcomponent.FetchBoneIndex(Utility.hash(Spine), out boneindex);
            BoxCollider boxcolliderSpine = G_Spine.AddComponent<BoxCollider>();
            G_Spine.transform.local_world = Matrix.CreateScale(100) * renderskinnedcomponent.GetLocalBoneMatrix((int)boneindex);
            boxcolliderSpine.center = calculatePosbetweenTwoSkeletonschanges(Spine, Neck,renderskinnedcomponent); //renderskinnedcomponent.GetBoneMatrix((int)boneindex).Translation;
            boxcolliderSpine.size = new Vector3(0.2f,0.2f,0.2f) * 100;
            G_Spine.transform.localPosition = new Vector3(G_Spine.transform.localPosition.x * 100, G_Spine.transform.localPosition.y * 100, G_Spine.transform.localPosition.z * 100);
            Rigidbody rigidbodySpine = G_Spine.AddComponent<Rigidbody>();
            rigidbodySpine.IsKinematic = AllobjectKinectic;
           
            //Joint from hips to Spine
            Joint HipSpineJoint=G_Hips.AddComponent<Joint>();
            HipSpineJoint.Axis = new Vector3(0, 0, -90);
            HipSpineJoint.SwingAxis = new Vector3(0, 0, -90);
            HipSpineJoint.NoCollision = true;
            HipSpineJoint.ConnectedRigidbody = rigidbodySpine;
            HipSpineJoint.SwingAngle1 = 64;
            HipSpineJoint.SwingAngle2 = 90;
            HipSpineJoint.ConnectedAnchor = -boxcolliderSpine.center;


            //Head
            G_Head = new GameObject("Head");
            G_Head.transform.SetParent(gameObject.transform);
            BoneTransformComponent B_Head = G_Head.AddComponent<BoneTransformComponent>();
            B_Head.BoneName = Head;
            B_Head.AnimatedObject = gameObject;
            renderskinnedcomponent.FetchBoneIndex(Utility.hash(Head), out boneindex);
            G_Head.transform.local_world = Matrix.CreateScale(100) * renderskinnedcomponent.GetLocalBoneMatrix((int)boneindex);
            G_Head.transform.localPosition = new Vector3(G_Head.transform.localPosition.x*100, G_Head.transform.localPosition.y * 100, G_Head.transform.localPosition.z * 100);
            SphereCollider spherecolliderHead = G_Head.AddComponent<SphereCollider>();
            spherecolliderHead.radius = 0.2f * 100;
            Rigidbody rigidbodyHead = G_Head.AddComponent<Rigidbody>();
            rigidbodyHead.IsKinematic = AllobjectKinectic;
            spherecolliderHead.center = calculatePosbetweenTwoSkeletonschanges(Neck, Head, renderskinnedcomponent);

            //Joint from spine to head
            Joint HeadSpineJoint = G_Head.AddComponent<Joint>();
            HeadSpineJoint.Axis = new Vector3(0, 0, 90);
            HeadSpineJoint.SwingAxis = new Vector3(0, 0, 90);
            HeadSpineJoint.NoCollision = true;
            HeadSpineJoint.ConnectedRigidbody = rigidbodySpine;
            HeadSpineJoint.SwingAngle1 = 90;
            HeadSpineJoint.SwingAngle2 = 90;
            HeadSpineJoint.ConnectedAnchor = spherecolliderHead.center + calculatePosbetweenTwoSkeletonschanges(Spine,Neck,renderskinnedcomponent);
            HeadSpineJoint.Anchor = spherecolliderHead.center * -2;

            //left arm

            G_LeftArm = new GameObject("LeftArm");
            G_LeftArm.transform.SetParent(gameObject.transform);
            BoneTransformComponent B_LeftArm = G_LeftArm.AddComponent<BoneTransformComponent>();
            B_LeftArm.BoneName = UpperLeftArm;
            B_LeftArm.AnimatedObject = gameObject;
            renderskinnedcomponent.FetchBoneIndex(Utility.hash(UpperLeftArm), out boneindex);
            G_LeftArm.transform.local_world = Matrix.CreateScale(100) * renderskinnedcomponent.GetLocalBoneMatrix((int)boneindex);
            G_LeftArm.transform.localPosition = new Vector3(G_LeftArm.transform.localPosition.x * 100, G_LeftArm.transform.localPosition.y * 100, G_LeftArm.transform.localPosition.z * 100);
            CapsuleCollider CapsuleColliderLeftArm = G_LeftArm.AddComponent<CapsuleCollider>();
            CapsuleColliderLeftArm.rotation = ThomasEngine.CapsuleCollider.ColliderRotation.RotateX;
            CapsuleColliderLeftArm.height = calculateLengthBetweenSkeleton(UpperLeftArm, LowerLeftArm, renderskinnedcomponent);
            CapsuleColliderLeftArm.radius = 0.065f * 100f;
            Rigidbody rigidbodyLeftArm = G_LeftArm.AddComponent<Rigidbody>();
            rigidbodyLeftArm.IsKinematic = AllobjectKinectic;
            CapsuleColliderLeftArm.center = -SwapXY(calculatePosbetweenTwoSkeletonschanges(UpperLeftArm, LowerLeftArm, renderskinnedcomponent));

            //Joint from body
            Joint LeftArmTorsoJoint = G_LeftArm.AddComponent<Joint>();
            LeftArmTorsoJoint.Axis = new Vector3(-90, 0, 0);
            LeftArmTorsoJoint.SwingAxis = new Vector3(0, 0, 0);
            LeftArmTorsoJoint.NoCollision = true;
            LeftArmTorsoJoint.ConnectedRigidbody = rigidbodySpine;
            LeftArmTorsoJoint.SwingAngle1 = 90;
            LeftArmTorsoJoint.SwingAngle2 = 90;
            LeftArmTorsoJoint.ConnectedAnchor = calculatePosbetweenTwoSkeletonschanges(LeftAxel, UpperLeftArm, renderskinnedcomponent) + calculatePosbetweenTwoSkeletonschanges(Spine, UpperLeftArm, renderskinnedcomponent);
            LeftArmTorsoJoint.Anchor = SwapXY(calculatePosbetweenTwoSkeletonschanges(LeftAxel, UpperLeftArm, renderskinnedcomponent)) * 2;// * new Vector3(1,2,1);



            G_LeftUnderArm = new GameObject("LeftLowerArm");
            G_LeftUnderArm.transform.SetParent(gameObject.transform);
            BoneTransformComponent B_LeftUnderArm = G_LeftUnderArm.AddComponent<BoneTransformComponent>();
            B_LeftUnderArm.BoneName = LowerLeftArm;
            B_LeftUnderArm.AnimatedObject = gameObject;
            renderskinnedcomponent.FetchBoneIndex(Utility.hash(LowerLeftArm), out boneindex);
            G_LeftUnderArm.transform.local_world = Matrix.CreateScale(100) * renderskinnedcomponent.GetLocalBoneMatrix((int)boneindex);
            G_LeftUnderArm.transform.localPosition = new Vector3(G_LeftUnderArm.transform.localPosition.x * 100, G_LeftUnderArm.transform.localPosition.y * 100, G_LeftUnderArm.transform.localPosition.z * 100);
            CapsuleCollider CapsuleColliderLeftUnderArm = G_LeftUnderArm.AddComponent<CapsuleCollider>();
            CapsuleColliderLeftUnderArm.rotation = ThomasEngine.CapsuleCollider.ColliderRotation.RotateX;
            CapsuleColliderLeftUnderArm.height = calculateLengthBetweenSkeleton(LowerLeftArm, LowerLeftHand, renderskinnedcomponent);
            CapsuleColliderLeftUnderArm.radius = 0.065f * 100f;
            Rigidbody rigidbodyLeftUnderArm = G_LeftUnderArm.AddComponent<Rigidbody>();
            rigidbodyLeftUnderArm.IsKinematic = AllobjectKinectic;
            CapsuleColliderLeftUnderArm.center = -SwapXY(calculatePosbetweenTwoSkeletonschanges(LowerLeftArm, LowerLeftHand, renderskinnedcomponent));



            //Joint from body
            Joint LowerLeftArmLeftArmJoint = G_LeftUnderArm.AddComponent<Joint>();
            LowerLeftArmLeftArmJoint.Axis = new Vector3(-90, 0, 0);
            LowerLeftArmLeftArmJoint.SwingAxis = new Vector3(0, 0, 0);
            LowerLeftArmLeftArmJoint.NoCollision = true;
            LowerLeftArmLeftArmJoint.ConnectedRigidbody = rigidbodyLeftArm;
            LowerLeftArmLeftArmJoint.SwingAngle1 = 90;
            LowerLeftArmLeftArmJoint.SwingAngle2 = 90;
            LowerLeftArmLeftArmJoint.ConnectedAnchor = new Vector3(calculateLengthBetweenSkeleton(UpperLeftArm, LowerLeftArm, renderskinnedcomponent), 0, 0) + SwapXY(calculatePosbetweenTwoSkeletonschanges(UpperLeftArm, LowerLeftArm, renderskinnedcomponent));
            LowerLeftArmLeftArmJoint.Anchor = SwapXY(calculatePosbetweenTwoSkeletonschanges(UpperLeftArm, LowerLeftArm, renderskinnedcomponent));


        //    new Vector3(calculateLengthBetweenSkeleton(UpperLeftArm, LowerLeftArm, renderskinnedcomponent), 0, 0)




        }
        //swapX with Y
        Vector3 SwapXY(Vector3 swapxy)
        {
            Vector3 savedvalue;
            savedvalue = swapxy;
            swapxy.x = savedvalue.y;
            swapxy.y = savedvalue.x;
            return swapxy;
        }

        // Divide the numerator by each vector component.
        Vector3 Divide(float numerator, Vector3 denominator)
        {
            denominator.x = numerator / denominator.x;
            denominator.y = numerator / denominator.y;
            denominator.z = numerator / denominator.z;
            return denominator;
        }
        // Divide each vector component separately
        Vector3 DivideComponent(Vector3 v, Vector3 div)
        {
            v.x /= div.x;
            v.y /= div.y;
            v.z /= div.z;
            return v;
        }
        // Mult. each vector component separately
        Vector3 MultiplyComponent(Vector3 v, Vector3 div)
        {
            v.x *= div.x;
            v.y *= div.y;
            v.z *= div.z;
            return v;
        }
        Vector3 calculatePosbetweenTwoSkeletonschanges(string BoneName1,string BoneName2, RenderSkinnedComponent renderskinnedcomponent)
        {
            uint boneindex = 0;
            renderskinnedcomponent.FetchBoneIndex(Utility.hash(BoneName1), out boneindex);
            Vector3 bone1 = renderskinnedcomponent.GetLocalBoneMatrix((int)boneindex).Translation;
            renderskinnedcomponent.FetchBoneIndex(Utility.hash(BoneName2), out boneindex);
            Vector3 bone2 = renderskinnedcomponent.GetLocalBoneMatrix((int)boneindex).Translation;
            Vector3 up = bone2 - bone1;
            float length = up.Length();
            if (length != 0)
                up /= length; // Normalize
            Vector3 Pos =  up * (length * 0.5f);
            return Pos*100;//DivideComponent(Pos, G_Hips.transform.localScale);
        }
        float calculateLengthBetweenSkeleton(string BoneName1, string BoneName2, RenderSkinnedComponent renderskinnedcomponent)
        {
            uint boneindex = 0;
            renderskinnedcomponent.FetchBoneIndex(Utility.hash(BoneName1), out boneindex);
            Vector3 bone1 = renderskinnedcomponent.GetLocalBoneMatrix((int)boneindex).Translation;
            renderskinnedcomponent.FetchBoneIndex(Utility.hash(BoneName2), out boneindex);
            Vector3 bone2 = renderskinnedcomponent.GetLocalBoneMatrix((int)boneindex).Translation;
            Vector3 up = bone2 - bone1;
            float length = up.Length();
            if (length != 0)
                up /= length; // Normalize
            Vector3 Pos = up * length;
            return length * 100;//DivideComponent(Pos, G_Hips.transform.localScale);
        }

        void initSkeleton(RenderSkinnedComponent Model)
        {
            
        }
        Quaternion getRotationTo(Vector3 from, Vector3 dest)
        {
            // Based on Stan Melax's article in Game Programming Gems
            Quaternion q;
            from.Normalize();
            dest.Normalize();
          
            float d = Vector3.Dot(from, dest);
            if (d >= 1.0f) // Vectors are parallel	(identical)
                return Quaternion.Identity;
            if (d < (1e-6f - 1.0f)) // Vectors are parallel (opposite)
            {
                // Generate an axis
                Vector3.Cross(Vector3.UnitX, from);
                Vector3 axis = Vector3.Cross(Vector3.UnitX, from);
                if (axis.LengthSquared() < 0.0001f) // Pick another if colinear
                    axis = Vector3.Cross(Vector3.UnitY, from);
                axis.Normalize();
                q = Quaternion.CreateFromAxisAngle(axis, (float)Math.PI);
            }
            else
            {
                float s = (float)Math.Sqrt((1 + d) * 2);
                float invs = 1 / s;

                Vector3 c = Vector3.Cross(from,dest);

                q.x = c.x * invs;
                q.y = c.y * invs;
                q.z = c.z * invs;
                q.w = s * 0.5f;
                q.Normalize();
            }
            return q;
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
