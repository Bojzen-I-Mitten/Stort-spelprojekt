using System;
using System.Threading;

using System.Windows;
using System.Windows.Controls;

using Xceed.Wpf.Toolkit.PropertyGrid;

using ThomasEngine;

namespace ThomasEditor.Inspectors
{
    /// <summary>
    /// Interaction logic for TextureEditor.xaml
    /// </summary>
    public partial class TextureEditor : UserControl
    {
        public TextureEditor()
        {
            InitializeComponent();
        }

        private void ResourceEditor_Drop(object sender, DragEventArgs e)
        {

            if (e.Data.GetDataPresent(typeof(TreeViewItem)))
            {
                TreeViewItem item = e.Data.GetData(typeof(TreeViewItem)) as TreeViewItem;
                if (item.DataContext is Resource)
                {
                    Resource resource = item.DataContext as Resource;
                    ContentControl label = sender as ContentControl;
                    PropertyItem pi = label.DataContext as PropertyItem;
                    if (resource.GetType() == pi.PropertyType)
                    {
                        ThomasWrapper.ENTER_SYNC_STATELOCK();
                        pi.Value = resource;
                        ThomasWrapper.EXIT_SYNC_STATELOCK();
                    }

                }
                else if (item.DataContext is ThomasEngine.Object)
                {
                    ThomasEngine.Object obj = item.DataContext as ThomasEngine.Object;
                    ContentControl label = sender as ContentControl;
                    PropertyItem pi = label.DataContext as PropertyItem;
                    if (obj.GetType() == pi.PropertyType)
                    {
                        ThomasWrapper.ENTER_SYNC_STATELOCK();
                        pi.Value = obj;
                        ThomasWrapper.EXIT_SYNC_STATELOCK();
                    }
                    else if (obj is GameObject && (obj as GameObject).inScene && typeof(Component).IsAssignableFrom(pi.PropertyType))
                    {
                        var method = typeof(GameObject).GetMethod("GetComponent").MakeGenericMethod(pi.PropertyType);
                        var component = method.Invoke(obj, null);
                        if (component != null && component.GetType() == pi.PropertyType)
                        {
                            pi.Value = component;
                        }
                    }
                }
            }
        }

        private void ResourceEditor_PreviewDragOver(object sender, DragEventArgs e)
        {
            if (e.Data.GetDataPresent(typeof(TreeViewItem)))
            {
                TreeViewItem item = e.Data.GetData(typeof(TreeViewItem)) as TreeViewItem;
                if (item.DataContext is Resource)
                {
                    Resource resource = item.DataContext as Resource;
                    ContentControl label = sender as ContentControl;
                    PropertyItem pi = label.DataContext as PropertyItem;
                    if (resource.GetType() == pi.PropertyType)
                        e.Handled = true;
                }
            }

        }

        private void ResourceList_Open(object sender, RoutedEventArgs e)
        {
            if (ResourceListPopup.instance != null && ResourceListPopup.instance.IsLoaded)
            {
                ResourceListPopup.instance.Close();
            }
            Button b = sender as Button;
            PropertyItem pi = b.DataContext as PropertyItem;
            Type resourceType = pi.PropertyType;

            ResourceListPopup.instance = new ResourceListPopup(pi, resourceType);
        }
    }
}
