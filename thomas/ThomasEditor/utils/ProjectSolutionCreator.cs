using Microsoft.Build.Evaluation;
using Microsoft.Build.Execution;
using Microsoft.Build.Framework;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Threading;
using ThomasEngine;
namespace ThomasEditor.utils
{

    class ScriptAssemblyManager
    {
        public static string assemblyPath = "";
        public static string projectPath = "";
        private static Microsoft.Build.Evaluation.Project projectEval;
        public static FileSystemWatcher fsw;
        private static bool building = false;
        public static void SetWatcher(string path)
        {
            if(fsw != null)
                fsw.EnableRaisingEvents = false;
            fsw = new FileSystemWatcher();
            fsw.Changed += Fsw_Changed;
            fsw.Renamed += Fsw_Renamed;
            fsw.Path = path;
            fsw.IncludeSubdirectories = true;
            fsw.Filter = "*.cs";
            fsw.EnableRaisingEvents = true;
        }

        private static void Fsw_Renamed(object sender, RenamedEventArgs e)
        {
            if (Path.GetExtension(e.Name) != ".cs" || Path.GetExtension(e.OldName) != ".cs")
            {
                if(Path.GetExtension(e.Name) == ".cs")
                {
                    FileChanged();
                }
                return;
            }
            Thread worker = new Thread(new ThreadStart(() =>
            {
                var includePath = e.OldFullPath.Substring(projectEval.DirectoryPath.Length + 1);
                var projectItem = projectEval.GetItems("Compile").FirstOrDefault(item => item.EvaluatedInclude.Equals(includePath));
                if (projectItem != null)
                {
                    var newIncludePath = e.FullPath.Substring(projectEval.DirectoryPath.Length + 1);
                    projectItem.Rename(newIncludePath);
                    projectEval.Save();
                }
            }));
            worker.SetApartmentState(ApartmentState.STA);
            worker.Start();
        }

        private static void FileChanged()
        {
            Thread worker = new Thread(new ThreadStart(() =>
            {
                BuildSolution();
            }));
            worker.SetApartmentState(ApartmentState.STA);
            worker.Start();
        }

        private static void Fsw_Changed(object sender, FileSystemEventArgs e)
        {
            Thread worker = new Thread(new ThreadStart(() =>
            {
                fsw.EnableRaisingEvents = false;
                BuildSolution();
                fsw.EnableRaisingEvents = true;
            }));
            worker.SetApartmentState(ApartmentState.STA);
            worker.Start();
            
        }

        public static bool CreateSolution(string path, string name)
        {
            Type type = Type.GetTypeFromProgID("VisualStudio.DTE");
            object obj = Activator.CreateInstance(type, true);
            EnvDTE.DTE dte = (EnvDTE.DTE)obj;
            MessageFilter.Register();
            try
            {
                
                dte.MainWindow.Visible = false; // optional if you want to See VS doing its thing

                string template = System.IO.Path.GetFullPath("../Data/assemblyFiles/MyTemplate.vstemplate");
                dte.Solution.AddFromTemplate(template, path, name);
                //create a new solution
                dte.Solution.Create(path, name + ".sln");
                var solution = dte.Solution;
                projectPath = path + "\\" + name + ".csproj";
                EnvDTE.Project project = solution.AddFromFile(projectPath);
                

                // create a C# class library
                System.IO.Directory.CreateDirectory(path + "\\Assets");
                project.ProjectItems.AddFromDirectory(path + "\\Assets");
                assemblyPath = path + "\\" + name + ".sln";


                solution.SolutionBuild.Build(true);
                // save and quit
                dte.ExecuteCommand("File.SaveAll");
                dte.Quit();
                MessageFilter.Revoke();

                projectEval = new Microsoft.Build.Evaluation.Project(projectPath, null, null, new ProjectCollection());
                return true;

            }
            catch(Exception e)
            {
                Debug.Log("Error creating project: " + e.Message);
                MessageFilter.Revoke();
                return false;
            }
        }

        public static bool OpenSolution(string path)
        {
            assemblyPath = path;
            projectPath = path.Replace(".sln", ".csproj");
            projectEval = new Microsoft.Build.Evaluation.Project(projectPath, null, null, new ProjectCollection());
            return BuildSolution();
        }

        public static bool BuildSolution()
        {
            if (building)
                return true;
            building = true;
            MainWindow._instance.showBusyIndicator("Compiling scripts...");
            projectEval = new Microsoft.Build.Evaluation.Project(projectPath, null, null, new ProjectCollection());
            GC.Collect();
            var logger = new ThomasBuildLogger();
#if DEBUG
            projectEval.SetGlobalProperty("Configuration", "Debug");
#else
            projectEval.SetGlobalProperty("Configuration", "Release");
#endif
            
            projectEval.Build(logger);
            MainWindow._instance.hideBusyIndicator();
            building = false;
            return true;
        }

