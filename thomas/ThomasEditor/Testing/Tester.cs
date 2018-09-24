using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Diagnostics;
using CommandLine;
using ThomasEditor;

namespace ThomasEditor.Testing
{
    public class Options
    {
        [Option('f', "filename", Required = false)]
        public bool Filename { get; set; }
    }

    class Tester
    {
        private Stopwatch stopwatch;
        private long sessionDuration;
        public Tester(long sessionDuration, string[] args)
        {
            Parser.Default.ParseArguments<Options>(args).WithParsed<Options>(o =>
            {
                if (o.Filename)
                {
                    o.Filename.ToString();
                }
            });
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
