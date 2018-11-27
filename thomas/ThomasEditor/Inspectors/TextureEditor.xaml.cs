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
        /* Apply a resource property to an item
        */
        private void SetResourceProperty(PropertyItem pi, Resource res)
        {
            if (res.GetType() == pi.PropertyType)
            {
                ThomasWrapper.ENTER_SYNC_STATELOCK();
                pi.Value = res;
                ThomasWrapper.EXIT_SYNC_STATELOCK();
            }
        }
        /* Apply a resource to a button
         */
        private void SetButtonResource(Button b, Resource res)
        {
            PropertyItem pi = b.DataContext as PropertyItem;
            if(pi != null)
            {
                SetResourceProperty(pi, res);
            }
        }
        private void SetResource(object target, Resource res)
        {
            if (target is ContentControl)
            {
                ContentControl label = target as ContentControl;
                PropertyItem pi = label.DataContext as PropertyItem;
                SetResourceProperty(pi, res);
            }
            else if (target is Button)
            {
                Button b = target as Button;
                SetButtonResource(b, res);
            }
            else if(target is Grid)
            {
                Grid g = target as Grid;
                foreach( object o in g.Children)
                {
                    SetResource(o, res);
                }
            }
            else
            {
                String n = target == null ? "NULL" : target.GetType().ToString();
                Debug.LogWarning("Item: " + n + " does not support drop operations.");
            }
        }

        private void ResourceEditor_Drop(object sender, DragEventArgs e)
        {

            if (e.Data.GetDataPresent(typeof(TreeViewItem)))
            {
                TreeViewItem item = e.Data.GetData(typeof(TreeViewItem)) as TreeViewItem;
                if (item.DataContext is Resource)
                {
                    Resource resource = item.DataContext as Resource;
                    SetResource(sender, resource);
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
                    if (label != null && resource != null) // Verify objects are valid...
                    {
                        PropertyItem pi = label.DataContext as PropertyItem;
                        if (resource.GetType() == pi.PropertyType)
                            e.Handled = true;
                    }
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
