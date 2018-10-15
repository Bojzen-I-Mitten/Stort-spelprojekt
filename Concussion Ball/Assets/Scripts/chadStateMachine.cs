using System.Collections.Generic;
using System.Collections;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ThomasEngine;
using ThomasEngine.Network;

namespace ThomasEditor
{
    enum State
    {
        IDLE = 0,
        MOVING = 1,    // running without enough velocity to tackle someone
        THROWING = 2,   // player throws ball / power-up, not all power-ups activate this state
        JUMPING = 3,    // user got tackled / hit by a power-up
        RAGDOLL = 4,    // user pressed Space to jump tackle
    };

    public class ChadStateMachine : ScriptComponent
    {
        private State m_state;
        private Rigidbody m_rBody;

        //State checks
        private float m_xStep = 0.0f;
        private float m_yStep = 0.0f;
        private float m_velocity = 0.0f;
        private float m_chargeForce = 0.0f;

        private float m_runningSpeed = 5.0f;

        private ChadControls _chadControls;

        private ChadControls m_chadControls
        {
            get
            {
                if (_chadControls == null)
                    _chadControls = gameObject.GetComponent<ChadControls>();
                return _chadControls;
            }
            set { _chadControls = value; }
        }

        public override void Start()
        {
            m_chadControls = null;
            m_state = State.IDLE;
            m_xStep = Input.GetMouseX() * Time.ActualDeltaTime;
            m_yStep = Input.GetMouseY() * Time.ActualDeltaTime;

            m_rBody = gameObject.GetComponent<Rigidbody>();
        }

