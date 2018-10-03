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
using System.Windows.Threading;
using System.Threading;
using ThomasEngine;

namespace ThomasEditor
{
    /// <summary>
    /// Interaction logic for OpenProjectWindow.xaml
    /// </summary>
    public partial class OpenProjectWindow : Window
    {
        public static OpenProjectWindow _instance;
        private bool xClose = true;
        public OpenProjectWindow()
        {
            Thread.Sleep(2000);
            InitializeComponent();
            IsEnabled = true;
            Focusable = true;
            Focus();

            Closed += OpenProjectWindow_Closed;

            _instance = this;
        }

        public void ProjectLoadedClose()
        {
            xClose = false;

            Close();
        }

        private void OpenProjectWindow_Closed(object sender, EventArgs e)
        {
            if (xClose)
                MainWindow._instance.Close();
            else
            {
                MainWindow._instance.Dispatcher.Invoke(() =>
                {
                    MainWindow._instance.IsEnabled = true;
                });
            }
            
        }

        private void NewProject_Click(object sender, RoutedEventArgs e)
        {
            MainWindow._instance.Dispatcher.Invoke(() =>
            {
                MainWindow._instance.NewProject_Click(sender, e);
            });
        }
        
        private void Cancel_Click(object sender, RoutedEventArgs e)
        {   
            if (MainWindow._instance.Dispatcher.CheckAccess())
                MainWindow._instance.Close();
            else
                MainWindow._instance.Dispatcher.Invoke(DispatcherPriority.Normal, new ThreadStart(MainWindow._instance.Close));
            Close();
        }

        private void OpenProject_Click(object sender, RoutedEventArgs e)
        {
            MainWindow._instance.Dispatcher.Invoke(() =>
            {
                MainWindow._instance.OpenProject_Click(sender, e);
            });
        }
    }
}
