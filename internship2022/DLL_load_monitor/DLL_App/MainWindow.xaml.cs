using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Controls.Primitives;
using Microsoft.UI.Xaml.Data;
using Microsoft.UI.Xaml.Input;
using Microsoft.UI.Xaml.Media;
using Microsoft.UI.Xaml.Navigation;
using Microsoft.UI.Xaml.Documents;
using System;
using System.Windows;
using System.Threading.Tasks;
using System.Threading;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using System.Text.Json;
using System.Text.Json.Serialization;
using Newtonsoft.Json;
using System.Drawing;
using System.Data;
using System.Windows.Media;
using System.Windows.Controls;
using System.Globalization;


// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace DLL_App
{
    /// <summary>
    /// An empty window that can be used on its own or navigated to within a Frame.
    /// </summary>
    /// 
    public sealed partial class MainWindow : System.Windows.Window
    {
        public const string CppFunctionsDLL = "C:\\Users\\anicu\\git\\bdinternship\\internship2022\\DLL_load_monitor\\Debug\\DLL_load_monitor.dll";

        [DllImport(CppFunctionsDLL, CallingConvention = CallingConvention.Cdecl)]
        public static extern int SolveProcesses();

        System.Windows.Controls.Grid RootGrid = new System.Windows.Controls.Grid();

        System.Windows.Controls.ScrollViewer scrollViewer = new System.Windows.Controls.ScrollViewer();

        public string[] InitHeader()
        {
            // Process_Name + PID + CPU + Physical + Virtual + Modules
            System.Windows.Controls.ColumnDefinition gridCol1 = new System.Windows.Controls.ColumnDefinition();
            System.Windows.Controls.ColumnDefinition gridCol2 = new System.Windows.Controls.ColumnDefinition();
            System.Windows.Controls.ColumnDefinition gridCol3 = new System.Windows.Controls.ColumnDefinition();
            System.Windows.Controls.ColumnDefinition gridCol4 = new System.Windows.Controls.ColumnDefinition();
            System.Windows.Controls.ColumnDefinition gridCol5 = new System.Windows.Controls.ColumnDefinition();
            System.Windows.Controls.ColumnDefinition gridCol6 = new System.Windows.Controls.ColumnDefinition();
            RootGrid.ColumnDefinitions.Add(gridCol1);
            RootGrid.ColumnDefinitions.Add(gridCol2);
            RootGrid.ColumnDefinitions.Add(gridCol3);
            RootGrid.ColumnDefinitions.Add(gridCol4);
            RootGrid.ColumnDefinitions.Add(gridCol5);
            RootGrid.ColumnDefinitions.Add(gridCol6);

            RootGrid.HorizontalAlignment = System.Windows.HorizontalAlignment.Left;
            RootGrid.VerticalAlignment = System.Windows.VerticalAlignment.Top;

            RootGrid.ShowGridLines = true;

            RootGrid.Background = new System.Windows.Media.SolidColorBrush(Colors.Navy);

            System.Windows.Controls.RowDefinition gridRow1 = new System.Windows.Controls.RowDefinition();
            gridRow1.Height = new System.Windows.GridLength(25);
            RootGrid.RowDefinitions.Add(gridRow1);

            System.Windows.Controls.TextBlock textBlock = new System.Windows.Controls.TextBlock();
            textBlock.Text = "Process Name";
            textBlock.FontSize = 14;
            textBlock.FontWeight = FontWeights.Bold;
            textBlock.Foreground = new System.Windows.Media.SolidColorBrush(Colors.Green);
            textBlock.VerticalAlignment = System.Windows.VerticalAlignment.Top;
            System.Windows.Controls.Grid.SetRow(textBlock, 0);
            System.Windows.Controls.Grid.SetColumn(textBlock, 0);
            RootGrid.Children.Add(textBlock);

            System.Windows.Controls.TextBlock textBlock1 = new System.Windows.Controls.TextBlock();
            textBlock1.Text = "PID";
            textBlock1.FontSize = 14;
            textBlock1.FontWeight = FontWeights.Bold;
            textBlock1.Foreground = new System.Windows.Media.SolidColorBrush(Colors.Green);
            textBlock1.VerticalAlignment = System.Windows.VerticalAlignment.Top;
            System.Windows.Controls.Grid.SetRow(textBlock1, 0);
            System.Windows.Controls.Grid.SetColumn(textBlock1, 1);
            RootGrid.Children.Add(textBlock1);

            System.Windows.Controls.TextBlock textBlock2 = new System.Windows.Controls.TextBlock();
            textBlock2.Text = "CPU";
            textBlock2.FontSize = 14;
            textBlock2.FontWeight = FontWeights.Bold;
            textBlock2.Foreground = new System.Windows.Media.SolidColorBrush(Colors.Green);
            textBlock2.VerticalAlignment = System.Windows.VerticalAlignment.Top;
            System.Windows.Controls.Grid.SetRow(textBlock2, 0);
            System.Windows.Controls.Grid.SetColumn(textBlock2, 2);
            RootGrid.Children.Add(textBlock2);

            System.Windows.Controls.TextBlock textBlock3 = new System.Windows.Controls.TextBlock();
            textBlock3.Text = "Physical Memory";
            textBlock3.FontSize = 14;
            textBlock3.FontWeight = FontWeights.Bold;
            textBlock3.Foreground = new System.Windows.Media.SolidColorBrush(Colors.Green);
            textBlock3.VerticalAlignment = System.Windows.VerticalAlignment.Top;
            System.Windows.Controls.Grid.SetRow(textBlock3, 0);
            System.Windows.Controls.Grid.SetColumn(textBlock3, 3);
            RootGrid.Children.Add(textBlock3);

            System.Windows.Controls.TextBlock textBlock4 = new System.Windows.Controls.TextBlock();
            textBlock4.Text = "Virtual Memory";
            textBlock4.FontSize = 14;
            textBlock4.FontWeight = FontWeights.Bold;
            textBlock4.Foreground = new System.Windows.Media.SolidColorBrush(Colors.Green);
            textBlock4.VerticalAlignment = System.Windows.VerticalAlignment.Top;
            System.Windows.Controls.Grid.SetRow(textBlock4, 0);
            System.Windows.Controls.Grid.SetColumn(textBlock4, 4);
            RootGrid.Children.Add(textBlock4);

            System.Windows.Controls.TextBlock textBlock5 = new System.Windows.Controls.TextBlock();
            textBlock5.Text = "Modules";
            textBlock5.FontSize = 14;
            textBlock5.FontWeight = FontWeights.Bold;
            textBlock5.Foreground = new System.Windows.Media.SolidColorBrush(Colors.Green);
            textBlock5.VerticalAlignment = System.Windows.VerticalAlignment.Top;
            System.Windows.Controls.Grid.SetRow(textBlock5, 0);
            System.Windows.Controls.Grid.SetColumn(textBlock5, 5);
            RootGrid.Children.Add(textBlock5);

            SolveProcesses();
            string json = File.ReadAllText("C:\\Users\\anicu\\git\\bdinternship\\internship2022\\DLL_load_monitor\\DLL_load_monitor\\load_monitor.json");
            List<Process> process = JsonConvert.DeserializeObject<List<Process>>(json);

            string[] Modules = new string[process.Count];

            for ( int i = 0; i < process.Count; i++ ) 
            {
                System.Windows.Controls.RowDefinition gridRow = new System.Windows.Controls.RowDefinition();
                RootGrid.RowDefinitions.Add(gridRow);

                System.Windows.Controls.TextBlock module = new System.Windows.Controls.TextBlock();
                for (int j = 0; j < process[i].Modules.Count; j++)
                {
                    module.Text += process[i].Modules[j].ModuleName;
                    module.Text += "\n";
                }
                module.FontSize = 14;
                module.FontWeight = FontWeights.Bold;
                module.Foreground = new System.Windows.Media.SolidColorBrush(Colors.Green);
                module.VerticalAlignment = System.Windows.VerticalAlignment.Top;
                System.Windows.Controls.Grid.SetRow(module, i + 1);
                System.Windows.Controls.Grid.SetColumn(module, 5);
                RootGrid.Children.Add(module);
                Modules[i] = module.Text;
            }
            return Modules;
        }

        public void InitProcesses(string[] Modules)
        {
            SolveProcesses();
            string json = File.ReadAllText("C:\\Users\\anicu\\git\\bdinternship\\internship2022\\DLL_load_monitor\\DLL_load_monitor\\load_monitor.json");
            List<Process> process = JsonConvert.DeserializeObject<List<Process>>(json);

            for (int i = 0; i < process.Count; i++)
            {
                System.Windows.Controls.TextBlock process_name = new System.Windows.Controls.TextBlock();
                process_name.Text = process[i].ProcessName;
                process_name.FontSize = 14;
                process_name.FontWeight = FontWeights.Bold;
                process_name.Foreground = new System.Windows.Media.SolidColorBrush(Colors.Green);
                process_name.VerticalAlignment = System.Windows.VerticalAlignment.Top;
                System.Windows.Controls.Grid.SetRow(process_name, i + 1);
                System.Windows.Controls.Grid.SetColumn(process_name, 0);
                RootGrid.Children.Add(process_name);

                System.Windows.Controls.TextBlock PID = new System.Windows.Controls.TextBlock();
                PID.Text = process[i].PID.ToString();
                PID.FontSize = 14;
                PID.FontWeight = FontWeights.Bold;
                PID.Foreground = new System.Windows.Media.SolidColorBrush(Colors.Green);
                PID.VerticalAlignment = System.Windows.VerticalAlignment.Top;
                System.Windows.Controls.Grid.SetRow(PID, i + 1);
                System.Windows.Controls.Grid.SetColumn(PID, 1);
                RootGrid.Children.Add(PID);

                NumberFormatInfo setPrecision = new NumberFormatInfo();
                setPrecision.NumberDecimalDigits = 12;
                System.Windows.Controls.TextBlock CPU = new System.Windows.Controls.TextBlock();
                CPU.Text = process[i].CPU.ToString("N", setPrecision) + '%';
                CPU.FontSize = 14;
                CPU.FontWeight = FontWeights.Bold;
                CPU.Foreground = new System.Windows.Media.SolidColorBrush(Colors.Green);
                CPU.VerticalAlignment = System.Windows.VerticalAlignment.Top;
                System.Windows.Controls.Grid.SetRow(CPU, i + 1);
                System.Windows.Controls.Grid.SetColumn(CPU, 2);
                RootGrid.Children.Add(CPU);

                System.Windows.Controls.TextBlock physical = new System.Windows.Controls.TextBlock();
                physical.Text = ((double)process[i].physMemUsed / 1024 / 1024).ToString() + " MB";
                physical.FontSize = 14;
                physical.FontWeight = FontWeights.Bold;
                physical.Foreground = new System.Windows.Media.SolidColorBrush(Colors.Green);
                physical.VerticalAlignment = System.Windows.VerticalAlignment.Top;
                System.Windows.Controls.Grid.SetRow(physical, i + 1);
                System.Windows.Controls.Grid.SetColumn(physical, 3);
                RootGrid.Children.Add(physical);

                System.Windows.Controls.TextBlock virutal = new System.Windows.Controls.TextBlock();
                virutal.Text = ((double)process[i].virtualMemUsed / 1024 / 1024).ToString() + " MB";
                virutal.FontSize = 14;
                virutal.FontWeight = FontWeights.Bold;
                virutal.Foreground = new System.Windows.Media.SolidColorBrush(Colors.Green);
                virutal.VerticalAlignment = System.Windows.VerticalAlignment.Top;
                System.Windows.Controls.Grid.SetRow(virutal, i + 1);
                System.Windows.Controls.Grid.SetColumn(virutal, 4);
                RootGrid.Children.Add(virutal);

                System.Windows.Controls.TextBlock module = new System.Windows.Controls.TextBlock();
                module.Text = Modules[i];
                module.FontSize = 14;
                module.FontWeight = FontWeights.Bold;
                module.Foreground = new System.Windows.Media.SolidColorBrush(Colors.Green);
                module.VerticalAlignment = System.Windows.VerticalAlignment.Top;
                System.Windows.Controls.Grid.SetRow(module, i + 1);
                System.Windows.Controls.Grid.SetColumn(module, 5);
                RootGrid.Children.Add(module);

                scrollViewer.VerticalScrollBarVisibility = System.Windows.Controls.ScrollBarVisibility.Auto;
                scrollViewer.HorizontalScrollBarVisibility = System.Windows.Controls.ScrollBarVisibility.Auto;
                scrollViewer.Content = RootGrid;
                this.Content = scrollViewer;
            }
        }

        public async void UpdateProcesses()
        {
            string[] Modules = InitHeader();
            scrollViewer.VerticalScrollBarVisibility = System.Windows.Controls.ScrollBarVisibility.Auto;
            scrollViewer.HorizontalScrollBarVisibility = System.Windows.Controls.ScrollBarVisibility.Auto;
            scrollViewer.Content = RootGrid;
            this.Content = scrollViewer;
            while (true)
            {
                InitProcesses(Modules);
                await Task.Delay(4500); // thread separat pentru wait
                RootGrid.Children.Clear();
            }
        }

        public MainWindow()
        {
            this.InitializeComponent();
            this.Show();
            UpdateProcesses();
        }

        public class Module
        {
            public string ModuleName { get; set; }
        }

        public class Process
        {
            public double CPU { get; set; }
            public List<Module> Modules { get; set; }
            public int PID { get; set; }
            public string ProcessName { get; set; }
            public long physMemUsed { get; set; }
            public long virtualMemUsed { get; set; }
        }
    }
}
