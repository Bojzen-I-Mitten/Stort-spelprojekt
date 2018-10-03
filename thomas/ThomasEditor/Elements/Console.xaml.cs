using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Globalization;
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
        ObservableCollection<DebugMessage> debugMessages = new ObservableCollection<DebugMessage>();
        ObservableCollection<DebugMessage> collapsedDebugMessages = new ObservableCollection<DebugMessage>();

        public Console()
        {
            InitializeComponent();
            messages.ItemsSource = debugMessages;
            CollectionView view = CollectionViewSource.GetDefaultView(messages.ItemsSource) as CollectionView;
            view.Filter = CustomFilter;

            numberOfLogs.Content = 0;
            numberOfWarnings.Content = 0;
            numberOfErrors.Content = 0;

            Debug.OnDebugMessage += Debug_OnDebugMessage;
            ThomasWrapper.OnStartPlaying += ThomasWrapper_OnStartPlaying;
        }

        private bool CustomFilter(object obj)
        {
            DebugMessage msg = obj as DebugMessage;
            if (msg.Message.IndexOf(messageFilter.Text, 0, StringComparison.CurrentCultureIgnoreCase) == -1)
                return false;

            switch (msg.Severity)
            {
                case MessageSeverity.Info:
                    return showInfoMessages.IsChecked == true;
                case MessageSeverity.Warning:
                    return showWarningMessages.IsChecked == true;
                case MessageSeverity.Error:
                    return showErrorMessages.IsChecked == true;
                case MessageSeverity.ThomasCore:
                    return showErrorMessages.IsChecked == true;
            }
            return false;
        }

        private void Debug_OnDebugMessage(DebugMessage newMessage)
        {
            this.Dispatcher.BeginInvoke((Action)(() =>
            {
                AddMessage(newMessage);
            }));
            
        }

        private void AddMessage(DebugMessage newMessage)
        {
            switch (newMessage.Severity)
            {
                case MessageSeverity.Info:
                    numberOfLogs.Content = (int)numberOfLogs.Content + 1;
                    break;
                case MessageSeverity.Warning:
                    numberOfWarnings.Content = (int)numberOfWarnings.Content + 1;
                    break;
                case MessageSeverity.Error:
                    numberOfErrors.Content = (int)numberOfErrors.Content + 1;
                    break;
                case MessageSeverity.ThomasCore:
                    numberOfErrors.Content = (int)numberOfErrors.Content + 1;
                    break;
            }

            debugMessages.Insert(0, newMessage);


            foreach(DebugMessage message in collapsedDebugMessages)
            {
                if(message.Message == newMessage.Message)
                {
                    message.Count++;
                    return;
                }
            }
            collapsedDebugMessages.Insert(0, newMessage);

        }


        private void ThomasWrapper_OnStartPlaying()
        {
            if (clearOnPlay.IsChecked.Value)
                ClearConsole();
        }

        
       
        private void messages_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            selectedMessage.DataContext = (DebugMessage)messages.SelectedItem;
        }


        private void ClearConsole()
        {
            debugMessages.Clear();
            collapsedDebugMessages.Clear();
            numberOfLogs.Content = 0;
            numberOfWarnings.Content = 0;
            numberOfErrors.Content = 0;
        }

        private void ClearConsole_Click(object sender, RoutedEventArgs e)
        {
            ClearConsole();
        }

        private void CollapseMessages_Click(object sender, RoutedEventArgs e)
        {
            messages.ItemsSource = collapsed.IsChecked == true ? collapsedDebugMessages : debugMessages;

            CollectionView view = CollectionViewSource.GetDefaultView(messages.ItemsSource) as CollectionView;
            view.Filter = CustomFilter;
        }

        private void FilterButton_Click(object sender, RoutedEventArgs e)
        {
            CollectionViewSource.GetDefaultView(messages.ItemsSource).Refresh();
        }

        private void messageFilter_TextChanged(object sender, TextChangedEventArgs e)
        {
            CollectionViewSource.GetDefaultView(messages.ItemsSource).Refresh();
        }
    }

    public class DebugMessageCollapsedConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            DebugMessage message = value as DebugMessage;
            return message.Count > 1 ? Visibility.Visible : Visibility.Hidden;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new InvalidOperationException("DebugMessageCollapsedConverter can only be used OneWay.");
        }
    }

    public class SeverityToImageConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            DebugMessage message = value as DebugMessage;
            switch (message.Severity)
            {
                case MessageSeverity.Info:
                    return new BitmapImage(new Uri("pack://application:,,/icons/console/console-log.ico"));
                case MessageSeverity.Warning:
                    return new BitmapImage(new Uri("pack://application:,,/icons/console/console-warning.ico"));
                case MessageSeverity.Error:
                    return new BitmapImage(new Uri("pack://application:,,/icons/console/console-error.ico"));
                case MessageSeverity.ThomasCore:
                    return new BitmapImage(new Uri("pack://application:,,/icons/assets/scene.png"));
                default:
                    return new BitmapImage(new Uri("pack://application:,,/icons/console/console-log.ico"));
            }
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new InvalidOperationException("SeverityToImageConverter can only be used OneWay.");
        }
    }
}
