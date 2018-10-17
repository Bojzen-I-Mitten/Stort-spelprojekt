using System.Linq;
using System.Collections.Generic;
using ThomasEngine;
using ThomasEngine.Network;
using LiteNetLib;
using LiteNetLib.Utils;
using System;

public class ChadStateMachine : NetworkComponent
{
    public enum CHAD_STATE
    {
        CHADING,   // Idle/moving
        THROWING,   // player throws ball / power-up, not all power-ups activate this state
        DIVING,    // user got tackled / hit by a power-up
        RAGDOLL,    // user pressed Space to jump tackle

        NUM_STATES
    };

    // camera is never used, but someone program crashes if this is removed as it was previously used and
    // probably still rots inside the .tds file, can probably be removed when a new scene is used
    //public Camera m_camera { get; set; }

    private Chadimations m_animations;
    //private Dictionary<Chadimations.STATE, float> Weights;
    private CHAD_STATE State { get; set; }
    public CHAD_STATE _State { get { return State; } }
    private bool ChangeState;
    private Rigidbody m_rBody;

    //Player information
    private float m_xStep = 0.0f;
    private float m_yStep = 0.0f;

    public Vector2 m_velocity = new Vector2(2.0f, 2.0f);
    private float m_runningSpeed = 5.0f;
    private float BaseSpeed = 2.0f;
    private float m_maxSpeedWithBall = 6.0f;
    private float m_maxSpeed = 10.0f;
    public float Acceleration { get; set; } = 2.0f; //2 m/s^2

    private float m_incrementSpeed = 4.0f;

    private bool m_ragdolling = false;
    private bool m_isTackled = false;
    private float m_chargeForce = 2.0f;
    private float m_maxForce = 10.0f;

    //y holds roll for animation purposes
    public Vector3 Direction;

    private Ball m_ball = null;

    private float _DiveTimer = 0f;
    public float DiveTimer { get { return _DiveTimer; } }

    //private ChadControls _chadControls;
    //public ChadControls ChadControls
    //{
    //    get
    //    {
    //        if (_chadControls == null)
    //            _chadControls = gameObject.GetComponent<ChadControls>();
    //        return _chadControls;
    //    }
    //    set { _chadControls = value; }
    //}

    public override void Start()
    {
        State = CHAD_STATE.CHADING;

        m_rBody = gameObject.GetComponent<Rigidbody>();
        m_animations = gameObject.GetComponent<Chadimations>();
        m_ball = GetObjectsOfType<Ball>().FirstOrDefault();
        //Weights = new Dictionary<Chadimations.STATE, float>((int)Chadimations.STATE.NUM_STATES);
        //Weights.Add(Chadimations.STATE.IDLE, 0);
        //Weights.Add(Chadimations.STATE.TURNING_LEFT, 0);
        //Weights.Add(Chadimations.STATE.TURNING_RIGHT, 0);

    }

    public override void Update()
    {
        if(isOwner) //if owner, handle inputs
        {
            switch(State)
            {
                case CHAD_STATE.CHADING:
                    if (Direction.z > 0)
                    {
                        Direction.x = 0;
                        Direction.y = 0;
                    }

                    m_velocity.y += Direction.y * Acceleration * Time.DeltaTime;
                    m_velocity.x = Direction.x * BaseSpeed;

                    m_velocity.y = MathHelper.Clamp(m_velocity.y, -BaseSpeed, m_maxSpeed);
                    transform.position += new Vector3(m_velocity.x, 0, m_velocity.x) * Time.DeltaTime;

                    break;
                case CHAD_STATE.THROWING:
                    m_velocity.y = Direction.y * BaseSpeed;
                    m_velocity.x = Direction.x * BaseSpeed;

                    transform.position += new Vector3(m_velocity.x, 0, m_velocity.x) * Time.DeltaTime;
                    break;
                case CHAD_STATE.DIVING:
                    _DiveTimer += Time.DeltaTime;
                    break;
                case CHAD_STATE.RAGDOLL:
                    break;
            }
        }
    }

