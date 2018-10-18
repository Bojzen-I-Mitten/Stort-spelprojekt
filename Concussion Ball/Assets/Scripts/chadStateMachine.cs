using System.Linq;
using System.Collections;
using System.Collections.Generic;
using ThomasEngine;
using ThomasEngine.Network;
using LiteNetLib;
using LiteNetLib.Utils;
using System;

public class ChadStateMachine : NetworkComponent
{

    private Chadimations m_animations;
    //private Dictionary<Chadimations.STATE, float> Weights;
    private bool ChangeState;
    private Rigidbody m_rBody;

    //Player information
    private float m_xStep = 0.0f;
    private float m_yStep = 0.0f;

    private float m_runningSpeed = 5.0f;
    private float m_maxSpeedWithBall = 6.0f;

    private float m_incrementSpeed = 4.0f;

    private bool m_ragdolling = false;
    private bool m_isTackled = false;
    private bool m_tackling = false;

    private float m_chargeForce = 2.0f;
    private float m_baseForce = 2.0f;
    private float m_incrementForce = 4.0f;
    //private float m_maxForce = 10.0f;

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

    IEnumerator DivingCoroutine()
    {
        m_tackling = true;
        float timer = 1.0f;
        while (timer > 0.0f)
        {
            m_chadControls.HandleMovement(m_maxSpeed, 0);
            timer -= Time.DeltaTime;
            Debug.Log("Test1");
            yield return null;

        }
        Debug.Log("Test2");
        m_velocity = BaseSpeed;
        m_state = CHAD_STATE.IDLE;
    }

    //Don't extend
    IEnumerator ChargingCoroutine()
    {
        float timer = 4.0f;
        bool charging = false;

        while (m_state == CHAD_STATE.THROWING && !charging)
        {
            while (timer > 0.0f && Input.GetMouseButton(Input.MouseButtons.LEFT) && Input.GetMouseButton(Input.MouseButtons.RIGHT))
            {
                m_chargeForce += m_incrementForce * Time.DeltaTime;
                m_chadControls.ChargeBall();

                timer -= Time.DeltaTime;
                charging = true;
                yield return null;
            }
            if (!Input.GetMouseButton(Input.MouseButtons.RIGHT))
            {
                m_chadControls.ResetCamera();

                m_chargeForce = m_baseForce;

                // check what mode to enter
                if (Input.GetKey(Input.Keys.W) || Input.GetKey(Input.Keys.S) || Input.GetKey(Input.Keys.A) || Input.GetKey(Input.Keys.D))
                {
                    m_state = CHAD_STATE.MOVING;
                    Weights.Clear();
                    Weights.Add(Chadimations.STATE.RUNNING, 0);
                    Weights.Add(Chadimations.STATE.STRAFING_LEFT, 0);
                    Weights.Add(Chadimations.STATE.STRAFING_RIGHT, 0);
                    Weights.Add(Chadimations.STATE.BACKWARDS, 0);
                }
                else
                {
                    m_state = CHAD_STATE.IDLE;
                    Weights.Clear();
                    Weights.Add(Chadimations.STATE.IDLE, 0);
                    Weights.Add(Chadimations.STATE.TURNING_LEFT, 0);
                    Weights.Add(Chadimations.STATE.TURNING_RIGHT, 0);
                }
                m_ball.Cleanup();

                StopCoroutine(ChargingCoroutine());
            }
                
            yield return null;
        }

        if (charging)
        {
            timer = 1.0f;
            while (Input.GetMouseButton(Input.MouseButtons.RIGHT) && charging)
            {
                while (timer > 0.0f && Input.GetMouseButton(Input.MouseButtons.RIGHT) && !Input.GetMouseButton(Input.MouseButtons.LEFT))
                {
                    Weights[Chadimations.STATE.THROWING] = 1f;
                    m_chadControls.ThrowBall(m_chargeForce);
                    timer -= Time.DeltaTime;

                    charging = false;
                    m_animations.SetAnimations(Weights);
                    yield return null;
                }
                yield return null;
            }
            
            m_chadControls.ResetCamera();

            m_chargeForce = m_baseForce;
            m_ball.Cleanup();

            // check what mode to enter
            if (Input.GetKey(Input.Keys.W) || Input.GetKey(Input.Keys.S) || Input.GetKey(Input.Keys.A) || Input.GetKey(Input.Keys.D))
            {
                m_state = CHAD_STATE.MOVING;
                Weights.Clear();
                Weights.Add(Chadimations.STATE.RUNNING, 0);
                Weights.Add(Chadimations.STATE.STRAFING_LEFT, 0);
                Weights.Add(Chadimations.STATE.STRAFING_RIGHT, 0);
                Weights.Add(Chadimations.STATE.BACKWARDS, 0);
            }
            else
            {
                m_state = CHAD_STATE.IDLE;
                Weights.Clear();
                Weights.Add(Chadimations.STATE.IDLE, 0);
                Weights.Add(Chadimations.STATE.TURNING_LEFT, 0);
                Weights.Add(Chadimations.STATE.TURNING_RIGHT, 0);
            }
        }
    }

