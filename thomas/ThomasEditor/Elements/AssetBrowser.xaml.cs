﻿using System;
using System.Collections.Generic;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.IO;

using ThomasEditor.utils;
using ThomasEditor.Inspectors;
using ThomasEngine;
using System.Threading;

namespace ThomasEditor
{
    /// <summary>
    /// Interaction logic for AssetBrowser.xaml
    /// </summary>
    
    public partial class AssetBrowser : UserControl
    {
        bool wasUnselected = false;
        bool _isDragging = false;
        bool renameNextAddedItem = false;

        public static Dictionary<ThomasEngine.Resources.AssetTypes, BitmapImage> assetImages = new Dictionary<ThomasEngine.Resources.AssetTypes, BitmapImage>();

        FileSystemWatcher watcher;

        public static AssetBrowser instance;

        public AssetBrowser()
        {
            InitializeComponent();
            LoadAssetImages();

            assetBrowserContextMenu.DataContext = false;
            watcher = new FileSystemWatcher();
            watcher.IncludeSubdirectories = true;

            watcher.Renamed += ResourceRenamed;
            watcher.Changed += ResourceChanged;
            watcher.Created += ResourceCreated;
            watcher.Deleted += ResourceDeleted;
            
            instance = this;

            ThomasEngine.Application.currentProjectChanged += Application_currentProjectChanged;
            
        }

        private void Application_currentProjectChanged(Project newProject)
        {
            Dispatcher.BeginInvoke(new Action<String>((String assetPath) => {
                fileTree.Items.Clear();
                ShowAssetsFrom(assetPath);
            }), newProject.assetPath);
        }

        public void ShowAssetsFrom(String path)
        {
            List<object> items = CreateTree(path);
            foreach (object item in items)
            {
                fileTree.Items.Add(item);
            }
            watcher.Path = path;
            watcher.EnableRaisingEvents = true;
        }

        public void UnselectItem()
        {
            if (fileTree.SelectedItem != null)
            {
                wasUnselected = true;
                (fileTree.SelectedItem as TreeViewItem).IsSelected = false;
            }
                
        }

        private void ResourceRenamed(object sender, RenamedEventArgs e)
        {
            Dispatcher.BeginInvoke(new Action<String, String>((String oldPath, String newPath) =>
            {
                ThomasEngine.Resources.AssetTypes oldType = ThomasEngine.Resources.GetResourceAssetType(oldPath);
                ThomasEngine.Resources.AssetTypes newType = ThomasEngine.Resources.GetResourceAssetType(newPath);

                TreeViewItem foundItem = FindNode(oldPath);
                if (foundItem != null)
                {
                    
                    if (oldType != newType)
                        return;

                    ThomasEngine.Resources.RenameResource(oldPath, newPath);

                    StackPanel stack = foundItem.Header as StackPanel;
                    stack.DataContext = newPath;
                    String visibleName;
                    if (Directory.Exists(newPath)) //dir
                        visibleName = new DirectoryInfo(newPath).Name;
                    else
                        visibleName = Path.GetFileNameWithoutExtension(newPath);

                   (stack.Children[1] as EditableTextBlock).Text = visibleName;


                    if (newType == ThomasEngine.Resources.AssetTypes.SCRIPT)
                    {
                        string capitalized = visibleName.Substring(0, 1).ToUpper() + visibleName.Substring(1);
                        ReplaceClassNameOfScript(newPath, capitalized);
                    }
                }
                else
                {
                    if(newType == ThomasEngine.Resources.AssetTypes.SHADER)
                    {
                        Resource shader = ThomasEngine.Resources.Find(newPath);
                        if (shader != null)
                            shader.Reload();
                    }
                }

            }), e.OldFullPath, e.FullPath);
        }

