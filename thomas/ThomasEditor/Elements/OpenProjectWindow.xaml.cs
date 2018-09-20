using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using System.ComponentModel;
using System.Threading;
using ThomasEngine;

namespace ThomasEditor
{
    /// <summary>
    /// Interaction logic for OpenProjectWindow.xaml
    /// </summary>
    public partial class OpenProjectWindow : Window
    {
        OpenProjectWindow()
        {
            MainWindow._instance.IsEnabled = false;
            //InitializeComponent();
        }

        ~OpenProjectWindow()
        {
            MainWindow._instance.IsEnabled = true;
        }

        private void NewProject_Click(object sender, RoutedEventArgs e)
        {
            Microsoft.Win32.SaveFileDialog saveFileDialog = new Microsoft.Win32.SaveFileDialog();
            saveFileDialog.Filter = "Thomas Project (*.thomas) |*.thomas";

            saveFileDialog.InitialDirectory = Environment.GetFolderPath(Environment.SpecialFolder.UserProfile);
            saveFileDialog.RestoreDirectory = true;
            saveFileDialog.FileName = "New Project";


            if (saveFileDialog.ShowDialog() == true)
            {
                MainWindow._instance.showBusyIndicator("Creating new project...");
                Thread worker = new Thread(new ThreadStart(() =>
                {
                    string fileName = System.IO.Path.GetFileNameWithoutExtension(saveFileDialog.FileName);
                    string dir = System.IO.Path.GetDirectoryName(saveFileDialog.FileName);

                    if (utils.ScriptAssemblyManager.CreateSolution(dir + "\\" + fileName, fileName))
                    {
                        ThomasEngine.Project proj = new ThomasEngine.Project(fileName, dir);
                        ThomasEngine.Application.currentProject = proj;
                        utils.ScriptAssemblyManager.SetWatcher(ThomasEngine.Application.currentProject.assetPath);
                    }
                    MainWindow._instance.hideBusyIndicator();
                }));
                worker.SetApartmentState(ApartmentState.STA);
                worker.Start();
            }
        }
        
        private void Cancel_Click(object sender, RoutedEventArgs e)
        {
            MainWindow._instance.Close();
        }

        private void LoadProject_Click(object sender, RoutedEventArgs e)
        {
            Microsoft.Win32.OpenFileDialog openFileDialog = new Microsoft.Win32.OpenFileDialog();
            openFileDialog.Filter = "Thomas Project (*.thomas) |*.thomas";
            openFileDialog.InitialDirectory = Environment.GetFolderPath(Environment.SpecialFolder.UserProfile);

            openFileDialog.RestoreDirectory = true;
            if (openFileDialog.ShowDialog() == true)
            {
                MainWindow._instance.OpenProject(openFileDialog.FileName);
            }
        }
    }
}