    public void EnterThrow()
    {
        State = CHAD_STATE.THROWING;
    }

    internal void ExitThrow()
    {
        State = CHAD_STATE.CHADING;
    }


    //public override void Update()
    //{
    //    //List<Chadimations.STATE> keys = Weights.Keys.ToList();
    //    //keys.ForEach((key) => Weights[key] = 0);
    //    switch (m_state)
    //    {
    //        case CHAD_STATE.IDLE:
    //            {
    //                // __SIMULTAENOUS__ IDLE and: nothing
    //                // __CAN_ENTER__  MOVING/THROWING/JUMPING/RAGDOLL
    //                if (m_isTackled)
    //                {
    //                    m_state = CHAD_STATE.RAGDOLL;
    //                }
    //                else
    //                {
    //                    if (Input.GetKey(Input.Keys.A) || Input.GetKey(Input.Keys.D) || Input.GetKey(Input.Keys.S) || Input.GetKey(Input.Keys.W))
    //                    {
    //                        m_state = CHAD_STATE.MOVING;
    //                        Weights.Clear();
    //                        Weights.Add(Chadimations.STATE.RUNNING, 0);
    //                        Weights.Add(Chadimations.STATE.STRAFING_LEFT, 0);
    //                        Weights.Add(Chadimations.STATE.STRAFING_RIGHT, 0);
    //                        Weights.Add(Chadimations.STATE.BACKWARDS, 0);
    //                        //Weights.Add(Chadimations.STATE.WALKING BACKWARDS, 0);
    //                    }
    //                    else if (Input.GetMouseButton(Input.MouseButtons.RIGHT) && m_chadControls.HasBall)
    //                    {
    //                        m_state = CHAD_STATE.THROWING;
    //                        Weights.Clear();
    //                        Weights.Add(Chadimations.STATE.THROWING, 0);
    //                        Weights.Add(Chadimations.STATE.STRAFING_LEFT, 0);
    //                        Weights.Add(Chadimations.STATE.STRAFING_RIGHT, 0);
    //                        Weights.Add(Chadimations.STATE.BACKWARDS, 0);
    //                        Weights.Add(Chadimations.STATE.TURNING_LEFT, 0);
    //                        Weights.Add(Chadimations.STATE.TURNING_RIGHT, 0);
    //                    }
    //                    //else if (Input.GetKeyDown(Input.Keys.Space))
    //                    //{
    //                    //    m_state = CHAD_STATE.DIVING;
    //                    //    Weights.Clear();
    //                    //    Weights.Add(Chadimations.STATE.DIVING, 0);
    //                    //}
    //                }
    //                if (Input.GetMouseX() * Time.ActualDeltaTime != m_xStep || Input.GetMouseY() * Time.ActualDeltaTime != m_yStep)
    //                {

    //                    if (!Input.GetKey(Input.Keys.LeftShift)) //normal controls
    //                    {
    //                        // play turning animation
    //                        if (m_xStep > 0)
    //                            Weights[Chadimations.STATE.TURNING_LEFT] = 1f;
    //                        else
    //                            Weights[Chadimations.STATE.TURNING_RIGHT] = 1f;
    //                        m_chadControls.FondleCamera(m_velocity, m_xStep, m_yStep);
    //                    }
    //                    else //free look
    //                        m_chadControls.FreeLookCamera(m_velocity, m_xStep, m_yStep);

    //                }
    //                if (Input.GetKeyDown(Input.Keys.LeftShift))
    //                {
    //                    m_chadControls.InitFreeLookCamera();
    //                }
    //                m_animations.SetAnimations(Weights);
    //                break;
    //            }
    //        case CHAD_STATE.THROWING:
    //            Weights[Chadimations.STATE.THROWING] = 1f;
    //            m_chadControls.ThrowingCamera(m_velocity, m_xStep, m_yStep);
    //            // __SIMULTAENOUS__ THROW and: MOVING
    //            // __CAN_ENTER__  IDLE/RAGDOLL/JUMP/MOVING

