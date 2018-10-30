using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ThomasEngine;

public class RagdollImpact : ScriptComponent
{

    public GameObject SoundImpactObject { get; set; }

    public override void OnCollisionEnter(Collider collider)
    {
        if (collider.gameObject == SoundImpactObject)
        {
            Debug.Log(gameObject.GetComponent<Rigidbody>().LinearVelocity.x + " " + gameObject.GetComponent<Rigidbody>().LinearVelocity.y + " " + gameObject.GetComponent<Rigidbody>().LinearVelocity.z);
        }
    }
}