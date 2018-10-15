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
        GameObject G_Hips;
        GameObject G_Spine;
       

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
            G_Hips.transform.local_world = renderskinnedcomponent.GetLocalBoneMatrix((int)boneindex);
            SphereCollider spherecolliderhips = G_Hips.AddComponent<SphereCollider>();
            spherecolliderhips.radius = 0.2f*(1/G_Hips.transform.localScale.x);
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
            G_Spine.transform.local_world = renderskinnedcomponent.GetLocalBoneMatrix((int)boneindex);
            boxcolliderSpine.center = calculatePosbetweenTwoSkeletonschanges(Spine, Neck,renderskinnedcomponent); //renderskinnedcomponent.GetBoneMatrix((int)boneindex).Translation;
            boxcolliderSpine.size = new Vector3(0.2f,0.2f,0.2f) * (1 / G_Hips.transform.localScale.x);
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
            return DivideComponent(Pos, G_Hips.transform.localScale);
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