        public override void Update()
        {
            Input.SetMouseMode(Input.MouseMode.POSITION_RELATIVE);
            switch (m_state)
            {
                case State.IDLE:
                    // __SIMULTAENOUS__ IDLE and: nothing
                    // __CAN_ENTER__  MOVING/THROWING/JUMPING/RAGDOLL

                    ////Debug.Log("State: " + m_state);
                    if (false /*m_rBody.isCollidingWithPowerUpOrChad(), compare velocity, check if velocity big enough*/)
                    {
                        //Debug.Log("Changed state from IDLE to RAGDOLL");
                        m_state = State.RAGDOLL;
                    }
                    else
                    {
                        if (Input.GetKey(Input.Keys.A) || Input.GetKey(Input.Keys.D) || Input.GetKey(Input.Keys.S) || Input.GetKey(Input.Keys.W))
                        {
                            m_state = State.MOVING;
                            //Debug.Log("Changed state from IDLE to MOVING");
                        }
                        else if (Input.GetMouseButton(Input.MouseButtons.LEFT) /* && character is actually holding something to throw*/)
                        {
                            //Debug.Log("Changed state from IDLE to THROWING");
                            m_state = State.THROWING;
                        }
                        else if (Input.GetKeyDown(Input.Keys.Space))
                        {
                            //Debug.Log("Changed state from IDLE to JUMPING");
                            m_state = State.JUMPING;
                        }
                    }
                    if (Input.GetMouseX() * Time.ActualDeltaTime != m_xStep || Input.GetMouseY() * Time.ActualDeltaTime != m_yStep)
                    {
                        //Debug.Log("Idle and turning");
                        if (!Input.GetKey(Input.Keys.LeftShift)) //normal controls
                            m_chadControls.FondleCamera(m_velocity, m_xStep, m_yStep);
                        else //free look
                            m_chadControls.FreeLookCamera(m_velocity, m_xStep, m_yStep);

                    }
                    if(Input.GetKeyDown(Input.Keys.LeftShift))
                    {
                        m_chadControls.InitFreeLookCamera();
                    }
                    break;
                case State.THROWING:
                    m_chadControls.ThrowingCamera(m_velocity, m_xStep, m_yStep);
                    // __SIMULTAENOUS__ THROW and: MOVING
                    // __CAN_ENTER__  IDLE/RAGDOLL/JUMP/MOVING

                    // m_chargeForce += en faktor för charge
                    // chadBall.charge(m_chargeForce) ändrar utseende osv beroende på force

                    //Debug.Log("Charging ball..");
                    if (false /*m_rBody.isCollidingWithPowerUpOrChad(), compare velocity, check if velocity big enough*/)
                    {
                        m_state = State.RAGDOLL;
                        //Debug.Log("Changed state from IDLE to RAGDOLL");
                    }
                    else if(Input.GetKey(Input.Keys.Space))
                    {
                        //Debug.Log("Was charging ball, but decided to jump instead");
                        m_state = State.JUMPING;
                    }
                    else
                    {
                        // m_velocity = ett värde, ingen acceleration

                        // Throwing and pressing W, checking if also ASD pressed
                        if (Input.GetKey(Input.Keys.W))
                        {
                            if (Input.GetKey(Input.Keys.A))
                            {
                                // blend anim strafing and throwing
                                // chadMovement.Move(m_velocity*0.66f, -m_velocity*0.66f);
                            }
                            else if (Input.GetKey(Input.Keys.D))
                            {
                                //blend anim strafing and throwing
                                // chadMovement.Move(m_velocity*0.66f, m_velocity*0.66f);
                            }
                            else if (!Input.GetKey(Input.Keys.S))
                            {
                                // blend anim walking backwards and throwing
                                // chadMovement.Move(m_velocity, 0);
                            }

                            //Debug.Log("Charging ball and walking");
                        }
                        // Throwing and pressed S, checkingif also AWD pressed
                        else if (Input.GetKey(Input.Keys.S))
                        {
                            if (Input.GetKey(Input.Keys.A))
                            {
                                // blend anim backing and throwing
                                // chadMovement.Move(-m_velocity*0.66f, -m_velocity*0.66f);
                            }
                            else if (Input.GetKey(Input.Keys.D))
                            {
                                //blend anim backing and throwing
                                // chadMovement.Move(-m_velocity*0.66f, m_velocity*0.66f);
                            }
                            else if (!Input.GetKey(Input.Keys.W))
                            {
                                // blend anim backing and throwing
                                // chadMovement.Move(-m_velocity, 0);
                            }
                            //Debug.Log("Charging ball and backing");
                        }
                        // Throwing and pressing A, checking if also D pressed
                        else if (Input.GetKey(Input.Keys.A))
                        {
                            if(!Input.GetKey(Input.Keys.D))
                            {
                                // blend anim strafing and throwing
                                // chadMovement.Move(0,-m_velocity);
                            }
                            //Debug.Log("Charging ball and strafing left");
                        }
                        else if (Input.GetKey(Input.Keys.D))
                        {
                            if (!Input.GetKey(Input.Keys.A))
                            {
                                // blend anim strafing and throwing
                                // chadMovement.Move(0,m_velocity);
                            }
                            //Debug.Log("Charging ball and strafing right");
                        }

                        if (Input.GetMouseX() * Time.ActualDeltaTime != m_xStep || Input.GetMouseY() * Time.ActualDeltaTime != m_yStep)
                        {
                            //Debug.Log("Charging ball and turning");

                            //m_chadControls.FondleCamera(m_velocity, m_xStep, m_yStep); TODO: make a throwing camera func.
                        }

                        // can enter following after releasing ball: walking/backing/strafing/turning/idle
                        if (Input.GetMouseButtonUp(Input.MouseButtons.LEFT))
                        {
                            m_chadControls.ResetCamera();
                            // player was charging and is not throwing

                            // play throwing anim
                            // chadBall.Throw(m_chargeForce) // release the Kraken

                            //Debug.Log("Releasing ball charge to throw");
                            // check what mode to enter
                            if (Input.GetKey(Input.Keys.W) || Input.GetKey(Input.Keys.S) || Input.GetKey(Input.Keys.A) || Input.GetKey(Input.Keys.D))
                            {
                                //Debug.Log("Changed from THROWING to WALKING");
                                m_state = State.MOVING;
                            }
                            else
                            {
                                //Debug.Log("Changed from THROWING to IDLE");
                                m_state = State.IDLE;
                            }
                        }
                    }
                    break;
                case State.MOVING:
                    // __SIMULTAENOUS__ MOVE and: nothing
                    // __CAN_ENTER__  IDLE/THROWING/RAGDOLL/JUMP

                    //Debug.Log("Moving");
                    if (false /*m_rBody.isCollidingWithPowerUpOrChad(), compare velocity, if won comparison check if velocity big enough*/)
                    {
                        //Debug.Log("Changed state from IDLE to RAGDOLL");
                        m_state = State.RAGDOLL;
                    }
                    else if (Input.GetKey(Input.Keys.Space))
                    {
                        m_state = State.JUMPING;
                        //Debug.Log("Player was walking and decided to jump");
                    }
                    else
                    {
                        //if m_velocity > m_runningSpeed && anim != running {anim = running}
                        //elif m_velocity < m_runningSpeed && anim != walking {anim = walking}
                        if (Input.GetMouseButton(Input.MouseButtons.LEFT) /*&& actually holding something to throw*/)
                        {
                            m_state = State.THROWING;
                            //Debug.Log("Walking and wanting to throw, change state to THROWING");
                        }
                        else if (Input.GetKey(Input.Keys.W))
                        {
                            if (Input.GetKey(Input.Keys.A) && !Input.GetKey(Input.Keys.D) && !Input.GetKey(Input.Keys.S))
                            {
                                // uppdatera inte m_velocity då ingen acceleration because also strafing
                                // chadMovement.Move(m_velocity * 0.66f, -m_velocity * 0.66f);

                                //Debug.Log("Walking and strafing together");
                            }
                            else if (Input.GetKey(Input.Keys.D) && !Input.GetKey(Input.Keys.A) && !Input.GetKey(Input.Keys.S))
                            {
                                // uppdatera inte m_velocity då ingen acceleration because also strafing
                                // chadMovement.Move(m_velocity *0.66f, m_velocity * 0.66f);

                                //Debug.Log("Walking and strafing together");
                            }
                            else if (!Input.GetKey(Input.Keys.S))
                            {
                                // m_velocity += en faktor för acceleration, if m_velocity < maxVelocity
                                // chadMovement.Move(m_velocity, 0);
                                //Debug.Log("Accelerating");
                            }
                        }
                        else if (Input.GetKey(Input.Keys.S))
                        {
                            //m_velocity = en faktor, ingen acceleration because backing
                            if (Input.GetKey(Input.Keys.A) && !Input.GetKey(Input.Keys.D) && !Input.GetKey(Input.Keys.W))
                            {
                                // backing anim
                                // chadMovement.Move(-m_velocity * 0.66f, -m_velocity * 0.66f);

                                //Debug.Log("Walking backwards and strafing together");
                            }
                            else if (Input.GetKey(Input.Keys.D) && !Input.GetKey(Input.Keys.A) && !Input.GetKey(Input.Keys.W))
                            {
                                // backing anim
                                // chadMovement.Move(-m_velocity *0.66f, m_velocity * 0.66f);

                                //Debug.Log("Walking backwards and strafing together");
                            }
                            else if (!Input.GetKey(Input.Keys.W))
                            {
                                //backing anim
                                // chadMovement.Move(m_velocity, 0);
                                //Debug.Log("Walking backwards");
                            }
                        }
                        else if (Input.GetKey(Input.Keys.A))
                        {
                            //strafing left anim
                            if (!Input.GetKey(Input.Keys.D))
                            {
                                //chadMovement.Move(0, -m_velocity);
                                //Debug.Log("Strafing left");
                            }
                        }
                        else if (Input.GetKey(Input.Keys.D))
                        {
                            //strafing right anim
                            if (!Input.GetKey(Input.Keys.A))
                            {
                                //chadMovement.Move(0, m_velocity);
                                //Debug.Log("Strafing right");
                            }
                        }
                        else if (!Input.GetKey(Input.Keys.W) && !Input.GetKey(Input.Keys.S) && !Input.GetKey(Input.Keys.A) && !Input.GetKey(Input.Keys.D))
                        {
                            m_velocity = 0;
                            m_state = State.IDLE;
                            //Debug.Log("User stopped moving, changing state to IDLE");
                        }
                    }

                    if (Input.GetMouseX() * Time.ActualDeltaTime != m_xStep || Input.GetMouseY() * Time.ActualDeltaTime != m_yStep)
                    {
                        //Debug.Log("Moving and turning..");

                        if (!Input.GetKey(Input.Keys.LeftShift)) //normal controls
                            m_chadControls.FondleCamera(m_velocity, m_xStep, m_yStep);
                        else //free look
                            m_chadControls.FreeLookCamera(m_velocity, m_xStep, m_yStep);
                    }

                    if (Input.GetKeyDown(Input.Keys.LeftShift))
                    {
                        m_chadControls.InitFreeLookCamera();
                    }

                    break;

                case State.JUMPING:
                    // chadMovement.JUMP() // gets player camera forward and launches him in that direction

                    // do we want to this to also set state to ragdoll?
                    //Debug.Log("WHERE WE JUMPIN', BOIS?!");
                    // if !chadMovement.is_jumping 
                    //      m_state = State.IDLE;
                    break;

                case State.RAGDOLL: //  last state, cancels sets state to IDLE after RAGDOLL animation is finished
                    //Debug.Log("Ragdolling..");
                    // chadMovement.Ragdoll(posPlayer, posEnemy) // calculates direction to launch player in and calls ragdoll function

                    break;


            }

            m_xStep = Input.GetMouseX() * Time.ActualDeltaTime;
            m_yStep = Input.GetMouseY() * Time.ActualDeltaTime;

            if (Input.GetKeyUp(Input.Keys.LeftShift))
            {
                m_chadControls.ResetCamera();
            }
        }
    }
}
