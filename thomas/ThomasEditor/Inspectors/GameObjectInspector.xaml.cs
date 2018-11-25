
using System;

using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;

using System.Globalization;

using System.Windows.Input;

using Xceed.Wpf.Toolkit.PropertyGrid;

using System.Collections.Generic;

using ThomasEngine;
using ThomasEditor.utils;

namespace ThomasEditor
{
    namespace Converters
    {
        public class ComponentToNameConverter : IValueConverter
        {
            public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
            {
                return value != null ? value.GetType().Name : "NULL";
            }

            public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
            {
                throw new NotImplementedException();
            }
        }
        public class ComponentIndexConverter : IValueConverter
        {
            public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
            {
                if(value != null && value is Component)
                {
                    Component c = value as Component;
                    return c.gameObject.GetComponentIndex(c).ToString();
                }
                return "Nan";
            }

            public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
            {
                throw new NotImplementedException();
            }
        }
    }

    namespace Inspectors
    {

        
        /// <summary>
        /// Interaction logic for GameObjectInspector.xaml
        /// </summary>
        /// 
        
        public partial class GameObjectInspector : UserControl
        {
           
            GameObject prevGameObject;
            //For selecting the firt element in the components list
           // int selectedComponent = 0;

            //public Collection<EditorDefinitionBase> customEditors;
            public GameObjectInspector()
            {

                InitializeComponent();
                DataContextChanged += GameObjectInspector_DataContextChanged;
                Unloaded += GameObjectInspector_Unloaded;
                // propertyGrid.Editors.Add(editor);

            }

            private void GameObjectInspector_DataContextChanged(object sender, DependencyPropertyChangedEventArgs e)
            {

                LayerBox.ItemsSource = Physics.GetCollisionLayerNames();

                if (DataContext is GameObject)
                {
                    gameObjectGrid.Visibility = Visibility.Visible;
                    GameObject SelectedGameObject = DataContext as GameObject;
                    SelectedGameObject.Subscribe(Components_CollectionChanged);
                    prevGameObject = SelectedGameObject;
                    RenderComponent rc = SelectedGameObject.GetComponent<RenderComponent>();
                    if (rc && rc.material != null)
                    {
                        Binding materialBinding = new Binding("material");
                        materialBinding.Source = rc;
                        MaterialEditor.SetBinding(MaterialInspector.DataContextProperty, materialBinding);
                    }else
                    {
                        BindingOperations.ClearBinding(MaterialEditor, MaterialInspector.DataContextProperty);
                        MaterialEditor.DataContext = null;
                    }
                }
                else
                    gameObjectGrid.Visibility = Visibility.Hidden;



            }

            private void GameObjectInspector_Unloaded(object sender, RoutedEventArgs e)
            {
                if(prevGameObject != null)
                {
                    prevGameObject.UnSubscribe(Components_CollectionChanged);
                }
            }

            private void Components_CollectionChanged(object sender, GameObject.ComponentsChangedArgs e)
            {
                
                this.Dispatcher.BeginInvoke((Action)(() => {
                    componentItemList.Items.Refresh();// Refresh component list
                    if ((DataContext is GameObject) == false)
                    {
                        BindingOperations.ClearBinding(MaterialEditor, MaterialInspector.DataContextProperty);
                        MaterialEditor.DataContext = null;
                        return;
                    }
                    GameObject SelectedGameObject = DataContext as GameObject;
                    RenderComponent rc = SelectedGameObject.GetComponent<RenderComponent>();
                    if (rc != null)
                    {
                        Binding materialBinding = new Binding("material");
                        materialBinding.Source = rc;
                        MaterialEditor.SetBinding(MaterialInspector.DataContextProperty, materialBinding);
                    }
                    else
                    {
                        BindingOperations.ClearBinding(MaterialEditor, MaterialInspector.DataContextProperty);
                        MaterialEditor.DataContext = null;
                    }
                }));
                
              
                    
            }


            private bool ComponentsFilter(object item)
            {
                if (String.IsNullOrEmpty(AddComponentsFilter.Text))
                    return true;
                else
                    return ((item as Type).Name.IndexOf(AddComponentsFilter.Text, StringComparison.OrdinalIgnoreCase) >= 0);
            }

            //private void AddComponentButton_Click(object sender, RoutedEventArgs e)
            //{
            //    addComponentList.SelectedItem = null;
            //    addComponentsListPopup.IsOpen = true;
            //    AddComponentsFilter.Focus();
            //    addComponentList.ItemsSource = Component.GetAllAddableComponentTypes();
            //    CollectionViewSource.GetDefaultView(addComponentList.ItemsSource).Filter = ComponentsFilter;
            //}

            private void GameObjectInspector_PreviewDragOver(object sender, DragEventArgs e)
            {
                //Debug.Log("Entered preview function");
                if (e.Data.GetDataPresent(typeof(TreeViewItem)))
                {
                    //e.Effects = DragDropEffects.None;
                    TreeViewItem item = e.Data.GetData(typeof(TreeViewItem)) as TreeViewItem;
                    StackPanel sourceHeader = item.Header as StackPanel;

                    if (ThomasEngine.Resources.GetResourceAssetType((string)sourceHeader.DataContext) == ThomasEngine.Resources.AssetTypes.SCRIPT)
                    {
                        e.Effects = DragDropEffects.Move;
                        e.Handled = true;
                    }
                    else
                        e.Effects = DragDropEffects.None;
                }

            }
            private void GameObjectInspector_Drop(object sender, DragEventArgs e)
            {
                GameObjectHierarchy.instance.SetInspector(true);
                GameObjectHierarchy.instance.hierarchy_Drop(sender, e);
            }