    //            if (m_chargeForce < m_maxForce)
    //                m_chargeForce += m_incrementSpeed * Time.DeltaTime;
    //            m_chadControls.ChargeBall();

    //            //Debug.Log("Charge force: " + m_chargeForce);

    //            if (m_isTackled)
    //            {
    //                m_state = CHAD_STATE.RAGDOLL;
    //                Weights.Clear();
    //                Weights.Add(Chadimations.STATE.RAGDOLL, 0);
    //            }
    //            //else if (Input.GetKey(Input.Keys.Space))
    //            //{
    //            //    m_state = CHAD_STATE.DIVING;
    //            //    Weights.Clear();
    //            //    Weights.Add(Chadimations.STATE.DIVING, 0);
    //            //}
    //            else
    //            {
    //                // Throwing and pressing W, checking if also ASD pressed
    //                if (Input.GetKey(Input.Keys.W))
    //                {
    //                    if (Input.GetKey(Input.Keys.A) && !Input.GetKey(Input.Keys.D))
    //                    {
    //                        // blend anim strafing and throwing
    //                        Weights[Chadimations.STATE.THROWING] = 1f / 3f;
    //                        Weights[Chadimations.STATE.WALKING] = 1f / 3f;
    //                        Weights[Chadimations.STATE.STRAFING_LEFT] = 1f / 3f;

    //                        m_chadControls.HandleMovement(m_velocity * 0.66f, -m_velocity * 0.66f);
    //                    }
    //                    else if (Input.GetKey(Input.Keys.D) && !Input.GetKey(Input.Keys.A))
    //                    {
    //                        //blend anim strafing and throwing
    //                        Weights[Chadimations.STATE.THROWING] = 1f / 3f;
    //                        Weights[Chadimations.STATE.WALKING] = 1f / 3f;
    //                        Weights[Chadimations.STATE.STRAFING_RIGHT] = 1f / 3f;

    //                        m_chadControls.HandleMovement(m_velocity * 0.66f, m_velocity * 0.66f);
    //                    }
    //                    else if (!Input.GetKey(Input.Keys.S))
    //                    {
    //                        // blend anim walking backwards and throwing
    //                        Weights[Chadimations.STATE.THROWING] = 0.5f;
    //                        Weights[Chadimations.STATE.WALKING] = 0.5f;

    //                        m_chadControls.HandleMovement(m_velocity, 0);
    //                    }

