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
        RAGDOLL = 5,    // user got tackled / hit by a power-up
        JUMPING = 6,    // user pressed Space to jump tackle
    };

    public class ChadStateMachine : ScriptComponent
    {
        State m_state;
        Rigidbody rb;
        float xStep = 0.0f;

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
                    // stop all animations
                    // stop all actions
                    //Debug.Log("State: " + m_state);

                    if (Input.GetMouseButton(Input.MouseButtons.LEFT))
                    {
                        Debug.Log("Changed state to THROWING");
                        m_state = State.THROWING;
                    }
                    //else if (rb.isCollidingWithPowerUpOrChad())
                    //    m_state = State.RAGDOLL;
                    else if (Input.GetKey(Input.Keys.Space))
                    {
                        Debug.Log("Changed state to JUMPING");
                        m_state = State.JUMPING;
                    }
                    else if (Input.GetKey(Input.Keys.W))
                    {
                        m_state = State.WALKING;
                        Debug.Log("Changed state to WALKING");
                    }
                    else if (Input.GetMouseX() * Time.ActualDeltaTime != xStep)
                        m_state = State.TURNING;
                    break;
                case State.WALKING:
                    // call chadMovement bla bla
                    Debug.Log("Walking");
                    if(!Input.GetKey(Input.Keys.W))
                    {
                        m_state = State.IDLE;
                        Debug.Log("Changed state to IDLE");
                    }

                    break;
            }
        }
    }
}
