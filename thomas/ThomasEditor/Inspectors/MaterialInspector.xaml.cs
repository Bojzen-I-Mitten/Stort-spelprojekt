using System;
using System.Collections.Generic;
using System.ComponentModel;

using System.Globalization;
using System.Linq;

using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using ThomasEngine;
namespace ThomasEditor
{
    namespace Converters
    {
        public class IsEditableMaterial : IValueConverter
        {
            public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
            {
                Material mat = value as Material;
                return (mat != Material.StandardMaterial);
            }

            public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
            {
                throw new InvalidOperationException("IsEditableMaterial can only be used OneWay.");
            }
        }
    }
    namespace Inspectors
    {
        /// <summary>
        /// Interaction logic for MaterialEditor.xaml
        /// </summary>
        /// 
        public partial class MaterialInspector : UserControl, INotifyPropertyChanged
        {
            public virtual event PropertyChangedEventHandler PropertyChanged;
            public void OnPropertyChanged(String info)
            {
                if(PropertyChanged != null)
                    PropertyChanged(this, new PropertyChangedEventArgs(info));
            }


            public DictionaryPropertyGridAdapter MaterialProperties { get; set; }

            public Shader SelectedShader
            {
                get { return (DataContext as Material).Shader; }
                set { (DataContext as Material).Shader = value; MaterialUpdated(); }
            }

            public List<Shader> AvailableShaders
            {
                get { return new List<Shader>(ThomasEngine.Resources.GetResourcesOfType(typeof(Shader)).Cast<Shader>()); }
            }

            public MaterialInspector()
            {
                InitializeComponent();
                DataContextChanged += MaterialInspector_DataContextChanged;
            }

            private void MaterialInspector_DataContextChanged(object sender, DependencyPropertyChangedEventArgs e)
            {
                MaterialUpdated();
            }

            private void MaterialUpdated()
            {
                if(DataContext is Material)
                {
                    if (MaterialProperties != null)
                    {
                        MaterialProperties.OnPropertyChanged -= Adapter_OnPropertyChanged;
                    }
                    MaterialProperties = new DictionaryPropertyGridAdapter((DataContext as Material).EditorProperties);
                    MaterialProperties.OnPropertyChanged += Adapter_OnPropertyChanged;

                    OnPropertyChanged("MaterialProperties");
                }

            }


            DictionaryPropertyGridAdapter adapter;

            private void Adapter_OnPropertyChanged()
            {
                (DataContext as Material).EditorProperties = MaterialProperties._dictionary as Dictionary<String, object>;
            }


        }

        
    }
   
}