        public static void AddScript(string script)
        {
            Thread worker = new Thread(new ThreadStart(() =>
            {
                var includePath = script.Substring(projectEval.DirectoryPath.Length + 1);
                var projectItem = projectEval.GetItems("Compile").FirstOrDefault(item => item.EvaluatedInclude.Equals(includePath));
                if(projectItem == null)
                {
                    projectEval.AddItem("Compile", includePath);
                    projectEval.Save();
                }
                
            }));
            worker.SetApartmentState(ApartmentState.STA);
            worker.Start();
        }

        public static void RemoveScript(string script)
        {
            Thread worker = new Thread(new ThreadStart(() =>
            {   try
                {

                    var includePath = script.Substring(projectEval.DirectoryPath.Length + 1);
                    var projectItem = projectEval.GetItems("Compile").FirstOrDefault(item => item.EvaluatedInclude.Equals(includePath));
                    projectEval.RemoveItem(projectItem);
                    projectEval.Save();
                }catch(Exception e)
                {

                }
                
            }));
            worker.SetApartmentState(ApartmentState.STA);
            worker.Start();
        }

    }

    public class ThomasBuildLogger : Microsoft.Build.Framework.ILogger
    {
        public LoggerVerbosity Verbosity { get => LoggerVerbosity.Normal; set => throw new NotImplementedException(); }
        public string Parameters { get => throw new NotImplementedException(); set => throw new NotImplementedException(); }

        public void Initialize(IEventSource eventSource)
        {
            eventSource.ErrorRaised += EventSource_ErrorRaised;
            eventSource.WarningRaised += EventSource_WarningRaised;
        }

        private void EventSource_WarningRaised(object sender, BuildWarningEventArgs e)
        {
            //string line = String.Format("Warning {0}({1},{2}): {3}", e.File, e.LineNumber, e.ColumnNumber, e.Message);
            //Debug.Log(line);
        }

        private void EventSource_ErrorRaised(object sender, BuildErrorEventArgs e)
        {
            string line = String.Format("ERROR {0}({1},{2}): {3}", e.File, e.LineNumber, e.ColumnNumber, e.Message);
            Debug.Log(line);
        }

        public void Shutdown()
        {
        }
    }

    public class MessageFilter : IOleMessageFilter
    {
        //
        // Class containing the IOleMessageFilter
        // thread error-handling functions.

        // Start the filter.
        public static void Register()
        {
            IOleMessageFilter newFilter = new MessageFilter();
            IOleMessageFilter oldFilter = null;
            CoRegisterMessageFilter(newFilter, out oldFilter);
        }

        // Done with the filter, close it.
        public static void Revoke()
        {
            IOleMessageFilter oldFilter = null;
            CoRegisterMessageFilter(null, out oldFilter);
        }

        //
        // IOleMessageFilter functions.
        // Handle incoming thread requests.
        int IOleMessageFilter.HandleInComingCall(int dwCallType,
          System.IntPtr hTaskCaller, int dwTickCount, System.IntPtr
          lpInterfaceInfo)
        {
            //Return the flag SERVERCALL_ISHANDLED.
            return 0;
        }

        // Thread call was rejected, so try again.
        int IOleMessageFilter.RetryRejectedCall(System.IntPtr
          hTaskCallee, int dwTickCount, int dwRejectType)
        {
            if (dwRejectType == 2)
            // flag = SERVERCALL_RETRYLATER.
            {
                // Retry the thread call immediately if return >=0 & 
                // <100.
                return 99;
            }
            // Too busy; cancel call.
            return -1;
        }

        int IOleMessageFilter.MessagePending(System.IntPtr hTaskCallee,
          int dwTickCount, int dwPendingType)
        {
            //Return the flag PENDINGMSG_WAITDEFPROCESS.
            return 2;
        }

        // Implement the IOleMessageFilter interface.
        [DllImport("Ole32.dll")]
        private static extern int
          CoRegisterMessageFilter(IOleMessageFilter newFilter, out
          IOleMessageFilter oldFilter);
    }

    [ComImport(), Guid("00000016-0000-0000-C000-000000000046"),
    InterfaceTypeAttribute(ComInterfaceType.InterfaceIsIUnknown)]
    interface IOleMessageFilter
    {
        [PreserveSig]
        int HandleInComingCall(
            int dwCallType,
            IntPtr hTaskCaller,
            int dwTickCount,
            IntPtr lpInterfaceInfo);

        [PreserveSig]
        int RetryRejectedCall(
            IntPtr hTaskCallee,
            int dwTickCount,
            int dwRejectType);

        [PreserveSig]
        int MessagePending(
            IntPtr hTaskCallee,
            int dwTickCount,
            int dwPendingType);
    }
}
