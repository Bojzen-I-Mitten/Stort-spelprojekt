using System;
using System.IO;

using System.ComponentModel;
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
        List<GameObject> m_copiedObjects = new List<GameObject>(); //??correct code convention?
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

        public void Unselect()
        {
            foreach (TreeItemViewModel node in RootNodes)
                node.IsSelected = false;
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
                        }
                    }
                }

                if (e.OldItems != null)
                {
                    foreach (GameObject oldItem in e.OldItems)
                    {
                        oldItem.Destroy();
                        DeleteObjectInTree(RootNodes.ToList(), oldItem);
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
        }



        private void SelectOrDeselectInTree(List<TreeItemViewModel> nodes, System.Collections.IList items, bool select)
        {
            foreach (TreeItemViewModel node in nodes)
            {
                if(items.Contains(node.Data))
                {
                    node.IsSelected = select;
                }
                SelectOrDeselectInTree(node.Children, items, select);
            }
        }

        private void ResetTree(List<TreeItemViewModel> nodes)
        {
            foreach (TreeItemViewModel node in nodes)
            {
                node.IsSelected = false;
                ResetTree(node.Children);
            }
        }

        private void SceneSelectedGameObjectChanged(object sender, System.Collections.Specialized.NotifyCollectionChangedEventArgs e)
        {
            this.Dispatcher.BeginInvoke((Action)(() =>
            {
                if (e.NewItems != null)
                {
                    SelectOrDeselectInTree(RootNodes.ToList(), e.NewItems, true);
                }
                if (e.OldItems != null)
                {
                    SelectOrDeselectInTree(RootNodes.ToList(), e.OldItems, false);
                }

                if (e.Action == System.Collections.Specialized.NotifyCollectionChangedAction.Reset)
                {
                    if(hierarchy.Items.Count > 0)
                        ResetTree(RootNodes.ToList());
                }
            }));

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

        private void ChangeParent(ThomasEngine.Transform parentTransform, List<TreeItemViewModel> allNodes)
        {
            foreach (TreeItemViewModel node in allNodes)
            {
                if (node.IsSelected && ((GameObject)node.Data).transform != parentTransform)
                {
                    ((GameObject)node.Data).transform.parent = parentTransform; //parentTransform is null if no parent is given.
                }
                    ChangeParent(parentTransform, node.Children);
            }
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
                        ChangeParent(parent.transform, RootNodes.ToList());
                    }
                    else if (sourceData != null && target == null)
                    {
                        if (sourceData.inScene)
                        {
                            ChangeParent(null, RootNodes.ToList());
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

        private void hierarchy_MouseUp(object sender, MouseButtonEventArgs e)
        {
            if (_isDragging)
                return;
            TreeViewItem item = GetItemAtLocation(e.GetPosition(hierarchy));
            if(item != null)
                item.IsSelected = true;
        }

        private void MenuItem_CopyGameObject(object sender, RoutedEventArgs e)
        {
            Debug.Log("Entered copy function..");

            List<TreeItemViewModel> items = GetSelectedRootNodes(RootNodes.ToList());
            ClearCopies();
            Debug.Log("Copying object..");

            foreach (TreeItemViewModel item in items)
                SetCopies((GameObject)item.Data);
            return;
        }

        public void MenuItem_PasteGameObject(object sender, RoutedEventArgs e)
        {
            if(m_copiedObjects.Count > 0)
            {
                DetachParent();
                foreach (GameObject copiedObject in m_copiedObjects)
                {
                    GameObject.Instantiate(copiedObject);
                    Debug.Log("Pasted object.");
                }
                
                return;
            }
        }

        private void MenuItem_DuplicateGameObject(object sender, RoutedEventArgs e)
        {
            MenuItem_CopyGameObject(sender, e);
            MenuItem_PasteGameObject(sender, e);
        }

        //Can only click copy when an object is selected
        private void CopyObject_CanExecute(object sender, CanExecuteRoutedEventArgs e)
        {
            List<TreeItemViewModel> nodes = new List<TreeItemViewModel>();
            if (RootNodes != null)
            {
                nodes = GetSelectedRootNodes(RootNodes.ToList());

                if (nodes.Count > 0)
                {
                    e.CanExecute = true;
                    return;
                }
            }
        }

        //Can only paste when an object has been copied
        private void PasteObject_CanExecute(object sender, CanExecuteRoutedEventArgs e)
        {
            if (m_copiedObjects.Count > 0)
            {
                e.CanExecute = true;
            }
        }

        public List<GameObject> GetCopies()
        {
            return m_copiedObjects;
        }

        public void SetCopies(GameObject copiedObject)
        {
            m_copiedObjects.Add(copiedObject);
        }

        public void ClearCopies()
        {
            m_copiedObjects.Clear();
        }

        public List<TreeItemViewModel> GetSelection()
        {
            return GetSelectedRootNodes(RootNodes.ToList());
        }

        private List<TreeItemViewModel>GetSelectedRootNodes(List<TreeItemViewModel> nodes)
        {
            List<TreeItemViewModel> selected = new List<TreeItemViewModel>();
            foreach (TreeItemViewModel node in nodes)
            {
                if (node.IsSelected)
                    selected.Add(node);
                selected.AddRange(GetSelectedRootNodes(node.Children));
            }
            return selected;
        }

        /// <summary>
        /// Will remove parent from child if Parent is not selected to copy. When copy is executed.
        /// </summary>
        private void DetachParent()
        {
            for (int i = 0; i < m_copiedObjects.Count; i++)
            {
                if (m_copiedObjects[i].transform.parent != null)
                {
                    //if (!m_copiedObjects.Contains(gObj.transform.parent.gameObject))
                    //{
                    //    gObj.transform.parent = null;
                    //}
                    if (m_copiedObjects.Contains(m_copiedObjects[i].transform.parent.gameObject))
                    {
                        m_copiedObjects[i] = null;
                    }
                }
            }
        }
    }
}
