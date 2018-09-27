using System;

using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;

using System.Linq;
using System.Windows.Input;
using System.Windows.Media;
using System.Collections.Generic;
using System.Collections.ObjectModel;

using ThomasEditor.Inspectors;
using ThomasEngine;
using HierarchyTreeView;

namespace ThomasEditor
{
    /// <summary>
    /// Interaction logic for GameObjectHierarchy.xaml
    /// </summary>
    public partial class GameObjectHierarchy : UserControl
    {

        bool _isDragging;
        bool wasUnselected = false;
        public static GameObjectHierarchy instance;
        public GameObjectHierarchy()
        {
            InitializeComponent();

            ThomasWrapper.Selection.Ref.CollectionChanged += SceneSelectedGameObjectChanged;
            ThomasEngine.Transform.OnParentChanged += Transform_OnParentChanged;
            instance = this;

            RootNodes = new ObservableCollection<TreeItemViewModel>();
            hierarchy.ItemsSource = RootNodes;

            Scene.OnCurrentSceneChanged += Scene_OnCurrentSceneChanged;
        }

        private void Scene_OnCurrentSceneChanged(Scene newScene)
        {
            if (newScene != null)
            {
                ResetTreeView();
                newScene.GameObjects.CollectionChanged += SceneGameObjectsChanged;
            }

        }

        public ObservableCollection<TreeItemViewModel> RootNodes { get; set; }

        public ObservableCollection<TreeItemViewModel> SelectedNodes { get; set; }

        private void ResetTreeView()
        {


            this.Dispatcher.Invoke((Action)(() =>
            {
                RootNodes.Clear();
                foreach (GameObject gObj in Scene.CurrentScene.GameObjects)
                {
                    if (gObj.transform.parent == null)
                    {
                        TreeItemViewModel item = new TreeItemViewModel(gObj.Name, gObj)
                        {
                            IsExpanded = true,
                            Children = BuildTree(gObj.transform)      
                        };
                        RootNodes.Add(item);
                    }
                }
            }));
        }

        private void Transform_OnParentChanged(ThomasEngine.Transform child, ThomasEngine.Transform oldParent, ThomasEngine.Transform newParent)
        {
            if (oldParent == newParent || !child.gameObject)
                return;

            ResetTreeView();
            //this.Dispatcher.BeginInvoke((Action)(() =>
            //{
            //    if (oldParent == newParent || !child.gameObject)
            //        return;

            //    var newParentNode = newParent && newParent.gameObject ? FindNode(RootNodes.ToList(), newParent.gameObject) as ItemsControl : hierarchy as ItemsControl;

            //    var childNode = FindNode(RootNodes.ToList(), child.gameObject);

            //    if (childNode != null)
            //    {
            //        Thickness newPadding = childNode.Padding;
            //        if (childNode.Parent != null)
            //        {
            //            ItemsControl oldParentItems = childNode.Parent as ItemsControl;
            //            oldParentItems.Items.Remove(childNode);


            //        }
            //        if (newParentNode != null)
            //        {
            //            ItemsControl newParentItems = newParentNode as ItemsControl;
            //            newParentItems.Items.Add(childNode);
            //            if (newParentNode == hierarchy)
            //                newPadding.Left = 0;
            //            else
            //                newPadding.Left = newParentNode.Padding.Left + 2;

            //        }
            //        childNode.Padding = newPadding;
            //        // childNode.Padding = new Thickness(0, 0, 0, 2);
            //    }

            //}));
        }

        private TreeItemViewModel FindNode(List<TreeItemViewModel> nodes, GameObject gameObject)
        {
            foreach (TreeItemViewModel node in nodes)
            {
                if ((node.Data as GameObject) == gameObject)
                {
                    return node;
                }
                TreeItemViewModel child = FindNode(node.Children, gameObject);
                if (child != null) return child;
            }
            return null;

        }

        private List<TreeItemViewModel> BuildTree(ThomasEngine.Transform parent)
        {
            List<TreeItemViewModel> children = new List<TreeItemViewModel>();

            foreach (ThomasEngine.Transform child in parent.children)
            {
                TreeItemViewModel item = new TreeItemViewModel(child.gameObject.Name, child.gameObject)
                {
                    IsExpanded = true,
                    Children = BuildTree(child.transform)
                };
                children.Add(item);
            }
            return children;
        }

        private void BuildTree(ThomasEngine.Transform parent, TreeViewItem parentTree)
        {
            parentTree.IsExpanded = true;
            if (ThomasWrapper.Selection.Contain(parent.gameObject))
                parentTree.IsSelected = true;

            foreach (ThomasEngine.Transform child in parent.children)
            {
                TreeViewItem node = new TreeViewItem { DataContext = child.gameObject };
               // node.MouseRightButtonUp += Node_MouseRightButtonUp;
                node.SetBinding(TreeViewItem.HeaderProperty, new Binding("Name"));
                //node.Padding = new Thickness(0, 0, 0, 2);
                BuildTree(child, node);
                parentTree.Items.Add(node);
            }
        }

