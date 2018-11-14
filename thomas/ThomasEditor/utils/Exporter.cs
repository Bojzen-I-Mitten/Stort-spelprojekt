using System;
using System.CodeDom.Compiler;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;
using Microsoft.CSharp;
namespace ThomasEditor.utils
{
    class Exporter
    {
        public static bool ExportProject(string path, ThomasEngine.Project project)
        {
            if(!ScriptAssemblyManager.BuildSolution())
            {
                return false;
            }

            MainWindow._instance.showBusyIndicator("Builing " + project.name + "...");

            string fileNameNoExe = System.IO.Path.GetFileNameWithoutExtension(path);
            string fileName = System.IO.Path.GetFileName(path);
            string dir = System.IO.Path.GetDirectoryName(path);

            System.IO.Directory.CreateDirectory(dir + "\\Bin");
            System.IO.Directory.CreateDirectory(dir + "\\Data");
            System.IO.Directory.CreateDirectory(dir + "\\Data\\Assets");

            CSharpCodeProvider codeProvider = new CSharpCodeProvider();
            
            string iconPath = ThomasEngine.Application.currentProject.assetPath + "\\icon.ico";
            string extraCommands = "";
            if (System.IO.File.Exists(iconPath))
                extraCommands += String.Format(@"/win32icon:""{0}""", iconPath);

            CompilerParameters parameters = new CompilerParameters();
            parameters.GenerateExecutable = true;
            parameters.IncludeDebugInformation = false;
            parameters.CompilerOptions = @"/optimize+ /platform:x64 /target:exe " + @extraCommands;
            parameters.OutputAssembly = dir + "\\Bin\\" + fileName;
            
            parameters.ReferencedAssemblies.Add("System.dll");
            parameters.ReferencedAssemblies.Add("ThomasEngine.dll");
            parameters.ReferencedAssemblies.Add("WPF\\WindowsBase.dll");
            


            CompilerResults results = codeProvider.CompileAssemblyFromFile(parameters, ThomasEngine.Application.editorAssets + "\\AssemblyFiles\\ExportProject.cs");
            if (results.Errors.Count > 0)
            {
                foreach (CompilerError CompErr in results.Errors)
                {
                    ThomasEngine.Debug.LogError(CompErr.ErrorText);
                }
                return false;
            }
            MainWindow._instance.showBusyIndicator("Transfering files...");
            //Copy all the files & Replaces any files with the same name
            CopyFiles(project.assetPath, dir + "\\Data\\Assets");
            CopyFiles(ThomasEngine.Application.editorAssets, dir + "\\Data");

#if DEBUG
            System.IO.File.Copy(project.assembly + "\\Debug\\Assembly.dll", dir + "\\Bin\\Assembly.dll", true);
#else
            File.Copy(project.assembly + "\\Release\\Assembly.dll", dir + "\\Bin\\Assembly.dll", true);
#endif

            File.Copy(project.path + "\\" +project.name + ".thomas", dir + "\\Data\\project.thomas", true);

            foreach (string dll in Directory.GetFiles(Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location), "Thomas*.dll", SearchOption.TopDirectoryOnly))
            {
                string dllName = System.IO.Path.GetFileName(dll);
                File.Copy(dll, dir + "\\Bin\\" + dllName, true);
            }

            
            File.Copy("Newtonsoft.Json.dll", dir + "\\Bin\\Newtonsoft.Json.dll", true);

#if DEBUG
            File.Copy(project.path + "\\" + project.name + "\\dll\\fmodL64.dll", dir + "\\Bin\\fmodL64.dll", true);
            File.Copy(project.path + "\\" + project.name + "\\dll\\fmodstudioL64.dll", dir + "\\Bin\\fmodstudioL64.dll", true);
#else
            File.Copy(project.path + "\\" + project.name + "\\dll\\fmod64.dll", dir + "\\Bin\\fmod64.dll", true);
            File.Copy(project.path + "\\" + project.name + "\\dll\\fmodstudio64.dll", dir + "\\Bin\\fmodstudio64.dll", true);
#endif

            CreateShortcut(dir + "\\" + fileNameNoExe + ".lnk", dir + "\\Bin\\" + fileName, dir + "\\Bin");

            return true;
        }


        private static void CopyFiles(string sourceDir, string targetDir)
        {
            foreach (string dirPath in Directory.GetDirectories(sourceDir, "*",
               SearchOption.AllDirectories))
                Directory.CreateDirectory(dirPath.Replace(sourceDir, targetDir));

            //Copy all the files & Replaces any files with the same name
            foreach (string newPath in Directory.GetFiles(sourceDir, "*.*",
                SearchOption.AllDirectories))
                File.Copy(newPath, newPath.Replace(sourceDir, targetDir), true);
        }

        private static void CreateShortcut(string path, string targetPath, string workingDir)
        {
            var shell = new IWshRuntimeLibrary.WshShell();
            var shortcut = shell.CreateShortcut(path) as IWshRuntimeLibrary.IWshShortcut;
            shortcut.TargetPath = targetPath;
            shortcut.WorkingDirectory = workingDir;
            //shortcut...
            shortcut.Save();
        }
    }
}