        private void ResourceChanged(object sender, FileSystemEventArgs e)
        {
            watcher.EnableRaisingEvents = false;
            Dispatcher.Invoke(new Action<String>((String p) =>
            {

                TreeViewItem foundItem = FindNode(p);
                if (foundItem != null)
                {
                   if(foundItem.DataContext is Resource)
                    {
                        Resource resource = foundItem.DataContext as Resource;
                        resource.Reload();
                    }
                   if(foundItem.DataContext is GameObject)
                    {
                        GameObject gameObject = foundItem.DataContext as GameObject;
                        GameObject.Destroy(gameObject);
                        foundItem.DataContext = ThomasEngine.Resources.LoadPrefabResource(p);
                    }
                }

            }), e.FullPath);
            watcher.EnableRaisingEvents = true;
        }

        private void ResourceDeleted(object sender, FileSystemEventArgs e)
        {
            Dispatcher.BeginInvoke(new Action<String>((String p) =>
            {

                TreeViewItem foundItem = FindNode(p);
                if(foundItem != null)
                {
                    if (foundItem.Parent is TreeView)
                    {
                        fileTree.Items.Remove(foundItem);
                    }
                    else if (foundItem.Parent is TreeViewItem)
                    {
                        TreeViewItem parent = foundItem.Parent as TreeViewItem;
                        parent.Items.Remove(foundItem);
                    }
                    if(foundItem.DataContext is Resource)
                    {
                        ThomasEngine.Resources.Unload(foundItem.DataContext as Resource);
                    }
                    if (foundItem.DataContext is GameObject)
                    {
                        GameObject gameObject = foundItem.DataContext as GameObject;
                        GameObject.Destroy(gameObject);
                    }
               

                    ThomasEngine.Resources.AssetTypes type = ThomasEngine.Resources.GetResourceAssetType(p);
                    if (type == ThomasEngine.Resources.AssetTypes.SCRIPT)
                    {
                        utils.ScriptAssemblyManager.RemoveScript(p);
                    }
                }
              
            }), e.FullPath);
           
        }

        private List<TreeViewItem> FlattenTree(ItemCollection children)
        {
            List<TreeViewItem> flatTree = new List<TreeViewItem>();
            foreach(TreeViewItem child in children)
            {
                flatTree.Add(child);
                if(child.Items.Count > 0)
                {
                    flatTree.AddRange(FlattenTree(child.Items));
                }
            }
            return flatTree;
        }

        private TreeViewItem FindNode(String nodeName)
        {
            List<TreeViewItem> flat = FlattenTree(fileTree.Items);
            foreach (TreeViewItem item in flat)
            {
                if (((item.Header as StackPanel).DataContext as String) == nodeName)
                {
                    return item;
                }
            }
            return null;
        }

        private void AddNode(String filePath)
        {
            object item = CreateItem(filePath);
            if (item == null)
                return;
            String parentDir = filePath.Remove(filePath.LastIndexOf(Path.DirectorySeparatorChar, filePath.Length-1));

            if(parentDir == watcher.Path)
            {
                    fileTree.Items.Add(item);
            }
            else
            {
                TreeViewItem dir = FindNode(parentDir);
                if (dir != null)
                    dir.Items.Add(item);
              
            }
            
            if(renameNextAddedItem)
            {
                TreeViewItem tvi = item as TreeViewItem;
                tvi.Focus();
                StartRename();
            }
        }

        private void ResourceCreated(object sender, FileSystemEventArgs e)
        {
            watcher.EnableRaisingEvents = false;
            Dispatcher.Invoke(new Action<String>((String p) =>
            {
                
                AddNode(p);
                ThomasEngine.Resources.AssetTypes assetType = ThomasEngine.Resources.GetResourceAssetType(p);
            }), e.FullPath);
            watcher.EnableRaisingEvents = true;
        }


