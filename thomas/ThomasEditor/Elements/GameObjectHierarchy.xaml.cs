using System;
using System.IO;

using System.ComponentModel;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;

using System.Windows.Input;
using System.Windows.Media;

using ThomasEditor.Inspectors;
using ThomasEngine;
namespace ThomasEditor
{
    /// <summary>
    /// Interaction logic for GameObjectHierarchy.xaml
    /// </summary>
    public partial class GameObjectHierarchy : UserControl
    {

        bool _isDragging;
        bool wasUnselected = false;
        GameObject m_copiedObject; //??correct code convention?
        public static GameObjectHierarchy instance;
        public GameObjectHierarchy()
        {
            InitializeComponent();

            ThomasWrapper.Selection.Ref.CollectionChanged += SceneSelectedGameObjectChanged;
            ThomasEngine.Transform.OnParentChanged += Transform_OnParentChanged;
            instance = this;
            Scene.OnCurrentSceneChanged += Scene_OnCurrentSceneChanged;
        }

        private void Scene_OnCurrentSceneChanged(Scene newScene)
        {
            if(newScene != null)
            {
                ResetTreeView();
                newScene.GameObjects.CollectionChanged += SceneGameObjectsChanged;
            }
                
        }

        private void ResetTreeView()
        {
            this.Dispatcher.Invoke((Action)(() =>
            {
                hierarchy.Items.Clear();
                foreach (GameObject gObj in Scene.CurrentScene.GameObjects)
                {
                    if (gObj.transform.parent == null)
                    {
                        TreeViewItem node = new TreeViewItem { DataContext = gObj };
                        //node.MouseRightButtonUp += Node_MouseRightButtonUp;
                        node.SetBinding(TreeViewItem.HeaderProperty, new Binding("Name"));
                        node.Padding = new Thickness(0, 0, 0, 2);
                        BuildTree(gObj.transform, node);
                        hierarchy.Items.Add(node);
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
            //    if (oldParent == newParent  || !child.gameObject)
            //        return;

            //    var newParentNode = newParent && newParent.gameObject ? FindNode(hierarchy.Items, newParent.gameObject) as ItemsControl : hierarchy as ItemsControl;

            //    var childNode = FindNode(hierarchy.Items, child.gameObject);

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
            //         childNode.Padding = newPadding;
            //       // childNode.Padding = new Thickness(0, 0, 0, 2);
            //    }

        //}));
        }

        private TreeViewItem FindNode(ItemCollection nodes, GameObject gameObject)
        {
            foreach (TreeViewItem node in nodes)
            {
                if ((node.DataContext as GameObject) == gameObject)
                {
                    return node;
                }
                TreeViewItem child = FindNode(node.Items, gameObject);
                if (child != null) return child;
            }
            return null;
            
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
            if(hierarchy.SelectedItem != null)
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
                            TreeViewItem node = new TreeViewItem { DataContext = newItem };
                            node.IsSelected = true;
                            //node.MouseRightButtonUp += Node_MouseRightButtonUp;
                            node.SetBinding(TreeViewItem.HeaderProperty, new Binding("Name"));
                            //node.Padding = new Thickness(0, 0, 0, 2);

                            BuildTree(newItem.transform, node);
                            hierarchy.Items.Add(node);
                        }
                    }
                }

                if (e.OldItems != null)
                {
                    foreach (GameObject oldItem in e.OldItems)
                    {
                        //oldItem.Destroy();
                        DeleteObjectInTree(hierarchy.Items, oldItem);
                        ////oldItem.Destroy();
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

        private void DeleteObjectInTree(ItemCollection nodes, GameObject item)
        {
            foreach (TreeViewItem node in nodes)
            {
                if (node.DataContext == item)
                {
                    nodes.Remove(node);
                    break;
                }
                DeleteObjectInTree(node.Items, item);
            }
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
            this.Dispatcher.BeginInvoke((Action)(() =>
            {
                if (e.NewItems != null)
                {
                    SelectOrDeselectInTree(hierarchy.Items, e.NewItems, true);
                }
                if (e.OldItems != null)
                {
                    SelectOrDeselectInTree(hierarchy.Items, e.OldItems, false);
                }

                if (e.Action == System.Collections.Specialized.NotifyCollectionChangedAction.Reset)
                {
                    ResetTree(hierarchy.Items);
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
                TreeViewItem item = hierarchy.SelectedItem as TreeViewItem;
                if (item != null)
                {
                    Inspector.instance.SelectedObject = (GameObject)item.DataContext;
                    
                    if (!ThomasWrapper.Selection.Contain((GameObject)item.DataContext))
                        ThomasWrapper.Selection.SelectGameObject((GameObject)item.DataContext);
                    hiearchyContextMenu.DataContext = true;
                }

            }else
            {
                if(Inspector.instance.SelectedObject is GameObject)
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

        private void hierarchy_Drop(object sender, DragEventArgs e)
        {
            if (e.Data.GetDataPresent(typeof(TreeViewItem)))
            {
                TreeViewItem source = (TreeViewItem)e.Data.GetData(typeof(TreeViewItem));
                TreeViewItem target = GetItemAtLocation(e.GetPosition(hierarchy));
                if(source.DataContext is GameObject)
                {
                    if (target != null && source != null && target != source)
                    {
                        GameObject parent = target.DataContext as GameObject;
                        GameObject child = source.DataContext as GameObject;
                        if (!parent.transform.IsChildOf(child.transform))
                        {
                            child.transform.parent = parent.transform;
                        }
                    }
                    else if (source != null && target == null)
                    {
                        GameObject gameObject = source.DataContext as GameObject;
                        if(gameObject.inScene)
                        {
                            gameObject.transform.parent = null;
                        }
                        else
                        {
                            GameObject.Instantiate(gameObject);
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
                if(target != null)
                {
                    _isDragging = true;
                    DragDrop.DoDragDrop(hierarchy, target,
                        DragDropEffects.Move);
                }
                
            }
            else if(e.LeftButton != MouseButtonState.Pressed)
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
            if(item != null)
                item.IsSelected = true;
        }

        private void MenuItem_CopyGameObject(object sender, RoutedEventArgs e)
        {
            Debug.Log("Entered copy function..");

            TreeViewItem item = hierarchy.SelectedItem as TreeViewItem;

            if (item != null)
            {
                Debug.Log("Copying object..");
                m_copiedObject = (GameObject)item.DataContext;

                if(m_copiedObject)
                {
                    Debug.Log("GameObject successfully copied.");
                }
                return;
            }
        }

        private void MenuItem_PasteGameObject(object sender, RoutedEventArgs e)
        {
            if(m_copiedObject)
            {
                GameObject.Instantiate(m_copiedObject);

                Debug.Log("Pasted object.");
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
            TreeViewItem item = hierarchy.SelectedItem as TreeViewItem;

            if (item != null)
            {
                e.CanExecute = true;
                return;
            }
        }

        //Can only paste when an object has been copied
        private void PasteObject_CanExecute(object sender, CanExecuteRoutedEventArgs e)
        {
            if (m_copiedObject)
            {
                e.CanExecute = true;
            }
        }

        public GameObject GetCopy()
        {
            return m_copiedObject;
        }

        public void SetCopy(GameObject copiedObject)
        {
            m_copiedObject = copiedObject;
        }

        public TreeViewItem GetSelection()
        {
            return hierarchy.SelectedItem as TreeViewItem;
        }
    }
}
