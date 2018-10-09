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
        public override void Start()
        {
           
             gameObject.AddComponent<LightComponent>();
            //    GameObject RagdollCharacter = new GameObject("RagdollCharacter");
            //    RenderSkinnedComponent Skeleton =  RagdollCharacter.AddComponent<RenderSkinnedComponent>(); 
            
        }

        public override void Update()
        {
        }
        public override void OnEnable()
        {
           
        }
        public override void Awake()
        {
            //
        }
        public override void OnDisable()
        {
            
        }

    }
}
