using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;
using System.Reflection;
using System.ComponentModel;

using System.IO;

using ThomasEngine;
using System.Threading;
using System.Windows.Threading;
using Xceed.Wpf.AvalonDock.Layout.Serialization;

namespace ThomasEditor
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    /// 

    public partial class MainWindow : Window
    {
        
        TimeSpan lastRender;
        public static MainWindow _instance;

        Guid g;
        public MainWindow()
        {
            string enginePath = Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location);

            Environment.SetEnvironmentVariable("THOMAS_ENGINE", enginePath, EnvironmentVariableTarget.User);

            _instance = this;
            InitializeComponent();

            playPauseButton.DataContext = false;
            ThomasEngine.Component.editorAssembly = Assembly.GetAssembly(this.GetType());

            //Changeds decimals to . instead of ,
            System.Globalization.CultureInfo customCulture = (System.Globalization.CultureInfo)System.Threading.Thread.CurrentThread.CurrentCulture.Clone();
            customCulture.NumberFormat.NumberDecimalSeparator = ".";

            System.Threading.Thread.CurrentThread.CurrentCulture = customCulture;
            CompositionTarget.Rendering += DoUpdates;

            if (Properties.Settings.Default.latestProjectPath != "")
                OpenProject(Properties.Settings.Default.latestProjectPath);
            else
            {
                this.IsEnabled = false;
                Loaded += new RoutedEventHandler(Timer_OpenProjWindow);
            }

            LoadLayout();
            Closing += MainWindow_Closing;

            ScriptingManger.scriptReloadStarted += ScriptingManger_scriptReloadStarted;
            ScriptingManger.scriptReloadFinished += ScriptingManger_scriptReloadFinished;
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
            }catch(Exception e)
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
            }catch(Exception e)
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
            
            if(this.lastRender != args.RenderingTime)
            {
                ThomasWrapper.Update();
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
            (x.DataContext as GameObject).Destroy();
        }

        private void RemoveSelectedGameObjects_CanExecute(object sender, CanExecuteRoutedEventArgs e)
        {
            e.CanExecute = true;
        }

        private void RemoveSelectedGameObjects(object sender, RoutedEventArgs e)
        {
            for(int i=0; i < ThomasWrapper.Selection.Count; i++)
            {
                GameObject gObj = ThomasWrapper.Selection.op_Subscript(i);
                ThomasWrapper.Selection.UnSelectGameObject(gObj);
                gObj.Destroy();
                //i--;
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

            if(Scene.CurrentScene.RelativeSavePath != null)
            {
                Scene sceneToSave = Scene.CurrentScene;
                Scene.SaveScene(sceneToSave);
            }
            else
            {
                SaveSceneAs_Click(sender, e);
            }
        }

        private void NewScene_Click(object sender, RoutedEventArgs e)
        {
            Scene.CurrentScene.UnLoad();
            Scene.CurrentScene = new Scene("Scene");
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
            saveFileDialog.FileName = Scene.CurrentScene.Name;
            if (saveFileDialog.ShowDialog() == true)
            {
                Scene sceneToSave = Scene.CurrentScene;
                Scene.SaveSceneAs(sceneToSave, saveFileDialog.FileName);
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
                Scene newScene = Scene.LoadScene(path);
                if (newScene != null)
                {
                    Scene.CurrentScene.UnLoad();
                    Scene.CurrentScene = newScene;
                    Scene.CurrentScene.PostLoad();
                }
                else
                {
                    Scene.CurrentScene.UnLoad();
                    Scene.CurrentScene = null;
                    Debug.LogError("Scene failed to load...");
                }
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
                ThomasWrapper.Stop();
            else
            {
                ThomasWrapper.Play();
                game.Focus();
            }
                

            playPauseButton.DataContext = ThomasWrapper.IsPlaying();
        }

        #region primitives

        private void AddNewCubePrimitive(object sender, RoutedEventArgs e)
        {
            var x = GameObject.CreatePrimitive(PrimitiveType.Cube);
            ThomasWrapper.Selection.SelectGameObject(x);
        }

        private void AddNewSpherePrimitive(object sender, RoutedEventArgs e)
        {
            var x = GameObject.CreatePrimitive(PrimitiveType.Sphere);
            ThomasWrapper.Selection.SelectGameObject(x);
        }

        private void AddNewQuadPrimitive(object sender, RoutedEventArgs e)
        {
            var x = GameObject.CreatePrimitive(PrimitiveType.Quad);
            ThomasWrapper.Selection.SelectGameObject(x);
        }

        private void AddNewPlanePrimitive(object sender, RoutedEventArgs e)
        {
            var x = GameObject.CreatePrimitive(PrimitiveType.Plane);
            ThomasWrapper.Selection.SelectGameObject(x);
        }

        private void AddNewCylinderPrimitive(object sender, RoutedEventArgs e)
        {
            var x = GameObject.CreatePrimitive(PrimitiveType.Cylinder);
            ThomasWrapper.Selection.SelectGameObject(x);
        }

        private void AddNewCapsulePrimitive(object sender, RoutedEventArgs e)
        {
            var x = GameObject.CreatePrimitive(PrimitiveType.Capsule);
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
            TreeViewItem item = hierarchy.GetSelection();

            if (item != null)
            {
                Debug.Log("Copying object..");
                hierarchy.SetCopy((GameObject)item.DataContext);

                if (hierarchy.GetCopy())
                {
                    Debug.Log("GameObject successfully copied.");
                }
                return;
            }
        }

        //Main window CTRL + V
        private void MW_PasteObject(object sender, RoutedEventArgs e)
        {
            Debug.Log("Pasting object..");
            GameObjectHierarchy hierarchy = GameObjectHierarchy.instance;

            if (hierarchy.GetCopy())
            {
                GameObject.Instantiate(hierarchy.GetCopy());

                Debug.Log("Pasted object.");

                return;
            }
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
            TreeViewItem item = hierarchy.GetSelection();

            if (item != null)
            {
                e.CanExecute = true;
                return;
            }
        }

        //Can only paste when an object has been copied
        private void MW_PasteObject_CanExecute(object sender, CanExecuteRoutedEventArgs e)
        {
            GameObjectHierarchy hierarchy = GameObjectHierarchy.instance;
            if (hierarchy.GetCopy())
            {
                e.CanExecute = true;
                return;
            }
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
            
            //ScriptingManger.LoadAssembly();
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
            ThomasWrapper.ToggleEditorRendering();
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
