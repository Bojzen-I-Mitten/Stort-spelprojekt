using System;
using System.Threading;

using System.Windows;
using System.Windows.Controls;

using Xceed.Wpf.Toolkit.PropertyGrid;

using ThomasEngine;

namespace ThomasEditor.Inspectors
{
    /// <summary>
    /// Interaction logic for ResourceEditor.xaml
    /// </summary>
    public partial class ResourceEditor : UserControl
    {
        public ResourceEditor()
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
                        //Monitor.Enter(ThomasWrapper.CurrentScene.GetGameObjectsLock());
                        pi.Value = resource;

                        //Monitor.Exit(ThomasWrapper.CurrentScene.GetGameObjectsLock());
                    }

                }
                else if (item.DataContext is ThomasEngine.Object)
                {
                    ThomasEngine.Object obj = item.DataContext as ThomasEngine.Object;
                    ContentControl label = sender as ContentControl;
                    PropertyItem pi = label.DataContext as PropertyItem;
                    if (obj.GetType() == pi.PropertyType)
                    {
                        //Monitor.Enter(ThomasWrapper.CurrentScene.GetGameObjectsLock());
                        pi.Value = obj;

                        //Monitor.Exit(ThomasWrapper.CurrentScene.GetGameObjectsLock());
                    }
                    else if (obj is GameObject && (obj as GameObject).inScene && typeof(Component).IsAssignableFrom(pi.PropertyType))
                    {


                        var component = (obj as GameObject).GetComponent(pi.PropertyType);
                        if (component != null)
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
                //e.Effects = DragDropEffects.None;
                TreeViewItem item = e.Data.GetData(typeof(TreeViewItem)) as TreeViewItem;
                if (item.DataContext is Resource)
                {
                    Resource resource = item.DataContext as Resource;
                    ContentControl label = sender as ContentControl;
                    PropertyItem pi = label.DataContext as PropertyItem;
                    if (resource.GetType() == pi.PropertyType)
                    {
                        e.Effects = DragDropEffects.Move;
                        e.Handled = true;
                    }
                    else
                        e.Effects = DragDropEffects.None;
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
