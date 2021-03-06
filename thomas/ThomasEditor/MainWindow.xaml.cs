﻿using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;
using System.Reflection;
using System.ComponentModel;
using System.Collections.Generic;

using System.IO;

using ThomasEngine;
using ThomasEditor;
using System.Threading;
using System.Windows.Threading;
using Xceed.Wpf.AvalonDock.Layout.Serialization;
using ThomasEditor.Testing;
using HierarchyTreeView;
using ThomasEditor.utils;

namespace ThomasEditor
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    /// 

    public partial class MainWindow : Window
    {

        private Tester tester;
        TimeSpan lastRender;
        public static MainWindow _instance;

        Guid g;
        public MainWindow()
        {
            //EngineAutoProfiler profile = new EngineAutoProfiler("MainWindow");
            _instance = this;
            InitializeComponent();

            playPauseButton.DataContext = false;
            ThomasEngine.Component.editorAssembly = Assembly.GetAssembly(this.GetType());

            //Changeds decimals to . instead of ,
            System.Globalization.CultureInfo customCulture = (System.Globalization.CultureInfo)System.Threading.Thread.CurrentThread.CurrentCulture.Clone();
            customCulture.NumberFormat.NumberDecimalSeparator = ".";

            System.Threading.Thread.CurrentThread.CurrentCulture = customCulture;
            CompositionTarget.Rendering += DoUpdates;


            ThomasEngine.Resources.OnResourceLoadStarted += Resources_OnResourceLoadStarted;
            ThomasEngine.Resources.OnResourceLoadEnded += Resources_OnResourceLoadEnded;
            ThomasEngine.Resources.OnResourceLoad += Resources_OnResourceLoad;

            tester = new Tester(this);
            // Hi there, jag hörde att du gillade fin kod, 
            // Så jag skrev det här för att pigga upp dig.
            if (ThomasEditor.App.args.Args.Length > 0)
            {
                tester.Parse(ThomasEditor.App.args.Args);
            }
            else if (Properties.Settings.Default.latestProjectPath != "")
            {
                OpenProject(Properties.Settings.Default.latestProjectPath);
            }
            else
            {
                this.IsEnabled = false;
                Loaded += new RoutedEventHandler(Timer_OpenProjWindow);
            }
            LoadLayout();
            Closing += MainWindow_Closing;

            ThomasWrapper.OnStartPlaying += ThomasWrapper_OnStartPlaying;
            ThomasWrapper.OnStopPlaying += ThomasWrapper_OnStopPlaying;

            ScriptingManager.scriptReloadStarted += ScriptingManger_scriptReloadStarted;
            ScriptingManager.scriptReloadFinished += ScriptingManger_scriptReloadFinished;

            ThomasWrapper.RenderEditor = Properties.Settings.Default.RenderEditor;
            ThomasWrapper.RenderPhysicsDebug = Properties.Settings.Default.RenderPhysicsDebug;

            GameObjectHierarchy.instance.updateHiearchyParenting = Properties.Settings.Default.updateHiearchyParenting;

            menuItem_editorRendering.IsChecked = ThomasWrapper.RenderEditor;
            menuItem_physicsDebug.IsChecked = ThomasWrapper.RenderPhysicsDebug;
            //profile.sendSample();
        }

        private void Resources_OnResourceLoad(string name, int index, int total)
        {
            this.Dispatcher.Invoke((Action)(() =>
            {
                string fileName = System.IO.Path.GetFileName(name);
                busyCator.BusyContent = String.Format("Loading {0} ({1}/{2})", fileName, index, total);

            }));
        }

        private void Resources_OnResourceLoadEnded()
        {
            this.Dispatcher.Invoke((Action)(() =>
            {
                busyCator.IsBusy = false;
                editor.Visibility = Visibility.Visible;
                game.Visibility = Visibility.Visible;
            }));
        }

        private void Resources_OnResourceLoadStarted()
        {
            this.Dispatcher.Invoke((Action)(() =>
            {
                busyCator.IsBusy = true;
                busyCator.BusyContent = "";
                editor.Visibility = Visibility.Hidden;
                game.Visibility = Visibility.Hidden;
            }));
        }

        private void ThomasWrapper_OnStartPlaying()
        {
            this.Dispatcher.BeginInvoke((Action)(() =>
            {
                game.Focus();
                playPauseButton.DataContext = ThomasWrapper.IsPlaying();
            }));

        }
        private void ThomasWrapper_OnStopPlaying()
        {
            this.Dispatcher.BeginInvoke((Action)(() =>
            {
                playPauseButton.DataContext = ThomasWrapper.IsPlaying();
            }));

        }

        private DispatcherTimer timer;

        private void Timer_OpenProjWindow(object sender, RoutedEventArgs e)
        {
            timer = new DispatcherTimer();
            timer.Interval = TimeSpan.FromSeconds(1);
            timer.Tick += Tick_OpenProjWindow;
            timer.Start();
        }

        private void Tick_OpenProjWindow(object sender, EventArgs e)
        {
            new OpenProjectWindow().Show();
            timer.Stop();
        }

        private void ScriptingManger_scriptReloadFinished()
        {
            this.Dispatcher.Invoke((Action)(() =>
            {
                busyCator.IsBusy = false;
                editor.Visibility = Visibility.Visible;
                game.Visibility = Visibility.Visible;
                ThomasWrapper.Selection.SelectGameObject(g);
            }));
        }

        private void ScriptingManger_scriptReloadStarted()
        {
            g = ThomasWrapper.Selection.GetSelectedGUID();
            this.Dispatcher.Invoke((Action)(() =>
            {
                busyCator.IsBusy = true;
                busyCator.BusyContent = "Reloading scripts...";
                editor.Visibility = Visibility.Hidden;
                game.Visibility = Visibility.Hidden;
            }));
        }

        private void MainWindow_Closing(object sender, CancelEventArgs e)
        {
            SaveLayout();
        }

        private void SaveLayout()
        {
            try
            {
                Directory.CreateDirectory(Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData), "thomas"));
                string target = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData), "thomas/layout.dock");
                using (var sw = new StreamWriter(target))
                {
                    using (StringWriter fs = new StringWriter())
                    {
                        XmlLayoutSerializer xmlLayout = new XmlLayoutSerializer(dockManager);
                        xmlLayout.Serialize(fs);
                        sw.Write(fs.ToString());
                    }
                }
            }
            catch (Exception e)
            {
                Debug.LogError("Failed to save layout.dock. Error: " + e.Message);
            }


            if (WindowState == WindowState.Maximized)
            {
                // Use the RestoreBounds as the current values will be 0, 0 and the size of the screen
                Properties.Settings.Default.Top = RestoreBounds.Top;
                Properties.Settings.Default.Left = RestoreBounds.Left;
                Properties.Settings.Default.Height = RestoreBounds.Height;
                Properties.Settings.Default.Width = RestoreBounds.Width;
                Properties.Settings.Default.Maximized = true;
            }
            else
            {
                Properties.Settings.Default.Top = this.Top;
                Properties.Settings.Default.Left = this.Left;
                Properties.Settings.Default.Height = this.Height;
                Properties.Settings.Default.Width = this.Width;
                Properties.Settings.Default.Maximized = false;
            }

            Properties.Settings.Default.Save();
        }

        private void LoadLayout()
        {
            try
            {
                string target = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData), "thomas/layout.dock");
                if (System.IO.File.Exists(target))
                {
                    using (var sr = new StreamReader(target))
                    {
                        using (StringWriter fs = new StringWriter())
                        {
                            XmlLayoutSerializer xmlLayout = new XmlLayoutSerializer(dockManager);
                            xmlLayout.Deserialize(sr);
                        }
                    }
                }
            }
            catch (Exception e)
            {
                Debug.LogError("Failed to load editor layout. Error: " + e.Message);
            }

            this.Top = Properties.Settings.Default.Top;
            this.Left = Properties.Settings.Default.Left;
            this.Height = Properties.Settings.Default.Height;
            this.Width = Properties.Settings.Default.Width;
            // Very quick and dirty - but it does the job
            if (Properties.Settings.Default.Maximized)
            {
                WindowState = WindowState.Maximized;
            }
        }

        private void Node_MouseRightButtonUp(object sender, MouseButtonEventArgs e)
        {
            ContextMenu cm = this.FindResource("gameObjectContext") as ContextMenu;
            cm.PlacementTarget = sender as TreeViewItem;
            cm.IsOpen = true;
        }

        private void DoUpdates(object sender, EventArgs e)
        {
            RenderingEventArgs args = (RenderingEventArgs)e;

            if (this.lastRender != args.RenderingTime)
            {
                ThomasWrapper.Update();
                tester.Update();
                editorWindow.Title = ThomasWrapper.FrameRate.ToString();
                lastRender = args.RenderingTime;
                transformGizmo.UpdateTransformGizmo();
            }

        }

        private void NewEmptyObject_CanExecute(object sender, CanExecuteRoutedEventArgs e)
        {
            e.CanExecute = true;
        }

        private void AddEmptyGameObject(object sender, RoutedEventArgs e)
        {
            var x = new GameObject("newEmptyGameObject");
            ThomasWrapper.Selection.SelectGameObject(x);
        }

        private void OpenOptionsWindow_CanExecute(object sender, CanExecuteRoutedEventArgs e)
        {
            e.CanExecute = true;
        }

        private void OpenOptionsWindow(object sender, RoutedEventArgs e)
        {
            OptionsWindow oW = new OptionsWindow();
            oW.ShowDialog();
            oW.Focus();
        }

        private void Menu_RemoveGameObject(object sender, RoutedEventArgs e)
        {
            var x = sender as MenuItem;
            GameObject.Destroy((x.DataContext as GameObject));
        }

        private void RemoveSelectedGameObjects_CanExecute(object sender, CanExecuteRoutedEventArgs e)
        {
            e.CanExecute = true;
        }

        private void RemoveSelectedGameObjects(object sender, RoutedEventArgs e)
        {
            var c = Utils.DetachParents(ThomasWrapper.Selection.Ref);
            ThomasWrapper.Selection.UnselectGameObjects();
            foreach (GameObject gObj in c)
            {
                ThomasEngine.Object.Destroy(gObj);
            }
        }




        private void Recompile_Shader_Click(object sender, RoutedEventArgs e)
        {
            Shader.RecompileShaders();
        }

        private void Play_CanExecute(object sender, CanExecuteRoutedEventArgs e)
        {
            e.CanExecute = true;
        }

        private void SaveScene_Click(object sender, RoutedEventArgs e)
        {

            if (ThomasWrapper.CurrentScene.RelativeSavePath != null)
            {
                ThomasWrapper.CurrentScene.SaveScene();
            }
            else
            {
                SaveSceneAs_Click(sender, e);
            }
        }

        private void NewScene_Click(object sender, RoutedEventArgs e)
        {
            ThomasWrapper.Thomas.SceneManagerRef.NewScene("Scene");
        }

        private void SaveSceneAs_Click(object sender, RoutedEventArgs e)
        {

            Microsoft.Win32.SaveFileDialog saveFileDialog = new Microsoft.Win32.SaveFileDialog();
            saveFileDialog.Filter = "Thomas Dank Scene (*.tds) |*.tds";

            if (ThomasEngine.Application.currentProject == null)
                saveFileDialog.InitialDirectory = Environment.GetFolderPath(Environment.SpecialFolder.UserProfile);
            else
                saveFileDialog.InitialDirectory = ThomasEngine.Application.currentProject.assetPath;
            saveFileDialog.RestoreDirectory = true;
            saveFileDialog.FileName = ThomasWrapper.CurrentScene.Name;
            if (saveFileDialog.ShowDialog() == true)
            {
                Scene sceneToSave = ThomasWrapper.CurrentScene;
                sceneToSave.SaveSceneAs(saveFileDialog.FileName);
                sceneToSave.Name = System.IO.Path.GetFileNameWithoutExtension(saveFileDialog.FileName);
            }
        }

        private void LoadScene_Click(object sender, RoutedEventArgs e)
        {
            Microsoft.Win32.OpenFileDialog openFileDialog = new Microsoft.Win32.OpenFileDialog();
            openFileDialog.Filter = "Thomas Dank Scene (*.tds) |*.tds";

            if (ThomasEngine.Application.currentProject == null)
                openFileDialog.InitialDirectory = Environment.GetFolderPath(Environment.SpecialFolder.UserProfile);
            else
                openFileDialog.InitialDirectory = ThomasEngine.Application.currentProject.assetPath;

            openFileDialog.RestoreDirectory = true;
            if (openFileDialog.ShowDialog() == true)
            {
                LoadScene(openFileDialog.FileName);
            }
        }

        public void LoadScene(string path)
        {
            BackgroundWorker worker = new BackgroundWorker();
            showBusyIndicator("Loading scene...");
            worker.DoWork += (o, ea) =>
            {
                ThomasWrapper.Thomas.SceneManagerRef.LoadScene(path);
            };
            worker.RunWorkerCompleted += (o, ea) =>
            {
                hideBusyIndicator();
            };
            worker.RunWorkerAsync();
        }

        private void PlayPauseButton_Click(object sender, ExecutedRoutedEventArgs e)
        {
            if (ThomasWrapper.IsPlaying())
            {
                ThomasWrapper.IssueStopPlay();
            }
            else
            {
                ThomasWrapper.IssuePlay();
                //OnStartPlaying();
               // game.Focus();
            }
        }

        #region primitives

        private void AddNewCubePrimitive(object sender, RoutedEventArgs e)
        {
            // Let's add a indirection to GameObjectManager here
            var x = GameObjectManager.addPrimitive(PrimitiveType.Cube, false);
            ThomasWrapper.Selection.SelectGameObject(x);

        }

        private void AddNewSpherePrimitive(object sender, RoutedEventArgs e)
        {
            var x = GameObjectManager.addPrimitive(PrimitiveType.Sphere, false);
            ThomasWrapper.Selection.SelectGameObject(x);
        }

        private void AddNewQuadPrimitive(object sender, RoutedEventArgs e)
        {
            var x = GameObjectManager.addPrimitive(PrimitiveType.Quad, false);
            ThomasWrapper.Selection.SelectGameObject(x);
        }

        private void AddNewPlanePrimitive(object sender, RoutedEventArgs e)
        {
            var x = GameObjectManager.addPrimitive(PrimitiveType.Plane, false);
            ThomasWrapper.Selection.SelectGameObject(x);
        }

        private void AddNewCylinderPrimitive(object sender, RoutedEventArgs e)
        {
            var x = GameObjectManager.addPrimitive(PrimitiveType.Cylinder, false);
            ThomasWrapper.Selection.SelectGameObject(x);
        }

        private void AddNewCapsulePrimitive(object sender, RoutedEventArgs e)
        {
            var x = GameObjectManager.addPrimitive(PrimitiveType.Capsule, false);
            ThomasWrapper.Selection.SelectGameObject(x);
        }
        private void AddNewTorusPrimitive(object sender, RoutedEventArgs e)
        {
            // Let's add a indirection to GameObjectManager here
            var x = GameObjectManager.addPrimitive(PrimitiveType.Torus, false);
            ThomasWrapper.Selection.SelectGameObject(x);
        }

        private void AddNewMonkeyPrimitive(object sender, RoutedEventArgs e)
        {
            var x = GameObjectManager.addPrimitive(PrimitiveType.Monkey, false);
            ThomasWrapper.Selection.SelectGameObject(x);
        }

        private void AddNewCameraPrimitive(object sender, RoutedEventArgs e)
        {
            var x = new GameObject("newCamera");
            x.AddComponent<Camera>();
            ThomasWrapper.Selection.SelectGameObject(x);
        }

        private void AddNewPointLightPrimitive(object sender, RoutedEventArgs e)
        {
            var x = new GameObject("newPointLight");
            x.AddComponent<LightComponent>();
            x.GetComponent<LightComponent>().Type = LightComponent.LIGHT_TYPES.POINT;
            ThomasWrapper.Selection.SelectGameObject(x);
        }
        private void AddNewSpotLightPrimitive(object sender, RoutedEventArgs e)
        {
            var x = new GameObject("newSpotLight");
            x.AddComponent<LightComponent>();
            x.GetComponent<LightComponent>().Type = LightComponent.LIGHT_TYPES.SPOT;
            ThomasWrapper.Selection.SelectGameObject(x);
        }
        private void AddNewDirectionalLightPrimitive(object sender, RoutedEventArgs e)
        {
            var x = new GameObject("newDirectionalLight");
            x.AddComponent<LightComponent>();
            x.GetComponent<LightComponent>().Type = LightComponent.LIGHT_TYPES.DIRECTIONAL;
            ThomasWrapper.Selection.SelectGameObject(x);
        }
        private void AddNewAreaLightPrimitive(object sender, RoutedEventArgs e)
        {
            var x = new GameObject("newAreaLight");
            x.AddComponent<LightComponent>();
            x.GetComponent<LightComponent>().Type = LightComponent.LIGHT_TYPES.AREA;
            ThomasWrapper.Selection.SelectGameObject(x);
        }

        //Main window CTRL + C
        private void MW_CopyObject(object sender, RoutedEventArgs e)
        {
            GameObjectHierarchy hierarchy = GameObjectHierarchy.instance;
            hierarchy.MenuItem_CopyGameObject(sender, e);
        }

        //Main window CTRL + V
        private void MW_PasteObject(object sender, RoutedEventArgs e)
        {
            Debug.Log("Pasting object..");
            GameObjectHierarchy hierarchy = GameObjectHierarchy.instance;
            hierarchy.MenuItem_PasteGameObject(sender, e);
        }

        private void MW_DuplicateObject(object sender, RoutedEventArgs e)
        {
            MW_CopyObject(sender, e);
            MW_PasteObject(sender, e);
        }

        //Can only be copied if item is selected
        private void MW_CopyObject_CanExecute(object sender, CanExecuteRoutedEventArgs e)
        {
            GameObjectHierarchy hierarchy = GameObjectHierarchy.instance;
            hierarchy.CopyObject_CanExecute(sender, e);
        }

        //Can only paste when an object has been copied
        private void MW_PasteObject_CanExecute(object sender, CanExecuteRoutedEventArgs e)
        {
            GameObjectHierarchy hierarchy = GameObjectHierarchy.instance;
            hierarchy.PasteObject_CanExecute(sender, e);
        }

        #endregion

        public void NewProject_Click(object sender, RoutedEventArgs e)
        {
            Microsoft.Win32.SaveFileDialog saveFileDialog = new Microsoft.Win32.SaveFileDialog();
            saveFileDialog.Filter = "Thomas Project (*.thomas) |*.thomas";


            saveFileDialog.InitialDirectory = Environment.GetFolderPath(Environment.SpecialFolder.UserProfile);
            saveFileDialog.RestoreDirectory = true;
            saveFileDialog.FileName = "New Project";

            if (saveFileDialog.ShowDialog() == true)
            {
                showBusyIndicator("Creating new project...");
                Thread worker = new Thread(new ThreadStart(() =>
                {
                    string fileName = System.IO.Path.GetFileNameWithoutExtension(saveFileDialog.FileName);
                    string dir = System.IO.Path.GetDirectoryName(saveFileDialog.FileName);

                    if (utils.ScriptAssemblyManager.CreateSolution(dir + "\\" + fileName, fileName))
                    {
                        Project proj = new Project(fileName, dir);
                        ThomasEngine.Application.currentProject = proj;
                        utils.ScriptAssemblyManager.SetWatcher(ThomasEngine.Application.currentProject.assetPath);
                    }
                    hideBusyIndicator();
                }));
                worker.SetApartmentState(ApartmentState.STA);
                worker.Start();
                if (timer != null)
                {
                    OpenProjectWindow._instance.ProjectLoadedClose();
                }
            }
        }
        public void OpenProject_Click(object sender, RoutedEventArgs e)
        {
            Microsoft.Win32.OpenFileDialog openFileDialog = new Microsoft.Win32.OpenFileDialog();
            openFileDialog.Filter = "Thomas Project (*.thomas) |*.thomas";

            openFileDialog.InitialDirectory = Environment.GetFolderPath(Environment.SpecialFolder.UserProfile);
            openFileDialog.RestoreDirectory = true;

            if (openFileDialog.ShowDialog() == true)
            {
                OpenProject(openFileDialog.FileName);
                if (timer != null)
                {
                    OpenProjectWindow._instance.ProjectLoadedClose();
                }
            }
        }
        public void OpenProject(string projectPath)
        {
            showBusyIndicator("Opening project...");

            Thread worker = new Thread(new ThreadStart(() =>
            {
                string fileName = System.IO.Path.GetFileNameWithoutExtension(projectPath);
                string dir = System.IO.Path.GetDirectoryName(projectPath);
                if (utils.ScriptAssemblyManager.OpenSolution(dir + "/" + fileName + ".sln"))
                {
                    ThomasEngine.Application.currentProject = Project.LoadProject(projectPath);
                    Properties.Settings.Default.latestProjectPath = projectPath;
                    Properties.Settings.Default.Save();
                    utils.ScriptAssemblyManager.SetWatcher(ThomasEngine.Application.currentProject.assetPath);
                }
                hideBusyIndicator();
            }));
            worker.SetApartmentState(ApartmentState.STA);
            worker.Start();
        }


        public void showBusyIndicator(string message)
        {
            this.Dispatcher.Invoke((Action)(() =>
            {
                busyCator.IsBusy = true;
                busyCator.BusyContent = message;
                editor.Visibility = Visibility.Hidden;
                game.Visibility = Visibility.Hidden;
            }));
        }
        public void hideBusyIndicator()
        {
            this.Dispatcher.Invoke((Action)(() =>
            {
                busyCator.IsBusy = false;
                editor.Visibility = Visibility.Visible;
                game.Visibility = Visibility.Visible;
            }));
        }


        private void ReloadAssembly(object sender, RoutedEventArgs e)
        {
            Thread worker = new Thread(new ThreadStart(() =>
            {
                utils.ScriptAssemblyManager.BuildSolution();
            }));
            worker.SetApartmentState(ApartmentState.STA);
            worker.Start();
        }

        private void __layoutRoot_PropertyChanged(object sender, PropertyChangedEventArgs e)
        {

        }

        private void SaveLayout_Click(object sender, RoutedEventArgs e)
        {
            SaveLayout();
        }

        private void MenuItem_ToggleEditorRendering(object sender, RoutedEventArgs e)
        {
            MenuItem item = sender as MenuItem;
            ThomasWrapper.RenderEditor = item.IsChecked;
            Properties.Settings.Default.RenderEditor = item.IsChecked;
            Properties.Settings.Default.Save();
        }

        private void MenuItem_TogglePhysicsDebug(object sender, RoutedEventArgs e)
        {
            MenuItem item = sender as MenuItem;
            ThomasWrapper.RenderPhysicsDebug = item.IsChecked;
            Properties.Settings.Default.RenderPhysicsDebug = item.IsChecked;
            Properties.Settings.Default.Save();
        }


        private void MenuItem_ToggleHiearchy(object sender, RoutedEventArgs e)
        {
            MenuItem item = sender as MenuItem;
            GameObjectHierarchy.instance.updateHiearchyParenting = item.IsChecked;
            if (item.IsChecked)
                GameObjectHierarchy.instance.ResetTreeView();
            Properties.Settings.Default.updateHiearchyParenting = item.IsChecked;
            Properties.Settings.Default.Save();
        }

        private bool Build(String filename, Project project)
        {
            try
            {
                return utils.Exporter.ExportProject(filename, project);
            }
            catch (Exception err)
            {
                Debug.LogError("Build failed with exception:");
                Debug.LogError(err);
            }
            finally
            {
                hideBusyIndicator();
            }
            return false;
        }
        private String AcquireBuildDirectory(Project project)
        {
            Microsoft.Win32.SaveFileDialog saveFileDialog = new Microsoft.Win32.SaveFileDialog();
            saveFileDialog.Filter = "Executable (*.exe) |*.exe";


            //saveFileDialog.InitialDirectory = Environment.GetFolderPath(Environment.SpecialFolder.UserProfile);
            saveFileDialog.RestoreDirectory = false;
            saveFileDialog.FileName = project.name;

            if (saveFileDialog.ShowDialog() == true)
            {
                showBusyIndicator("Builing " + project.name + "...");
                return saveFileDialog.FileName;
            }
            return String.Empty;
        }

        private void BuildProject_Click(object sender, RoutedEventArgs e)
        {
            Project project = ThomasEngine.Application.currentProject;
            String diagFile = AcquireBuildDirectory(project);
            if (diagFile != String.Empty)
            {
                Thread worker = new Thread(new ThreadStart(() =>
                {
                    Build(diagFile, project);
                }));
                worker.SetApartmentState(ApartmentState.STA);
                worker.Start();
            }
        }

        private void BuildAndRunProject_Click(object sender, RoutedEventArgs e)
        {
            Project project = ThomasEngine.Application.currentProject;
            String diagFile = AcquireBuildDirectory(project);
            if (diagFile != String.Empty)
            {
                Thread worker = new Thread(new ThreadStart(() =>
                {
                    string fileName = System.IO.Path.GetFileName(diagFile);
                    string dir = System.IO.Path.GetDirectoryName(diagFile);
                    bool success = Build(diagFile, project);
                    if (success)
                    {
                        System.Diagnostics.Process pr = new System.Diagnostics.Process();
                        pr.StartInfo.FileName = dir + "\\Bin\\" + fileName;
                        pr.StartInfo.WorkingDirectory = dir + "\\Bin";
                        pr.Start();
                    }

                    hideBusyIndicator();

                }));
                worker.SetApartmentState(ApartmentState.STA);
                worker.Start();
            }
        }
    }

    public static class Extensions
    {

        public static T GetDescendantByType<T>(this Visual element) where T : class
        {
            if (element == null)
            {
                return default(T);
            }
            if (element.GetType() == typeof(T))
            {
                return element as T;
            }
            T foundElement = null;
            if (element is FrameworkElement)
            {
                (element as FrameworkElement).ApplyTemplate();
            }
            for (var i = 0; i < VisualTreeHelper.GetChildrenCount(element); i++)
            {
                var visual = VisualTreeHelper.GetChild(element, i) as Visual;
                foundElement = visual.GetDescendantByType<T>();
                if (foundElement != null)
                {
                    break;
                }
            }
            return foundElement;
        }
    }
}