        public void Unselect()
        {
            if (hierarchy.SelectedItem != null)
            {
                wasUnselected = true;
                ThomasWrapper.Selection.UnselectGameObjects();
            }

        }

        private void SceneGameObjectsChanged(object sender, System.Collections.Specialized.NotifyCollectionChangedEventArgs e)
        {
            this.Dispatcher.BeginInvoke((Action)(() => 
            {
                if (e.NewItems != null)
                {
                    foreach (GameObject newItem in e.NewItems)
                    {
                        if (newItem.transform.parent == null)
                        {
                            TreeItemViewModel item = new TreeItemViewModel(newItem.Name, newItem)
                            {
                                IsExpanded = true,
                                Children = BuildTree(newItem.transform),

                            };
                            RootNodes.Add(item);

                            //TreeViewItem node = new TreeViewItem { DataContext = newItem };
                            ////node.MouseRightButtonUp += Node_MouseRightButtonUp;
                            //node.SetBinding(TreeViewItem.HeaderProperty, new Binding("Name"));
                            ////node.Padding = new Thickness(0, 0, 0, 2);
                            //RootNodes.Add(BuildTree(newItem.transform));
                            ////BuildTree(newItem.transform, node);
                            ////hierarchy.Items.Add(node);
                        }
                    }
                }

                if (e.OldItems != null)
                {
                    foreach (GameObject oldItem in e.OldItems)
                    {
                        oldItem.Destroy();
                        DeleteObjectInTree(RootNodes.ToList(), oldItem);
                        //oldItem.Destroy();
                        //foreach (TreeViewItem node in hierarchy.Items)
                        //{
                        //    if (node.DataContext == oldItem)
                        //    {
                        //        //node.MouseRightButtonUp -= Node_MouseRightButtonUp;
                        //        hierarchy.Items.Remove(node);
                        //        break;
                        //    }
                        //}
                    }
                }
            }));
            

        }

        private void DeleteObjectInTree(List<TreeItemViewModel> nodes, GameObject item)
        {
            foreach (TreeItemViewModel node in nodes)
            {
                if((GameObject)node.Data == item)
                {
                    RootNodes.Remove(node);
                    break;
                }
                DeleteObjectInTree(node.Children, item);
            }
            //foreach (TreeViewItem node in nodes)
            //{
            //    if (node.DataContext == item)
            //    {
            //        nodes.Remove(node);
            //        break;
            //    }
            //    DeleteObjectInTree(node.Items, item);
            //}
        }



        private void SelectOrDeselectInTree(ItemCollection nodes, System.Collections.IList items, bool select)
        {
            foreach (TreeViewItem node in nodes)
            {
                if(items.Contains(node.DataContext))
                {
                    node.IsSelected = select;
                }
                SelectOrDeselectInTree(node.Items, items, select);
            }
        }

        private void ResetTree(ItemCollection nodes)
        {
            foreach (TreeViewItem node in nodes)
            {
                node.IsSelected = false;
                ResetTree(node.Items);
            }
        }

        private void SceneSelectedGameObjectChanged(object sender, System.Collections.Specialized.NotifyCollectionChangedEventArgs e)
        {
            //this.Dispatcher.BeginInvoke((Action)(() =>
            //{
            //    if (e.NewItems != null)
            //    {
            //        SelectOrDeselectInTree(hierarchy.Items, e.NewItems, true);
            //    }
            //    if (e.OldItems != null)
            //    {
            //        SelectOrDeselectInTree(hierarchy.Items, e.OldItems, false);
            //    }

            //    if (e.Action == System.Collections.Specialized.NotifyCollectionChangedAction.Reset)
            //    {
            //        ResetTree(hierarchy.Items);
            //    }
            //}));

        }


        private void SelectedGameObjectChanged(object sender, RoutedPropertyChangedEventArgs<object> e)
        {
            ThomasWrapper.Selection.Ref.CollectionChanged -= SceneSelectedGameObjectChanged;
            if (!wasUnselected)
                AssetBrowser.instance.UnselectItem();
            wasUnselected = false;
            ItemContainerGenerator gen = hierarchy.ItemContainerGenerator;
            if (hierarchy.SelectedItem != null)
            {
                TreeItemViewModel item = hierarchy.SelectedItem as TreeItemViewModel;
                if (item != null)
                {
                    Inspector.instance.SelectedObject = (GameObject)item.Data;

                    if (!ThomasWrapper.Selection.Contain((GameObject)item.Data))
                        ThomasWrapper.Selection.SelectGameObject((GameObject)item.Data);
                    hiearchyContextMenu.DataContext = true;
                }

            }
            else
            {
                if (Inspector.instance.SelectedObject is GameObject)
                    Inspector.instance.SelectedObject = null;
            }
            ThomasWrapper.Selection.Ref.CollectionChanged += SceneSelectedGameObjectChanged;
        }

