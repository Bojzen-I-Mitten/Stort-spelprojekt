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
            get { return (Type)GetValue(SelectedObjectProperty); }
            set { SetValue(SelectedObjectProperty, value); SelectedObjectType = value != null ? value.GetType() : null; }
        }

        public Type SelectedObjectType
        {
            get { return (Type)GetValue(SelectedObjectTypeProperty); }
            set { SetValue(SelectedObjectTypeProperty, value); }
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
