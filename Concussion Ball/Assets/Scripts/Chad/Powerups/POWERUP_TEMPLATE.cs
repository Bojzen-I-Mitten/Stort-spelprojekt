using ThomasEngine;

public class POWERUP_TEMPLATE : Powerup
{
    public override void Start()
    {
        base.Start();

        m_throwable = true; // change depending on power-up
    }

    public override void Update()
    {

    }

    // if this is a throwable power-up this function will be called
    public override void Throw(Vector3 camPos, Vector3 force)
    {
        base.Throw(camPos, force);

        // add more code here for what happens upon throw
        // in current state Chad will just throw like it would be the ball
    }

    //COMMENT BACK IF YOU WANT POWERUP TO PASS THROUGH ALLIED PLAYERS
    //public override void OnCollisionEnter(Collider collider)
    //{
    //    //Check if colliding with a player
    //    ChadControls otherChad = collider.gameObject.GetComponent<ChadControls>();
    //    if (!otherChad)
    //    {
    //        base.OnCollisionEnter(collider);
    //    }
    //    else
    //    {
    //        ChadControls localChad = MatchSystem.instance.LocalChad;

    //        TEAM_TYPE playerTeam = MatchSystem.instance.GetPlayerTeam(ObjectOwner.gameObject);
    //        TEAM_TYPE otherPlayerTeam = MatchSystem.instance.GetPlayerTeam(collider.gameObject);

    //        if (localChad && (otherPlayerTeam != playerTeam))
    //            base.OnCollisionEnter(collider);
    //    }
    //}

    // this function will be called upon powerup use / collision after trown
    public override void OnActivate()
    {

    }
}