    //                }
    //                // Throwing and pressed S, checking if also AWD pressed
    //                else if (Input.GetKey(Input.Keys.S))
    //                {
    //                    if (Input.GetKey(Input.Keys.A) && !Input.GetKey(Input.Keys.D))
    //                    {
    //                        // blend anim backing and throwing
    //                        Weights[Chadimations.STATE.THROWING] = 1f / 3f;
    //                        Weights[Chadimations.STATE.BACKWARDS] = 1f / 3f;
    //                        Weights[Chadimations.STATE.STRAFING_LEFT] = 1f / 3f;
    //                        m_chadControls.HandleMovement(-m_velocity * 0.66f, -m_velocity * 0.66f);
    //                    }
    //                    else if (Input.GetKey(Input.Keys.D) && !Input.GetKey(Input.Keys.A))
    //                    {
    //                        //blend anim backing and throwing
    //                        Weights[Chadimations.STATE.THROWING] = 1f / 3f;
    //                        Weights[Chadimations.STATE.BACKWARDS] = 1f / 3f;
    //                        Weights[Chadimations.STATE.STRAFING_RIGHT] = 1f / 3f;
    //                        m_chadControls.HandleMovement(-m_velocity * 0.66f, m_velocity * 0.66f);
    //                    }
    //                    else if (!Input.GetKey(Input.Keys.W))
    //                    {
    //                        // blend anim backing and throwing
    //                        Weights[Chadimations.STATE.THROWING] = 0.5f;
    //                        Weights[Chadimations.STATE.BACKWARDS] = 0.5f;
    //                        m_chadControls.HandleMovement(-m_velocity, 0);
    //                    }
    //                }
    //                // Throwing and pressing A, checking if also D pressed
    //                else if (Input.GetKey(Input.Keys.A) && !Input.GetKey(Input.Keys.D))
    //                {
    //                    // blend anim strafing and throwing
    //                    Weights[Chadimations.STATE.THROWING] = 0.5f;
    //                    Weights[Chadimations.STATE.STRAFING_LEFT] = 0.5f;
    //                    m_chadControls.HandleMovement(0, -m_velocity * 0.66f);
    //                }
    //                else if (Input.GetKey(Input.Keys.D) && !Input.GetKey(Input.Keys.A))
    //                {
    //                    // blend anim strafing and throwing
    //                    Weights[Chadimations.STATE.THROWING] = 0.5f;
    //                    Weights[Chadimations.STATE.STRAFING_RIGHT] = 0.5f;
    //                    m_chadControls.HandleMovement(0, m_velocity);
    //                }

    //                if (Input.GetMouseX() * Time.ActualDeltaTime != m_xStep || Input.GetMouseY() * Time.ActualDeltaTime != m_yStep)
    //                {
    //                    keys.ForEach((key) => Weights[key] *= 2f / 3f);

    //                    if (m_xStep > 0)
    //                        Weights[Chadimations.STATE.TURNING_LEFT] = 1f / 3f;
    //                    else
    //                        Weights[Chadimations.STATE.TURNING_RIGHT] = 1f / 3f;
    //                }

    //                if (Input.GetMouseButtonUp(Input.MouseButtons.LEFT))
    //                {
    //                    m_chadControls.ResetCamera();
    //                    // player was charging and is not throwing

    //                    // play throwing anim
    //                    m_chadControls.ThrowBall(m_chargeForce);
    //                    m_chargeForce = BaseSpeed;

    //                    // check what mode to enter
    //                    if (Input.GetKey(Input.Keys.W) || Input.GetKey(Input.Keys.S) || Input.GetKey(Input.Keys.A) || Input.GetKey(Input.Keys.D))
    //                    {
    //                        m_state = CHAD_STATE.MOVING;
    //                        Weights.Clear();
    //                        Weights.Add(Chadimations.STATE.RUNNING, 0);
    //                        Weights.Add(Chadimations.STATE.STRAFING_LEFT, 0);
    //                        Weights.Add(Chadimations.STATE.STRAFING_RIGHT, 0);
    //                        Weights.Add(Chadimations.STATE.BACKWARDS, 0);
    //                    }
    //                    else
    //                    {
    //                        m_state = CHAD_STATE.IDLE;
    //                        Weights.Clear();
    //                        Weights.Add(Chadimations.STATE.IDLE, 0);
    //                        Weights.Add(Chadimations.STATE.TURNING_LEFT, 0);
    //                        Weights.Add(Chadimations.STATE.TURNING_RIGHT, 0);
    //                    }
    //                }
    //            }

