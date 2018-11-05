using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ThomasEngine;

public class RagdollImpact : ScriptComponent
{
    public bool GetActive = false;
    public float Volume;
    public float DistanceToCollition;
    Ray ray;
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

    public GameObject[] G_BodyParts = new GameObject[(int)BODYPART.COUNT];
    bool Bodypartcheck = true;
    public override void OnCollisionEnter(Collider collider)
    {
        for (int i = 0; i < (int)BODYPART.COUNT; i++)
        {
            if (collider.gameObject == G_BodyParts[i])
            {
                Bodypartcheck = false;
                break;
            }
        }

        if (Bodypartcheck)
        {
            Volume = Math.Abs(gameObject.GetComponent<Rigidbody>().LinearVelocity.x) + Math.Abs(gameObject.GetComponent<Rigidbody>().LinearVelocity.y) + Math.Abs(gameObject.GetComponent<Rigidbody>().LinearVelocity.z);
            GetActive = true;
        }
    
        Bodypartcheck = true;
    }
    public override void Update()
    {
        RaycastHit hit;
        if (Physics.Raycast(ray, out hit))
        {
            DistanceToCollition = hit.distance;
           
        }
        GetActive = false;
    }
    public override void OnDrawGizmos()
    {
        ray = new Ray(transform.position, new Vector3(0,-90,0));
        Gizmos.SetMatrix(Matrix.Identity);
    //    Gizmos.SetColor(Color.Red);
    //    Gizmos.DrawRay(ref ray);
    }



}