        private void hierarchy_DragOver(object sender, DragEventArgs e)
        {
            if (e.Data.GetDataPresent(typeof(Task)))
            {
                e.Effects = DragDropEffects.Move;
            }
            else
            {
                e.Effects = DragDropEffects.None;
            }
        }

        private TreeViewItem GetNearestContainer(UIElement element)
        {
            // Walk up the element tree to the nearest tree view item.
            TreeViewItem container = element as TreeViewItem;
            while ((container == null) && (element != null))
            {
                element = VisualTreeHelper.GetParent(element) as UIElement;
                //No parent
                if (element.GetType() == typeof(MultiSelectTreeView))
                {
                    return null;
                }
                container = element as TreeViewItem;
            }
            return container;
        }

        TreeViewItem GetItemAtLocation(Point location)
        {
            TreeViewItem foundItem = default(TreeViewItem);
            HitTestResult hitTestResults = VisualTreeHelper.HitTest(hierarchy, location);
            if (hitTestResults != null && hitTestResults.VisualHit is FrameworkElement)
            {
                foundItem = GetNearestContainer(hitTestResults.VisualHit as FrameworkElement);
            }

            return foundItem;
        }

        private void ChangeHierarchy()
        {

        }

        private void hierarchy_Drop(object sender, DragEventArgs e)
        {
            if (e.Data.GetDataPresent(typeof(TreeViewItem)))
            {
                TreeViewItem source = (TreeViewItem)e.Data.GetData(typeof(TreeViewItem));
                //Check if object from hierarchy
                if (source.DataContext.GetType() == typeof(TreeItemViewModel))
                {
                    GameObject sourceData = (GameObject)(source.DataContext as TreeItemViewModel).Data;     //Reads the context as a TreeItemViewModel and retrieves the Data.
                    TreeViewItem target = GetItemAtLocation(e.GetPosition(hierarchy));
                    
                    if (target != null && sourceData != null && (GameObject)((TreeItemViewModel)target.DataContext).Data != sourceData)
                    {
                        
                        GameObject parent = ((TreeItemViewModel)target.DataContext).Data as GameObject;
                        if (!parent.transform.IsChildOf(sourceData.transform))
                        {
                            sourceData.transform.parent = parent.transform;
                        }
                    }
                    else if (sourceData != null && target == null)
                    {
                        if (sourceData.inScene)
                        {
                            sourceData.transform.parent = null;
                        }
                        else
                        {
                            GameObject.Instantiate(sourceData);
                        }   
                    }
                }
                //Check if brefab. (From outside hierarchy)
                else if (source.DataContext.GetType() == typeof(GameObject))
                {
                    GameObject sourceData = (GameObject)source.DataContext;
                    TreeViewItem target = GetItemAtLocation(e.GetPosition(hierarchy));

                    if (target != null && sourceData != null && (GameObject)target.DataContext != sourceData)
                    {
                        GameObject parent = target.DataContext as GameObject;
                        if (!parent.transform.IsChildOf(sourceData.transform))
                        {
                            sourceData.transform.parent = parent.transform;
                        }
                    }
                    else if (sourceData != null && target == null)
                    {
                        if (sourceData.inScene)
                        {
                            sourceData.transform.parent = null;
                        }
                        else
                        {
                            GameObject.Instantiate(sourceData);
                        }
                    }
                }
                // Code to move the item in the model is placed here...
            }
        }

        private void hierarchy_MouseMove(object sender, MouseEventArgs e)
        {
            if (!_isDragging && e.LeftButton == MouseButtonState.Pressed)
            {
                TreeViewItem target = GetItemAtLocation(e.GetPosition(hierarchy));
                if (target != null)
                {
                    _isDragging = true;
                    DragDrop.DoDragDrop(hierarchy, target,
                        DragDropEffects.Move);
                }

            }
            else if (e.LeftButton != MouseButtonState.Pressed)
            {
                _isDragging = false;
            }

        }


        private void MenuItem_DeleteGameObject(object sender, RoutedEventArgs e)
        {
            e.Handled = true;
        }
        private void MenuItem_RenameGameObject(object sender, RoutedEventArgs e)
        {
            e.Handled = true;
        }
        private void MenuItem_SaveAsPrefab(object sender, RoutedEventArgs e)
        {
            TreeViewItem item = hierarchy.SelectedItem as TreeViewItem;
            if (item != null)
            {
                GameObject gObj = (GameObject)item.DataContext;
                if (gObj)
                    ThomasEngine.Resources.SavePrefab(gObj, gObj.Name + ".prefab");

            }
            e.Handled = true;
        }

        private void hierarchy_PreviewMouseDown(object sender, MouseButtonEventArgs e)
        {
            e.Handled = true;
        }

        private void hierarchy_MouseUp(object sender, MouseButtonEventArgs e)
        {

            if (_isDragging)
                return;
            TreeViewItem item = GetItemAtLocation(e.GetPosition(hierarchy));
            if (item != null)
                item.IsSelected = true;
        }
    }
}
