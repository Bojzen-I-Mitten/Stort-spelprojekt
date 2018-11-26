using ThomasEngine;
using ThomasEngine.Network;
using ThomasEngine.Script;
using System.Collections;
using System.Collections.Generic;

public class TeamGoal : ScriptComponent
{
    public TEAM_TYPE Team { get; set; }

    private ParticleEmitter goalEmitterCenter;
    private ParticleEmitter goalEmitterSpark;
    private ParticleEmitter goalEmitterShock;

    public Texture2D goalCenterTexture { get; set; }
    public Texture2D goalSparkTexture { get; set; }
    public Texture2D goalShockWaveTexture { get; set; }

    List<Confetti> confettis;

    public override void Start()
    {
        confettis = new List<Confetti>(ScriptUtility.GetComponentsOfType<Confetti>());
        Debug.Log(confettis.Count);
        BoxCollider c = gameObject.AddComponent<BoxCollider>();
        c.isTrigger = true;
        c.size = new Vector3(0.5f, 0.5f, 0.5f);
        MatchSystem.instance.FindTeam(Team).GoalPosition = transform.position;

        goalEmitterCenter = gameObject.AddComponent<ParticleEmitter>();
        goalEmitterSpark = gameObject.AddComponent<ParticleEmitter>();
        goalEmitterShock = gameObject.AddComponent<ParticleEmitter>();

        goalEmitterCenter.Texture = goalCenterTexture;
        goalEmitterCenter.BlendState = ParticleEmitter.BLEND_STATES.ADDITIVE;
        goalEmitterCenter.MinSize = 1.0f;
        goalEmitterCenter.MaxSize = 1.0f;
        goalEmitterCenter.EndSize = 0.25f;
        goalEmitterCenter.MinLifeTime = 1.0f;
        goalEmitterCenter.MaxLifeTime = 1.0f;
        goalEmitterCenter.MinRotationSpeed = 10.0f;
        goalEmitterCenter.MaxRotationSpeed = 10.0f;
        goalEmitterCenter.MinSpeed = 5.0f;
        goalEmitterCenter.MaxSpeed = 5.0f;
        goalEmitterCenter.EndSpeed = -5.0f;

        goalEmitterSpark.Texture = goalSparkTexture;
        goalEmitterSpark.MinSize = 0.3f;
        goalEmitterSpark.MaxSize = 0.5f;
        goalEmitterSpark.EndSize = 0.04f;
        goalEmitterSpark.EmissionRate = 200;
        goalEmitterSpark.MinLifeTime = 0.3f;
        goalEmitterSpark.MaxLifeTime = 0.5f;
        goalEmitterSpark.MinRotationSpeed = 5.0f;
        goalEmitterSpark.MaxRotationSpeed = -5.0f;
        goalEmitterSpark.MinSpeed = 3.0f;
        goalEmitterSpark.MaxSpeed = 10.0f;
        goalEmitterSpark.EndSpeed = 1.0f;
        goalEmitterSpark.Radius = 2.0f;
        goalEmitterSpark.SpawnAtEdge = true;

        goalEmitterShock.Texture = goalShockWaveTexture;
        goalEmitterShock.MinSize = 0.0f;
        goalEmitterShock.MaxSize = 0.0f;
        goalEmitterShock.EndSize = 100.0f;
        goalEmitterShock.MinLifeTime = 1.0f;
        goalEmitterShock.MaxLifeTime = 1.0f;
        goalEmitterShock.MinRotationSpeed = 0.0f;
        goalEmitterShock.MaxRotationSpeed = 0.0f;
        goalEmitterShock.MinSpeed = 0.0f;
        goalEmitterShock.MaxSpeed = 0.0f;
        goalEmitterShock.EndSpeed = 0.0f;
    }

    public override void Update()
    {

    }

    public override void OnDrawGizmos()
    {
        Team t = MatchSystem.instance?.FindTeam(Team);
        if (t != null)
        {
            Gizmos.SetMatrix(transform.world);
            Gizmos.SetColor(t.Color * 0.5f);
            Gizmos.DrawBoundingBox(Vector3.Zero, new Vector3(0.5f, 0.5f, 0.5f));
        }
    }

    public override void OnTriggerEnter(Collider collider)
    {
        if (MatchSystem.instance)
        {
            if (collider.gameObject == MatchSystem.instance.Ball)
            {
                if (MatchSystem.instance.Ball.GetComponent<NetworkIdentity>().Owner)
                {
                    TEAM_TYPE teamThatScored = MatchSystem.instance.GetOpposingTeam(Team);
                    MatchSystem.instance.OnGoal(teamThatScored);
                    
                }
                StartCoroutine(EmitSparkForDuration(5.0f));
            }
        }
        
    }
    public IEnumerator EmitSparkForDuration(float duration)
    {
        confettis.ForEach((confetti) => confetti.Emit(duration));
        goalEmitterCenter.EmitOneShot(100);
        goalEmitterShock.EmitOneShot(1);
        goalEmitterSpark.Emit = true;
        yield return new WaitForSeconds(duration);
        goalEmitterSpark.Emit = false;
    }

}
