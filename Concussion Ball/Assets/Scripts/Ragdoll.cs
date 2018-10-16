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
        public string UpperRightArm { get; set; }
        public string LowerRightArm { get; set; }
        public string LowerLeftHand { get; set; }
        public string LowerRightHand { get; set; }
        public string LeftAxel { get; set; }
        public string RightAxel { get; set; }
        public bool AllobjectKinectic { get; set; }
        public string UpperRightLeg { get; set; }
        public string LowerRightLeg { get; set; }
        public string RightFoot { get; set; }
        public string UpperLeftLeg { get; set; }
        public string LowerLeftLeg { get; set; }
        public string LeftFoot { get; set; }
        public Vector2 AllobjectDamping { get; set; }
        public float Totalmass { get; set; }

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
        GameObject G_RightFoot;
        GameObject G_LeftFoot;

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
            rigidbodyhips.Damping = AllobjectDamping;
            rigidbodyhips.Mass = Totalmass*0.14f;
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
            rigidbodySpine.Damping = AllobjectDamping;
            rigidbodySpine.Mass = Totalmass*0.216f;

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
            rigidbodyHead.Damping = AllobjectDamping;
            rigidbodyHead.Mass = Totalmass*0.081f;
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
            rigidbodyLeftArm.Damping = AllobjectDamping;
            rigidbodyLeftArm.Mass = Totalmass*0.028f;
            CapsuleColliderLeftArm.center = -SwapXY(calculatePosbetweenTwoSkeletonschanges(UpperLeftArm, LowerLeftArm, renderskinnedcomponent));

            //Joint from leftarm totorso
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
            rigidbodyLeftUnderArm.Damping = AllobjectDamping;
            rigidbodyLeftUnderArm.Mass = Totalmass * 0.016f;
            CapsuleColliderLeftUnderArm.center = -SwapXY(calculatePosbetweenTwoSkeletonschanges(LowerLeftArm, LowerLeftHand, renderskinnedcomponent));



            //Joint from leftudnerarmtooverarm
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






            //Right arm


            G_RightArm = new GameObject("RightArm");
            G_RightArm.transform.SetParent(gameObject.transform);
            BoneTransformComponent B_RightArm = G_RightArm.AddComponent<BoneTransformComponent>();
            B_RightArm.BoneName = UpperRightArm;
            B_RightArm.AnimatedObject = gameObject;
            renderskinnedcomponent.FetchBoneIndex(Utility.hash(UpperRightArm), out boneindex);
            G_RightArm.transform.local_world = Matrix.CreateScale(100) * renderskinnedcomponent.GetLocalBoneMatrix((int)boneindex);
            G_RightArm.transform.localPosition = new Vector3(G_RightArm.transform.localPosition.x * 100, G_RightArm.transform.localPosition.y * 100, G_RightArm.transform.localPosition.z * 100);
            CapsuleCollider CapsuleColliderRightArm = G_RightArm.AddComponent<CapsuleCollider>();
            CapsuleColliderRightArm.rotation = ThomasEngine.CapsuleCollider.ColliderRotation.RotateX;
            CapsuleColliderRightArm.height = calculateLengthBetweenSkeleton(UpperRightArm, LowerRightArm, renderskinnedcomponent);
            CapsuleColliderRightArm.radius = 0.065f * 100f;
            Rigidbody rigidbodyRightArm = G_RightArm.AddComponent<Rigidbody>();
            rigidbodyRightArm.IsKinematic = AllobjectKinectic;
            rigidbodyRightArm.Damping = AllobjectDamping;
            rigidbodyRightArm.Mass = Totalmass;
            CapsuleColliderRightArm.center = -SwapXY(calculatePosbetweenTwoSkeletonschanges(UpperRightArm, LowerRightArm, renderskinnedcomponent));


            //Joint from Rightarm totorso
            Joint RightArmTorsoJoint = G_RightArm.AddComponent<Joint>();
            RightArmTorsoJoint.Axis = new Vector3(0, 0, 0);
            RightArmTorsoJoint.SwingAxis = new Vector3(0, 0, 0);
            RightArmTorsoJoint.NoCollision = true;
            RightArmTorsoJoint.ConnectedRigidbody = rigidbodySpine;
            RightArmTorsoJoint.SwingAngle1 = 90;
            RightArmTorsoJoint.SwingAngle2 = 90;
            RightArmTorsoJoint.ConnectedAnchor = calculatePosbetweenTwoSkeletonschanges(RightAxel, UpperRightArm, renderskinnedcomponent) + calculatePosbetweenTwoSkeletonschanges(Spine, UpperRightArm, renderskinnedcomponent);
            RightArmTorsoJoint.Anchor = SwapXY(calculatePosbetweenTwoSkeletonschanges(RightAxel, UpperRightArm, renderskinnedcomponent)) * 2;// * new Vector3(1,2,1);





            G_RightUnderArm = new GameObject("RightLowerArm");
            G_RightUnderArm.transform.SetParent(gameObject.transform);
            BoneTransformComponent B_RightUnderArm = G_RightUnderArm.AddComponent<BoneTransformComponent>();
            B_RightUnderArm.BoneName = LowerRightArm;
            B_RightUnderArm.AnimatedObject = gameObject;
            renderskinnedcomponent.FetchBoneIndex(Utility.hash(LowerRightArm), out boneindex);
            G_RightUnderArm.transform.local_world = Matrix.CreateScale(100) * renderskinnedcomponent.GetLocalBoneMatrix((int)boneindex);
            G_RightUnderArm.transform.localPosition = new Vector3(G_RightUnderArm.transform.localPosition.x * 100, G_RightUnderArm.transform.localPosition.y * 100, G_RightUnderArm.transform.localPosition.z * 100);
            CapsuleCollider CapsuleColliderRightUnderArm = G_RightUnderArm.AddComponent<CapsuleCollider>();
            CapsuleColliderRightUnderArm.rotation = ThomasEngine.CapsuleCollider.ColliderRotation.RotateX;
            CapsuleColliderRightUnderArm.height = calculateLengthBetweenSkeleton(LowerRightArm, LowerRightHand, renderskinnedcomponent);
            CapsuleColliderRightUnderArm.radius = 0.065f * 100f;
            Rigidbody rigidbodyRightUnderArm = G_RightUnderArm.AddComponent<Rigidbody>();
            rigidbodyRightUnderArm.IsKinematic = AllobjectKinectic;
            rigidbodyRightUnderArm.Damping = AllobjectDamping;
            rigidbodyRightUnderArm.Mass = Totalmass;
            CapsuleColliderRightUnderArm.center = -SwapXY(calculatePosbetweenTwoSkeletonschanges(LowerRightArm, LowerRightHand, renderskinnedcomponent));



            //Joint from Rightudnerarmtooverarm
            Joint LowerRightArmRightArmJoint = G_RightUnderArm.AddComponent<Joint>();
            LowerRightArmRightArmJoint.Axis = new Vector3(-90, 0, 0);
            LowerRightArmRightArmJoint.SwingAxis = new Vector3(0, 0, 0);
            LowerRightArmRightArmJoint.NoCollision = true;
            LowerRightArmRightArmJoint.ConnectedRigidbody = rigidbodyRightArm;
            LowerRightArmRightArmJoint.SwingAngle1 = 90;
            LowerRightArmRightArmJoint.SwingAngle2 = 90;
            LowerRightArmRightArmJoint.ConnectedAnchor = new Vector3(-calculateLengthBetweenSkeleton(UpperRightArm, LowerRightArm, renderskinnedcomponent), 0, 0) + SwapXY(calculatePosbetweenTwoSkeletonschanges(UpperRightArm, LowerRightArm, renderskinnedcomponent));
            LowerRightArmRightArmJoint.Anchor = SwapXY(calculatePosbetweenTwoSkeletonschanges(UpperRightArm, LowerRightArm, renderskinnedcomponent));



            G_RightLeg = new GameObject("RightLeg");
            G_RightLeg.transform.SetParent(gameObject.transform);
            BoneTransformComponent B_RightLeg = G_RightLeg.AddComponent<BoneTransformComponent>();
            B_RightLeg.BoneName = UpperRightLeg;
            B_RightLeg.AnimatedObject = gameObject;
            renderskinnedcomponent.FetchBoneIndex(Utility.hash(UpperRightLeg), out boneindex);
            G_RightLeg.transform.local_world = Matrix.CreateScale(100) * renderskinnedcomponent.GetLocalBoneMatrix((int)boneindex);
            G_RightLeg.transform.localPosition = new Vector3(G_RightLeg.transform.localPosition.x * 100, G_RightLeg.transform.localPosition.y * 100, G_RightLeg.transform.localPosition.z * 100);
            CapsuleCollider CapsuleColliderRightLeg = G_RightLeg.AddComponent<CapsuleCollider>();
            CapsuleColliderRightLeg.height = calculateLengthBetweenSkeleton(UpperRightLeg, LowerRightLeg, renderskinnedcomponent);
            CapsuleColliderRightLeg.radius = 0.065f * 100f;
            Rigidbody rigidbodyRightLeg = G_RightLeg.AddComponent<Rigidbody>();
            rigidbodyRightLeg.IsKinematic = AllobjectKinectic;
            rigidbodyRightLeg.Damping = AllobjectDamping;
            rigidbodyRightLeg.Mass = Totalmass;
            CapsuleColliderRightLeg.center = calculatePosbetweenTwoSkeletonschanges(UpperRightLeg, LowerRightLeg, renderskinnedcomponent);


            //Joint from RightLegJoint
            Joint RightLegJoint = G_RightLeg.AddComponent<Joint>();
            RightLegJoint.Axis = new Vector3(0, 0, 90);
            RightLegJoint.SwingAxis = new Vector3(0, 0, 0);
            RightLegJoint.NoCollision = true;
            RightLegJoint.ConnectedRigidbody = rigidbodyhips;
            RightLegJoint.SwingAngle1 = 90;
            RightLegJoint.SwingAngle2 = 90;
            RightLegJoint.Anchor = -calculatePosbetweenTwoSkeletonschanges(UpperRightLeg, LowerRightLeg, renderskinnedcomponent);//new Vector3(0, calculateLengthBetweenSkeleton(UpperRightLeg, LowerRightLeg, renderskinnedcomponent) * 0.5f, 0);
            RightLegJoint.ConnectedAnchor = calculatePosbetweenTwoSkeletonschanges(Hips, UpperRightLeg, renderskinnedcomponent)*2;



            //RightUnderLeg
            G_RightUnderLeg = new GameObject("RightLowerLeg");
            G_RightUnderLeg.transform.SetParent(gameObject.transform);
            BoneTransformComponent B_RightUnderLeg = G_RightUnderLeg.AddComponent<BoneTransformComponent>();
            B_RightUnderLeg.BoneName = LowerRightLeg;
            B_RightUnderLeg.AnimatedObject = gameObject;
            renderskinnedcomponent.FetchBoneIndex(Utility.hash(LowerRightLeg), out boneindex);
            G_RightUnderLeg.transform.local_world = Matrix.CreateScale(100) * renderskinnedcomponent.GetLocalBoneMatrix((int)boneindex);
            G_RightUnderLeg.transform.localPosition = new Vector3(G_RightUnderLeg.transform.localPosition.x * 100, G_RightUnderLeg.transform.localPosition.y * 100, G_RightUnderLeg.transform.localPosition.z * 100);
            CapsuleCollider CapsuleColliderRightUnderLeg = G_RightUnderLeg.AddComponent<CapsuleCollider>();
            CapsuleColliderRightUnderLeg.height = calculateLengthBetweenSkeleton(LowerRightLeg, RightFoot, renderskinnedcomponent);
            CapsuleColliderRightUnderLeg.radius = 0.065f * 100f;
            Rigidbody rigidbodyRightUnderLeg = G_RightUnderLeg.AddComponent<Rigidbody>();
            rigidbodyRightUnderLeg.Damping = AllobjectDamping;
            rigidbodyRightUnderLeg.Mass = Totalmass;
            rigidbodyRightUnderLeg.IsKinematic = AllobjectKinectic;
            CapsuleColliderRightUnderLeg.center = calculatePosbetweenTwoSkeletonschanges(LowerRightLeg, RightFoot, renderskinnedcomponent);

            Joint RightUnderLegJoint = G_RightUnderLeg.AddComponent<Joint>();
            RightUnderLegJoint.Axis = new Vector3(0, 0, 0);
            RightUnderLegJoint.SwingAxis = new Vector3(0, 0, 0);
            RightUnderLegJoint.NoCollision = true;
            RightUnderLegJoint.ConnectedRigidbody = rigidbodyRightLeg;
            RightUnderLegJoint.SwingAngle1 = 90;
            RightUnderLegJoint.SwingAngle2 = 10;
            RightUnderLegJoint.TwistAngle = 53;
            RightUnderLegJoint.ConnectedAnchor = CapsuleColliderRightUnderLeg.center;
            RightUnderLegJoint.Anchor = -CapsuleColliderRightUnderLeg.center;
            //       RightUnderLegJoint.Anchor = -calculatePosbetweenTwoSkeletonschanges(, , renderskinnedcomponent);//new Vector3(0, calculateLengthBetweenSkeleton(UpperRightUnderLeg, LowerRightUnderLeg, renderskinnedcomponent) * 0.5f, 0);
            //         RightUnderLegJoint.ConnectedAnchor

            //left leg

            G_LeftLeg = new GameObject("LeftLeg");
            G_LeftLeg.transform.SetParent(gameObject.transform);
            BoneTransformComponent B_LeftLeg = G_LeftLeg.AddComponent<BoneTransformComponent>();
            B_LeftLeg.BoneName = UpperLeftLeg;
            B_LeftLeg.AnimatedObject = gameObject;
            renderskinnedcomponent.FetchBoneIndex(Utility.hash(UpperLeftLeg), out boneindex);
            G_LeftLeg.transform.local_world = Matrix.CreateScale(100) * renderskinnedcomponent.GetLocalBoneMatrix((int)boneindex);
            G_LeftLeg.transform.localPosition = new Vector3(G_LeftLeg.transform.localPosition.x * 100, G_LeftLeg.transform.localPosition.y * 100, G_LeftLeg.transform.localPosition.z * 100);
            CapsuleCollider CapsuleColliderLeftLeg = G_LeftLeg.AddComponent<CapsuleCollider>();
            CapsuleColliderLeftLeg.height = calculateLengthBetweenSkeleton(UpperLeftLeg, LowerLeftLeg, renderskinnedcomponent);
            CapsuleColliderLeftLeg.radius = 0.065f * 100f;
            Rigidbody rigidbodyLeftLeg = G_LeftLeg.AddComponent<Rigidbody>();
            rigidbodyLeftLeg.IsKinematic = AllobjectKinectic;
            rigidbodyLeftLeg.Damping = AllobjectDamping;
            rigidbodyLeftLeg.Mass = Totalmass;
            CapsuleColliderLeftLeg.center = calculatePosbetweenTwoSkeletonschanges(UpperLeftLeg, LowerLeftLeg, renderskinnedcomponent);


            //Joint from LeftLegJoint
            Joint LeftLegJoint = G_LeftLeg.AddComponent<Joint>();
            LeftLegJoint.Axis = new Vector3(0, 0, -90);
            LeftLegJoint.SwingAxis = new Vector3(0, 0, 0);
            LeftLegJoint.NoCollision = true;
            LeftLegJoint.ConnectedRigidbody = rigidbodyhips;
            LeftLegJoint.SwingAngle1 = 90;
            LeftLegJoint.SwingAngle2 = 90;
            LeftLegJoint.Anchor = -calculatePosbetweenTwoSkeletonschanges(UpperLeftLeg, LowerLeftLeg, renderskinnedcomponent);//new Vector3(0, calculateLengthBetweenSkeleton(UpperLeftLeg, LowerLeftLeg, renderskinnedcomponent) * 0.5f, 0);
            LeftLegJoint.ConnectedAnchor = calculatePosbetweenTwoSkeletonschanges(Hips, UpperLeftLeg, renderskinnedcomponent) * 2;



            //LeftUnderLeg
            G_LeftUnderLeg = new GameObject("LeftLowerLeg");
            G_LeftUnderLeg.transform.SetParent(gameObject.transform);
            BoneTransformComponent B_LeftUnderLeg = G_LeftUnderLeg.AddComponent<BoneTransformComponent>();
            B_LeftUnderLeg.BoneName = LowerLeftLeg;
            B_LeftUnderLeg.AnimatedObject = gameObject;
            renderskinnedcomponent.FetchBoneIndex(Utility.hash(LowerLeftLeg), out boneindex);
            G_LeftUnderLeg.transform.local_world = Matrix.CreateScale(100) * renderskinnedcomponent.GetLocalBoneMatrix((int)boneindex);
            G_LeftUnderLeg.transform.localPosition = new Vector3(G_LeftUnderLeg.transform.localPosition.x * 100, G_LeftUnderLeg.transform.localPosition.y * 100, G_LeftUnderLeg.transform.localPosition.z * 100);
            CapsuleCollider CapsuleColliderLeftUnderLeg = G_LeftUnderLeg.AddComponent<CapsuleCollider>();
            CapsuleColliderLeftUnderLeg.height = calculateLengthBetweenSkeleton(LowerLeftLeg, LeftFoot, renderskinnedcomponent);
            CapsuleColliderLeftUnderLeg.radius = 0.065f * 100f;
            Rigidbody rigidbodyLeftUnderLeg = G_LeftUnderLeg.AddComponent<Rigidbody>();
            rigidbodyLeftUnderLeg.IsKinematic = AllobjectKinectic;
            rigidbodyLeftUnderLeg.Damping = AllobjectDamping;
            rigidbodyLeftUnderLeg.Mass = Totalmass;
            CapsuleColliderLeftUnderLeg.center = calculatePosbetweenTwoSkeletonschanges(LowerLeftLeg, LeftFoot, renderskinnedcomponent);

            Joint LeftUnderLegJoint = G_LeftUnderLeg.AddComponent<Joint>();
            LeftUnderLegJoint.Axis = new Vector3(0, 0, 0);
            LeftUnderLegJoint.SwingAxis = new Vector3(0, 0, 0);
            LeftUnderLegJoint.NoCollision = true;
            LeftUnderLegJoint.ConnectedRigidbody = rigidbodyLeftLeg;
            LeftUnderLegJoint.SwingAngle1 = 90;
            LeftUnderLegJoint.SwingAngle2 = 10;
            LeftUnderLegJoint.TwistAngle = 53;
            LeftUnderLegJoint.ConnectedAnchor = CapsuleColliderLeftUnderLeg.center;
            LeftUnderLegJoint.Anchor = -CapsuleColliderLeftUnderLeg.center;


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