        private void LoadAssetImages()
        {
            assetImages[ThomasEngine.Resources.AssetTypes.UNKNOWN]      = new BitmapImage(new Uri("pack://application:,,/icons/assets/unknown.png"));
            assetImages[ThomasEngine.Resources.AssetTypes.SCENE]        = new BitmapImage(new Uri("pack://application:,,/icons/assets/scene.png"));
            assetImages[ThomasEngine.Resources.AssetTypes.SHADER]       = new BitmapImage(new Uri("pack://application:,,/icons/assets/shader.png"));
            assetImages[ThomasEngine.Resources.AssetTypes.AUDIO_CLIP]   = new BitmapImage(new Uri("pack://application:,,/icons/assets/audio.png"));
            assetImages[ThomasEngine.Resources.AssetTypes.MODEL]        = new BitmapImage(new Uri("pack://application:,,/icons/assets/model.png"));
            assetImages[ThomasEngine.Resources.AssetTypes.ANIMATION]    = new BitmapImage(new Uri("pack://application:,,/icons/assets/model.png"));
            assetImages[ThomasEngine.Resources.AssetTypes.MATERIAL]     = new BitmapImage(new Uri("pack://application:,,/icons/assets/material.png"));
            assetImages[ThomasEngine.Resources.AssetTypes.SCRIPT]       = new BitmapImage(new Uri("pack://application:,,/icons/assets/script.png"));
            assetImages[ThomasEngine.Resources.AssetTypes.PREFAB]       = new BitmapImage(new Uri("pack://application:,,/icons/assets/prefab.png"));
            assetImages[ThomasEngine.Resources.AssetTypes.FONT]         = new BitmapImage(new Uri("pack://application:,,/icons/assets/font.png"));
            assetImages[ThomasEngine.Resources.AssetTypes.TEXTURE3D] = new BitmapImage(new Uri("pack://application:,,/icons/assets/unknown.png"));
        }


        private object CreateItem(string filePath)
        {
            if(Directory.Exists(filePath)) //dir
            {
                String dirName = new DirectoryInfo(filePath).Name;

                StackPanel stack = new StackPanel();
                stack.Orientation = Orientation.Horizontal;
                stack.Height = 18;
                System.Windows.Controls.Image image = new System.Windows.Controls.Image();
                image.Source = new BitmapImage(new Uri("pack://application:,,/icons/assets/dir.png"));
                EditableTextBlock lbl = new EditableTextBlock { Text = dirName };
                stack.Children.Add(image);
                stack.Children.Add(lbl);
                stack.DataContext = filePath;

                TreeViewItem item = new TreeViewItem { Header = stack };
                foreach (object i in CreateTree(filePath))
                    item.Items.Add(i);

                return item;
            }
            else if(File.Exists(filePath))
            {
                String fileName = Path.GetFileNameWithoutExtension(filePath);
                ThomasEngine.Resources.AssetTypes assetType = ThomasEngine.Resources.GetResourceAssetType(filePath);
                if (assetType == ThomasEngine.Resources.AssetTypes.UNKNOWN)
                    return null;
                StackPanel stack = new StackPanel();
                stack.Orientation = Orientation.Horizontal;
                stack.Height = 18;

                // Default case:
                ImageSource source = assetImages[ThomasEngine.Resources.AssetTypes.UNKNOWN];
                // Attempt load texture2D image representation
                if (assetType == ThomasEngine.Resources.AssetTypes.TEXTURE2D)
                {
                    int attempts = 5;
                    while (attempts-- <= 0)
                    {
                        try
                        {
                            source = new BitmapImage(new Uri(Path.GetFullPath(filePath)));
                            break;
                        }
                        catch (IOException e)
                        {
                            // Primarily check is constructed to validate image isn't locked by another process.
                            Debug.LogWarning(e.Message);
                            Thread.Sleep(500);
                        }
                    }
                }
                else
                    source = assetImages[assetType];

                System.Windows.Controls.Image image = new System.Windows.Controls.Image { Source = source };

                EditableTextBlock lbl = new EditableTextBlock { Text = fileName };
                stack.Children.Add(image);
                stack.Children.Add(lbl);
                stack.DataContext = filePath;

                if (assetType == ThomasEngine.Resources.AssetTypes.PREFAB)
                    return new TreeViewItem { Header = stack, DataContext = ThomasEngine.Resources.LoadPrefabResource(filePath) };
                else
                    return new TreeViewItem { Header = stack, DataContext = ThomasEngine.Resources.LoadSysPath(filePath) };
            }
            return null;
        }

