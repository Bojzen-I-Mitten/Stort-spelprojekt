using System;
using System.Threading;

using System.Windows;
using System.Windows.Controls;

using Xceed.Wpf.Toolkit.PropertyGrid;

using ThomasEngine;

namespace ThomasEditor
{
    namespace Inspectors
    {
        /// <summary>
        /// Interaction logic for ExtendedPropertyGrid.xaml
        /// </summary>
        public partial class ExtendedPropertyGrid : UserControl
        {
            bool isMaterialEditor = false;
            public ExtendedPropertyGrid()
            {
//#if DEBUG
//                System.Diagnostics.PresentationTraceSources.DataBindingSource.Switch.Level =
//                    System.Diagnostics.SourceLevels.Critical;
//#endif
                InitializeComponent();
                _grid.PreparePropertyItem += _grid_PreparePropertyItem;
                _grid.EditorDefinitions.Add(
                    new EditorTemplateDefinition {
                    EditingTemplate = Resources["ListEditor"] as DataTemplate,
                    TargetProperties = {
                            new TargetPropertyType { Type = typeof(ThomasEngine.GameObject[]) },
                            new TargetPropertyType { Type = typeof(System.Collections.Generic.List<ThomasEngine.GameObject>)},
                            new TargetPropertyType { Type = typeof(System.Collections.Generic.List<ThomasEngine.Transform>)},
                             new TargetPropertyType { Type = typeof(Resource[]) },
                            new TargetPropertyType { Type = typeof(System.Collections.Generic.List<Resource>)},
                    }
                    });
            }

            private void _grid_PreparePropertyItem(object sender, PropertyItemEventArgs e)
            {
            }

            private void PropertyGrid_Loaded(object sender, RoutedEventArgs e)
            {
                PropertyGrid grid = sender as PropertyGrid;
                if(grid.DataContext != null && grid.DataContext.GetType() == typeof(DictionaryPropertyGridAdapter) && !isMaterialEditor)
                {
                    EditorTemplateDefinition tex = new EditorTemplateDefinition();
                    tex.EditingTemplate = FindResource("TextureEditor") as DataTemplate;
                    tex.TargetProperties.Add(new TargetPropertyType() { Type = typeof(Texture2D) });
                    grid.EditorDefinitions.Add(tex);
                    grid.EditorDefinitions.Insert(0, tex);
                    isMaterialEditor = true;
                }
                if (Parent is MaterialInspector)
                {
                    MaterialInspector matEditor = Parent as MaterialInspector;
                    grid.IsReadOnly = (matEditor.DataContext as Material) == Material.StandardMaterial;
                }
                grid.CollapseAllProperties();
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
                    else if(item.DataContext is ThomasEngine.Object)
                    {
                        ThomasEngine.Object obj = item.DataContext as ThomasEngine.Object;
                        ContentControl label = sender as ContentControl;
                        PropertyItem pi = label.DataContext as PropertyItem;
                        if (obj.GetType() == pi.PropertyType)
                        {
                            //Monitor.Enter(ThomasWrapper.CurrentScene.GetGameObjectsLock());
                            pi.Value = obj;

                            //Monitor.Exit(ThomasWrapper.CurrentScene.GetGameObjectsLock());
                        }else if(obj is GameObject && (obj as GameObject).inScene && typeof(Component).IsAssignableFrom(pi.PropertyType))
                        {
                            var method = typeof(GameObject).GetMethod("GetComponent").MakeGenericMethod(pi.PropertyType);
                            var component = method.Invoke(obj, null);
                            if(component != null && component.GetType() == pi.PropertyType)
                            {
                                pi.Value = component;
                            }
                        }
                    }
                }
            }

            public void SetReadOnly(bool readOnly)
            {
                _grid.IsReadOnly = readOnly;
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

            private void PropertyGrid_DataContextChanged(object sender, DependencyPropertyChangedEventArgs e)
            {
                PropertyGrid grid = sender as PropertyGrid;
                if (grid.DataContext != null && grid.DataContext.GetType() == typeof(DictionaryPropertyGridAdapter) && !isMaterialEditor)
                {
                    EditorTemplateDefinition tex = new EditorTemplateDefinition();
                    tex.EditingTemplate = FindResource("TextureEditor") as DataTemplate;
                    tex.TargetProperties.Add(new TargetPropertyType() { Type = typeof(Texture2D) });
                    grid.EditorDefinitions.Add(tex);
                    grid.EditorDefinitions.Insert(0, tex);
                    isMaterialEditor = true;
                }
                if (Parent is MaterialInspector)
                {
                    MaterialInspector matEditor = Parent as MaterialInspector;
                    grid.IsReadOnly = (matEditor.DataContext as Material) == Material.StandardMaterial;
                }
                grid.CollapseAllProperties();
            }
        }
    }
}
