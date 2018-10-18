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
        public string Spine { get; set; } = "mixamorig:Spine";
        public string Hips { get; set; } = "mixamorig:Hips";
        public string Neck { get; set; } = "mixamorig:Neck";
        public string Head { get; set; } = "mixamorig:Head";
        public string UpperLeftArm { get; set; } = "mixamorig:LeftArm";
        public string LowerLeftArm { get; set; } = "mixamorig:LeftForeArm";
        public string UpperRightArm { get; set; } = "mixamorig:RightArm";
        public string LowerRightArm { get; set; } = "mixamorig:RightForeArm";
        public string LowerLeftHand { get; set; } = "mixamorig:LeftHand";
        public string LowerRightHand { get; set; } = "mixamorig:RightHand";
        public string LeftShoulder { get; set; } = "mixamorig:LeftShoulder";
        public string RightShoulder { get; set; } = "mixamorig:RightShoulder";
        public bool AllobjectKinectic { get; set; }
        public string UpperRightLeg { get; set; } = "mixamorig:RightUpLeg";
        public string LowerRightLeg { get; set; } = "mixamorig:RightLeg";
        public string RightFoot { get; set; } = "mixamorig:RightFoot";
        public string UpperLeftLeg { get; set; } = "mixamorig:LeftUpLeg";
        public string LowerLeftLeg { get; set; } = "mixamorig:LeftLeg";
        public string LeftFoot { get; set; } = "mixamorig:LeftFoot";
        public Vector2 AllobjectDamping { get; set; } = new Vector2(0.5f, 0.5f);
        public float Totalmass { get; set; } = 70.0f;

        public bool RagdollEnabled = false;

        GameObject G_Hips;
        GameObject G_Spine;
        GameObject G_Head;
        GameObject G_LeftArm;
        GameObject G_LeftUnderArm;
        GameObject G_RightArm;
        GameObject G_RightUnderArm;
        GameObject G_RightLeg;
        GameObject G_LeftLeg;
        GameObject G_RightUnderLeg;
        GameObject G_LeftUnderLeg;



        BoneTransformComponent B_Hips;
        BoneTransformComponent B_Spine;
        BoneTransformComponent B_Head;
        BoneTransformComponent B_LeftArm;
        BoneTransformComponent B_LeftUnderArm;
        BoneTransformComponent B_RightArm;
        BoneTransformComponent B_RightUnderArm;
        BoneTransformComponent B_RightLeg;
        BoneTransformComponent B_LeftLeg;
        BoneTransformComponent B_RightUnderLeg;
        BoneTransformComponent B_LeftUnderLeg;


        //hips
        SphereCollider spherecolliderhips;
        Rigidbody rigidbodyhips;
        //Spine
        BoxCollider boxcolliderSpine;
        Rigidbody rigidbodySpine;
        //Head
        SphereCollider spherecolliderHead;
        Rigidbody rigidbodyHead;

        CapsuleCollider CapsuleColliderLeftArm;
        Rigidbody rigidbodyLeftArm;




        CapsuleCollider CapsuleColliderLeftUnderArm;
        Rigidbody rigidbodyLeftUnderArm;

        CapsuleCollider CapsuleColliderRightArm;
        Rigidbody rigidbodyRightArm;



        CapsuleCollider CapsuleColliderRightUnderArm;
        Rigidbody rigidbodyRightUnderArm;

        CapsuleCollider CapsuleColliderRightLeg;
        Rigidbody rigidbodyRightLeg;

        CapsuleCollider CapsuleColliderRightUnderLeg;
        Rigidbody rigidbodyRightUnderLeg;

        CapsuleCollider CapsuleColliderLeftLeg;
        Rigidbody rigidbodyLeftLeg;

        CapsuleCollider CapsuleColliderLeftUnderLeg;
        Rigidbody rigidbodyLeftUnderLeg;

        Joint HipSpineJoint;
        Joint HeadSpineJoint;
        Joint LeftArmTorsoJoint;
        Joint LowerLeftArmLeftArmJoint;
        Joint RightArmTorsoJoint;
        Joint LowerRightArmRightArmJoint;
        Joint RightLegJoint;
        Joint RightUnderLegJoint;
        Joint LeftLegJoint;
        Joint LeftUnderLegJoint;
        float[] RealBodyMass;

        public override void Start()
        {
            
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
            //up.Normalize(); // Normalize
            Vector3 Pos =  up * 0.5f;
            return Pos;//DivideComponent(Pos, G_Hips.transform.localScale);
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

            return length;//DivideComponent(Pos, G_Hips.transform.localScale);
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
            if(Input.GetKeyDown(Input.Keys.Space))
            {
                if (RagdollEnabled)
                    DisableRagdoll();
                else
                   EnableRagdoll();
            }
        }
        public void EnableRagdoll()
        {
            if (RagdollEnabled)
                return;
            RenderSkinnedComponent renderskinnedcomponent = gameObject.GetComponent<RenderSkinnedComponent>();
            if (renderskinnedcomponent == null)
            { 
                ThomasEngine.Debug.LogError("No renderskinnedcomponent available Noragdoll will be created");
                return;
            }
            uint boneindex = 0;
            Vector3 center = new Vector3(0);
            //Hips
            B_Hips.BoneName = Hips;
            B_Hips.AnimatedObject = gameObject;
            renderskinnedcomponent.FetchBoneIndex(Utility.hash(Hips), out boneindex);
            G_Hips.transform.local_world = renderskinnedcomponent.GetLocalBoneMatrix((int)boneindex);
            G_Hips.transform.localPosition = new Vector3(G_Hips.transform.localPosition.x, G_Hips.transform.localPosition.y, G_Hips.transform.localPosition.z);
            spherecolliderhips.radius = 0.2f;
            rigidbodyhips.IsKinematic = AllobjectKinectic;
            rigidbodyhips.Damping = AllobjectDamping;
            rigidbodyhips.Mass = Totalmass * RealBodyMass[0];
            //Spine
       
            B_Spine.BoneName = Spine;
            B_Spine.AnimatedObject = gameObject;
            renderskinnedcomponent.FetchBoneIndex(Utility.hash(Spine), out boneindex);
            G_Spine.transform.local_world = renderskinnedcomponent.GetLocalBoneMatrix((int)boneindex);
            center = calculatePosbetweenTwoSkeletonschanges(Spine, Neck, renderskinnedcomponent); //renderskinnedcomponent.GetBoneMatrix((int)boneindex).Translation;
            center.x = 0;
            center.z = 0;
            boxcolliderSpine.center = center;
            boxcolliderSpine.size = new Vector3(0.2f, 0.2f, 0.2f);
            G_Spine.transform.localPosition = new Vector3(G_Spine.transform.localPosition.x, G_Spine.transform.localPosition.y, G_Spine.transform.localPosition.z);
            rigidbodySpine.IsKinematic = AllobjectKinectic;
            rigidbodySpine.Damping = AllobjectDamping;
            rigidbodySpine.Mass = Totalmass * RealBodyMass[1];

     


            //Joint from hips to Spine
         
            HipSpineJoint.Axis = new Vector3(0, 0, -90);
            HipSpineJoint.SwingAxis = new Vector3(0, 0, -90);
            HipSpineJoint.NoCollision = true;

            HipSpineJoint.SwingAngle1 = 64;
            HipSpineJoint.SwingAngle2 = 90;
            HipSpineJoint.ConnectedAnchor = -boxcolliderSpine.center;


            //Head
            B_Head.BoneName = Head;
            B_Head.AnimatedObject = gameObject;
            renderskinnedcomponent.FetchBoneIndex(Utility.hash(Head), out boneindex);
            G_Head.transform.local_world = renderskinnedcomponent.GetLocalBoneMatrix((int)boneindex);
            G_Head.transform.localPosition = new Vector3(G_Head.transform.localPosition.x, G_Head.transform.localPosition.y, G_Head.transform.localPosition.z);
            spherecolliderHead.radius = 0.2f;
            rigidbodyHead.IsKinematic = AllobjectKinectic;
            rigidbodyHead.Damping = AllobjectDamping;
            rigidbodyHead.Mass = Totalmass * RealBodyMass[2];
            center = calculatePosbetweenTwoSkeletonschanges(Neck, Head, renderskinnedcomponent);
            center.x = 0;
            center.z = 0;

            spherecolliderHead.center = center;



            //Joint from spine to head

            HeadSpineJoint.Axis = new Vector3(0, 0, 90);
            HeadSpineJoint.SwingAxis = new Vector3(0, 0, 90);
            HeadSpineJoint.NoCollision = true;
  
            HeadSpineJoint.SwingAngle1 = 90;
            HeadSpineJoint.SwingAngle2 = 90;
            HeadSpineJoint.ConnectedAnchor = spherecolliderHead.center + calculatePosbetweenTwoSkeletonschanges(Spine, Neck, renderskinnedcomponent);
            HeadSpineJoint.Anchor = spherecolliderHead.center * -2;

            //left arm

         
            B_LeftArm.BoneName = UpperLeftArm;
            B_LeftArm.AnimatedObject = gameObject;
            renderskinnedcomponent.FetchBoneIndex(Utility.hash(UpperLeftArm), out boneindex);
            G_LeftArm.transform.local_world = renderskinnedcomponent.GetLocalBoneMatrix((int)boneindex);
            G_LeftArm.transform.localPosition = new Vector3(G_LeftArm.transform.localPosition.x, G_LeftArm.transform.localPosition.y, G_LeftArm.transform.localPosition.z);

            CapsuleColliderLeftArm.rotation = ThomasEngine.CapsuleCollider.ColliderRotation.RotateX;
            CapsuleColliderLeftArm.height = calculateLengthBetweenSkeleton(UpperLeftArm, LowerLeftArm, renderskinnedcomponent);
            CapsuleColliderLeftArm.radius = 0.065f;
 
            rigidbodyLeftArm.IsKinematic = AllobjectKinectic;
            rigidbodyLeftArm.Damping = AllobjectDamping;
            rigidbodyLeftArm.Mass = Totalmass * RealBodyMass[3];
            //center = -SwapXY(calculatePosbetweenTwoSkeletonschanges(UpperLeftArm, LowerLeftArm, renderskinnedcomponent));
            center = calculatePosbetweenTwoSkeletonschanges(UpperLeftArm, LowerLeftArm, renderskinnedcomponent);
            center.x = 0;
            center.z = 0;
            float value = calculateLengthBetweenSkeleton(UpperLeftArm, LowerLeftArm, renderskinnedcomponent);
                center.y = -value * 0.5f;
            CapsuleColliderLeftArm.center = center;





            //Joint from leftarm totorso

            LeftArmTorsoJoint.Axis = new Vector3(-90, 0, 0);
            LeftArmTorsoJoint.SwingAxis = new Vector3(0, 0, 0);
            LeftArmTorsoJoint.NoCollision = true;

            LeftArmTorsoJoint.SwingAngle1 = 90;
            LeftArmTorsoJoint.SwingAngle2 = 90;
            LeftArmTorsoJoint.ConnectedAnchor = calculatePosbetweenTwoSkeletonschanges(LeftShoulder, UpperLeftArm, renderskinnedcomponent) + calculatePosbetweenTwoSkeletonschanges(Spine, UpperLeftArm, renderskinnedcomponent);
            LeftArmTorsoJoint.Anchor = SwapXY(calculatePosbetweenTwoSkeletonschanges(LeftShoulder, UpperLeftArm, renderskinnedcomponent)) * 2;// * new Vector3(1,2,1);



            B_LeftUnderArm.BoneName = LowerLeftArm;
            B_LeftUnderArm.AnimatedObject = gameObject;
            renderskinnedcomponent.FetchBoneIndex(Utility.hash(LowerLeftArm), out boneindex);
            G_LeftUnderArm.transform.local_world = renderskinnedcomponent.GetLocalBoneMatrix((int)boneindex);
            G_LeftUnderArm.transform.localPosition = new Vector3(G_LeftUnderArm.transform.localPosition.x, G_LeftUnderArm.transform.localPosition.y, G_LeftUnderArm.transform.localPosition.z);
 
            CapsuleColliderLeftUnderArm.rotation = ThomasEngine.CapsuleCollider.ColliderRotation.RotateX;
            CapsuleColliderLeftUnderArm.height = calculateLengthBetweenSkeleton(LowerLeftArm, LowerLeftHand, renderskinnedcomponent);
            CapsuleColliderLeftUnderArm.radius = 0.065f;
       
            rigidbodyLeftUnderArm.IsKinematic = AllobjectKinectic;
            rigidbodyLeftUnderArm.Damping = AllobjectDamping;
            rigidbodyLeftUnderArm.Mass = Totalmass * RealBodyMass[4];
            center = -SwapXY(calculatePosbetweenTwoSkeletonschanges(LowerLeftArm, LowerLeftHand, renderskinnedcomponent));
            center.x = 0;
            center.z = 0;
            CapsuleColliderLeftUnderArm.center = center;
             value = calculateLengthBetweenSkeleton(LowerLeftArm, LowerLeftHand, renderskinnedcomponent);
                center.y = -value * 0.5f;
            CapsuleColliderLeftUnderArm.center = center;

            //Joint from leftudnerarmtooverarm

            LowerLeftArmLeftArmJoint.Axis = new Vector3(-90, 0, 0);
            LowerLeftArmLeftArmJoint.SwingAxis = new Vector3(0, 0, 0);
            LowerLeftArmLeftArmJoint.NoCollision = true;
            LowerLeftArmLeftArmJoint.SwingAngle1 = 90;
            LowerLeftArmLeftArmJoint.SwingAngle2 = 90;
            LowerLeftArmLeftArmJoint.ConnectedAnchor = new Vector3(calculateLengthBetweenSkeleton(UpperLeftArm, LowerLeftArm, renderskinnedcomponent), 0, 0) + SwapXY(calculatePosbetweenTwoSkeletonschanges(UpperLeftArm, LowerLeftArm, renderskinnedcomponent));
            LowerLeftArmLeftArmJoint.Anchor = SwapXY(calculatePosbetweenTwoSkeletonschanges(UpperLeftArm, LowerLeftArm, renderskinnedcomponent));


            //    new Vector3(calculateLengthBetweenSkeleton(UpperLeftArm, LowerLeftArm, renderskinnedcomponent), 0, 0)






            //Right arm


      
            B_RightArm.BoneName = UpperRightArm;
            B_RightArm.AnimatedObject = gameObject;
            renderskinnedcomponent.FetchBoneIndex(Utility.hash(UpperRightArm), out boneindex);
            G_RightArm.transform.local_world = renderskinnedcomponent.GetLocalBoneMatrix((int)boneindex);
            G_RightArm.transform.localPosition = new Vector3(G_RightArm.transform.localPosition.x, G_RightArm.transform.localPosition.y, G_RightArm.transform.localPosition.z);
       
            CapsuleColliderRightArm.rotation = ThomasEngine.CapsuleCollider.ColliderRotation.RotateX;
            CapsuleColliderRightArm.height = calculateLengthBetweenSkeleton(UpperRightArm, LowerRightArm, renderskinnedcomponent);
            CapsuleColliderRightArm.radius = 0.065f;
          
            rigidbodyRightArm.IsKinematic = AllobjectKinectic;
            rigidbodyRightArm.Damping = AllobjectDamping;
            rigidbodyRightArm.Mass = Totalmass * RealBodyMass[5];
            center = -SwapXY(calculatePosbetweenTwoSkeletonschanges(UpperRightArm, LowerRightArm, renderskinnedcomponent));
            center.x = 0;
            center.z = 0;
            CapsuleColliderRightArm.center = center;
            value = calculateLengthBetweenSkeleton(UpperRightArm, LowerRightArm, renderskinnedcomponent);
                center.y = value * 0.5f;
            CapsuleColliderRightArm.center = center;



            //Joint from Rightarm totorso

            RightArmTorsoJoint.Axis = new Vector3(0, 0, 0);
            RightArmTorsoJoint.SwingAxis = new Vector3(0, 0, 0);
            RightArmTorsoJoint.NoCollision = true;

            RightArmTorsoJoint.SwingAngle1 = 90;
            RightArmTorsoJoint.SwingAngle2 = 90;
            RightArmTorsoJoint.ConnectedAnchor = calculatePosbetweenTwoSkeletonschanges(RightShoulder, UpperRightArm, renderskinnedcomponent) + calculatePosbetweenTwoSkeletonschanges(Spine, UpperRightArm, renderskinnedcomponent);
            RightArmTorsoJoint.Anchor = SwapXY(calculatePosbetweenTwoSkeletonschanges(RightShoulder, UpperRightArm, renderskinnedcomponent)) * 2;// * new Vector3(1,2,1);




            B_RightUnderArm.BoneName = LowerRightArm;
            B_RightUnderArm.AnimatedObject = gameObject;
            renderskinnedcomponent.FetchBoneIndex(Utility.hash(LowerRightArm), out boneindex);
            G_RightUnderArm.transform.local_world = renderskinnedcomponent.GetLocalBoneMatrix((int)boneindex);
            G_RightUnderArm.transform.localPosition = new Vector3(G_RightUnderArm.transform.localPosition.x, G_RightUnderArm.transform.localPosition.y, G_RightUnderArm.transform.localPosition.z);
     
            CapsuleColliderRightUnderArm.rotation = ThomasEngine.CapsuleCollider.ColliderRotation.RotateX;
            CapsuleColliderRightUnderArm.height = calculateLengthBetweenSkeleton(LowerRightArm, LowerRightHand, renderskinnedcomponent);
            CapsuleColliderRightUnderArm.radius = 0.065f;
    
            rigidbodyRightUnderArm.IsKinematic = AllobjectKinectic;
            rigidbodyRightUnderArm.Damping = AllobjectDamping;
            rigidbodyRightUnderArm.Mass = Totalmass * RealBodyMass[6];
            center = -SwapXY(calculatePosbetweenTwoSkeletonschanges(LowerRightArm, LowerRightHand, renderskinnedcomponent));
            center.x = 0;
            center.z = 0;
            CapsuleColliderRightUnderArm.center = center;
            value = calculateLengthBetweenSkeleton(LowerRightArm, LowerRightHand, renderskinnedcomponent);
                center.y = value*0.5f;
            CapsuleColliderRightUnderArm.center = center;



            //Joint from Rightudnerarmtooverarm

            LowerRightArmRightArmJoint.Axis = new Vector3(-90, 0, 0);
            LowerRightArmRightArmJoint.SwingAxis = new Vector3(0, 0, 0);
            LowerRightArmRightArmJoint.NoCollision = true;
            LowerRightArmRightArmJoint.SwingAngle1 = 90;
            LowerRightArmRightArmJoint.SwingAngle2 = 90;
            LowerRightArmRightArmJoint.ConnectedAnchor = new Vector3(-calculateLengthBetweenSkeleton(UpperRightArm, LowerRightArm, renderskinnedcomponent), 0, 0) + SwapXY(calculatePosbetweenTwoSkeletonschanges(UpperRightArm, LowerRightArm, renderskinnedcomponent));
            LowerRightArmRightArmJoint.Anchor = SwapXY(calculatePosbetweenTwoSkeletonschanges(UpperRightArm, LowerRightArm, renderskinnedcomponent));



     
            B_RightLeg.BoneName = UpperRightLeg;
            B_RightLeg.AnimatedObject = gameObject;
            renderskinnedcomponent.FetchBoneIndex(Utility.hash(UpperRightLeg), out boneindex);
            G_RightLeg.transform.local_world = renderskinnedcomponent.GetLocalBoneMatrix((int)boneindex);
            G_RightLeg.transform.localPosition = new Vector3(G_RightLeg.transform.localPosition.x, G_RightLeg.transform.localPosition.y, G_RightLeg.transform.localPosition.z);
          
            CapsuleColliderRightLeg.height = calculateLengthBetweenSkeleton(UpperRightLeg, LowerRightLeg, renderskinnedcomponent);
            CapsuleColliderRightLeg.radius = 0.065f;
          
            rigidbodyRightLeg.IsKinematic = AllobjectKinectic;
            rigidbodyRightLeg.Damping = AllobjectDamping;
            rigidbodyRightLeg.Mass = Totalmass * RealBodyMass[7];
            center = calculatePosbetweenTwoSkeletonschanges(UpperRightLeg, LowerRightLeg, renderskinnedcomponent);
            center.x = 0;
            center.z = 0;
            CapsuleColliderRightLeg.center = center;
            value = calculateLengthBetweenSkeleton(UpperRightLeg, LowerRightLeg, renderskinnedcomponent);
            center.y = -value * 0.5f;
            CapsuleColliderRightLeg.center = center;




            //Joint from RightLegJoint
            RightLegJoint.Axis = new Vector3(0, 0, 90);
            RightLegJoint.SwingAxis = new Vector3(0, 0, 0);
            RightLegJoint.NoCollision = true;
            RightLegJoint.SwingAngle1 = 90;
            RightLegJoint.SwingAngle2 = 90;
            RightLegJoint.Anchor = -calculatePosbetweenTwoSkeletonschanges(UpperRightLeg, LowerRightLeg, renderskinnedcomponent);//new Vector3(0, calculateLengthBetweenSkeleton(UpperRightLeg, LowerRightLeg, renderskinnedcomponent) * 0.5f, 0);
            RightLegJoint.ConnectedAnchor = calculatePosbetweenTwoSkeletonschanges(Hips, UpperRightLeg, renderskinnedcomponent) * 2;



            //RightUnderLeg
         
            B_RightUnderLeg.BoneName = LowerRightLeg;
            B_RightUnderLeg.AnimatedObject = gameObject;
            renderskinnedcomponent.FetchBoneIndex(Utility.hash(LowerRightLeg), out boneindex);
            G_RightUnderLeg.transform.local_world = renderskinnedcomponent.GetLocalBoneMatrix((int)boneindex);
            G_RightUnderLeg.transform.localPosition = new Vector3(G_RightUnderLeg.transform.localPosition.x, G_RightUnderLeg.transform.localPosition.y, G_RightUnderLeg.transform.localPosition.z);
          
            CapsuleColliderRightUnderLeg.height = calculateLengthBetweenSkeleton(LowerRightLeg, RightFoot, renderskinnedcomponent);
            CapsuleColliderRightUnderLeg.radius = 0.065f;
            
            rigidbodyRightUnderLeg.Damping = AllobjectDamping;
            rigidbodyRightUnderLeg.Mass = Totalmass * RealBodyMass[8];
            rigidbodyRightUnderLeg.IsKinematic = AllobjectKinectic;
            center = calculatePosbetweenTwoSkeletonschanges(LowerRightLeg, RightFoot, renderskinnedcomponent);
            center.x = 0;
            center.z = 0;
            CapsuleColliderRightUnderLeg.center = center;
            value = calculateLengthBetweenSkeleton(LowerRightLeg, RightFoot, renderskinnedcomponent);
            center.y = -value * 0.5f;
            CapsuleColliderRightUnderLeg.center = center;





            RightUnderLegJoint.Axis = new Vector3(0, 0, 0);
            RightUnderLegJoint.SwingAxis = new Vector3(0, 0, 0);
            RightUnderLegJoint.NoCollision = true;
            RightUnderLegJoint.SwingAngle1 = 90;
            RightUnderLegJoint.SwingAngle2 = 10;
            RightUnderLegJoint.TwistAngle = 53;
            RightUnderLegJoint.ConnectedAnchor = CapsuleColliderRightUnderLeg.center;
            RightUnderLegJoint.Anchor = -CapsuleColliderRightUnderLeg.center;

            //left leg

            B_LeftLeg.BoneName = UpperLeftLeg;
            B_LeftLeg.AnimatedObject = gameObject;
            renderskinnedcomponent.FetchBoneIndex(Utility.hash(UpperLeftLeg), out boneindex);
            G_LeftLeg.transform.local_world = renderskinnedcomponent.GetLocalBoneMatrix((int)boneindex);
            G_LeftLeg.transform.localPosition = new Vector3(G_LeftLeg.transform.localPosition.x, G_LeftLeg.transform.localPosition.y, G_LeftLeg.transform.localPosition.z);
          
            CapsuleColliderLeftLeg.height = calculateLengthBetweenSkeleton(UpperLeftLeg, LowerLeftLeg, renderskinnedcomponent);
            CapsuleColliderLeftLeg.radius = 0.065f;
           
            rigidbodyLeftLeg.IsKinematic = AllobjectKinectic;
            rigidbodyLeftLeg.Damping = AllobjectDamping;
            rigidbodyLeftLeg.Mass = Totalmass * RealBodyMass[9];
            center = calculatePosbetweenTwoSkeletonschanges(UpperLeftLeg, LowerLeftLeg, renderskinnedcomponent);
            center.x = 0;
            center.z = 0;
            CapsuleColliderLeftLeg.center = center;
            value = calculateLengthBetweenSkeleton(UpperLeftLeg, LowerLeftLeg, renderskinnedcomponent);
            center.y = -value * 0.5f;
            CapsuleColliderLeftLeg.center = center;
            //Joint from LeftLegJoint

            LeftLegJoint.Axis = new Vector3(0, 0, -90);
            LeftLegJoint.SwingAxis = new Vector3(0, 0, 0);
            LeftLegJoint.NoCollision = true;
            
            LeftLegJoint.SwingAngle1 = 90;
            LeftLegJoint.SwingAngle2 = 90;
            LeftLegJoint.Anchor = -calculatePosbetweenTwoSkeletonschanges(UpperLeftLeg, LowerLeftLeg, renderskinnedcomponent);//new Vector3(0, calculateLengthBetweenSkeleton(UpperLeftLeg, LowerLeftLeg, renderskinnedcomponent) * 0.5f, 0);
            LeftLegJoint.ConnectedAnchor = calculatePosbetweenTwoSkeletonschanges(Hips, UpperLeftLeg, renderskinnedcomponent) * 2;



            //LeftUnderLeg
            B_LeftUnderLeg.BoneName = LowerLeftLeg;
            B_LeftUnderLeg.AnimatedObject = gameObject;
            renderskinnedcomponent.FetchBoneIndex(Utility.hash(LowerLeftLeg), out boneindex);
            G_LeftUnderLeg.transform.local_world = renderskinnedcomponent.GetLocalBoneMatrix((int)boneindex);
            G_LeftUnderLeg.transform.localPosition = new Vector3(G_LeftUnderLeg.transform.localPosition.x, G_LeftUnderLeg.transform.localPosition.y, G_LeftUnderLeg.transform.localPosition.z);
            CapsuleColliderLeftUnderLeg.height = calculateLengthBetweenSkeleton(LowerLeftLeg, LeftFoot, renderskinnedcomponent);
            CapsuleColliderLeftUnderLeg.radius = 0.065f;
            
            rigidbodyLeftUnderLeg.IsKinematic = AllobjectKinectic;
            rigidbodyLeftUnderLeg.Damping = AllobjectDamping;
            rigidbodyLeftUnderLeg.Mass = Totalmass * RealBodyMass[10];
            center = calculatePosbetweenTwoSkeletonschanges(LowerLeftLeg, LeftFoot, renderskinnedcomponent);
            center.x = 0;
            center.z = 0;
            CapsuleColliderLeftUnderLeg.center = center;
            value = calculateLengthBetweenSkeleton(LowerLeftLeg, LeftFoot, renderskinnedcomponent);
            center.y = -value * 0.5f;
            CapsuleColliderLeftUnderLeg.center = center;



            LeftUnderLegJoint.Axis = new Vector3(0, 0, 0);
            LeftUnderLegJoint.SwingAxis = new Vector3(0, 0, 0);
            LeftUnderLegJoint.NoCollision = true;
            LeftUnderLegJoint.SwingAngle1 = 90;
            LeftUnderLegJoint.SwingAngle2 = 10;
            LeftUnderLegJoint.TwistAngle = 53;
            LeftUnderLegJoint.ConnectedAnchor = CapsuleColliderLeftUnderLeg.center;
            LeftUnderLegJoint.Anchor = -CapsuleColliderLeftUnderLeg.center;

            G_Hips.activeSelf = true;
            G_Spine.activeSelf = true;
            G_Head.activeSelf = true;
            G_LeftArm.activeSelf = true;
            G_LeftUnderArm.activeSelf = true;
            G_RightArm.activeSelf = true;
            G_RightUnderArm.activeSelf = true;
            G_RightLeg.activeSelf = true;
            G_LeftLeg.activeSelf = true;
            G_RightUnderLeg.activeSelf = true;
            G_LeftUnderLeg.activeSelf = true;

            RagdollEnabled = true;
        }
        public override void Awake()
        {
            RealBodyMass = new float[11];
            RealBodyMass[0] = 0.14f;
            RealBodyMass[1] = 0.216f;
            RealBodyMass[2] = 0.081f;
            RealBodyMass[3] = 0.028f; 
            RealBodyMass[4] = 0.016f;
            RealBodyMass[5] = 0.028f;
            RealBodyMass[6] = 0.016f;
            RealBodyMass[7] = 0.1f;
            RealBodyMass[8] = 0.047f;
            RealBodyMass[9] = 0.1f;
            RealBodyMass[10] = 0.047f;

            //Hips
            G_Hips = new GameObject("Hips");
            G_Hips.transform.SetParent(gameObject.transform);
            HipSpineJoint = G_Hips.AddComponent<Joint>();
            B_Hips = G_Hips.AddComponent<BoneTransformComponent>();
            spherecolliderhips = G_Hips.AddComponent<SphereCollider>();
            rigidbodyhips = G_Hips.AddComponent<Rigidbody>();

            //Spine
            G_Spine = new GameObject("Spine");
            G_Spine.transform.SetParent(gameObject.transform);
            B_Spine = G_Spine.AddComponent<BoneTransformComponent>();
            boxcolliderSpine = G_Spine.AddComponent<BoxCollider>();
            rigidbodySpine = G_Spine.AddComponent<Rigidbody>();


            
            //Head
            G_Head = new GameObject("Head");
            G_Head.transform.SetParent(gameObject.transform);
            HeadSpineJoint = G_Head.AddComponent<Joint>();
            B_Head = G_Head.AddComponent<BoneTransformComponent>();
            spherecolliderHead = G_Head.AddComponent<SphereCollider>();
            rigidbodyHead = G_Head.AddComponent<Rigidbody>();

          
            //left arm
            G_LeftArm = new GameObject("LeftArm");
            G_LeftArm.transform.SetParent(gameObject.transform);
            LeftArmTorsoJoint = G_LeftArm.AddComponent<Joint>();
            B_LeftArm = G_LeftArm.AddComponent<BoneTransformComponent>();
            CapsuleColliderLeftArm = G_LeftArm.AddComponent<CapsuleCollider>();
            rigidbodyLeftArm = G_LeftArm.AddComponent<Rigidbody>();

  

            G_LeftUnderArm = new GameObject("LeftLowerArm");
            G_LeftUnderArm.transform.SetParent(gameObject.transform);
            LowerLeftArmLeftArmJoint = G_LeftUnderArm.AddComponent<Joint>();
            B_LeftUnderArm = G_LeftUnderArm.AddComponent<BoneTransformComponent>();
            CapsuleColliderLeftUnderArm = G_LeftUnderArm.AddComponent<CapsuleCollider>();
            rigidbodyLeftUnderArm = G_LeftUnderArm.AddComponent<Rigidbody>();

            
            G_RightArm = new GameObject("RightArm");
            G_RightArm.transform.SetParent(gameObject.transform);
            RightArmTorsoJoint = G_RightArm.AddComponent<Joint>();
            B_RightArm = G_RightArm.AddComponent<BoneTransformComponent>();
            CapsuleColliderRightArm = G_RightArm.AddComponent<CapsuleCollider>();
            rigidbodyRightArm = G_RightArm.AddComponent<Rigidbody>();




            G_RightUnderArm = new GameObject("RightLowerArm");
            G_RightUnderArm.transform.SetParent(gameObject.transform);
            LowerRightArmRightArmJoint = G_RightUnderArm.AddComponent<Joint>();
            B_RightUnderArm = G_RightUnderArm.AddComponent<BoneTransformComponent>();
            CapsuleColliderRightUnderArm = G_RightUnderArm.AddComponent<CapsuleCollider>();
            rigidbodyRightUnderArm = G_RightUnderArm.AddComponent<Rigidbody>();


            
            G_RightLeg = new GameObject("RightLeg");
            G_RightLeg.transform.SetParent(gameObject.transform);
            RightLegJoint = G_RightLeg.AddComponent<Joint>();
            B_RightLeg = G_RightLeg.AddComponent<BoneTransformComponent>();
            CapsuleColliderRightLeg = G_RightLeg.AddComponent<CapsuleCollider>();
            rigidbodyRightLeg = G_RightLeg.AddComponent<Rigidbody>();

            //RightUnderLeg
            G_RightUnderLeg = new GameObject("RightLowerLeg");
            G_RightUnderLeg.transform.SetParent(gameObject.transform);
            RightUnderLegJoint = G_RightUnderLeg.AddComponent<Joint>();
            B_RightUnderLeg = G_RightUnderLeg.AddComponent<BoneTransformComponent>();
            CapsuleColliderRightUnderLeg = G_RightUnderLeg.AddComponent<CapsuleCollider>();
            rigidbodyRightUnderLeg = G_RightUnderLeg.AddComponent<Rigidbody>();

            //left leg
            G_LeftLeg = new GameObject("LeftLeg");
            G_LeftLeg.transform.SetParent(gameObject.transform);
            LeftLegJoint = G_LeftLeg.AddComponent<Joint>();
            B_LeftLeg = G_LeftLeg.AddComponent<BoneTransformComponent>();
            CapsuleColliderLeftLeg = G_LeftLeg.AddComponent<CapsuleCollider>();
            rigidbodyLeftLeg = G_LeftLeg.AddComponent<Rigidbody>();

            //LeftUnderLeg
            G_LeftUnderLeg = new GameObject("LeftLowerLeg");
            G_LeftUnderLeg.transform.SetParent(gameObject.transform);
            LeftUnderLegJoint = G_LeftUnderLeg.AddComponent<Joint>();
            B_LeftUnderLeg = G_LeftUnderLeg.AddComponent<BoneTransformComponent>();
            CapsuleColliderLeftUnderLeg = G_LeftUnderLeg.AddComponent<CapsuleCollider>();
            rigidbodyLeftUnderLeg = G_LeftUnderLeg.AddComponent<Rigidbody>();

            //


            HeadSpineJoint.ConnectedRigidbody = rigidbodySpine;
            HipSpineJoint.ConnectedRigidbody = rigidbodySpine;
            LeftArmTorsoJoint.ConnectedRigidbody = rigidbodySpine;
            LowerLeftArmLeftArmJoint.ConnectedRigidbody = rigidbodyLeftArm;
            RightArmTorsoJoint.ConnectedRigidbody = rigidbodySpine;
            LowerRightArmRightArmJoint.ConnectedRigidbody = rigidbodyRightArm;
            RightLegJoint.ConnectedRigidbody = rigidbodyhips;
            RightUnderLegJoint.ConnectedRigidbody = rigidbodyRightLeg;
            LeftLegJoint.ConnectedRigidbody = rigidbodyhips;
            LeftUnderLegJoint.ConnectedRigidbody = rigidbodyLeftLeg;


            G_Hips.activeSelf = false;
            G_Spine.activeSelf = false;
            G_Head.activeSelf = false;
            G_LeftArm.activeSelf = false;
            G_LeftUnderArm.activeSelf = false;
            G_RightArm.activeSelf = false;
            G_RightUnderArm.activeSelf = false;
            G_RightLeg.activeSelf = false;
            G_LeftLeg.activeSelf = false;
            G_RightUnderLeg.activeSelf = false;
            G_LeftUnderLeg.activeSelf = false;

            RagdollEnabled = false;
        }
        public void DisableRagdoll()
        {
            if (!RagdollEnabled)
                return;
            G_Hips.activeSelf = false;
            G_Spine.activeSelf = false;
            G_Head.activeSelf = false;
            G_LeftArm.activeSelf = false;
            G_LeftUnderArm.activeSelf = false;
            G_RightArm.activeSelf = false;
            G_RightUnderArm.activeSelf = false;
            G_RightLeg.activeSelf = false;
            G_LeftLeg.activeSelf = false;
            G_RightUnderLeg.activeSelf = false;
            G_LeftUnderLeg.activeSelf = false;

            RagdollEnabled = false;
        }
    

    }
}
