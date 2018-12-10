using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.ComponentModel;    // for BackgroundWorker
using EventLogger;
using Microsoft.Win32;
using System.Windows.Forms;
using System.IO;
using System.Diagnostics;

namespace DbgPrintView
{
    /// <summary>
    /// Interaction logic for MainPage.xaml
    /// </summary>
    public partial class MainPage : Page
    {
        CEventLogger logger;
        EventCallback _EventCallback;
        public static MyDataContext MyContext;

        public MainPage()
        {
            InitializeComponent();

            // Create Event Callback object.
            _EventCallback = new EventCallback();

            // Register the Event handler.
            _EventCallback.EventLoggerCallback += OnLogArrived;

            // Add key-shortcut.
            MyContext = new MyDataContext();
        }

        private void Page_Loaded(object sender, RoutedEventArgs e)
        {
            if (false == CheckIfDbgPrintInstalled())
            {
                return;
            }

            logger = new CEventLogger("DbgPrintRt");
            // Register the Event Callback object to the CBiometricsRuntime.
            logger.SetEventCallback(_EventCallback);

            BackgroundWorker loggerLoop = new BackgroundWorker();
            loggerLoop.DoWork += (s, e1) =>
            {
                uint result = logger.Loop();
                if (result != 0)
                {
                    System.Windows.MessageBox.Show(String.Format("Error in opening DbgPrintRT session ({0:X})", result));
                }
            };

            loggerLoop.RunWorkerAsync();
        }

        public void OnLogArrived(object sender, EventLoggerArgs args)
        {
            string logstr = logger.GetLogString();

            Dispatcher.BeginInvoke(new Action(() =>
            {
                textBox.Text += logstr;
                textBox.ScrollToEnd();
            }));

        }


        bool CheckIfDbgPrintInstalled()
        {
            bool installed = false;
            RegistryKey hklm_retaillog;

            hklm_retaillog = Registry.LocalMachine; //HKLM Registry 
            hklm_retaillog = Registry.LocalMachine.OpenSubKey(@"System\CurrentControlSet\Control\WMI\AutoLogger\DbgPrintRT");

            if (hklm_retaillog == null) /* If RetailLog AutoLogger is not there */
            {
                installed = false;
            }
            else
            {
                installed = true;
            }

            if (installed != true)
            {
                if (System.Windows.Forms.MessageBox.Show("DbgPrintRT.reg is not installed yet. Do you want to install DbgPrintRT.reg? The reboot is required to take effect.", "Install DbgPrintRT.reg", MessageBoxButtons.YesNo)
    == DialogResult.Yes)
                {
                    if (File.Exists(System.Windows.Forms.Application.StartupPath + @"\DbgPrintRT.reg"))
                    {
                        // Import RetailLog.reg in current directory.
                        ProcessStartInfo startInfo = new ProcessStartInfo("reg.exe");
                        startInfo.CreateNoWindow = false;
                        startInfo.UseShellExecute = true;
                        startInfo.WindowStyle = ProcessWindowStyle.Hidden;
                        startInfo.Arguments = "import \"" + System.Windows.Forms.Application.StartupPath + "\\DbgPrintRT.reg\"";
                        // Import RetailLog.reg in current directory.
                        using (Process exeProcess = Process.Start(startInfo))
                        {
                            exeProcess.WaitForExit();
                        }

                        ConfirmRebootPC();
                    }
                    else
                    {
                        System.Windows.MessageBox.Show("DbgPrintRT.reg is missing.");
                    }
                }
            }

            if (hklm_retaillog != null) hklm_retaillog.Close();

            return installed;
        }

        private void ConfirmRebootPC()
        {
            if (System.Windows.Forms.MessageBox.Show("Do you want to reboot PC to take effect?", "Reboot PC", MessageBoxButtons.YesNo) == DialogResult.Yes)
            {
                Process.Start("shutdown.exe", "-r -t 0");
            }
        }

        private void ClearLog_Click(object sender, RoutedEventArgs e)
        {
            textBox.Text = "";
        }

        private void Clear_Click(object sender, RoutedEventArgs e)
        {
            textBox.Text = "";
        }

    }
}
