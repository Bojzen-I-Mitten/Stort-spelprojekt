﻿using System;
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
        public Tester(MainWindow editor)
        {
            sessionDuration = -1;
            this.editor = editor;
            stopwatch = new Stopwatch();
            stopwatch.Start();
            //Application.currentProjectChanged += SceneDone;
            ThomasWrapper.Thomas.SceneManagerRef.OnCurrentSceneChanged += SceneDone;
        }

        public void Parse(string[] args)
        {
            Parser.Default.ParseArguments<Options>(args).WithParsed<Options>(o =>
            {
                if (o.Filename != null)
                {
                    if (o.Filename.Length > 0)
                    {

                        editor.OpenProject(o.Filename);
                    }
                }

        
                if (o.Duration > 0)
                {
                    sessionDuration = o.Duration;
                }
      
            });

      
        }

        public void SceneDone(Scene oldScene, Scene newScene)
        {
            if (newScene != null)
            {
                //ThomasWrapper.Play();
            }
        }

        public void Update()
        {
            if (stopwatch.ElapsedMilliseconds > sessionDuration && sessionDuration != -1)
            {
                // Session should terminate, as we have exceed the 
                // time the session should run
                System.Windows.Application.Current.Shutdown();

            }
        }
    }
}
