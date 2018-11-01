
using System;
using System.Data;
using System.Globalization;
using System.Linq;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using ThomasEngine;
namespace ThomasEditor
{


    /// <summary>
    /// Interaction logic for OptionsWindow.xaml
    /// </summary>
    public partial class OptionsWindow : Window
    {
        public OptionsWindow()
        {
            InitializeComponent();

            GenerateData();

        }

        private void GenerateData(bool refreshList = true)
        {
            var myDataTable = new DataTable();
            var colums = Physics.GetCollisionLayerNames().Reverse();
            
            if(refreshList)
                collisionLayers.ItemsSource = Physics.GetCollisionLayerNames();

            var rows = Physics.GetCollisionLayerNames();
            var vals = Physics.GetCollisionLayerValues();

            DataColumn column = new DataColumn();
            column.DataType = typeof(string);
            column.ColumnName = "---";
            myDataTable.Columns.Add(column);

            foreach (var value in colums)
            {
                column = new DataColumn();
                column.DataType = typeof(bool);
                column.ColumnName = value;

                myDataTable.Columns.Add(column);
            }
            int index = 0;

            foreach (string value in rows)
            {
                object[] row = new object[myDataTable.Columns.Count];
                row[0] = value;

                for (int i = 0; i < vals[index].Length; i++)
                {
                    row[1 + i] = null;
                }

                for (int i = 0; i < vals[index].Length-index; i++)
                {
                    row[1 + i] = vals[index][(vals[index].Length-1)-i];
                }
                myDataTable.Rows.Add(row);
                index++;
            }
            collisionMatrix.ItemsSource = myDataTable.DefaultView;
        }

        private void TextBox_TextChanged(object sender, System.Windows.Controls.TextChangedEventArgs e)
        {
            TextBox text = sender as TextBox;
            int index = (int)text.Tag;
            Physics.SetCollisionLayer(text.Text, index, Physics.GetCollisionMask(index));
            GenerateData(false);
        }


        private void AddLayer_Button_Click(object sender, RoutedEventArgs e)
        {
            int count = Physics.GetCollisionLayerCount();
            Physics.SetCollisionLayer("Collision Layer " + count, count, ~0);
            GenerateData();
            ThomasEngine.Application.currentProject.Save();
        }

        private void RemoveLayer_Button_Click(object sender, RoutedEventArgs e)
        {
            int count = Physics.GetCollisionLayerCount();
            Physics.SetCollisionLayer("None", count-1, ~0);
            GenerateData();
            ThomasEngine.Application.currentProject.Save();
        }

        private void SaveMatrix_Button_Click(object sender, RoutedEventArgs e)
        {
            int count = Physics.GetCollisionLayerCount();
            for (int i = 0; i < count; i++)
            {
                DataGridRow gridRow = (DataGridRow)collisionMatrix.ItemContainerGenerator.ContainerFromIndex(i);
                object[] row = ((DataRowView)gridRow.Item).Row.ItemArray;
                for (int j = 0; j < count - i; j++)
                {
                    bool collide = (bool)row[j + 1];
                    Physics.SetGroupCollisionFlag(i, (count - 1) - j, collide);
                    Physics.SetGroupCollisionFlag((count - 1) - j, i, collide);
                }
            }
            ThomasEngine.Application.currentProject.Save();
        }
    }
}
