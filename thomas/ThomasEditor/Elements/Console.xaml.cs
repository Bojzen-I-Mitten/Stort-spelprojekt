using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
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
using System.Windows.Navigation;
using System.Windows.Shapes;
using ThomasEngine;
namespace ThomasEditor
{
    /// <summary>
    /// Interaction logic for Console.xaml
    /// </summary>
    public partial class Console : UserControl
    {
        double scrollRatio = 0;
        ObservableCollection<MessageTest> messageList = new ObservableCollection<MessageTest>();
        class MessageTest
        {
            public String Message { get; set; }
            public String MethodInfo { get; set; }
            public Visibility Collapsed {get; set;}
            public int Count { get; set; }
            public MessageTest(String msg, String info) { this.Message = msg; this.MethodInfo = info; this.Collapsed = Visibility.Hidden; this.Count = 1; }
        }

        public Console()
        {
            InitializeComponent();
            messages.ItemsSource = messageList;

            numberOfLogs.Content = 0;
            numberOfWarnings.Content = 0;
            numberOfErrors.Content = 0;

            ThomasWrapper.OutputLog.CollectionChanged += OutputLog_CollectionChanged;
            ThomasWrapper.OnStartPlaying += ThomasWrapper_OnStartPlaying;
        }

        private void ThomasWrapper_OnStartPlaying()
        {
            if (clearOnPlay.IsChecked.Value)
                ClearConsole();
        }

        private void OutputLog_CollectionChanged(object sender, System.Collections.Specialized.NotifyCollectionChangedEventArgs e)
        {
            this.Dispatcher.BeginInvoke((Action)(() =>
            {
                if (e.NewItems != null)
                {
                    foreach (String item in e.NewItems)
                    {
                        AddMessage(item);
                    }
                        
                }
            }));

        }

        private void AddMessage(String msg)
        {
            numberOfLogs.Content = (int)numberOfLogs.Content + 1;
            for (int i = 0; i < messageList.Count; i++)
            {
                MessageTest existingMsg = messageList[i];
                if (existingMsg.Message == msg)
                {
                    existingMsg.Collapsed = Visibility.Visible;
                    existingMsg.Count++;
                    return;
                }
                    
            }
            messageList.Add(new MessageTest(msg, "Debug.Log"));
        }

        private void Console_ScrollChanged(object sender, ScrollChangedEventArgs e)
        {

            ScrollViewer scrollViewer = Extensions.GetDescendantByType<ScrollViewer>(sender as ListBox);

            if (e.ExtentWidthChange != 0 || e.ExtentHeightChange != 0)
            {
                //calculate and set accordingly
                double offset = scrollRatio * e.ExtentHeight - 0.5 * e.ViewportHeight;
                //see if it is negative because of initial values
                if (offset < 0)
                {
                    //center the content
                    //this can be set to 0 if center by default is not needed
                    offset = 0.5 * scrollViewer.ScrollableHeight;
                }
                scrollViewer.ScrollToVerticalOffset(offset);
            }
            else
            {
                //store the relative values if normal scroll
                if (e.ExtentHeight > 0)
                    scrollRatio = (e.VerticalOffset + 0.5 * e.ViewportHeight) / e.ExtentHeight;
            }


        }

        private void messages_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            selectedMessage.DataContext = (MessageTest)messages.SelectedItem;
        }


        private void ClearConsole()
        {
            messageList.Clear();
            numberOfLogs.Content = 0;
        }

        private void ClearConsole_Click(object sender, RoutedEventArgs e)
        {
            ClearConsole();
        }
    }
}
