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
    public GameObject SoundImpactObject;
    public float Volume;

    public override void OnCollisionEnter(Collider collider)
    {
        if (collider.gameObject == SoundImpactObject)
        {
            Volume = Math.Abs(gameObject.GetComponent<Rigidbody>().LinearVelocity.x) + Math.Abs(gameObject.GetComponent<Rigidbody>().LinearVelocity.y) + Math.Abs(gameObject.GetComponent<Rigidbody>().LinearVelocity.z);
            GetActive = true;
        }
        else
            GetActive = false;
    }
    public override void Update()
    {
        GetActive = false;
    }
}