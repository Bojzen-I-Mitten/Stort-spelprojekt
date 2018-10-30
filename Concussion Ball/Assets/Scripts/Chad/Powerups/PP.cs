using ThomasEngine;
using ThomasEngine.Network;
using System.Collections;
using System.Linq;

public class PP : Powerup
{
    public GameObject shipGO { get; set; }


    public override void Start()
    {
        base.Start();

        m_throwable = true; // change depending on power-up

     

    }

    public override void Update()
    {

    }

    // if this is a throwable power-up this function will be called
    public override void Throw(Vector3 force)
    {
        base.Throw(force);
    }

    // this function will be called upon powerup use / collision after trown
    public override void OnActivate()
    {
        //Make sure powerups can only be activated once!
        if (activated)
            return;
        activated = true;

        WaitActivate();
        DeSpawnShip();

       

    }
    private void InstantiateShip()
    {
        GameObject.Instantiate(shipGO);
        float theta = Random.Range(MathHelper.PiOver2 / 3.0f, MathHelper.PiOver2);
        float phi = Random.Range(0.0f, MathHelper.TwoPi);
        float radius = 5;

        shipGO.transform.position = transform.position + Utility.SphericalCoordinate(phi, theta, radius);
        Rigidbody rb = shipGO.GetComponent<Rigidbody>();
        Vector3 direction = transform.position - shipGO.transform.position;

        rb.AddForce(direction);
    }

    

    private IEnumerator WaitActivate()
    {
        float timer = 3.0f;
        while (timer > 0.0f && m_rigidBody.LinearVelocity.Length() > 1.0f)
        {
            timer -= Time.DeltaTime;
            yield return null;
        }

        InstantiateShip();
        
            
    }

    private IEnumerator DeSpawnShip()
    {
        
        yield return new WaitForSeconds(10.0f);
        
        // despawn gameobject
        Remove();
    }
    

}
