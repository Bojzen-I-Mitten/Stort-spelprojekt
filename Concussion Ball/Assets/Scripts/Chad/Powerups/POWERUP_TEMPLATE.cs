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
    public override void Throw(Vector3 force)
    {
        base.Throw(force);

        // add more code here for what happens upon throw
        // in current state Chad will just throw like it would be the ball
    }

    // if this is a non-throwable this function will be called
    public override void OnActivate()
    {

    }
}
