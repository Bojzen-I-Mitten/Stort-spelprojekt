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
        TURNING = 3,    // turning has to call different functions depending on if previous state was idle or walking/running
        THROWING = 4,   // player throws ball / power-up, not all power-ups activate this state
        JUMPING = 5,    // user got tackled / hit by a power-up
        RAGDOLL = 6,    // user pressed Space to jump tackle
    };

    public class ChadStateMachine : ScriptComponent
    {
        State m_state;
        Rigidbody rb;
        float xStep = 0.0f;
        float m_velocity = 0.0f;

        public override void Start()
        {
            m_state = State.IDLE;
            xStep = Input.GetMouseX() * Time.ActualDeltaTime;

            rb = gameObject.GetComponent<Rigidbody>();
        }

        public override void Update()
        {
            //Debug.Log("Updating..");
            Input.SetMouseMode(Input.MouseMode.POSITION_RELATIVE);
            switch (m_state)
            {
                case State.IDLE:
                    // __SIMULTAENOUS__ IDLE and: nothing
                    // __CAN_ENTER__  WALKING/TURNING/THROWING/RAGDOLL
                    Debug.Log("State: " + m_state);

                    if (Input.GetMouseButton(Input.MouseButtons.LEFT))
                    {
                        Debug.Log("Changed state to THROWING from IDLE");
                        m_state = State.THROWING;
                    }
                    else if (Input.GetKeyDown(Input.Keys.Space))
                    {
                        Debug.Log("Changed state to JUMPING from IDLE");
                        m_state = State.JUMPING;
                    }
                    else if (Input.GetKey(Input.Keys.W))
                    {
                        m_state = State.WALKING;
                        Debug.Log("Changed state to WALKING from IDLE");
                    }
                    else if (Input.GetMouseX() * Time.ActualDeltaTime != xStep)
                        m_state = State.TURNING;
                    //else if (rb.isCollidingWithPowerUpOrChad())
                    //    m_state = State.RAGDOLL;
                    break;
                case State.WALKING:
                    // m_velocity += en faktor för acceleration
                    // chadMovement.Move(m_velocity);

                    // __SIMULTAENOUS__ WALK and: THROW/TURN
                    // __CAN_ENTER__  IDLE/RAGDOLL/JUMP
                    Debug.Log("Walking");
                    if(Input.GetMouseButton(Input.MouseButtons.LEFT))
                    {

                    }
                    //Check if user stopped accelerating and has reached 0 velocity, only check velocity == 0 if we have deacceleration
                    else if(!Input.GetKey(Input.Keys.W) && m_velocity == 0)
                    {
                        m_state = State.IDLE;
                        Debug.Log("Changed state to IDLE");
                    }
                    break;
                case State.THROWING:
                    Debug.Log("Throwing");
                    break;
                case State.RAGDOLL: //  last state, cancels sets state to IDLE after RAGDOLL animation is finished
                    Debug.Log("Throwing");
                    break;
            }
        }
    }
}