        private List<object> CreateTree(String directory)
        {
            List<object> nodes = new List<object>();
            String[] directories = Directory.GetDirectories(directory);
            String[] files = Directory.GetFiles(directory);
            foreach(String dir in directories)
            {
                object item = CreateItem(dir);
                if(item != null)
                    nodes.Add(item);
            }
            foreach(String file in files)
            {

                object item = CreateItem(file);
                if (item != null)
                    nodes.Add(item);
            }
            return nodes;
        }


      
        private void AssetBrowser_MouseMove(object sender, MouseEventArgs e)
        {
            if (!_isDragging && e.LeftButton == MouseButtonState.Pressed && fileTree.SelectedValue != null)
            {
                _isDragging = true;
                DragDrop.DoDragDrop(fileTree, fileTree.SelectedValue,
                    DragDropEffects.Move);
               
            }
            else if (e.LeftButton != MouseButtonState.Pressed)
            {
                _isDragging = false;

            }

        }
        bool hasSelection { get { return true; } }

        private void AssetBrowser_PreviewMouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            
            TreeViewItem treeViewItem = VisualUpwardSearch(e.OriginalSource as DependencyObject);
            if(treeViewItem == null)
            {
                TreeViewItem item = fileTree.SelectedItem as TreeViewItem;
                if (item != null)
                    item.IsSelected = false;
                return;
            }else
            
            //rename on click. Refactor
            if (fileTree.SelectedItem == treeViewItem && e.ClickCount == 1)
            {
                //EditableTextBlock lbl = stack.Children[1] as EditableTextBlock;
                //treeViewItem.IsSelected = false;
                //lbl.IsInEditMode = true;
                //lbl.OnTextChanged += Lbl_OnTextChanged;
            }
        }

        private void Lbl_OnTextChanged(object sender)
        {
            EditableTextBlock lbl = sender as EditableTextBlock;
            lbl.OnTextChanged -= Lbl_OnTextChanged;
            StackPanel stack = lbl.Parent as StackPanel;
            String fullPath = stack.DataContext as String;
            String oldName = Path.GetFileNameWithoutExtension(fullPath);
            String newFullPath = fullPath.Replace(oldName, lbl.Text);

            try
            {
                //Check if file name is not empty string
                if (lbl.Text != "")
                {
                    //Rename if file/dir does not exist
                    if (File.Exists(fullPath) && !File.Exists(newFullPath))
                    {
                        Serializer.WaitForFile(fullPath, 15);
                        File.Move(fullPath, newFullPath);
                    }

                    else if (Directory.Exists(fullPath) && !Directory.Exists(newFullPath))
                        Directory.Move(fullPath, newFullPath);
                    else
                    {
                        lbl.Text = oldName;
                    }
                }
                else
                {
                    lbl.Text = oldName;
                }
            }catch(Exception e)
            {
                Debug.LogError("Failed to rename asset: " + oldName + " error:" + e.Message);
                lbl.Text = oldName;
            }
           
            TreeViewItem item = stack.Parent as TreeViewItem;
            item.Focus();
        }


        private bool CheckSceneFile()
        {
            TreeViewItem item = fileTree.SelectedItem as TreeViewItem;
            StackPanel stack = item.Header as StackPanel;

            String file = stack.DataContext as String;
            ThomasEngine.Resources.AssetTypes assetType = ThomasEngine.Resources.GetResourceAssetType(file);
            if (assetType == ThomasEngine.Resources.AssetTypes.SCENE)
            {
                return true;
            }
            return false;
        }

