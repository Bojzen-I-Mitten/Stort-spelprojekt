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
        // camera is never used, but someone program crashes if this is removed as it was previously used and
        // probably still rots inside the .tds file, can probably be removed when a new scene is used
        public Camera m_camera {get; set;}

        private State m_state;
        private Rigidbody m_rBody;

        //Player information
        private float m_xStep = 0.0f;
        private float m_yStep = 0.0f;

        private float m_velocity = 2.0f;
        private float m_runningSpeed = 5.0f;
        private float m_maxSpeedWithBall = 6.0f;
        private float m_maxSpeed = 10.0f;

        private float m_incrementSpeed = 4.0f;

        private bool m_ragdolling = false;
        private bool m_isTackled = false;
        private bool m_hasBall = false;
        private float m_chargeForce = 2.0f;
        private float m_maxForce = 10.0f;

        private Ball m_ball = null;

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
            //if(!m_camera)
            //{
            //    Debug.Log("Camera not found.");
            //}
            m_chadControls = null;
            m_state = State.IDLE;
            m_xStep = Input.GetMouseX() * Time.ActualDeltaTime;
            m_yStep = Input.GetMouseY() * Time.ActualDeltaTime;

            m_rBody = gameObject.GetComponent<Rigidbody>();

            m_ball = GetObjectsOfType<Ball>().FirstOrDefault();
        }

        public override void OnCollisionEnter(Collider collider)
        {
            if (m_ball)
            {
                if (collider.gameObject == m_ball.gameObject)
                {
                    m_hasBall = true;
                }
            }
            // collision with objects that have m_chadControls added, players that is
            else if (collider.gameObject == m_chadControls.gameObject)
            {
                Debug.Log("Collision with player");

                // velocity comparison
                //if (m_velocity < m_chadControls.velocity && m_chadControls.velocity > m_runningSpeed)
                    m_isTackled = true;
            }
        }

        public override void Update()
        {
            Input.SetMouseMode(Input.MouseMode.POSITION_RELATIVE);
            switch (m_state)
            {
                case State.IDLE:
                    // __SIMULTAENOUS__ IDLE and: nothing
                    // __CAN_ENTER__  MOVING/THROWING/JUMPING/RAGDOLL

                    if (m_isTackled)
                    {
                        m_state = State.RAGDOLL;
                    }
                    else
                    {
                        if (Input.GetKey(Input.Keys.A) || Input.GetKey(Input.Keys.D) || Input.GetKey(Input.Keys.S) || Input.GetKey(Input.Keys.W))
                        {
                            m_state = State.MOVING;
                        }
                        else if (Input.GetMouseButton(Input.MouseButtons.LEFT) && m_hasBall)
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
                        // play turning animation
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

                    if (m_chargeForce < m_maxForce)
                        m_chargeForce += m_incrementSpeed * Time.DeltaTime;
                    m_chadControls.ChargeBall();

                    //Debug.Log("Charge force: " + m_chargeForce);

                    if (m_isTackled)
                    {
                        m_state = State.RAGDOLL;
                    }
                    else if(Input.GetKey(Input.Keys.Space))
                    {
                        m_state = State.JUMPING;
                    }
                    else
                    {
                        // Throwing and pressing W, checking if also ASD pressed
                        if (Input.GetKey(Input.Keys.W))
                        {
                            if (Input.GetKey(Input.Keys.A) && !Input.GetKey(Input.Keys.D))
                            {
                                // blend anim strafing and throwing
                                m_chadControls.HandleMovement(m_velocity * 0.66f, -m_velocity * 0.66f);
                            }
                            else if (Input.GetKey(Input.Keys.D) && !Input.GetKey(Input.Keys.A))
                            {
                                //blend anim strafing and throwing
                                m_chadControls.HandleMovement(m_velocity * 0.66f, m_velocity * 0.66f);
                            }
                            else if (!Input.GetKey(Input.Keys.S))
                            {
                                // blend anim walking backwards and throwing
                                m_chadControls.HandleMovement(m_velocity, 0);
                            }

                        }
                        // Throwing and pressed S, checking if also AWD pressed
                        else if (Input.GetKey(Input.Keys.S))
                        {
                            if (Input.GetKey(Input.Keys.A) && !Input.GetKey(Input.Keys.D))
                            {
                                // blend anim backing and throwing
                                m_chadControls.HandleMovement(-m_velocity * 0.66f, -m_velocity * 0.66f);
                            }
                            else if (Input.GetKey(Input.Keys.D) && !Input.GetKey(Input.Keys.A))
                            {
                                //blend anim backing and throwing
                                m_chadControls.HandleMovement(-m_velocity * 0.66f, m_velocity * 0.66f);
                            }
                            else if (!Input.GetKey(Input.Keys.W))
                            {
                                // blend anim backing and throwing
                                m_chadControls.HandleMovement(-m_velocity, 0);
                            }
                        }
                        // Throwing and pressing A, checking if also D pressed
                        else if (Input.GetKey(Input.Keys.A) && !Input.GetKey(Input.Keys.D))
                        {
                                // blend anim strafing and throwing
                                m_chadControls.HandleMovement(0, -m_velocity * 0.66f);
                        }
                        else if (Input.GetKey(Input.Keys.D) && !Input.GetKey(Input.Keys.A))
                        {
                            // blend anim strafing and throwing
                            m_chadControls.HandleMovement(0, m_velocity);
                        }

                        if (Input.GetMouseX() * Time.ActualDeltaTime != m_xStep || Input.GetMouseY() * Time.ActualDeltaTime != m_yStep)
                            m_chadControls.FondleCamera(m_velocity, m_xStep, m_yStep);

                        if (Input.GetMouseButtonUp(Input.MouseButtons.LEFT))
                        {
                            m_chadControls.ResetCamera();
                            // player was charging and is not throwing

                            // play throwing anim
                            m_chadControls.ThrowBall(m_chargeForce);
                            m_chargeForce = 2.0f;
                            m_hasBall = false;

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
                    // Debug.Log("Movement speed: " + m_velocity);
                    if (m_isTackled)
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
                        if (Input.GetMouseButton(Input.MouseButtons.LEFT) && m_hasBall)
                        {
                            m_state = State.THROWING;
                        }
                        else if (Input.GetKey(Input.Keys.W))
                        {
                            //Debug.Log("Speed: " + m_velocity);
                            if ((m_velocity < m_maxSpeedWithBall && m_hasBall) || (m_velocity < m_maxSpeed && !m_hasBall))
                            {
                                m_velocity += m_incrementSpeed * Time.DeltaTime;
                            }
                                
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
                    // if !chadMovement.is_jumping 
                    //      m_state = State.IDLE;
                    break;

                case State.RAGDOLL:
                    if (!m_ragdolling)
                    {
                        Debug.Log("Entered ragdoll mode, calculating force and direction.");
                        // toggle ragdoll on
                        // calculate vector

                        // apply force to player
                        m_ragdolling = true;
                    }
                    else if (m_ragdolling/* && isOnGround*/)
                    {
                        Debug.Log("Landed after ragdoll, disabling ragdoll and entering IDLE state.");
                        
                        // toggle ragdoll off
                        m_ragdolling = false;
                        m_isTackled = false;
                        m_state = State.IDLE;
                    }

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