    //            m_animations.SetAnimations(Weights);
    //            break;
    //        case CHAD_STATE.MOVING:
    //            // __SIMULTAENOUS__ MOVE and: nothing
    //            // __CAN_ENTER__  IDLE/THROWING/RAGDOLL/JUMP
    //            // Debug.Log("Movement speed: " + m_velocity);
    //            if (m_isTackled)
    //            {
    //                m_state = CHAD_STATE.RAGDOLL;
    //                Weights.Clear();
    //                Weights.Add(Chadimations.STATE.RAGDOLL, 0);
    //            }
    //            //else if (Input.GetKey(Input.Keys.Space))
    //            //{
    //            //    m_state = CHAD_STATE.DIVING;
    //            //    Weights.Clear();
    //            //    Weights.Add(Chadimations.STATE.DIVING, 0);
    //            //}
    //            else
    //            {
    //                //if m_velocity > m_runningSpeed && anim != running {anim = running}
    //                //elif m_velocity < m_runningSpeed && anim != walking {anim = walking}
    //                if (Input.GetMouseButton(Input.MouseButtons.LEFT) && m_chadControls.HasBall)
    //                {
    //                    m_state = CHAD_STATE.THROWING;
    //                    Weights.Clear();
    //                    Weights.Add(Chadimations.STATE.THROWING, 0);
    //                    Weights.Add(Chadimations.STATE.STRAFING_LEFT, 0);
    //                    Weights.Add(Chadimations.STATE.STRAFING_RIGHT, 0);
    //                    Weights.Add(Chadimations.STATE.BACKWARDS, 0);
    //                    Weights.Add(Chadimations.STATE.TURNING_LEFT, 0);
    //                    Weights.Add(Chadimations.STATE.TURNING_RIGHT, 0);
    //                }
    //                else if (Input.GetKey(Input.Keys.W) && !Input.GetKey(Input.Keys.S))
    //                {
    //                    //Debug.Log("Speed: " + m_velocity);
    //                    if ((m_velocity < m_maxSpeedWithBall && m_chadControls.HasBall) || (m_velocity < m_maxSpeed && !m_chadControls.HasBall))
    //                    {
    //                        m_velocity += m_incrementSpeed * Time.DeltaTime;
    //                    }
    //                    Weights[Chadimations.STATE.RUNNING] = 1;
    //                    m_chadControls.HandleMovement(m_velocity, 0);
    //                }
    //                else if (Input.GetKey(Input.Keys.S))
    //                {
    //                    m_velocity = 2.0f;//en faktor, ingen acceleration because backing
    //                    if (Input.GetKey(Input.Keys.A) && !Input.GetKey(Input.Keys.D) && !Input.GetKey(Input.Keys.W))
    //                    {
    //                        // backing anim
    //                        Weights[Chadimations.STATE.BACKWARDS] = 0.5f;
    //                        Weights[Chadimations.STATE.STRAFING_LEFT] = 0.5f;
    //                        m_chadControls.HandleMovement(-m_velocity * 0.66f, -m_velocity * 0.66f);
    //                    }
    //                    else if (Input.GetKey(Input.Keys.D) && !Input.GetKey(Input.Keys.A) && !Input.GetKey(Input.Keys.W))
    //                    {
    //                        // backing anim
    //                        Weights[Chadimations.STATE.BACKWARDS] = 0.5f;
    //                        Weights[Chadimations.STATE.STRAFING_RIGHT] = 0.5f;
    //                        m_chadControls.HandleMovement(-m_velocity * 0.66f, m_velocity * 0.66f);
    //                    }
    //                    else if (!Input.GetKey(Input.Keys.W))
    //                    {
    //                        //backing anim
    //                        Weights[Chadimations.STATE.BACKWARDS] = 1;
    //                        m_chadControls.HandleMovement(-m_velocity, 0);
    //                    }
    //                }
    //                else if (Input.GetKey(Input.Keys.A) && !Input.GetKey(Input.Keys.D))
    //                {
    //                    //strafing left anim
    //                    m_velocity = BaseSpeed;
    //                    Weights[Chadimations.STATE.STRAFING_LEFT] = 1f;
    //                    m_chadControls.HandleMovement(0, -m_velocity);
    //                }
    //                else if (Input.GetKey(Input.Keys.D) && !Input.GetKey(Input.Keys.A))
    //                {
    //                    //strafing right anim
    //                    m_velocity = BaseSpeed;
    //                    Weights[Chadimations.STATE.STRAFING_RIGHT] = 1f;
    //                    m_chadControls.HandleMovement(0, m_velocity);
    //                }
    //                else if (!Input.GetKey(Input.Keys.W) && !Input.GetKey(Input.Keys.S) && !Input.GetKey(Input.Keys.A) && !Input.GetKey(Input.Keys.D))
    //                {
    //                    m_velocity = BaseSpeed;
    //                    m_state = CHAD_STATE.IDLE;
    //                    Weights.Clear();
    //                    Weights.Add(Chadimations.STATE.IDLE, 0);
    //                    Weights.Add(Chadimations.STATE.TURNING_LEFT, 0);
    //                    Weights.Add(Chadimations.STATE.TURNING_RIGHT, 0);
    //                }
    //            }

