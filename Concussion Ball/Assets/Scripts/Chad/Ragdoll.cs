using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ThomasEngine;
using ThomasEngine.Network;

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
    public string UpperRightLeg { get; set; } = "mixamorig:RightUpLeg";
    public string LowerRightLeg { get; set; } = "mixamorig:RightLeg";
    public string RightFoot { get; set; } = "mixamorig:RightFoot";
    public string UpperLeftLeg { get; set; } = "mixamorig:LeftUpLeg";
    public string LowerLeftLeg { get; set; } = "mixamorig:LeftLeg";
    public string LeftFoot { get; set; } = "mixamorig:LeftFoot";
    public float AllobjectDamping { get; set; } = 0.5f;
    public float Totalmass { get; set; } = 70.0f;
    public bool RagdollEnabled = true;
    public bool PostiveMapping { get; set; } = true;
    public bool AllobjectKinectic { get; set; } = false;
    public AudioClip ImpactSound { get; set; }
    RagdollImpact ImpactSpine;

    enum BODYPART
    {
        HIPS,
        SPINE,
        HEAD,

        LEFT_UPPER_ARM,
        RIGHT_UPPER_ARM,

        LEFT_LOWER_ARM,
        RIGHT_LOWER_ARM,

        LEFT_UPPER_LEG,
        RIGHT_UPPER_LEG,

        LEFT_LOWER_LEG,
        RIGHT_LOWER_LEG,

        COUNT
    }

    GameObject[] G_BodyParts = new GameObject[(int)BODYPART.COUNT];
    BoneTransformComponent[] BT_BodyParts = new BoneTransformComponent[(int)BODYPART.COUNT];
    Joint[] J_BodyParts = new Joint[(int)BODYPART.COUNT];
    Rigidbody[] RB_BodyParts = new Rigidbody[(int)BODYPART.COUNT];
    float[] Mass_BodyParts = new float[(int)BODYPART.COUNT];
    uint[] BoneIndexes = new uint[(int)BODYPART.COUNT];
    Collider[] C_BodyParts = new Collider[(int)BODYPART.COUNT];
    SoundComponent RagdollSound;
    NetworkIdentity identity;
    public override void Start()
    {
        DisableRagdoll();

        // Load the ragdoll impact sound
        RagdollSound = gameObject.AddComponent<SoundComponent>();
        RagdollSound.Type = SoundComponent.SoundType.Effect;
        RagdollSound.Clip = ImpactSound;
        RagdollSound.Looping = false;
        RagdollSound.Is3D = true;
        identity = gameObject.GetComponent<NetworkIdentity>();

    }
    #region Utility functions

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

    #endregion
    public void AddForce(Vector3 force, bool diveTackle)
    {
        for (int i = 0; i < (int)BODYPART.COUNT; i++)
        {
            RB_BodyParts[i].AddForce(force * Mass_BodyParts[i], Rigidbody.ForceMode.Impulse);
        }
        if(diveTackle)
        {
            RB_BodyParts[(int)BODYPART.RIGHT_LOWER_LEG].AddForce(force * 0.3f, Rigidbody.ForceMode.Impulse);
            RB_BodyParts[(int)BODYPART.LEFT_LOWER_LEG].AddForce(force * 0.3f, Rigidbody.ForceMode.Impulse);
        }

    }
    public float DistanceToWorld()
    {
        return ImpactSpine.DistanceToCollision;
    }

    public void Smack()
    {
        if(RagdollSound)
            RagdollSound.Play();
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

        // Play the ragdoll sound
        if(identity.Owner && RagdollSound)
            RagdollSound.PlayOneShot();

        //enable all GameObjects
        foreach(GameObject gObj in  G_BodyParts)
        {
            gObj.activeSelf = true;
        }

        RagdollEnabled = true;
    }



    public override void Awake()
    {

        Mass_BodyParts[(int)BODYPART.HIPS] = 0.15f;
        Mass_BodyParts[(int)BODYPART.SPINE] = 0.35f;
        Mass_BodyParts[(int)BODYPART.HEAD] = 0.1f;
        Mass_BodyParts[(int)BODYPART.LEFT_UPPER_ARM] = 0.03f;
        Mass_BodyParts[(int)BODYPART.RIGHT_UPPER_ARM] = 0.03f;
        Mass_BodyParts[(int)BODYPART.LEFT_LOWER_ARM] = 0.02f;
        Mass_BodyParts[(int)BODYPART.RIGHT_LOWER_ARM] = 0.02f;
        Mass_BodyParts[(int)BODYPART.LEFT_UPPER_LEG] = 0.1f;
        Mass_BodyParts[(int)BODYPART.RIGHT_UPPER_LEG] = 0.1f;
        Mass_BodyParts[(int)BODYPART.LEFT_LOWER_LEG] = 0.05f;
        Mass_BodyParts[(int)BODYPART.RIGHT_LOWER_LEG] = 0.05f;


        string[] BoneNames = {
            Hips, Spine, Head,
            UpperLeftArm, UpperRightArm, LowerLeftArm, LowerRightArm,
            UpperLeftLeg, UpperRightLeg, LowerLeftLeg, LowerRightLeg
        };




        RenderSkinnedComponent skinn = gameObject.GetComponent<RenderSkinnedComponent>();

        for (int i =0; i < (int)BODYPART.COUNT; i++)
        {
            BODYPART part = (BODYPART)i;
            G_BodyParts[i] = new GameObject(part.ToString());
            G_BodyParts[i].transform.SetParent(gameObject.transform);
            if(i != (int)BODYPART.SPINE)
            {
                J_BodyParts[i] = G_BodyParts[i].AddComponent<Joint>();
                J_BodyParts[i].NoCollision = true;
            }
                

            
            BT_BodyParts[i] = G_BodyParts[i].AddComponent<BoneTransformComponent>();

            BT_BodyParts[i].BoneName = BoneNames[i];
            BT_BodyParts[i].AnimatedObject = gameObject;
            skinn.FetchBoneIndex(Utility.hash(BoneNames[i]), out BoneIndexes[i]);
            G_BodyParts[i].transform.local_world = skinn.GetLocalBoneMatrix((int)BoneIndexes[i]);

        }

        Vector3 center;
        //Hips
        SphereCollider hipsCollider = G_BodyParts[(int)BODYPART.HIPS].AddComponent<SphereCollider>();
        hipsCollider.radius = 0.2f;
        C_BodyParts[(int)BODYPART.HIPS] = hipsCollider;


        //Spine

        BoxCollider spineCollider = G_BodyParts[(int)BODYPART.SPINE].AddComponent<BoxCollider>();
        center = calculatePosbetweenTwoSkeletonschanges(Spine, Neck, skinn);
        center.x = 0;
        center.z = 0;
        spineCollider.center = center;
        spineCollider.size = new Vector3(center.y, center.y, center.y);
        C_BodyParts[(int)BODYPART.SPINE] = spineCollider;

        //Head
        SphereCollider headCollider = G_BodyParts[(int)BODYPART.HEAD].AddComponent<SphereCollider>();
        headCollider.radius = 0.2f;
        center = calculatePosbetweenTwoSkeletonschanges(Neck, Head, skinn);
        center.x = 0;
        center.z = 0;
        headCollider.center = center;


        C_BodyParts[(int)BODYPART.HEAD] = headCollider;

        for (int i = (int)BODYPART.LEFT_UPPER_ARM; i <= (int)BODYPART.RIGHT_LOWER_ARM; i++)
        {
            CapsuleCollider c = G_BodyParts[i].AddComponent<CapsuleCollider>();
            if(PostiveMapping)
            { 
                c.rotation = CapsuleCollider.ColliderRotation.RotateY;
            }
            else
            { 
                c.rotation = CapsuleCollider.ColliderRotation.RotateX;
            }
            c.radius = 0.065f;
            C_BodyParts[i] = c;

            J_BodyParts[i].SwingAngle1 = 90;
            J_BodyParts[i].SwingAngle2 = 90;
            J_BodyParts[i].SwingAxis = Vector3.Zero;
        }

        for (int i = (int)BODYPART.LEFT_UPPER_LEG; i <= (int)BODYPART.RIGHT_LOWER_LEG; i++)
        {
            CapsuleCollider c = G_BodyParts[i].AddComponent<CapsuleCollider>();
            c.radius = 0.065f;
            C_BodyParts[i] = c;

            J_BodyParts[i].SwingAngle1 = 90;
            J_BodyParts[i].SwingAngle2 = 90;
            J_BodyParts[i].SwingAxis = Vector3.Zero;
        }

        for (int i = 0; i < (int)BODYPART.COUNT; i++)
        {
            RB_BodyParts[i] = G_BodyParts[i].AddComponent<Rigidbody>();

            RB_BodyParts[i].IsKinematic = AllobjectKinectic;
            RB_BodyParts[i].Damping = AllobjectDamping;
            RB_BodyParts[i].AngularDamping = 0.85f;
            RB_BodyParts[i].Mass = Totalmass * Mass_BodyParts[i];
            RB_BodyParts[i].ActiveState = Rigidbody.ActivationState.Always_Active;
        }

        J_BodyParts[(int)BODYPART.HIPS].ConnectedRigidbody = RB_BodyParts[(int)BODYPART.SPINE];
        J_BodyParts[(int)BODYPART.HEAD].ConnectedRigidbody = RB_BodyParts[(int)BODYPART.SPINE];

        J_BodyParts[(int)BODYPART.LEFT_LOWER_ARM].ConnectedRigidbody = RB_BodyParts[(int)BODYPART.LEFT_UPPER_ARM];
        J_BodyParts[(int)BODYPART.LEFT_UPPER_ARM].ConnectedRigidbody = RB_BodyParts[(int)BODYPART.SPINE];

        J_BodyParts[(int)BODYPART.RIGHT_LOWER_ARM].ConnectedRigidbody = RB_BodyParts[(int)BODYPART.RIGHT_UPPER_ARM];
        J_BodyParts[(int)BODYPART.RIGHT_UPPER_ARM].ConnectedRigidbody = RB_BodyParts[(int)BODYPART.SPINE];

        J_BodyParts[(int)BODYPART.LEFT_LOWER_LEG].ConnectedRigidbody = RB_BodyParts[(int)BODYPART.LEFT_UPPER_LEG];
        J_BodyParts[(int)BODYPART.LEFT_UPPER_LEG].ConnectedRigidbody = RB_BodyParts[(int)BODYPART.HIPS];

        J_BodyParts[(int)BODYPART.RIGHT_LOWER_LEG].ConnectedRigidbody = RB_BodyParts[(int)BODYPART.RIGHT_UPPER_LEG];
        J_BodyParts[(int)BODYPART.RIGHT_UPPER_LEG].ConnectedRigidbody = RB_BodyParts[(int)BODYPART.HIPS];


        Vector3 ExtraVector = new Vector3(0);



        //Joint from hips to Spine
        J_BodyParts[(int)BODYPART.HIPS].Axis = new Vector3(0, 0, -90);
        J_BodyParts[(int)BODYPART.HIPS].SwingAxis = new Vector3(0, 0, -90);
        J_BodyParts[(int)BODYPART.HIPS].NoCollision = true;
        J_BodyParts[(int)BODYPART.HIPS].SwingAngle1 = 64;
        J_BodyParts[(int)BODYPART.HIPS].SwingAngle2 = 90;
        J_BodyParts[(int)BODYPART.HIPS].ConnectedAnchor = -spineCollider.center;


        //Head



        //Joint from spine to head
        J_BodyParts[(int)BODYPART.HEAD].Axis = new Vector3(0, 0, 90);
        J_BodyParts[(int)BODYPART.HEAD].SwingAxis = new Vector3(0, 0, 90);
        J_BodyParts[(int)BODYPART.HEAD].NoCollision = true;
        J_BodyParts[(int)BODYPART.HEAD].SwingAngle1 = 90;
        J_BodyParts[(int)BODYPART.HEAD].SwingAngle2 = 90;
        J_BodyParts[(int)BODYPART.HEAD].ConnectedAnchor = headCollider.center + calculatePosbetweenTwoSkeletonschanges(Spine, Neck, skinn);
        if (PostiveMapping)
        {
            J_BodyParts[(int)BODYPART.HEAD].SwingAngle1 = 64;
            J_BodyParts[(int)BODYPART.HEAD].SwingAngle2 = 59;

            J_BodyParts[(int)BODYPART.HEAD].NoCollision = true;
            ExtraVector = -J_BodyParts[(int)BODYPART.HEAD].ConnectedAnchor;
            ExtraVector.x = -ExtraVector.x;
            ExtraVector.y = -ExtraVector.y;
            ExtraVector.z = ExtraVector.z + 0.1f; //* 3;
            J_BodyParts[(int)BODYPART.HEAD].ConnectedAnchor = ExtraVector;
        }

        //left arm

        CapsuleCollider limbCollider;

        limbCollider = (C_BodyParts[(int)BODYPART.LEFT_UPPER_ARM] as CapsuleCollider);
        limbCollider.height = calculateLengthBetweenSkeleton(UpperLeftArm, LowerLeftArm, skinn);



        center = calculatePosbetweenTwoSkeletonschanges(UpperLeftArm, LowerLeftArm, skinn);
        center.x = 0;
        center.z = 0;
        float value = calculateLengthBetweenSkeleton(UpperLeftArm, LowerLeftArm, skinn);
        if (PostiveMapping)
            center.y = value * 0.5f;
        else
            center.y = -value * 0.5f;
        limbCollider.center = center;

        //Joint from leftarm totorso 
        J_BodyParts[(int)BODYPART.LEFT_UPPER_ARM].Axis = new Vector3(-90, 0, 0);
        J_BodyParts[(int)BODYPART.LEFT_UPPER_ARM].SwingAxis = new Vector3(0, 0, 0);
        J_BodyParts[(int)BODYPART.LEFT_UPPER_ARM].NoCollision = true;
        J_BodyParts[(int)BODYPART.LEFT_UPPER_ARM].SwingAngle1 = 90;
        J_BodyParts[(int)BODYPART.LEFT_UPPER_ARM].SwingAngle2 = 90;
        ExtraVector = calculatePosbetweenTwoSkeletonschanges(LeftShoulder, UpperLeftArm, skinn) + calculatePosbetweenTwoSkeletonschanges(Spine, UpperLeftArm, skinn);
        ExtraVector.z = Math.Abs(ExtraVector.z);
        J_BodyParts[(int)BODYPART.LEFT_UPPER_ARM].ConnectedAnchor = ExtraVector;
        J_BodyParts[(int)BODYPART.LEFT_UPPER_ARM].Anchor = -limbCollider.center;
        if(PostiveMapping)
        {
            J_BodyParts[(int)BODYPART.LEFT_UPPER_ARM].Axis = new Vector3(0, -2.5f, 45);
            J_BodyParts[(int)BODYPART.LEFT_UPPER_ARM].SwingAxis = new Vector3(0, 2.5f, -45);
            J_BodyParts[(int)BODYPART.LEFT_UPPER_ARM].SwingAngle1 = 45;
            J_BodyParts[(int)BODYPART.LEFT_UPPER_ARM].SwingAngle2 = 48;
            ExtraVector.x = -ExtraVector.x;
            J_BodyParts[(int)BODYPART.LEFT_UPPER_ARM].ConnectedAnchor = ExtraVector;
        }



        //left under arm
        limbCollider = (C_BodyParts[(int)BODYPART.LEFT_LOWER_ARM] as CapsuleCollider);
        limbCollider.height = calculateLengthBetweenSkeleton(LowerLeftArm, LowerLeftHand, skinn);

        center = -SwapXY(calculatePosbetweenTwoSkeletonschanges(LowerLeftArm, LowerLeftHand, skinn));
        center.x = 0;
        center.z = 0;
        limbCollider.center = center;
        value = calculateLengthBetweenSkeleton(LowerLeftArm, LowerLeftHand, skinn);
        if (PostiveMapping)
            center.y = value * 0.5f;
        else
            center.y = -value * 0.5f;
        limbCollider.center = center;

        //Joint from leftudnerarmtooverarm
        J_BodyParts[(int)BODYPART.LEFT_LOWER_ARM].Axis = new Vector3(-90, 0, 0);
        J_BodyParts[(int)BODYPART.LEFT_LOWER_ARM].SwingAxis = new Vector3(0, 0, 0);
        J_BodyParts[(int)BODYPART.LEFT_LOWER_ARM].NoCollision = true;
        J_BodyParts[(int)BODYPART.LEFT_LOWER_ARM].SwingAngle1 = 90;
        J_BodyParts[(int)BODYPART.LEFT_LOWER_ARM].SwingAngle2 = 90;
        J_BodyParts[(int)BODYPART.LEFT_LOWER_ARM].Anchor = -limbCollider.center;
        J_BodyParts[(int)BODYPART.LEFT_LOWER_ARM].ConnectedAnchor = new Vector3(calculateLengthBetweenSkeleton(UpperLeftArm, LowerLeftArm, skinn),
            (-calculateLengthBetweenSkeleton(LeftShoulder, UpperLeftArm, skinn) + calculateLengthBetweenSkeleton(Spine, UpperLeftArm, skinn)) * 0.5f, 0);
        if(PostiveMapping)
        {
            ExtraVector = J_BodyParts[(int)BODYPART.LEFT_LOWER_ARM].ConnectedAnchor;
            ExtraVector.x = 0;
            J_BodyParts[(int)BODYPART.LEFT_LOWER_ARM].ConnectedAnchor = ExtraVector;
        }

        //Right arm
        limbCollider = (C_BodyParts[(int)BODYPART.RIGHT_UPPER_ARM] as CapsuleCollider);
        limbCollider.height = calculateLengthBetweenSkeleton(UpperRightArm, LowerRightArm, skinn);
        center = -SwapXY(calculatePosbetweenTwoSkeletonschanges(UpperRightArm, LowerRightArm, skinn));
        center.x = 0;
        center.z = 0;
        limbCollider.center = center;
        value = calculateLengthBetweenSkeleton(UpperRightArm, LowerRightArm, skinn);
        center.y = value * 0.5f;
        limbCollider.center = center;

        //Joint from Rightarm totorso
        J_BodyParts[(int)BODYPART.RIGHT_UPPER_ARM].Axis = new Vector3(0, 0, 0);
        J_BodyParts[(int)BODYPART.RIGHT_UPPER_ARM].SwingAxis = new Vector3(0, 0, 0);
        J_BodyParts[(int)BODYPART.RIGHT_UPPER_ARM].NoCollision = true;
        J_BodyParts[(int)BODYPART.RIGHT_UPPER_ARM].SwingAngle1 = 90;
        J_BodyParts[(int)BODYPART.RIGHT_UPPER_ARM].SwingAngle2 = 90;
        ExtraVector = calculatePosbetweenTwoSkeletonschanges(RightShoulder, UpperRightArm, skinn) + calculatePosbetweenTwoSkeletonschanges(Spine, UpperRightArm, skinn);
        ExtraVector.z = Math.Abs(ExtraVector.z);
        J_BodyParts[(int)BODYPART.RIGHT_UPPER_ARM].ConnectedAnchor = ExtraVector;
        J_BodyParts[(int)BODYPART.RIGHT_UPPER_ARM].Anchor = -limbCollider.center;
        if (PostiveMapping)
        {
            J_BodyParts[(int)BODYPART.RIGHT_UPPER_ARM].Axis = new Vector3(0, -2.5f, 135);
            J_BodyParts[(int)BODYPART.RIGHT_UPPER_ARM].SwingAxis = new Vector3(0, 13.5f, -135);
            J_BodyParts[(int)BODYPART.RIGHT_UPPER_ARM].SwingAngle1 = 66;
            J_BodyParts[(int)BODYPART.RIGHT_UPPER_ARM].SwingAngle2 = 55;
            ExtraVector.y = -ExtraVector.y;
            J_BodyParts[(int)BODYPART.RIGHT_UPPER_ARM].ConnectedAnchor = -ExtraVector;
            ExtraVector = -ExtraVector;
        }

        if (PostiveMapping)
        {
            Vector3 Copydatahere = new Vector3();

            Copydatahere = J_BodyParts[(int)BODYPART.LEFT_UPPER_ARM].ConnectedAnchor;
            
            Copydatahere.z = J_BodyParts[(int)BODYPART.RIGHT_UPPER_ARM].ConnectedAnchor.z * 2;
            ExtraVector = J_BodyParts[(int)BODYPART.LEFT_UPPER_ARM].ConnectedAnchor;
            J_BodyParts[(int)BODYPART.LEFT_UPPER_ARM].ConnectedAnchor = Copydatahere;
            Copydatahere = J_BodyParts[(int)BODYPART.RIGHT_UPPER_ARM].ConnectedAnchor;
            Copydatahere.z = ExtraVector.z*2;
            J_BodyParts[(int)BODYPART.RIGHT_UPPER_ARM].ConnectedAnchor = Copydatahere;
        }
        


        //RightUnderArm
        limbCollider = (C_BodyParts[(int)BODYPART.RIGHT_LOWER_ARM] as CapsuleCollider);
        limbCollider.height = calculateLengthBetweenSkeleton(LowerRightArm, LowerRightHand, skinn);

        center = -SwapXY(calculatePosbetweenTwoSkeletonschanges(LowerRightArm, LowerRightHand, skinn));
        center.x = 0;
        center.z = 0;
        limbCollider.center = center;
        value = calculateLengthBetweenSkeleton(LowerRightArm, LowerRightHand, skinn);
        center.y = value * 0.5f;
        limbCollider.center = center;

        //Joint from Rightudnerarmtooverarm
        J_BodyParts[(int)BODYPART.RIGHT_LOWER_ARM].Axis = new Vector3(-90, 0, 0);
        J_BodyParts[(int)BODYPART.RIGHT_LOWER_ARM].SwingAxis = new Vector3(0, 0, 0);
        J_BodyParts[(int)BODYPART.RIGHT_LOWER_ARM].NoCollision = true;
        J_BodyParts[(int)BODYPART.RIGHT_LOWER_ARM].SwingAngle1 = 90;
        J_BodyParts[(int)BODYPART.RIGHT_LOWER_ARM].SwingAngle2 = 90;
        J_BodyParts[(int)BODYPART.RIGHT_LOWER_ARM].Anchor = -limbCollider.center;
        if(PostiveMapping)
            J_BodyParts[(int)BODYPART.RIGHT_LOWER_ARM].ConnectedAnchor = -new Vector3(0, (-calculateLengthBetweenSkeleton(RightShoulder, UpperRightArm, skinn) + calculateLengthBetweenSkeleton(Spine, UpperRightArm, skinn)) * -0.5f, 0);
        else
            J_BodyParts[(int)BODYPART.RIGHT_LOWER_ARM].ConnectedAnchor = -new Vector3(calculateLengthBetweenSkeleton(UpperRightArm, LowerRightArm, skinn), (-calculateLengthBetweenSkeleton(RightShoulder, UpperRightArm, skinn) + calculateLengthBetweenSkeleton(Spine, UpperRightArm, skinn)) * 0.5f, 0);

        //RightLeg
        limbCollider = (C_BodyParts[(int)BODYPART.RIGHT_UPPER_LEG] as CapsuleCollider);
        limbCollider.height = calculateLengthBetweenSkeleton(UpperRightLeg, LowerRightLeg, skinn);
        limbCollider.radius = 0.065f;

        center = calculatePosbetweenTwoSkeletonschanges(UpperRightLeg, LowerRightLeg, skinn);
        center.x = 0;
        center.z = 0;
        limbCollider.center = center;
        value = calculateLengthBetweenSkeleton(UpperRightLeg, LowerRightLeg, skinn);
        if (PostiveMapping)
            center.y = value * 0.5f;
        else
            center.y = -value * 0.5f;
        limbCollider.center = center;

        //Joint from RightLegJoint
        J_BodyParts[(int)BODYPART.RIGHT_UPPER_LEG].Axis = new Vector3(0, 0, 90);
        J_BodyParts[(int)BODYPART.RIGHT_UPPER_LEG].SwingAxis = new Vector3(0, 0, 0);
        J_BodyParts[(int)BODYPART.RIGHT_UPPER_LEG].NoCollision = true;
        J_BodyParts[(int)BODYPART.RIGHT_UPPER_LEG].SwingAngle1 = 90;
        J_BodyParts[(int)BODYPART.RIGHT_UPPER_LEG].SwingAngle2 = 10;
        J_BodyParts[(int)BODYPART.RIGHT_UPPER_LEG].TwistAngle = 60;
        J_BodyParts[(int)BODYPART.RIGHT_UPPER_LEG].Anchor = new Vector3(0, calculateLengthBetweenSkeleton(UpperRightLeg, LowerRightLeg, skinn) * 0.5f, 0);
        J_BodyParts[(int)BODYPART.RIGHT_UPPER_LEG].ConnectedAnchor = calculatePosbetweenTwoSkeletonschanges(Hips, UpperRightLeg, skinn) * 2;

        if(PostiveMapping)
        {
            J_BodyParts[(int)BODYPART.RIGHT_UPPER_LEG].SwingAxis = new Vector3(0, 0, 180);
            J_BodyParts[(int)BODYPART.RIGHT_UPPER_LEG].SwingAngle2 = 5;
            J_BodyParts[(int)BODYPART.RIGHT_UPPER_LEG].TwistAngle = 90;
            J_BodyParts[(int)BODYPART.RIGHT_UPPER_LEG].Axis = new Vector3(0, 180, 0);
            J_BodyParts[(int)BODYPART.RIGHT_UPPER_LEG].Anchor = -J_BodyParts[(int)BODYPART.RIGHT_UPPER_LEG].Anchor;
            J_BodyParts[(int)BODYPART.RIGHT_UPPER_LEG].ConnectedAnchor = new Vector3(-J_BodyParts[(int)BODYPART.RIGHT_UPPER_LEG].ConnectedAnchor.x, J_BodyParts[(int)BODYPART.RIGHT_UPPER_LEG].ConnectedAnchor.y, J_BodyParts[(int)BODYPART.RIGHT_UPPER_LEG].ConnectedAnchor.z);
        }

        //RightUnderLeg
        limbCollider = (C_BodyParts[(int)BODYPART.RIGHT_LOWER_LEG] as CapsuleCollider);
        limbCollider.height = calculateLengthBetweenSkeleton(LowerRightLeg, RightFoot, skinn);
        limbCollider.radius = 0.065f;

        center = calculatePosbetweenTwoSkeletonschanges(LowerRightLeg, RightFoot, skinn);
        center.x = 0;
        center.z = 0;
        limbCollider.center = center;
        value = calculateLengthBetweenSkeleton(LowerRightLeg, RightFoot, skinn);
        if (PostiveMapping)
            center.y = value * 0.5f;
        else
            center.y = -value * 0.5f;
        limbCollider.center = center;

        //RightUnderLegJoint
        J_BodyParts[(int)BODYPART.RIGHT_LOWER_LEG].Axis = new Vector3(0, 0, 0);
        J_BodyParts[(int)BODYPART.RIGHT_LOWER_LEG].SwingAxis = new Vector3(0, 0, 0);
        J_BodyParts[(int)BODYPART.RIGHT_LOWER_LEG].NoCollision = true;
        J_BodyParts[(int)BODYPART.RIGHT_LOWER_LEG].SwingAngle1 = 90;
        J_BodyParts[(int)BODYPART.RIGHT_LOWER_LEG].SwingAngle2 = 10;
        J_BodyParts[(int)BODYPART.RIGHT_LOWER_LEG].TwistAngle = 53;
        J_BodyParts[(int)BODYPART.RIGHT_LOWER_LEG].ConnectedAnchor = limbCollider.center;
        J_BodyParts[(int)BODYPART.RIGHT_LOWER_LEG].Anchor = -limbCollider.center;




            //left leg
            limbCollider = (C_BodyParts[(int)BODYPART.LEFT_UPPER_LEG] as CapsuleCollider);
        limbCollider.height = calculateLengthBetweenSkeleton(UpperLeftLeg, LowerLeftLeg, skinn);
        limbCollider.radius = 0.065f;

        center = calculatePosbetweenTwoSkeletonschanges(UpperLeftLeg, LowerLeftLeg, skinn);
        center.x = 0;
        center.z = 0;
        limbCollider.center = center;
        value = calculateLengthBetweenSkeleton(UpperLeftLeg, LowerLeftLeg, skinn);
        if (PostiveMapping)
            center.y = value * 0.5f;
        else
            center.y = -value * 0.5f;
        limbCollider.center = center;

        //Joint from LeftLegJoint
        J_BodyParts[(int)BODYPART.LEFT_UPPER_LEG].Axis = new Vector3(0, 0, -90);
        J_BodyParts[(int)BODYPART.LEFT_UPPER_LEG].SwingAxis = new Vector3(0, 0, 0);
        J_BodyParts[(int)BODYPART.LEFT_UPPER_LEG].NoCollision = true;
        J_BodyParts[(int)BODYPART.LEFT_UPPER_LEG].SwingAngle1 = 90;
        J_BodyParts[(int)BODYPART.LEFT_UPPER_LEG].SwingAngle2 = 10;
        J_BodyParts[(int)BODYPART.LEFT_UPPER_LEG].TwistAngle = 60;
        J_BodyParts[(int)BODYPART.LEFT_UPPER_LEG].Anchor = new Vector3(0, calculateLengthBetweenSkeleton(UpperLeftLeg, LowerLeftLeg, skinn) * 0.5f, 0);
        J_BodyParts[(int)BODYPART.LEFT_UPPER_LEG].ConnectedAnchor = calculatePosbetweenTwoSkeletonschanges(Hips, UpperLeftLeg, skinn) * 2;

        if (PostiveMapping)
        {

            J_BodyParts[(int)BODYPART.LEFT_UPPER_LEG].SwingAxis = new Vector3(0, 0, 180);
            J_BodyParts[(int)BODYPART.LEFT_UPPER_LEG].SwingAngle2 = 5;
            J_BodyParts[(int)BODYPART.LEFT_UPPER_LEG].TwistAngle = 90;
            J_BodyParts[(int)BODYPART.LEFT_UPPER_LEG].Axis = new Vector3(0, 180, 0);
          //  J_BodyParts[(int)BODYPART.LEFT_UPPER_LEG].Axis = new Vector3(0, 0, 0);
  //          J_BodyParts[(int)BODYPART.LEFT_UPPER_LEG].Swing = new Vector3(0, -90, -225);
            J_BodyParts[(int)BODYPART.LEFT_UPPER_LEG].Anchor = -J_BodyParts[(int)BODYPART.LEFT_UPPER_LEG].Anchor;
            J_BodyParts[(int)BODYPART.LEFT_UPPER_LEG].ConnectedAnchor = new Vector3(-J_BodyParts[(int)BODYPART.LEFT_UPPER_LEG].ConnectedAnchor.x, J_BodyParts[(int)BODYPART.LEFT_UPPER_LEG].ConnectedAnchor.y, J_BodyParts[(int)BODYPART.LEFT_UPPER_LEG].ConnectedAnchor.z);
        }


        //LeftUnderLeg
        limbCollider = (C_BodyParts[(int)BODYPART.LEFT_LOWER_LEG] as CapsuleCollider);
        limbCollider.height = calculateLengthBetweenSkeleton(LowerLeftLeg, LeftFoot, skinn);
        limbCollider.radius = 0.065f;

        center = calculatePosbetweenTwoSkeletonschanges(LowerLeftLeg, LeftFoot, skinn);
        center.x = 0;
        center.z = 0;
        limbCollider.center = center;
        value = calculateLengthBetweenSkeleton(LowerLeftLeg, LeftFoot, skinn);
        if (PostiveMapping)
            center.y = value * 0.5f;
        else
            center.y = -value * 0.5f;
        limbCollider.center = center;

        //leftUnderLegJoint
        J_BodyParts[(int)BODYPART.LEFT_LOWER_LEG].Axis = new Vector3(0, 0, 0);
        J_BodyParts[(int)BODYPART.LEFT_LOWER_LEG].SwingAxis = new Vector3(0, 0, 0);
        J_BodyParts[(int)BODYPART.LEFT_LOWER_LEG].NoCollision = true;
        J_BodyParts[(int)BODYPART.LEFT_LOWER_LEG].SwingAngle1 = 90;
        J_BodyParts[(int)BODYPART.LEFT_LOWER_LEG].SwingAngle2 = 10;
        J_BodyParts[(int)BODYPART.LEFT_LOWER_LEG].TwistAngle = 53;
        J_BodyParts[(int)BODYPART.LEFT_LOWER_LEG].ConnectedAnchor = limbCollider.center;
        J_BodyParts[(int)BODYPART.LEFT_LOWER_LEG].Anchor = -limbCollider.center;


        ImpactSpine = G_BodyParts[(int)BODYPART.SPINE].AddComponent<RagdollImpact>();
        ImpactSpine.G_BodyParts = G_BodyParts;
    }

    public GameObject GetHips()
    {
        return G_BodyParts[(int)BODYPART.SPINE];
    }

    public override void Update()
    {
        Vector3 spinepos = G_BodyParts[(int)BODYPART.SPINE].transform.position;


        if (RagdollEnabled)
        {

            if (ImpactSpine.GetActive && identity.Owner && RagdollSound)
            {
                RagdollSound.Volume = ImpactSpine.Volume;
                RagdollSound.Play();
            }
                
            return;
        }

        RenderSkinnedComponent skin = gameObject.GetComponent<RenderSkinnedComponent>();

        for(int i = 0; i < (int)BODYPART.COUNT; i++)
        {
            G_BodyParts[i].transform.local_world = skin.GetLocalBoneMatrix((int)BoneIndexes[i]);
        }
    }

    public override void OnDestroy()
    {
        DisableRagdoll();
    }

    public void DisableRagdoll()
    {

        foreach(GameObject gObj in G_BodyParts)
        {
            if(gObj != null)
                gObj.activeSelf = false;
        }
        RagdollEnabled = false;

    }
    

}
