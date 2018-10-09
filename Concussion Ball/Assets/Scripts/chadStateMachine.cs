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
        WALKING = 1,    // running without enough velocity to tackle someone
        RUNNING = 2,    // running with enough velocity to tackle someone
        STRAFING = 3,
        BACKING = 4,
        TURNING = 5,    // turning has to call different functions depending on if previous state was idle or walking/running
        THROWING = 6,   // player throws ball / power-up, not all power-ups activate this state
        JUMPING = 7,    // user got tackled / hit by a power-up
        RAGDOLL = 8,    // user pressed Space to jump tackle
    };

    public class ChadStateMachine : ScriptComponent
    {
        private State m_state;
        private Rigidbody m_rBody;

        //State checks
        private float m_xStep = 0.0f;
        private float m_velocity = 0.0f;
        private float m_chargeForce = 0.0f;

        public override void Start()
        {
            m_state = State.IDLE;
            m_xStep = Input.GetMouseX() * Time.ActualDeltaTime;

            m_rBody = gameObject.GetComponent<Rigidbody>();
        }

        public override void Update()
        {
            Input.SetMouseMode(Input.MouseMode.POSITION_RELATIVE);
            switch (m_state)
            {
                case State.IDLE:
                    // __SIMULTAENOUS__ IDLE and: nothing
                    // __CAN_ENTER__  WALKING/STRAFING/BACKING/TURNING/THROWING/JUMPING/RAGDOLL

                    Debug.Log("State: " + m_state);
                    if (false /*m_rBody.isCollidingWithPowerUpOrChad()*/)
                    {
                        Debug.Log("Changed state from IDLE to RAGDOLL");
                        m_state = State.RAGDOLL;
                    }
                    else
                    {
                        if (Input.GetMouseX() * Time.ActualDeltaTime != m_xStep)
                        {
                            m_state = State.TURNING;
                            Debug.Log("Changed state from IDLE to TURNING");
                        }
                        else if (Input.GetKey(Input.Keys.A) || Input.GetKey(Input.Keys.D))
                        {
                            m_state = State.STRAFING;
                            Debug.Log("Changed state from IDLE to STRAFING");
                        }
                        else if (Input.GetKey(Input.Keys.S))
                        {
                            m_state = State.BACKING;
                            Debug.Log("Changed state from IDLE to BACKING");
                        }
                        else if (Input.GetKey(Input.Keys.W))
                        {
                            m_state = State.WALKING;
                            Debug.Log("Changed state from IDLE to WALKING");
                        }
                        else if (Input.GetMouseButton(Input.MouseButtons.LEFT) /* && character is actually holding something to throw*/)
                        {
                            Debug.Log("Changed state from IDLE to THROWING");
                            m_state = State.THROWING;
                        }
                        else if (Input.GetKeyDown(Input.Keys.Space))
                        {
                            Debug.Log("Changed state from IDLE to JUMPING");
                            m_state = State.JUMPING;
                        }
                    }
                    break;
                case State.THROWING:
                    // __SIMULTAENOUS__ THROW and: WALK/BACKWARDS/STRAFING/TURN
                    // __CAN_ENTER__  IDLE/RAGDOLL/JUMP/RUNNING

                    // m_chargeForce += en faktor för charge
                    // chadBall.charge(m_chargeForce) ändrar utseende osv beroende på force

                    Debug.Log("Charging ball..");
                    if (false /*m_rBody.isCollidingWithPowerUpOrChad()*/)
                    {
                        m_state = State.RAGDOLL;
                        Debug.Log("Changed state from IDLE to RAGDOLL");
                    }
                    else
                    {
                        if (Input.GetKey(Input.Keys.W))
                        {
                            // blend walking and throwing anim (if S also pressed, only throw anim)

                            // m_velocity = ett värde, ingen acceleration
                            // chadMovement.Move(m_velocity,0);
                            Debug.Log("Charging ball and walking");
                        }
                        if (Input.GetKey(Input.Keys.S))
                        {
                            // blend reversing and throwing anim (if W also pressed, only throw anim)

                            // m_velocity = ett värde, ingen acceleration
                            // chadMovement.Move(-m_velocity,0);
                            Debug.Log("Charging ball and backing");
                        }
                        if (Input.GetKey(Input.Keys.A))
                        {
                            // blend strafing and throwing anim (if W also pressed walk anim blend, if S also pressed back anim blend)

                            // m_velocity = ett värde, ingen acceleration
                            // chadMovement.Move(0,-m_velocity);
                            Debug.Log("Charging ball and strafing left");
                        }
                        if (Input.GetKey(Input.Keys.D))
                        {
                            // blend strafing and throwing anim (if W also pressed walk anim blend, if S also pressed back anim blend)

                            // m_velocity = ett värde, ingen acceleration
                            // chadMovement.Move(0, m_velocity);
                            Debug.Log("Charging ball and strafing right");
                        }

                        // can enter following after releasing ball: walking/backing/strafing/turning/idle
                        if (Input.GetMouseButtonUp(Input.MouseButtons.LEFT))
                        {
                            // player was charging and is not throwing

                            // play throwing anim
                            // chadBall.Throw(m_chargeForce) // release the Kraken

                            Debug.Log("Releasing ball charge to throw");
                            // check what mode to enter
                            if (Input.GetKey(Input.Keys.W))
                            {
                                Debug.Log("Changed from THROWING to WALKING");
                                m_state = State.WALKING;
                            }
                            else if (Input.GetKey(Input.Keys.S))
                            {
                                Debug.Log("Changed from THROWING to BACKING");
                                m_state = State.BACKING;
                            }
                            else if (Input.GetKey(Input.Keys.A) || Input.GetKey(Input.Keys.D))
                            {
                                Debug.Log("Changed from THROWING to STRAFING");
                                m_state = State.STRAFING;
                            }
                            else if (Input.GetMouseX() * Time.ActualDeltaTime != m_xStep)
                            {
                                m_state = State.TURNING;
                                Debug.Log("Changed state from THROWING to TURNING");
                            }
                            else
                            {
                                Debug.Log("Changed from THROWING to IDLE");
                                m_state = State.IDLE;
                            }
                        }
                    }
                    break;
                case State.WALKING:
                    // __SIMULTAENOUS__ WALK and: THROW/TURN
                    // __CAN_ENTER__  IDLE/RAGDOLL/JUMP/RUNNING

                    // m_velocity += en faktor för acceleration
                    // chadMovement.Move(m_velocity);

                    //play walking anim
                    Debug.Log("Walking");
                    if(Input.GetMouseButton(Input.MouseButtons.LEFT) /*&& character is actually holding something to throw*/)
                    {
                        Debug.Log("WALKING and THROWING");
                        m_state = State.THROWING;
                    }
                    //Check if user stopped accelerating and has reached 0 velocity, only check velocity == 0 if we have deacceleration
                    else if(!Input.GetKey(Input.Keys.W)/* && m_velocity == 0*/)
                    {
                        m_velocity = 0; //rewrite if deacceleration
                        m_state = State.IDLE;
                        Debug.Log("Changed state to IDLE");
                    }
                    break;
                
                case State.RAGDOLL: //  last state, cancels sets state to IDLE after RAGDOLL animation is finished
                    Debug.Log("Throwing");
                    break;
            }
        }
    }
}