    //            if (Input.GetMouseX() * Time.ActualDeltaTime != m_xStep || Input.GetMouseY() * Time.ActualDeltaTime != m_yStep)
    //            {
    //                //Debug.Log("Moving and turning..");
    //                if (m_velocity < m_runningSpeed)
    //                {

    //                    keys.ForEach((key) => Weights[key] = 2f / 3f);

    //                    if (m_xStep > 0)
    //                        Weights[Chadimations.STATE.TURNING_LEFT] = 1f / 3f;
    //                    else
    //                        Weights[Chadimations.STATE.TURNING_RIGHT] = 1f / 3f;

    //                }
    //                if (!Input.GetKey(Input.Keys.LeftShift)) //normal controls
    //                    m_chadControls.FondleCamera(m_velocity, m_xStep, m_yStep);
    //                else //free look
    //                    m_chadControls.FreeLookCamera(m_velocity, m_xStep, m_yStep);
    //            }

    //            if (Input.GetKeyDown(Input.Keys.LeftShift))
    //            {
    //                m_chadControls.InitFreeLookCamera();
    //            }

    //            m_animations.SetAnimations(Weights);
    //            break;

    //        case CHAD_STATE.DIVING:
    //            //Weights[Chadimations.STATE.DIVING] = 1;
    //            //m_animations.SetAnimations(Weights);
    //            // chadMovement.JUMP() // gets player camera forward and launches him in that direction

    //            // do we want to this to also set state to ragdoll?
    //            // if !chadMovement.is_jumping 
    //            //      m_state = State.IDLE;
    //            break;

    //        case CHAD_STATE.RAGDOLL:
    //            if (!m_ragdolling)
    //            {
    //                Debug.Log("Entered ragdoll mode, calculating force and direction.");
    //                // toggle ragdoll on
    //                // calculate vector

    //                // apply force to player
    //                m_ragdolling = true;
    //            }
    //            else if (m_ragdolling/* && isOnGround*/)
    //            {
    //                Debug.Log("Landed after ragdoll, disabling ragdoll and entering IDLE state.");

    //                // toggle ragdoll off
    //                m_ragdolling = false;
    //                m_isTackled = false;
    //                m_state = CHAD_STATE.IDLE;
    //                Weights.Add(Chadimations.STATE.IDLE, 1);
    //                m_animations.SetAnimations(Weights);
    //            }

    //            break;
    //    }

    //    m_xStep = Input.GetMouseX() * Time.ActualDeltaTime;
    //    m_yStep = Input.GetMouseY() * Time.ActualDeltaTime;

    //    if (Input.GetKeyUp(Input.Keys.LeftShift))
    //    {
    //        m_chadControls.ResetCamera();
    //    }
    //}


    public override void OnRead(NetPacketReader reader, bool initialState)
    {
        if (isOwner)
        {
            reader.GetInt();
            return;
        }

        State = (CHAD_STATE)reader.GetInt();
    }

    public override bool OnWrite(NetDataWriter writer, bool initialState)
    {
        writer.Put((int)State);
        return true;
    }
}
