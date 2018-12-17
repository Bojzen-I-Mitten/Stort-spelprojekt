using ThomasEngine;
using ThomasEngine.Network;
using System.Collections;
using System.Linq;

public class Confetti : ScriptComponent
{
    private ParticleEmitter emitterConfetti1;
    private ParticleEmitter emitterConfetti2;
  //  private ParticleEmitter emitterConfetti3;
    

    public override void Start()
    {
        emitterConfetti1 = gameObject.AddComponent<ParticleEmitter>();
        emitterConfetti2 = gameObject.AddComponent<ParticleEmitter>();
       // emitterConfetti3 = gameObject.AddComponent<ParticleEmitter>();

        Texture2D confettiTex1 = (Texture2D)Resources.LoadThomasPath("%THOMAS_ASSETS%/Particles/confetti1_particle.png");
        if (confettiTex1 != null)
            emitterConfetti1.Texture = confettiTex1;
        emitterConfetti1.MinSize = 0.03f;
        emitterConfetti1.MaxSize = 0.15f;
        emitterConfetti1.EndSize = 0.15f;
        emitterConfetti1.MinLifeTime = 1.0f;
        emitterConfetti1.MaxLifeTime = 1.5f;
        emitterConfetti1.EmissionRate = 100;
        emitterConfetti1.MinRotationSpeed = -50.0f;
        emitterConfetti1.MaxRotationSpeed = 50.0f;
        emitterConfetti1.MinSpeed = 7.5f;
        emitterConfetti1.MaxSpeed = 15.0f;
        emitterConfetti1.EndSpeed = 0.0f;
        emitterConfetti1.DistanceFromSphereCenter = 20;
        emitterConfetti1.Radius = 0.4f;
        emitterConfetti1.Gravity = 5.0f;
        emitterConfetti1.SpawnAtEdge = true;

        Texture2D confettiTex2 = (Texture2D)Resources.LoadThomasPath("%THOMAS_ASSETS%/Particles/confetti2_particle.png");
        if (confettiTex2 != null)
            emitterConfetti2.Texture = confettiTex2;
        emitterConfetti2.MinSize = 0.03f;
        emitterConfetti2.MaxSize = 0.15f;
        emitterConfetti2.EndSize = 0.15f;
        emitterConfetti2.MinLifeTime = 0.8f;
        emitterConfetti2.MaxLifeTime = 1.3f;
        emitterConfetti2.EmissionRate = 125;
        emitterConfetti2.MinRotationSpeed = -25.0f;
        emitterConfetti2.MaxRotationSpeed = 25.0f;
        emitterConfetti2.MinSpeed = 12.0f;
        emitterConfetti2.MaxSpeed = 20.0f;
        emitterConfetti2.EndSpeed = 0.0f;
        emitterConfetti2.DistanceFromSphereCenter = 8;
        emitterConfetti2.Radius = 1.0f;
        emitterConfetti2.Gravity = 5.0f;
        emitterConfetti2.SpawnAtEdge = false;

        /*emitterConfetti3.Texture = particleTexture3;
        emitterConfetti3.MinSize = 0.10f;
        emitterConfetti3.MaxSize = 0.25f;
        emitterConfetti3.EndSize = 0.25f;
        emitterConfetti3.MinLifeTime = 0.8f;
        emitterConfetti3.MaxLifeTime = 1.3f;
        emitterConfetti3.EmissionRate = 175;
        emitterConfetti3.MinRotationSpeed = -35.0f;
        emitterConfetti3.MaxRotationSpeed = 35.0f;
        emitterConfetti3.MinSpeed = 13.0f;
        emitterConfetti3.MaxSpeed = 25.0f;
        emitterConfetti3.EndSpeed = 0.0f;
        emitterConfetti3.DistanceFromSphereCenter = 8;
        emitterConfetti3.Radius = 0.7f;
        emitterConfetti3.Gravity = 5.0f;
        emitterConfetti3.SpawnAtEdge = false;
        */
        
    }

    public override void Update()
    {
        //bool hasScored = MatchSystem.instance.hasScored;
        //if(hasScored)
        //{
        //    emitterConfetti1.Emit = true;
        //    emitterConfetti2.Emit = true;
        //    emitterConfetti3.Emit = true;
        //}


    }

    public void Emit(float duration)
    {
        StartCoroutine(EmitForDuration(duration));
    }

    private IEnumerator EmitForDuration(float duration)
    {
        emitterConfetti1.Emit = true;
        emitterConfetti2.Emit = true;
      //  emitterConfetti3.Emit = true;

        yield return new WaitForSeconds(duration);

        emitterConfetti1.Emit = false;
        emitterConfetti2.Emit = false;
       // emitterConfetti3.Emit = false;
    }
}
