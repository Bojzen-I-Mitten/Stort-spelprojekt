using ThomasEngine;
using ThomasEngine.Network;
using System.Collections;
using System.Linq;

public class Vindaloo : Powerup
{
    private ParticleEmitter _Fire;
    public Texture2D _FireTex { get; set; }
    private float fireMax;

    public float ExplosionRadius { get; set; } = 50.0f;
    public float ExplosionForce { get; set; } = 100.0f;

    public override void Start()
    {
        base.Start();

        m_throwable = true; // change depending on power-up
        _Fire = gameObject.AddComponent<ParticleEmitter>();

        _Fire.Texture = _FireTex;

        ResetFireEmitters();
    }

    public override void Update()
    {
        
    }

    // if this is a throwable power-up this function will be called
    public override void Throw(Vector3 force)
    {
        base.Throw(force);

        // add more code here for what happens upon throw
        // in current state Chad will just throw like it would be the ball
    }

    // if this is a non-throwable this function will be called
    public override void OnActivate()
    {
        Debug.Log("Vindaru onactivetu");

        //boom particles
        Explosion();

        var players = NetworkManager.instance.Scene.Players.Values.ToList();
        players.ForEach(player =>
        {
            Debug.Log("Rooping etto puraya desu");
            float distance = Vector3.Distance(player.transform.position, transform.position);
            if (distance < ExplosionRadius)
            {
                Vector3 forceDir = player.transform.position - transform.position;
                forceDir.y += 3.0f;

                Debug.Log("Nani desu ka?");
                player.gameObject.GetComponent<ChadControls>().PublicStartRagdoll(5.0f, forceDir * ExplosionForce);
            }
        });


        // despawn gameobject
    }

    private void Explosion()
    {
        _Fire.Emit = true;
        StartCoroutine(StopFire());
    }

    private void ResetFireEmitters()
    {
        _Fire.MinSize = 10.0f;
        _Fire.MaxSize = 20.0f;
        _Fire.EndSize = 0.0f;
        _Fire.MinLifeTime = 0.5f;
        _Fire.MaxLifeTime = 3.0f;
        _Fire.EmissionRate = 10;
        _Fire.MinRotationSpeed = -2.0f;
        _Fire.MaxRotationSpeed = 2.0f;
        _Fire.MinSpeed = 0.2f;
        _Fire.MaxSpeed = 0.5f;
        _Fire.EndSpeed = 0.0f;
        _Fire.Gravity = -1;
        _Fire.DistanceFromSphereCenter = 5.0f;
        _Fire.Radius = 0.7f;
    }

    private IEnumerator StopFire()
    {
        float timer = 3;
        while (timer > 0)
        {
            timer -= Time.DeltaTime;
            yield return null;
        }

        _Fire.Emit = false;
    }
}
