using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Diagnostics;

namespace ThomasEditor.Testing
{

    class Tester
    {
        private Stopwatch stopwatch;
        private long sessionDuration;
        public Tester(long sessionDuration)
        {
            this.sessionDuration = sessionDuration;
            stopwatch = new Stopwatch();
            stopwatch.Start();
        }

        public void Update()
        {
            if (stopwatch.ElapsedMilliseconds > sessionDuration)
            {
                // Session should terminate, as we have exceed the 
                // time the session should run
                System.Windows.Application.Current.Shutdown();

            }
        }
    }
}
