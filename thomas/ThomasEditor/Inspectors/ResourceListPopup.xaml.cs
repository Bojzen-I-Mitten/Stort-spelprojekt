using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;

using System.Threading;

using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;

using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;

using Xceed.Wpf.Toolkit.PropertyGrid;

using ThomasEngine;
using System.Collections;

namespace ThomasEditor
{
    /// <summary>
    /// Interaction logic for ResourceListPopup.xaml
    /// </summary>
    /// 
    namespace Converters
    {
        public class ResourceImageConverter : IValueConverter
        {
            public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
            {
                if (value is Resource)
                {
                    Resource res = value as Resource;
                    Resources.AssetTypes type = ThomasEngine.Resources.GetResourceAssetType(res.GetType());
                    if (type == Resources.AssetTypes.TEXTURE2D)
                    {
                        Texture2D tex = res as Texture2D;
                        BitmapSource bitmapSource = BitmapSource.Create(tex.width, tex.height, 300, 300, PixelFormats.Bgra32, BitmapPalettes.WebPaletteTransparent, tex.GetRawPixelData(), tex.width*tex.height*4,  4*tex.width);

                        return bitmapSource;
                    }
                    return AssetBrowser.assetImages[type].UriSource.LocalPath;
                }
                else if(value is ThomasEngine.Object)
                {
                    return "../icons/assets/prefab.png";
                }
                else
                    return "../icons/null.png";
            }

            public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
            {
                throw new NotImplementedException();
            }
        }
    }

    public partial class ResourceListPopup : Window
    {

        static public ResourceListPopup instance;
        public Type resourceType;
        private PropertyItem _property;
        private int index = -1;

        public delegate void PropertyChanged();
        public new event PropertyChanged OnPropertyChanged;

        public ResourceListPopup(PropertyItem property, Type resourceType, int elementIndex) : this(property, resourceType)
        {
            index = elementIndex;
        }

        public ResourceListPopup(PropertyItem property, Type resourceType)
        {
            this.resourceType = resourceType;
            InitializeComponent();
            _property = property;
            Title = "Select " + resourceType.Name;
           
                       
            List<object> resources = ThomasEngine.Resources.GetResourcesOfType(resourceType).Cast<object>().ToList();
           

            if(resourceType == typeof(Material))
            {
                resources.Insert(0, Material.StandardMaterial);
            }
            else if(resourceType == typeof(Texture2D))
            {
                resources.Insert(0, Texture2D.blackTexture);
                resources.Insert(0, Texture2D.whiteTexture);
            }
            else if ((typeof(ThomasEngine.GameObject).IsAssignableFrom(resourceType)))
            {
                resources.AddRange(ThomasEngine.GameObject.GetAllGameObjects(true));
            }
            else if ((typeof(ThomasEngine.Object).IsAssignableFrom(resourceType)))
            {
                resources.AddRange(ThomasEngine.Object.GetObjectsOfType(resourceType));
            }
            resources.Insert(0, "None");
            ResourceList.ItemsSource = resources;
            CollectionViewSource.GetDefaultView(ResourceList.ItemsSource).Filter = ResourcesFilter;
            ResourceFilter.Focus();
            Visibility = Visibility.Visible;
        }

        private void ResourceList_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            SetPropertyToSelection();

        }

        private void SetPropertyToSelection()
        {
            Monitor.Enter(Scene.CurrentScene.GetGameObjectsLock());
            if (ResourceList.SelectedItem != null)
            {
                if (ResourceList.SelectedItem is Resource)
                {
                    if (index >= 0)
                        (_property.Value as IList)[index] = ResourceList.SelectedItem as Resource;
                    else
                        _property.Value = ResourceList.SelectedItem as Resource;
                }
                else if (ResourceList.SelectedItem is String)
                {
                    if (index >= 0)
                        (_property.Value as IList)[index] = null;
                    else
                        _property.Value = null;
                }
                else if (ResourceList.SelectedItem is ThomasEngine.Object)
                {
                    if (index >= 0)
                        (_property.Value as IList)[index] = ResourceList.SelectedItem;
                    else
                        _property.Value = ResourceList.SelectedItem;
                }

            }
            Monitor.Exit(Scene.CurrentScene.GetGameObjectsLock());
            if(OnPropertyChanged != null)
                OnPropertyChanged();
        }

        private void ResourceList_MouseDoubleClick(object sender, MouseButtonEventArgs e)
        {
            SetPropertyToSelection();
            Close();
        }

        private bool ResourcesFilter(object item)
        {
            if (String.IsNullOrEmpty(ResourceFilter.Text))
                return true;
            else
            {
                if (item is String)
                    return true;
                else if(item is Resource)
                {
                    return ((item as Resource).ToString().IndexOf(ResourceFilter.Text, StringComparison.OrdinalIgnoreCase) >= 0);
                }
                return false;
            }
        }
        private void ResourceFilter_TextChanged(object sender, TextChangedEventArgs e)
        {
            CollectionViewSource.GetDefaultView(ResourceList.ItemsSource).Refresh();
        }
    }
}
