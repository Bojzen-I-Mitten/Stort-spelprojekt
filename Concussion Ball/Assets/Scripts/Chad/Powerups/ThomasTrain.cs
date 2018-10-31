using ThomasEngine;
using ThomasEngine.Network;
using System.Collections;
using System.Linq;

public class ThomasTrain : Powerup
{
    private ParticleEmitter _Fire;
    public GameObject Rail { get; set; }
    public Texture2D _FireTex { get; set; }
    private float fireMax;

    private GameObject[] rails;

    public float ExplosionRadius { get; set; } = 5.0f;
    public float ExplosionForce { get; set; } = 200.0f;

    public override void Start()
    {
        base.Start();

        m_throwable = true; // change depending on power-up
        _Fire = gameObject.AddComponent<ParticleEmitter>();

        _Fire.Texture = _FireTex;

        ResetFireEmitters();

        RenderComponent rc;
        rails = new GameObject[4];
        for (uint i = 0; i < 4; ++i)
        {
            rc = rails[i].AddComponent<RenderComponent>();
            rc.model = Rail.GetComponent<RenderComponent>().model;

            rails[i].transform.position = new Vector3(5 * i, 0, 0);
        }

    }

    public override void Update()
    {

    }

    // if this is a throwable power-up this function will be called
    public override void Throw(Vector3 force)
    {
        base.Throw(force * 5.0f);
        m_rigidBody.UseGravity = false;
        StartCoroutine(Scale());
    }

    IEnumerator Scale()
    {
        transform.scale *= 10.0f;
        yield return new WaitForSeconds(0.1f);
        float t = 2.0f;
        while(t > 0.0f)
        {
            transform.scale += new Vector3(1.0f) * Time.DeltaTime;
            t -= Time.DeltaTime;
            yield return null;
        }
    }

    // this function will be called upon powerup use / collision after trown
    public override void OnActivate()
    {
        //Make sure powerups can only be activated once!
        if (activated)
            return;
        activated = true;
        // boom particles, Gustav do your magic, sprinkla lite magic till boisen
        Explosion();

       // loop through players and check distance from explosion source

        var players = NetworkManager.instance.Scene.Players.Values.ToList();
        players.ForEach(player =>
        {
            float distance = Vector3.Distance(player.transform.position, transform.position);
            if (distance < ExplosionRadius)
            {
                Vector3 forceDir = player.transform.position - transform.position;
                forceDir.y += 3.0f;

                // ragdoll and knock-back
                player.gameObject.GetComponent<ChadControls>().PublicStartRagdoll(5.0f, forceDir * ExplosionForce);
            }
        });



    }

    private void Explosion()
    {
        //hide the train.
        StopAllCoroutines();
        m_rigidBody.enabled = false;
        gameObject.transform.scale = Vector3.Zero;

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
        //float timer = 3;
        //while (timer > 0)
        //{
        //    timer -= Time.DeltaTime;
        //    yield return null;
        //}
        yield return new WaitForSeconds(3.0f);
        _Fire.Emit = false;
        // despawn gameobject
        Remove();
    }
}
