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
        JUMP = 6,

    }

    public class ChadStateMachine : NetworkComponent
    {
        //public int testBoi { get; set; } = 0;
        public override void Start()
        {

        }

        public override void Update()
        {

        }
    }
}
