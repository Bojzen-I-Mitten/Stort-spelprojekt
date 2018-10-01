using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Diagnostics;
using CommandLine;
using ThomasEditor;
using ThomasEngine;

namespace ThomasEditor.Testing
{
    public class Options
    {
        [Option]
        public string Filename { get; set; }

        [Option]
        public int Duration { get; set; }
    }


    class Tester
    {
        private int sessionDuration;
        private MainWindow editor;
        private Stopwatch stopwatch;
        private string tempStringProjectPath;
        public Tester(MainWindow editor)
        {
            
            this.editor = editor;
            stopwatch = new Stopwatch();
            stopwatch.Start();
            //Application.currentProjectChanged += SceneDone;
            Scene.OnCurrentSceneChanged += SceneDone;
        }

        public void Parse(string[] args)
        {
            Parser.Default.ParseArguments<Options>(args).WithParsed<Options>(o =>
            {
                if (o.Filename.Length > 0)
                {
                    editor.OpenProject(o.Filename);
                    tempStringProjectPath = o.Filename;
                }

                if (o.Duration > 0)
                {
                    sessionDuration = o.Duration;
                }
                else
                {
                    sessionDuration = 11000000;
                }
            });

      
        }

        public void SceneDone(Scene newScene)
        {
            if (newScene != null)
            {
                //ThomasWrapper.Play();
            }
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
