using System;

using System.Windows;
using System.Windows.Controls;
using ThomasEngine;
namespace ThomasEditor.Inspectors
{
    /// <summary>
    /// Interaction logic for Inspector.xaml
    /// </summary>
    public partial class Inspector : UserControl
    {

        public object SelectedObject
        {
            get { return GetValue(SelectedObjectProperty); }
            set
            {
                try
                {
                    SetValue(SelectedObjectProperty, value);
                    SelectedObjectType = value != null ? value.GetType() : null;
                }
                catch (Exception e)
                {
                    // Could occur after file were renamed.
                    Debug.Log("Selection failed with message: " + e.Message);
                    SetValue(SelectedObjectProperty, null);
                    SelectedObjectType = null;
                }
            }
        }

        internal Type SelectedObjectType
        {
            get { return (Type)GetValue(SelectedObjectTypeProperty); }
            set
            {
                    SetValue(SelectedObjectTypeProperty, value);
            }
        }
        public static Inspector instance;
        public Inspector()
        {
            InitializeComponent();
            instance = this;
        }

        public static readonly DependencyProperty SelectedObjectProperty =
           DependencyProperty.Register(
           "SelectedObject",
           typeof(object),
           typeof(Inspector),
           new PropertyMetadata(null));

        public static readonly DependencyProperty SelectedObjectTypeProperty =
           DependencyProperty.Register(
           "SelectedObjectType",
           typeof(Type),
           typeof(Inspector),
           new PropertyMetadata(null));
    }
}