    public override void Start()
    {
        m_chadControls = null;
        m_state = CHAD_STATE.IDLE;
        m_xStep = Input.GetMouseX() * Time.ActualDeltaTime;
        m_yStep = Input.GetMouseY() * Time.ActualDeltaTime;

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
        Input.SetMouseMode(Input.MouseMode.POSITION_RELATIVE);
        List<Chadimations.STATE> keys = Weights.Keys.ToList();
        keys.ForEach((key) => Weights[key] = 0);
        switch (m_state)
        {
            case CHAD_STATE.IDLE:
                {
                    // __SIMULTAENOUS__ IDLE and: nothing
                    // __CAN_ENTER__  MOVING/THROWING/JUMPING/RAGDOLL
                    if (m_isTackled)
                    {
                        m_state = CHAD_STATE.RAGDOLL;
                    }
                    else
                    {
                        if (Input.GetKey(Input.Keys.A) || Input.GetKey(Input.Keys.D) || Input.GetKey(Input.Keys.S) || Input.GetKey(Input.Keys.W))
                        {
                            m_state = CHAD_STATE.MOVING;
                            Weights.Clear();
                            Weights.Add(Chadimations.STATE.RUNNING, 0);
                            Weights.Add(Chadimations.STATE.STRAFING_LEFT, 0);
                            Weights.Add(Chadimations.STATE.STRAFING_RIGHT, 0);
                            Weights.Add(Chadimations.STATE.BACKWARDS, 0);
                            //Weights.Add(Chadimations.STATE.WALKING BACKWARDS, 0);
                        }
                        else if (Input.GetMouseButton(Input.MouseButtons.RIGHT) && m_chadControls.HasBall)
                        {
                            m_state = CHAD_STATE.THROWING;
                            Weights.Clear();
                            Weights.Add(Chadimations.STATE.THROWING, 0);
                            Weights.Add(Chadimations.STATE.STRAFING_LEFT, 0);
                            Weights.Add(Chadimations.STATE.STRAFING_RIGHT, 0);
                            Weights.Add(Chadimations.STATE.BACKWARDS, 0);
                            Weights.Add(Chadimations.STATE.TURNING_LEFT, 0);
                            Weights.Add(Chadimations.STATE.TURNING_RIGHT, 0);
                            StartCoroutine(ChargingCoroutine());
                        }
                        else if (Input.GetKeyDown(Input.Keys.Space))
                        {
                            m_state = CHAD_STATE.DIVING;
                            Weights.Clear();
                            Weights.Add(Chadimations.STATE.DIVING, 0);
                            StartCoroutine(DivingCoroutine());
                        }
                    }
                    if (Input.GetMouseX() * Time.ActualDeltaTime != m_xStep || Input.GetMouseY() * Time.ActualDeltaTime != m_yStep)
                    {

                        if (!Input.GetKey(Input.Keys.LeftShift)) //normal controls
                        {
                            // play turning animation
                            if (m_xStep > 0)
                                Weights[Chadimations.STATE.TURNING_LEFT] = 1f;
                            else
                                Weights[Chadimations.STATE.TURNING_RIGHT] = 1f;
                            m_chadControls.FondleCamera(m_velocity, m_xStep, m_yStep);
                        }
                        else //free look
                            m_chadControls.FreeLookCamera(m_velocity, m_xStep, m_yStep);

                    }
                    if (Input.GetKeyDown(Input.Keys.LeftShift))
                    {
                        m_chadControls.InitFreeLookCamera();
                    }
                    m_animations.SetAnimations(Weights);
                    break;
                }
            case CHAD_STATE.THROWING:
                Weights[Chadimations.STATE.THROWING] = 0f;
                m_chadControls.ThrowingCamera(m_velocity, m_xStep, m_yStep);
                // __SIMULTAENOUS__ THROW and: MOVING
                // __CAN_ENTER__  IDLE/RAGDOLL/JUMP/MOVING

                if (m_isTackled)
                {
                    m_state = CHAD_STATE.RAGDOLL;
                    Weights.Clear();
                    Weights.Add(Chadimations.STATE.RAGDOLL, 0);
                }
                else if (Input.GetKey(Input.Keys.Space))
                {
                    m_state = CHAD_STATE.DIVING;
                    Weights.Clear();
                    Weights.Add(Chadimations.STATE.DIVING, 0);
                    StartCoroutine(DivingCoroutine());
                }
                else
                {
                    // Throwing and pressing W, checking if also ASD pressed
                    if (Input.GetKey(Input.Keys.W))
                    {
                        if (Input.GetKey(Input.Keys.A) && !Input.GetKey(Input.Keys.D))
                        {
                            // blend anim strafing and throwing
                            //Weights[Chadimations.STATE.THROWING] = 1f / 3f;
                            Weights[Chadimations.STATE.WALKING] = 1f / 2f;
                            Weights[Chadimations.STATE.STRAFING_LEFT] = 1f / 2f;

                            m_chadControls.HandleMovement(m_velocity * 0.66f, -m_velocity * 0.66f);
                        }
                        else if (Input.GetKey(Input.Keys.D) && !Input.GetKey(Input.Keys.A))
                        {
                            //blend anim strafing and throwing
                            //Weights[Chadimations.STATE.THROWING] = 1f / 3f;
                            Weights[Chadimations.STATE.WALKING] = 1f / 2f;
                            Weights[Chadimations.STATE.STRAFING_RIGHT] = 1f / 2f;

                            m_chadControls.HandleMovement(m_velocity * 0.66f, m_velocity * 0.66f);
                        }
                        else if (!Input.GetKey(Input.Keys.S))
                        {
                            // blend anim walking backwards and throwing
                            //Weights[Chadimations.STATE.THROWING] = 0.5f;
                            //Weights[Chadimations.STATE.STRAFING_LEFT] = 0.0f;
                            //Weights[Chadimations.STATE.STRAFING_RIGHT] = 0.0f;
                            //Weights[Chadimations.STATE.BACKWARDS] = 0.0f;
                            Weights[Chadimations.STATE.RUNNING] = 1.0f;
                            Debug.Log(Weights.Count);
                            m_chadControls.HandleMovement(m_velocity, 0);
                        }

                    }
                    // Throwing and pressed S, checking if also AWD pressed
                    else if (Input.GetKey(Input.Keys.S))
                    {
                        if (Input.GetKey(Input.Keys.A) && !Input.GetKey(Input.Keys.D))
                        {
                            // blend anim backing and throwing
                            //Weights[Chadimations.STATE.THROWING] = 1f / 3f;
                            Weights[Chadimations.STATE.BACKWARDS] = 1f / 2f;
                            Weights[Chadimations.STATE.STRAFING_LEFT] = 1f / 2f;
                            m_chadControls.HandleMovement(-m_velocity * 0.66f, -m_velocity * 0.66f);
                        }
                        else if (Input.GetKey(Input.Keys.D) && !Input.GetKey(Input.Keys.A))
                        {
                            //blend anim backing and throwing
                            //Weights[Chadimations.STATE.THROWING] = 1f / 3f;
                            Weights[Chadimations.STATE.BACKWARDS] = 1f / 2f;
                            Weights[Chadimations.STATE.STRAFING_RIGHT] = 1f / 2f;
                            m_chadControls.HandleMovement(-m_velocity * 0.66f, m_velocity * 0.66f);
                        }
                        else if (!Input.GetKey(Input.Keys.W))
                        {
                            // blend anim backing and throwing
                            //Weights[Chadimations.STATE.THROWING] = 0.5f;
                            Weights[Chadimations.STATE.BACKWARDS] = 1.0f;
                            m_chadControls.HandleMovement(-m_velocity, 0);
                        }
                    }
                    // Throwing and pressing A, checking if also D pressed
                    else if (Input.GetKey(Input.Keys.A) && !Input.GetKey(Input.Keys.D))
                    {
                        // blend anim strafing and throwing
                        //Weights[Chadimations.STATE.THROWING] = 0.5f;
                        Weights[Chadimations.STATE.STRAFING_LEFT] = 1.0f;
                        m_chadControls.HandleMovement(0, -m_velocity * 0.66f);
                    }
                    else if (Input.GetKey(Input.Keys.D) && !Input.GetKey(Input.Keys.A))
                    {
                        // blend anim strafing and throwing
                        //Weights[Chadimations.STATE.THROWING] = 0.5f;
                        Weights[Chadimations.STATE.STRAFING_RIGHT] = 1.0f;
                        m_chadControls.HandleMovement(0, m_velocity);
                    }

                    if (Input.GetMouseX() * Time.ActualDeltaTime != m_xStep || Input.GetMouseY() * Time.ActualDeltaTime != m_yStep)
                    {
                        keys.ForEach((key) => Weights[key] *= 2f / 3f);

                        if (m_xStep > 0)
                            Weights[Chadimations.STATE.TURNING_LEFT] = 1f / 3f;
                        else
                            Weights[Chadimations.STATE.TURNING_RIGHT] = 1f / 3f;
                    }
                }
                m_animations.SetAnimations(Weights);
                break;
            case CHAD_STATE.MOVING:
                // __SIMULTAENOUS__ MOVE and: nothing
                // __CAN_ENTER__  IDLE/THROWING/RAGDOLL/JUMP
                if (m_isTackled)
                {
                    m_state = CHAD_STATE.RAGDOLL;
                    Weights.Clear();
                    Weights.Add(Chadimations.STATE.RAGDOLL, 0);
                }
                else if (Input.GetKey(Input.Keys.Space))
                {
                    m_state = CHAD_STATE.DIVING;
                    Weights.Clear();
                    Weights.Add(Chadimations.STATE.DIVING, 0);
                    StartCoroutine(DivingCoroutine());
                }
                else
                {
                    //if m_velocity > m_runningSpeed && anim != running {anim = running}
                    //elif m_velocity < m_runningSpeed && anim != walking {anim = walking}
                    if (Input.GetMouseButton(Input.MouseButtons.RIGHT) && m_chadControls.HasBall)
                    {
                        m_state = CHAD_STATE.THROWING;
                        Weights.Clear();
                        Weights.Add(Chadimations.STATE.THROWING, 0);
                        Weights.Add(Chadimations.STATE.STRAFING_LEFT, 0);
                        Weights.Add(Chadimations.STATE.STRAFING_RIGHT, 0);
                        Weights.Add(Chadimations.STATE.BACKWARDS, 0);
                        Weights.Add(Chadimations.STATE.TURNING_LEFT, 0);
                        Weights.Add(Chadimations.STATE.TURNING_RIGHT, 0);
                        StartCoroutine(ChargingCoroutine());
                    }
                    else if (Input.GetKey(Input.Keys.W) && !Input.GetKey(Input.Keys.S))
                    {
                        if ((m_velocity < m_maxSpeedWithBall && m_chadControls.HasBall) || (m_velocity < m_maxSpeed && !m_chadControls.HasBall))
                        {
                            m_velocity += m_incrementSpeed * Time.DeltaTime;
                        }
                        Weights[Chadimations.STATE.RUNNING] = 1;
                        m_chadControls.HandleMovement(m_velocity, 0);
                    }
                    else if (Input.GetKey(Input.Keys.S))
                    {
                        m_velocity = BaseSpeed;
                        if (Input.GetKey(Input.Keys.A) && !Input.GetKey(Input.Keys.D) && !Input.GetKey(Input.Keys.W))
                        {
                            // backing anim
                            Weights[Chadimations.STATE.BACKWARDS] = 0.5f;
                            Weights[Chadimations.STATE.STRAFING_LEFT] = 0.5f;
                            m_chadControls.HandleMovement(-m_velocity * 0.66f, -m_velocity * 0.66f);
                        }
                        else if (Input.GetKey(Input.Keys.D) && !Input.GetKey(Input.Keys.A) && !Input.GetKey(Input.Keys.W))
                        {
                            // backing anim
                            Weights[Chadimations.STATE.BACKWARDS] = 0.5f;
                            Weights[Chadimations.STATE.STRAFING_RIGHT] = 0.5f;
                            m_chadControls.HandleMovement(-m_velocity * 0.66f, m_velocity * 0.66f);
                        }
                        else if (!Input.GetKey(Input.Keys.W))
                        {
                            //backing anim
                            Weights[Chadimations.STATE.BACKWARDS] = 1;
                            m_chadControls.HandleMovement(-m_velocity, 0);
                        }
                    }
                    else if (Input.GetKey(Input.Keys.A) && !Input.GetKey(Input.Keys.D))
                    {
                        //strafing left anim
                        m_velocity = BaseSpeed;
                        Weights[Chadimations.STATE.STRAFING_LEFT] = 1f;
                        m_chadControls.HandleMovement(0, -m_velocity);
                    }
                    else if (Input.GetKey(Input.Keys.D) && !Input.GetKey(Input.Keys.A))
                    {
                        //strafing right anim
                        m_velocity = BaseSpeed;
                        Weights[Chadimations.STATE.STRAFING_RIGHT] = 1f;
                        m_chadControls.HandleMovement(0, m_velocity);
                    }
                    else if (!Input.GetKey(Input.Keys.W) && !Input.GetKey(Input.Keys.S) && !Input.GetKey(Input.Keys.A) && !Input.GetKey(Input.Keys.D))
                    {
                        m_velocity = BaseSpeed;
                        m_state = CHAD_STATE.IDLE;
                        Weights.Clear();
                        Weights.Add(Chadimations.STATE.IDLE, 0);
                        Weights.Add(Chadimations.STATE.TURNING_LEFT, 0);
                        Weights.Add(Chadimations.STATE.TURNING_RIGHT, 0);
                    }
                }

                if (Input.GetMouseX() * Time.ActualDeltaTime != m_xStep || Input.GetMouseY() * Time.ActualDeltaTime != m_yStep)
                {
                    if (m_velocity < m_runningSpeed)
                    {
            case CHAD_STATE.DIVING:
                Weights[Chadimations.STATE.DIVING] = 1;
                m_animations.SetAnimations(Weights);
                break;
        }
    }
}
