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

        private float m_velocity = 2.0f;
        private float m_runningSpeed = 3.0f;
        private float m_maxSpeed = 5.0f;
        private float m_incrementSpeed = 0.01f;

        private float m_chargeForce = 0.0f;
        

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

                    if (false /*m_rBody.isCollidingWithPowerUpOrChad(), compare velocity, check if velocity big enough*/)
                    {
                        m_state = State.RAGDOLL;
                    }
                    else
                    {
                        if (Input.GetKey(Input.Keys.A) || Input.GetKey(Input.Keys.D) || Input.GetKey(Input.Keys.S) || Input.GetKey(Input.Keys.W))
                        {
                            m_state = State.MOVING;
                        }
                        else if (Input.GetMouseButton(Input.MouseButtons.LEFT) /* && character is actually holding something to throw*/)
                        {
                            m_state = State.THROWING;
                        }
                        else if (Input.GetKeyDown(Input.Keys.Space))
                        {
                            m_state = State.JUMPING;
                        }
                    }
                    if (Input.GetMouseX() * Time.ActualDeltaTime != m_xStep || Input.GetMouseY() * Time.ActualDeltaTime != m_yStep)
                    {
                        if (!Input.GetKey(Input.Keys.LeftShift)) //normal controls
                            m_chadControls.FondleCamera(m_velocity, m_xStep, m_yStep);
                        else //free look
                            m_chadControls.FreeLookCamera(m_velocity, m_xStep, m_yStep);

                    }
                    break;
                case State.THROWING:
                    // __SIMULTAENOUS__ THROW and: MOVING
                    // __CAN_ENTER__  IDLE/RAGDOLL/JUMP/MOVING

                    // m_chargeForce += en faktor för charge
                    // chadBall.charge(m_chargeForce) ändrar utseende osv beroende på force

                    if (false /*m_rBody.isCollidingWithPowerUpOrChad(), compare velocity, check if velocity big enough*/)
                    {
                        m_state = State.RAGDOLL;
                    }
                    else if(Input.GetKey(Input.Keys.Space))
                    {
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
                        }
                        // Throwing and pressing A, checking if also D pressed
                        else if (Input.GetKey(Input.Keys.A))
                        {
                            if(!Input.GetKey(Input.Keys.D))
                            {
                                // blend anim strafing and throwing
                                // chadMovement.Move(0,-m_velocity);
                            }
                        }
                        else if (Input.GetKey(Input.Keys.D))
                        {
                            if (!Input.GetKey(Input.Keys.A))
                            {
                                // blend anim strafing and throwing
                                // chadMovement.Move(0,m_velocity);
                            }
                        }

                        if (Input.GetMouseX() * Time.ActualDeltaTime != m_xStep || Input.GetMouseY() * Time.ActualDeltaTime != m_yStep)
                            m_chadControls.FondleCamera(m_velocity, m_xStep, m_yStep);

                        // can enter following after releasing ball: walking/backing/strafing/turning/idle
                        if (Input.GetMouseButtonUp(Input.MouseButtons.LEFT))
                        {
                            // player was charging and is not throwing

                            // play throwing anim
                            // chadBall.Throw(m_chargeForce) // release the Kraken

                            // check what mode to enter
                            if (Input.GetKey(Input.Keys.W) || Input.GetKey(Input.Keys.S) || Input.GetKey(Input.Keys.A) || Input.GetKey(Input.Keys.D))
                                m_state = State.MOVING;
                            else
                                m_state = State.IDLE;
                        }
                    }
                    break;
                case State.MOVING:
                    // __SIMULTAENOUS__ MOVE and: nothing
                    // __CAN_ENTER__  IDLE/THROWING/RAGDOLL/JUMP

                    if (false /*m_rBody.isCollidingWithPowerUpOrChad(), compare velocity, if won comparison check if velocity big enough*/)
                    {
                        m_state = State.RAGDOLL;
                    }
                    else if (Input.GetKey(Input.Keys.Space))
                    {
                        m_state = State.JUMPING;
                    }
                    else
                    {
                        //if m_velocity > m_runningSpeed && anim != running {anim = running}
                        //elif m_velocity < m_runningSpeed && anim != walking {anim = walking}
                        if (Input.GetMouseButton(Input.MouseButtons.LEFT) /*&& actually holding something to throw*/)
                        {
                            m_state = State.THROWING;
                        }
                        else if (Input.GetKey(Input.Keys.W))
                        {
                            //Debug.Log("Speed: " + m_velocity);
                            if (m_velocity < m_maxSpeed)
                                m_velocity += m_incrementSpeed;
                            if (Input.GetKey(Input.Keys.Q) && !Input.GetKey(Input.Keys.E) && !Input.GetKey(Input.Keys.S))
                            {
                                // move camera fixed in front from side, player runs straight, no strafing

                                m_chadControls.HandleMovement(m_velocity, 0);

                            }
                            else if (Input.GetKey(Input.Keys.E) && !Input.GetKey(Input.Keys.Q) && !Input.GetKey(Input.Keys.S))
                            {
                                // move camera fixed in front from side, player runs straight, no strafing

                                m_chadControls.HandleMovement(m_velocity, 0);

                            }
                            else if (Input.GetKey(Input.Keys.A) && !Input.GetKey(Input.Keys.D) && !Input.GetKey(Input.Keys.S))
                            {
                                // move camera fixed behind, player runs straight, no strafing

                                m_chadControls.HandleMovement(m_velocity, 0);

                            }
                            else if (Input.GetKey(Input.Keys.D) && !Input.GetKey(Input.Keys.A) && !Input.GetKey(Input.Keys.S))
                            {
                                // move camera fixed behind, player runs straight, no strafing

                                m_chadControls.HandleMovement(m_velocity, 0);

                            }
                            else if (!Input.GetKey(Input.Keys.S))
                            {
                                m_chadControls.HandleMovement(m_velocity, 0);
                            }
                        }
                        else if (Input.GetKey(Input.Keys.S))
                        {
                            m_velocity = 2.0f;//en faktor, ingen acceleration because backing
                            if (Input.GetKey(Input.Keys.A) && !Input.GetKey(Input.Keys.D) && !Input.GetKey(Input.Keys.W))
                            {
                                // backing anim
                                m_chadControls.HandleMovement(-m_velocity * 0.66f, -m_velocity * 0.66f);
                            }
                            else if (Input.GetKey(Input.Keys.D) && !Input.GetKey(Input.Keys.A) && !Input.GetKey(Input.Keys.W))
                            {
                                // backing anim
                                m_chadControls.HandleMovement(-m_velocity * 0.66f, m_velocity * 0.66f);
                            }
                            else if (!Input.GetKey(Input.Keys.W))
                            {
                                //backing anim
                                m_chadControls.HandleMovement(-m_velocity, 0);
                            }
                        }
                        else if (Input.GetKey(Input.Keys.A))
                        {
                            m_velocity = 2.0f;
                            //strafing left anim
                            if (!Input.GetKey(Input.Keys.D))
                            {
                                m_chadControls.HandleMovement(0, -m_velocity);
                            }
                        }
                        else if (Input.GetKey(Input.Keys.D))
                        {
                            m_velocity = 2.0f;
                            //strafing right anim
                            if (!Input.GetKey(Input.Keys.A))
                            {
                                m_chadControls.HandleMovement(0, m_velocity);
                            }
                        }
                        else if (!Input.GetKey(Input.Keys.W) && !Input.GetKey(Input.Keys.S) && !Input.GetKey(Input.Keys.A) && !Input.GetKey(Input.Keys.D))
                        {
                            m_velocity = 2.0f;
                            m_state = State.IDLE;
                        }
                    }

                    if (Input.GetMouseX() * Time.ActualDeltaTime != m_xStep || Input.GetMouseY() * Time.ActualDeltaTime != m_yStep)
                    {
                        m_chadControls.FondleCamera(m_velocity, m_xStep, m_yStep);
                    }

                    break;

                case State.JUMPING:
                    // chadMovement.JUMP() // gets player camera forward and launches him in that direction

                    // do we want to this to also set state to ragdoll?
                    // if !chadMovement.is_jumping 
                    //      m_state = State.IDLE;
                    break;

                case State.RAGDOLL: //  last state, cancels sets state to IDLE after RAGDOLL animation is finished
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