        private bool CheckPrefabFile()
        {
            TreeViewItem item = fileTree.SelectedItem as TreeViewItem;
            StackPanel stack = item.Header as StackPanel;

            String file = stack.DataContext as String;
            ThomasEngine.Resources.AssetTypes assetType = ThomasEngine.Resources.GetResourceAssetType(file);
            if (assetType == ThomasEngine.Resources.AssetTypes.PREFAB)
            {
                return true;
            }
            return false;
        }   


        private void AssetBrowser_PreviewMouseRightButtonDown(object sender, MouseButtonEventArgs e)
        {
            TreeViewItem treeViewItem = VisualUpwardSearch(e.OriginalSource as DependencyObject);

            if (treeViewItem != null)
            {
                treeViewItem.Focus();
                
                assetBrowserContextMenu.DataContext = true;
                contextMenuOpenItem.IsEnabled = CheckSceneFile();
                contextMenuSaveItem.IsEnabled = CheckPrefabFile();
                //e.Handled = true;
            }
            else
            {
                TreeViewItem item = fileTree.SelectedItem as TreeViewItem;
                
                if(item != null)
                    item.IsSelected = false;
                assetBrowserContextMenu.DataContext = false;
            }
        }

        private TreeViewItem VisualUpwardSearch(DependencyObject source)
        {
            while (source != null && !(source is TreeViewItem))
                source = VisualTreeHelper.GetParent(source);

            return source as TreeViewItem;
        }

        private void AssetBrowser_LeftMouseButtonUp(object sender, MouseButtonEventArgs e)
        {
            if (fileTree.SelectedItem != null)
            {
                if (!wasUnselected)
                    GameObjectHierarchy.instance.Unselect();
                wasUnselected = false;
                TreeViewItem item = fileTree.SelectedItem as TreeViewItem;
                Inspector.instance.SelectedObject = item.DataContext;
            }

        }

        private void AssetBrowser_DoubleClick(object sender, MouseButtonEventArgs e)
        {
            if (fileTree.SelectedItem == null)
                return;
            TreeViewItem item = fileTree.SelectedItem as TreeViewItem;

            OpenItem(item);
        }

        private void StartRename()
        {
            if (fileTree.SelectedItem != null)
            {
                TreeViewItem item = fileTree.SelectedItem as TreeViewItem;
                StackPanel stack = item.Header as StackPanel;
                EditableTextBlock lbl = stack.Children[1] as EditableTextBlock;
                item.IsSelected = false;
                lbl.IsInEditMode = true;
                lbl.OnTextChanged += Lbl_OnTextChanged;
            }
        }

