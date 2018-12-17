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
    
    List<Confetti> confettis;

    public override void Start()
    {
        confettis = new List<Confetti>(ScriptUtility.GetComponentsOfType<Confetti>());
        Debug.Log(confettis.Count);
        MatchSystem.instance.FindTeam(Team).GoalPosition = transform.position;

        goalEmitterCenter = gameObject.AddComponent<ParticleEmitter>();
        goalEmitterSpark = gameObject.AddComponent<ParticleEmitter>();
        goalEmitterShock = gameObject.AddComponent<ParticleEmitter>();

        Texture2D centerTexture = (Texture2D)Resources.LoadThomasPath("%THOMAS_ASSETS%/Particles/fire_particle.png");
        if (centerTexture != null)
            goalEmitterCenter.Texture = centerTexture;
        goalEmitterCenter.BlendState = ParticleEmitter.BLEND_STATES.ADDITIVE;
        goalEmitterCenter.MinSize = 2.0f;
        goalEmitterCenter.MaxSize = 2.0f;
        goalEmitterCenter.EndSize = 0.25f;
        goalEmitterCenter.MinLifeTime = 1.0f;
        goalEmitterCenter.MaxLifeTime = 1.0f;
        goalEmitterCenter.MinRotationSpeed = 10.0f;
        goalEmitterCenter.MaxRotationSpeed = 10.0f;
        goalEmitterCenter.MinSpeed = 5.0f;
        goalEmitterCenter.MaxSpeed = 5.0f;
        goalEmitterCenter.EndSpeed = -5.0f;

        Texture2D sparkTexture = (Texture2D)Resources.LoadThomasPath("%THOMAS_ASSETS%/Particles/spark_particle.png");
        if (sparkTexture != null)
            goalEmitterSpark.Texture = sparkTexture;
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

        Texture2D shockTexture = (Texture2D)Resources.LoadThomasPath("%THOMAS_ASSETS%/Particles/shockwave_particle.png");
        if (shockTexture != null)
            goalEmitterShock.Texture = shockTexture;
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

    public override void OnDrawGizmosSelected()
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
            if (collider.gameObject == MatchSystem.instance.Ball && MatchSystem.instance.MatchStarted)
            {
                if (MatchSystem.instance.Ball.GetComponent<NetworkIdentity>().Owner && !MatchSystem.instance.hasScored)
                {
                    TEAM_TYPE teamThatScored = MatchSystem.instance.GetOpposingTeam(Team);
                    MatchSystem.instance.OnGoal(teamThatScored);
                    if(teamThatScored == MatchSystem.instance.LocalChad.gameObject.GetComponent<NetworkPlayer>().Team.TeamType)
                        MatchSystem.instance.LocalChad.gameObject.GetComponent<NetworkPlayer>().GoalsScored += 1;
                    else
                        MatchSystem.instance.LocalChad.gameObject.GetComponent<NetworkPlayer>().Owngoal += 1;
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