            private void AddComponent_CanExecute(object sender, CanExecuteRoutedEventArgs e)
            {
                e.CanExecute = true;
            }

            private void AddComponent(object sender, ExecutedRoutedEventArgs e)
            {
                addComponentList.SelectedItem = null;
                addComponentsListPopup.IsOpen = true;
                AddComponentsFilter.Focus();
                addComponentList.ItemsSource = Component.GetAllAddableComponentTypes();
                CollectionViewSource.GetDefaultView(addComponentList.ItemsSource).Filter = ComponentsFilter;
                //selectedComponent = 0;
                //addComponentList.SelectedIndex = selectedComponent;
            }

            private void AddComponentsFilter_TextChanged(object sender, TextChangedEventArgs e)
            {

                CollectionViewSource.GetDefaultView(addComponentList.ItemsSource).Refresh();
                //selectedComponent = 0;
                //addComponentList.SelectedItem = addComponentList.Items[selectedComponent];
                //addComponentList.SelectedIndex = selectedComponent;
            }



            private void AddComponentList_SelectionChanged(object sender, SelectionChangedEventArgs e)
            {
                if (addComponentList.SelectedItem != null && Mouse.LeftButton == MouseButtonState.Pressed && addComponentList.IsMouseOver)
                {
                    if(DataContext is GameObject)
                    { 
                        Type component = addComponentList.SelectedItem as Type;
                        ThomasWrapper.IssueCommand(new AddComponentCommand((GameObject)DataContext, component));
                    }
                }
            }

            private void RemoveComponentButton_Click(object sender, RoutedEventArgs e)
            {
                Button button = (Button)sender;
                Component component = (Component)button.DataContext;
                if (component.GetType() != typeof(Transform))
                    GameObject.Destroy(component);
            }

            //Add so that element 0 is selected from the start.
            private void AddComponentList_KeyUp(object sender, KeyEventArgs e)
            {
                //var list = addComponentList.Items;
                //switch (e.Key)
                //{
                //    case Key.Down:
                //        if (selectedComponent >= list.Count - 1) selectedComponent = 0;
                //        else selectedComponent++;
                //        addComponentList.SelectedIndex = selectedComponent;
                //        break;

                //    case Key.Up:
                //        if (selectedComponent == 0) selectedComponent = list.Count - 1;
                //        else selectedComponent--;
                //        addComponentList.SelectedIndex = selectedComponent;
                //        break;
                //    case Key.Enter:
                //        Type component = addComponentList.SelectedItem as Type;
                //        var method = typeof(GameObject).GetMethod("AddComponent").MakeGenericMethod(component);
                //        method.Invoke(SelectedGameObject, null);
                //        break;
                //}
            }

            private void AddComponentsList_MLBUp(object sender, MouseButtonEventArgs e)
            {
                var n = sender.GetType().GetCustomAttributesData();
                if (addComponentList.SelectedItem != null)
                {
                    lock (DataContext)
                    {
                        Type component = addComponentList.SelectedItem as Type;
                        var method = typeof(GameObject).GetMethod("AddComponent").MakeGenericMethod(component);
                        method.Invoke(DataContext, null);
                        addComponentsListPopup.IsOpen = false;
                    }
                }
            }

            private void AddComponentsListContainer_LostFocus(object sender, RoutedEventArgs e)
            {
                addComponentsListPopup.IsOpen = false;
            }

            private void ComponentEnabled_Checked(object sender, RoutedEventArgs e)
            {
                FrameworkElement fE = e.OriginalSource as FrameworkElement;
                if (fE != null && fE.DataContext is Component)
                    ThomasWrapper.IssueCommand(new utils.EnableComponentCommand((Component)fE.DataContext, true));
            }

            private void ComponentEnabled_Unchecked(object sender, RoutedEventArgs e)
            {
                FrameworkElement fE = e.OriginalSource as FrameworkElement;
                if(fE != null && fE.DataContext is Component)
                    ThomasWrapper.IssueCommand(new utils.EnableComponentCommand((Component)fE.DataContext, false));
            }

            private void ComponentIndex_Changed(object sender, RoutedEventArgs e)
            {
                FrameworkElement fE = e.OriginalSource as FrameworkElement;
                if (fE != null && fE.DataContext is Component && !(fE.DataContext is Transform))
                {
                    Component c = (Component)fE.DataContext;
                    String text = (sender as TextBox).Text;
                    int ind;
                    if (Int32.TryParse(text, out ind))
                    {
                        uint index = (uint)ind;
                        if (ind < 1)    // Not 0, Reserved for transform
                            index = 1;
                        ThomasWrapper.IssueCommand(new ComponentSetIndexCommand(c, index));
                    }
                }
            }
        }
    }
}