        private void AssetBrowser_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.F2)
            {
                StartRename();
            }
        }

        #region MenuItems

        private void Menu_CreateFolder(object sender, RoutedEventArgs e)
        {

        }

        private void Menu_CreateCSharpScript(object sender, RoutedEventArgs e)
        {
            utils.ScriptAssemblyManager.fsw.EnableRaisingEvents = false;
            string selectedDir = ThomasEngine.Application.currentProject.assetPath;

            String uniquePath = ThomasEngine.Resources.GetUniqueName(selectedDir + "\\newComponent.cs");
            System.IO.File.Copy(ThomasEngine.Application.editorAssets + "\\assemblyFiles\\newComponent.cs", uniquePath);
            utils.ScriptAssemblyManager.AddScript(uniquePath);
            renameNextAddedItem = true;
            utils.ScriptAssemblyManager.fsw.EnableRaisingEvents = true;

            string text = File.ReadAllText(uniquePath);
            text = text.Replace("$itemname$", "NewComponent");
            File.WriteAllText(uniquePath, text);
        }

        private void Menu_CreatePrefab(object sender, RoutedEventArgs e)
        {
            GameObject prefab = GameObject.CreateEmptyPrefab();
            ThomasEngine.Resources.SavePrefab(prefab, prefab.Name + ".prefab");
        }

        private void Menu_CreateMaterial(object sender, RoutedEventArgs e)
        {
            ThomasEngine.Resources.CreateResource(new Material(), "New Material.mat");
            renameNextAddedItem = true;
        }

        private void Menu_OpenAsset(object sender, RoutedEventArgs e)
        {
            if (fileTree.SelectedItem != null)
            {
                TreeViewItem item = fileTree.SelectedItem as TreeViewItem;
                OpenItem(item);
            }
        }

        private void Menu_DeleteAsset(object sender, RoutedEventArgs e)
        {
            if (fileTree.SelectedItem != null)
            {
                try
                {
                    TreeViewItem item = fileTree.SelectedItem as TreeViewItem;
                    StackPanel stack = item.Header as StackPanel;
                    String file = stack.DataContext as String;
                    File.Delete(file);
                }catch(Exception error)
                {
                    Debug.LogWarning("Failed to delete file: " + error.Message);
                }
                
            }
        }

        private void MenuItem_Rename(object sender, RoutedEventArgs e)
        {
            StartRename();
            e.Handled = true;
        }

        private void Menu_OpenCSharpProject(object sender, RoutedEventArgs e)
        {
            if(ThomasEngine.Application.currentProject != null)
                System.Diagnostics.Process.Start(utils.ScriptAssemblyManager.assemblyPath);
        }
        #endregion


        private void OpenItem(TreeViewItem item)
        {
            StackPanel stack = item.Header as StackPanel;

            String file = stack.DataContext as String;
            ThomasEngine.Resources.AssetTypes assetType = ThomasEngine.Resources.GetResourceAssetType(file);
            if (assetType == ThomasEngine.Resources.AssetTypes.SCENE)
            {
                MainWindow._instance.LoadScene(file);
            }
            else if (assetType == ThomasEngine.Resources.AssetTypes.SHADER)
            {
                System.Diagnostics.Process.Start(file);
            }
            
        }

        private void AssetBrowser_SelectedItemChanged(object sender, RoutedPropertyChangedEventArgs<object> e)
        {

            if (fileTree.SelectedItem == null)
            {
                if (!wasUnselected)
                    GameObjectHierarchy.instance.Unselect();
                wasUnselected = false;
                Inspector.instance.SelectedObject = null;
                return;
            }
            if(Mouse.LeftButton == MouseButtonState.Released)
            {
                if (!wasUnselected)
                    GameObjectHierarchy.instance.Unselect();
                wasUnselected = false;

                TreeViewItem item = fileTree.SelectedItem as TreeViewItem;
                Inspector.instance.SelectedObject = item.DataContext;
            }
           
        }

        /*
         * path: The path to the new file.
         * newName: The new className of the new file.
         */
        private void ReplaceClassNameOfScript(string path, string newName)
        {
            string text = File.ReadAllText(path);
            int start = text.IndexOf("public class ") + 13;
            string typeOfScript = GetTypeOfScript(text);
            string oldClassName = text.Substring(start, text.IndexOf(typeOfScript) - start);
            string leftOf = text.Substring(0, start);
            string newClassName = text.Substring(start, text.IndexOf(typeOfScript) - start).Replace(oldClassName, newName);
            string rightOf = text.Substring(text.IndexOf(typeOfScript));
            string newText = leftOf + newClassName + rightOf;
            File.WriteAllText(path, newText);
        }

        /*
         * Will determine if the script is NetworkComponent or ScriptComponent
         */
        private string GetTypeOfScript(string fileText)
        {
            if(fileText.Contains(" : ScriptComponent"))
                return " : ScriptComponent";
            else if(fileText.Contains(" : NetworkComponent"))
                return " : NetworkComponent";
            else
                return "";
        }

        private void Menu_SaveAsset(object sender, RoutedEventArgs e)
        {
            TreeViewItem item = fileTree.SelectedItem as TreeViewItem;

            StackPanel stack = item.Header as StackPanel;
            String file = stack.DataContext as String;

            GameObject prefab = item.DataContext as GameObject;
            watcher.EnableRaisingEvents = false;

            ThomasEngine.Resources.SavePrefab(prefab, file);
            watcher.EnableRaisingEvents = true;
        }
    }


}
