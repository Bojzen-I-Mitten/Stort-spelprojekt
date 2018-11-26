using System;
using System.Threading;

using System.Windows;
using System.Windows.Controls;

using Xceed.Wpf.Toolkit.PropertyGrid;

using ThomasEngine;
using System.Collections;
using System.ComponentModel;
using System.Windows.Data;
using System.Windows.Media;
using System.Linq;

namespace ThomasEditor.Inspectors
{
    /// <summary>
    /// Interaction logic for ListEditor.xaml
    /// </summary>
    public partial class ListEditor : UserControl
    {

        private Type elementType;
        public ListEditor()
        {
            InitializeComponent();
            Loaded += ListEditor_Loaded;
        }

        private void ListEditor_Loaded(object sender, RoutedEventArgs e)
        {
            PropertyItem pi = DataContext as PropertyItem;
            elementType = pi.PropertyType.GetGenericArguments().Single();
        }

        private void PropertyGridEditorIntegerUpDown_ValueChanged(object sender, RoutedPropertyChangedEventArgs<object> e)
        {
            PropertyItem pi = DataContext as PropertyItem;
            IList list = pi.Value as IList;
            int newCount = (int)e.NewValue;
            if (newCount != list.Count)
            {
                if (newCount > list.Count)
                    while (list.Count < newCount)
                        list.Add(null);
                else if (newCount < list.Count)
                    while (list.Count > newCount)
                        list.RemoveAt(list.Count - 1);
                OnPropertyChanged();
            }


        }

        public static T FindParent<T>(DependencyObject child)
          where T : DependencyObject
        {
            //get parent item
            DependencyObject parentObject = VisualTreeHelper.GetParent(child);

            //we've reached the end of the tree
            if (parentObject == null) return null;

            //check if the parent matches the type we're looking for
            T parent = parentObject as T;
            if (parent != null)
            {
                return parent;
            }
            else
            {
                return FindParent<T>(parentObject);
            }
        }



        private void ResourceEditor_Drop(object sender, DragEventArgs e)
        {

            Grid g = (sender as Label).Parent as Grid;
            int index = (int)g.Tag;
            PropertyItem pi = DataContext as PropertyItem;
            if (e.Data.GetDataPresent(typeof(TreeViewItem)))
            {
                TreeViewItem item = e.Data.GetData(typeof(TreeViewItem)) as TreeViewItem;
                if (item.DataContext is Resource)
                {
                    Resource resource = item.DataContext as Resource;
                    ContentControl label = sender as ContentControl;
                    //PropertyItem pi = label.DataContext as PropertyItem;
                    if (resource.GetType() == elementType)
                    {
                        ThomasWrapper.ENTER_SYNC_STATELOCK();
                        (pi.Value as IList)[index] = resource;
                        ThomasWrapper.EXIT_SYNC_STATELOCK();
                    }

                }
                else if (item.DataContext is ThomasEngine.Object)
                {
                    ContentControl label = sender as ContentControl;
                    ThomasEngine.Object obj = item.DataContext as ThomasEngine.Object;
                    if (obj.GetType() == elementType)
                    {
                        ThomasWrapper.ENTER_SYNC_STATELOCK();
                        (pi.Value as IList)[index] = obj;
                        ThomasWrapper.EXIT_SYNC_STATELOCK();
                    }
                    else if (obj is GameObject && (obj as GameObject).inScene && typeof(ThomasEngine.Component).IsAssignableFrom(elementType))
                    {
                        var method = typeof(GameObject).GetMethod("GetComponent").MakeGenericMethod(elementType);
                        var component = method.Invoke(obj, null);
                        if (component != null && component.GetType() == elementType)
                        {
                            (pi.Value as IList)[index] = component;
                        }
                    }
                }
                OnPropertyChanged();
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
                    if (resource.GetType() == elementType)
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
            Grid g = (sender as Button).Parent as Grid;
            int index = (int)g.Tag;
            Button b = sender as Button;
            PropertyItem pi = DataContext as PropertyItem;
            ResourceListPopup.instance = new ResourceListPopup(pi, elementType, index);
            ResourceListPopup.instance.OnPropertyChanged += OnPropertyChanged;
        }

        private void OnPropertyChanged()
        {
            ICollectionView view = CollectionViewSource.GetDefaultView(listView.ItemsSource);
            view.Refresh();
        }
    }
}
