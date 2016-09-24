﻿using System;
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
using System.Diagnostics;
using System.Windows.Forms;
using System.Drawing;
using System.IO;
using MahApps.Metro.Controls;
using MahApps.Metro.Controls.Dialogs;


namespace StorageClientWPF
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : MetroWindow, StorageClientController
    {
        private const int UPDATE_INTERVAL = 300 * 1000;

        private string username;
        private string password;
        private string path;

        private StorageClientCore core;
        private Timer timer;

        public MainWindow()
        {
            InitializeComponent();
            this.second_pass_label.Visibility = Visibility.Collapsed;
            this.repeat_pass.Visibility = Visibility.Collapsed;
            this.Register_button.Visibility = Visibility.Collapsed;
            this.back_button.Visibility = Visibility.Collapsed;
            this.folder_label.Visibility = Visibility.Collapsed;
           // this.folder_picker.Visibility = Visibility.Collapsed;
            this.folder_testbox.Visibility = Visibility.Collapsed;
            this.LogOut.Visibility = Visibility.Collapsed;
            this.SoWriteGrid.Visibility = Visibility.Collapsed;
            this.RestoreButton.Visibility = Visibility.Collapsed;
            this.progressRing.IsActive = false;
        }

        private void Button_Click_1(object sender, RoutedEventArgs e)
        {
            //Click on Register            
            this.user.Visibility = Visibility.Visible;
            this.user_label.Visibility = Visibility.Visible;
            this.pass.Visibility = Visibility.Visible;
            this.pass_label.Visibility = Visibility.Visible;
            this.repeat_pass.Visibility = Visibility.Visible;
            this.second_pass_label.Visibility = Visibility.Visible;
            this.Register_button.Visibility = Visibility.Visible;
        }


        private void LoginButton_Click(object sender, RoutedEventArgs e)
        {
            //ha fatto login,
            //eliminare tutto ciò che è visualizzato e stampare la grid
            this.username = this.user.Text;
            this.password = this.pass.Password;

            if (this.core == null)
                this.core = new StorageClientCore(this);

            if (!this.core.issueRequest(new LoginRequest(this.username, this.password)))
                this.onLoginError("Login temporary not available! Try later.");
            else
                this.progressRing.IsActive = true;
        }

        private void relaunchUpdate()
        {
            //if (timer == null)
            //{
            //    timer = new Timer();
            //    timer.Tick += new EventHandler(startUpdate);
            //    timer.Interval = UPDATE_INTERVAL;
            //    timer.Start();
            //}

            if (this.timer == null)
            {
                this.timer = new Timer();
                this.timer.Tick += new EventHandler(startUpdate);
                this.timer.Interval = UPDATE_INTERVAL;
            }
            this.timer.Start();
        }

        private void startUpdate(object sender, EventArgs e)
        {
            if (this.timer != null)
            {
                this.timer.Stop();
                //timer.Dispose();
                //timer = null;
            }

            if (this.core == null)
                this.core = new StorageClientCore(this);

            if (!this.core.issueRequest(new UpdateRequest(this.username, this.password, this.path)))
                this.onUpdateError("Update temporary not available! Try later.");
            else
                this.progressRing.IsActive = true;
        }

        public void onGetVersionsError(string aMsg)
        {
            if (this.Dispatcher.CheckAccess())
            {
                //update UI safely
                this.status_label.Content = aMsg;
            }
            else
            {
                Dispatcher.BeginInvoke((Action)(() => onGetVersionsError(aMsg)));
            }
        }

        public void onGetVersionsSuccess(List<UserVersion> aVersionsList)
        {
            if (this.Dispatcher.CheckAccess())
            {
                //update UI safely
                this.DrawVersionBottons(aVersionsList);
            }
            else
            {
                Dispatcher.BeginInvoke((Action)(() => onGetVersionsSuccess(aVersionsList)));
            }
        }

        public void onLoginError(string aMsg)
        {
            if (this.Dispatcher.CheckAccess())
            {
                //update UI safely
                this.status_label.Content = aMsg;
                this.showErrorMessage("Invalid Credentials", aMsg);
            }
            else
            {
                Dispatcher.BeginInvoke((Action)(() => onLoginError(aMsg)));
            }
        }

        //per mostrare un errore all' utente
        private async Task showErrorMessage(string title,string message)
        {
            await this.ShowMessageAsync(title,message);
        }

        public void onLoginSuccess(string aPath)
        {
            if (this.Dispatcher.CheckAccess())
            {
                //update UI safely
                this.path = aPath;                
                this.startUpdate(this, null);

                this.progressRing.IsActive = false;
                //aggiornamento grafica
                this.user_label.Visibility = Visibility.Collapsed;
                this.user.Visibility = Visibility.Collapsed;
                this.pass_label.Visibility = Visibility.Collapsed;
                this.pass.Visibility = Visibility.Collapsed;
                this.LoginButton.Visibility = Visibility.Collapsed;
                this.label_reg.Visibility = Visibility.Collapsed;
                this.second_pass_label.Visibility = Visibility.Collapsed;
                this.repeat_pass.Visibility = Visibility.Collapsed;
                this.Register_button.Visibility = Visibility.Collapsed;
                this.back_button.Visibility = Visibility.Collapsed;
                this.folder_label.Visibility = Visibility.Collapsed;
                this.folder_testbox.Visibility = Visibility.Collapsed;
                this.LogOut.Visibility = Visibility.Visible;
                this.RestoreButton.Visibility = Visibility.Visible;

                this.status_label.Content = "Utente Loggato";
                this.SoWriteGrid.Visibility = Visibility.Visible;
            }
            else
            {
                Dispatcher.BeginInvoke((Action)(() => onLoginSuccess(aPath)));
            }
        }

        public void onRegistrationError(string aMsg)
        {
            if (this.Dispatcher.CheckAccess())
            {
                //update UI safely
                this.showErrorMessage("Registration Error", aMsg);
            }
            else
            {
                Dispatcher.BeginInvoke((Action)(() => onRegistrationError(aMsg)));
            }
        }

        public void onRegistrationSucces()
        {
            if (this.Dispatcher.CheckAccess())
            {
                //update UI safely
                this.status_label.Content = "Registered";
                this.startUpdate(this, null);

                //aggiorno la grafica
                this.user.Visibility = Visibility.Collapsed;
                this.user_label.Visibility = Visibility.Collapsed;
                this.pass.Visibility = Visibility.Collapsed;
                this.pass_label.Visibility = Visibility.Collapsed;
                this.repeat_pass.Visibility = Visibility.Collapsed;
                this.second_pass_label.Visibility = Visibility.Collapsed;
                this.Register_button.Visibility = Visibility.Collapsed;
                this.label_reg.Visibility = Visibility.Collapsed;
                this.back_button.Visibility = Visibility.Collapsed;
                this.LoginButton.Visibility = Visibility.Collapsed;
                this.label_reg.Visibility = Visibility.Collapsed;
                this.folder_label.Visibility = Visibility.Collapsed;
                this.folder_testbox.Visibility = Visibility.Collapsed;
                this.LoginButton.Visibility = Visibility.Collapsed;
                this.label_reg.Visibility = Visibility.Collapsed;
                this.LogOut.Visibility = Visibility.Visible;
                this.RestoreButton.Visibility = Visibility.Visible;

                this.SoWriteGrid.Visibility = Visibility.Visible;
                this.status_label.Content = "Utente registrato";

            }
            else
            {
                Dispatcher.BeginInvoke((Action)(() => onRegistrationSucces()));
            }
        }

        public void onRestoreError(string aMsg)
        {
            if (this.Dispatcher.CheckAccess())
            {
                //update UI safely

            }
            else
            {
                Dispatcher.BeginInvoke((Action)(() => onRestoreError(aMsg)));
            }
        }

        public void onRestoreSuccess(int aVersion, string aVersionDate)
        {
            if (this.Dispatcher.CheckAccess())
            {
                //update UI safely

            }
            else
            {
                Dispatcher.BeginInvoke((Action)(() => onRestoreSuccess(aVersion, aVersionDate)));
            }
        }

        public void onUpdateError(string aMsg)
        {
            if (this.Dispatcher.CheckAccess())
            {
                //update UI safely
                this.status_label.Content = aMsg;
                this.relaunchUpdate();
                this.progressRing.IsActive = false;
            }
            else
            {
                Dispatcher.BeginInvoke((Action)(() => onUpdateError(aMsg)));
            }
        }

        public void onUpdateStart()
        {
            if (this.Dispatcher.CheckAccess())
            {
                //update UI safely
                this.status_label.Content = "Update started";
                this.progressRing.IsActive = true;
            }
            else
            {
                Dispatcher.BeginInvoke((Action)(() => onUpdateStart()));
            }
        }

        public void onUpdateSuccess(List<UserFile> aFileList, int aVersion, string aVersionDate)
        {
            if (this.Dispatcher.CheckAccess())
            {
                //update UI safely
                this.status_label.Content = "Update finished: vesion " + aVersion + " [" + aVersionDate + "]";
                this.relaunchUpdate();
                this.progressRing.IsActive = false;

                this.DrawFileBottons(aFileList);
            }
            else
            {
                Dispatcher.BeginInvoke((Action)(() => onUpdateSuccess(aFileList, aVersion, aVersionDate)));
            }
        }

        private void Window_Closed(object sender, EventArgs e)
        {   
            //do clean-up operations
            if(this.core != null)
                this.core.Dispose();

            if(this.timer != null)
                this.timer.Dispose();
        }

        private void cl_MouseUp(object sender, MouseButtonEventArgs e)
        {
            //selezionata la register
            this.user.Visibility = Visibility.Visible;
            this.user_label.Visibility = Visibility.Visible;
            this.pass.Visibility = Visibility.Visible;
            this.pass_label.Visibility = Visibility.Visible;
            this.repeat_pass.Visibility = Visibility.Visible;
            this.second_pass_label.Visibility = Visibility.Visible;
            this.Register_button.Visibility = Visibility.Visible;
            this.label_reg.Visibility = Visibility.Collapsed;
            this.back_button.Visibility = Visibility.Visible;
            this.LoginButton.Visibility = Visibility.Visible;
            this.label_reg.Visibility = Visibility.Visible;
            this.folder_label.Visibility = Visibility.Visible;
        //    this.folder_picker.Visibility = Visibility.Visible;
            this.folder_testbox.Visibility = Visibility.Visible;
            this.LoginButton.Visibility = Visibility.Collapsed;
            this.label_reg.Visibility = Visibility.Collapsed;
            this.status_label.Content = " ";

        }

        private void cl_MouseEnter(object sender, System.Windows.Input.MouseEventArgs e)
        {
            this.label_reg.Foreground = new SolidColorBrush(Colors.Violet);
        }

        private void cl_MouseLeave(object sender, System.Windows.Input.MouseEventArgs e)
        {
            this.label_reg.Foreground = new SolidColorBrush(Colors.Black);
        }

        private void back_button_Click(object sender, RoutedEventArgs e)
        {
            this.user.Visibility = Visibility.Visible;
            this.user_label.Visibility = Visibility.Visible;
            this.pass.Visibility = Visibility.Visible;
            this.pass_label.Visibility = Visibility.Visible;
            this.repeat_pass.Visibility = Visibility.Collapsed;
            this.second_pass_label.Visibility = Visibility.Collapsed;
            this.Register_button.Visibility = Visibility.Collapsed;
            this.folder_label.Visibility = Visibility.Collapsed;
        //    this.folder_picker.Visibility = Visibility.Collapsed;
            this.folder_testbox.Visibility = Visibility.Collapsed;
            this.LoginButton.Visibility = Visibility.Visible;
            this.label_reg.Visibility = Visibility.Visible;
            this.back_button.Visibility = Visibility.Collapsed;

            this.status_label.Content = "";
        }


        private void LogOut_Click(object sender, RoutedEventArgs e)
        {
            this.user_label.Text = "Username ";
            
            this.user_label.Visibility = Visibility.Visible;
            this.user.Visibility = Visibility.Visible;
            this.pass_label.Visibility = Visibility.Visible;
            this.pass.Visibility = Visibility.Visible;
            this.LoginButton.Visibility = Visibility.Visible;
            this.label_reg.Visibility = Visibility.Visible;
            this.LogOut.Visibility = Visibility.Collapsed;
            this.status_label.Content = "Utente LogOut";
            this.RestoreButton.Visibility = Visibility.Collapsed;

            WriteGrid.RowDefinitions.Clear();
            WriteGrid.Children.Clear();
            

            this.SoWriteGrid.Visibility = Visibility.Collapsed;
        }

        private void Register_button_Click(object sender, RoutedEventArgs e)
        {
            //controllo che ci sia una cartella
            string path = this.folder_testbox.Text;

            // get the file attributes for file or directory
            try
            {
                //cerca di prendere gli attributi del path
                FileAttributes attr = File.GetAttributes(path);

                //detect whether its a directory or file
                if ((attr & FileAttributes.Directory) == FileAttributes.Directory)
                {
                    //è una directory valida
                    this.username = this.user.Text;
                    this.password = this.pass.Password;
                    this.path = this.folder_testbox.Text;

                    if (core == null)
                        core = new StorageClientCore(this);

                    if (!this.core.issueRequest(new RegistrRequest(username, password, path)))
                        this.onRegistrationError("Registration temporary not available! Try later.");
                }
                else
                {
                    this.showErrorMessage(" Error", "Insert a valid folder");
                }
            }
            catch (Exception ex)
            {
                this.showErrorMessage(" Error", "Insert a valid folder");
            }

        }

        private void DrawFileBottons(List<UserFile> aList)
        {
            //prima pulisci tutto
            WriteGrid.RowDefinitions.Clear();
            WriteGrid.Children.Clear();

            this.user_label.Text = "Files in folder: ";
            this.user_label.Visibility = Visibility.Visible;

            foreach (UserFile s in aList)
            {

                RowDefinition row = new RowDefinition();
                row.Height = new GridLength(30);
                //grid_files_versions.RowDefinitions.Add(row);
                WriteGrid.RowDefinitions.Add(row);
                int i = WriteGrid.RowDefinitions.Count;
                System.Windows.Controls.Label lb = new System.Windows.Controls.Label();
                lb.ToolTip = s.getFilePath();
                lb.Content = s.getFileName();
                lb.MouseUp += lb_MouseUp;
                lb.MouseEnter += lb_MouseEnter;
                lb.MouseLeave += lb_MouseLeave;
                //System.Windows.Controls.Button lb = new System.Windows.Controls.Button();

                StackPanel sp = new StackPanel();
                sp.Children.Clear();
                sp.SetValue(Grid.RowProperty, i - 1);

                sp.Children.Add(lb);

                //grid_files_versions.Children.Add(sp);
                WriteGrid.Children.Add(sp);
            }
     
        }

        private void DrawVersionBottons(List<UserVersion> aVersionsList)
        {
            //prima pulisci tutto
            WriteGrid.RowDefinitions.Clear();
            WriteGrid.Children.Clear();

            this.user_label.Text = "Available versions ";
            this.user_label.Visibility = Visibility.Visible;
            //ogni entry indica il numero di versione e la data

            foreach (UserVersion v in aVersionsList)
            {
                RowDefinition row = new RowDefinition();
                row.Height = new GridLength(30);
                //grid_files_versions.RowDefinitions.Add(row);
                WriteGrid.RowDefinitions.Add(row);
                int i = WriteGrid.RowDefinitions.Count;
                System.Windows.Controls.Label lb = new System.Windows.Controls.Label();
                lb.Content = v.getVersionID() + ": " + v.getVersionDate();
                lb.MouseUp += lb_MouseUpVersion;
                lb.MouseEnter += lb_MouseEnter;
                lb.MouseLeave += lb_MouseLeave;


                StackPanel sp = new StackPanel();
                sp.Children.Clear();
                sp.SetValue(Grid.RowProperty, i - 1);

                sp.Children.Add(lb);

                ////grid_files_versions.Children.Add(sp);
                WriteGrid.Children.Add(sp);
            }

        }

        void lb_MouseLeave(object sender, System.Windows.Input.MouseEventArgs e)
        {
            System.Windows.Controls.Label lb = (System.Windows.Controls.Label)sender;
            lb.Background = new SolidColorBrush(Colors.White);
        }

        void lb_MouseEnter(object sender, System.Windows.Input.MouseEventArgs e)
        {
            System.Windows.Controls.Label lb = (System.Windows.Controls.Label)sender;
            lb.Background = new SolidColorBrush(Colors.Beige);
        }

        void lb_MouseUp(object sender, MouseButtonEventArgs e)
        {
            //evento per gestire l' apertura del file
            System.Windows.Controls.Label lb = (System.Windows.Controls.Label)sender;

            String path = (String)lb.ToolTip;
            try
            {
                System.Diagnostics.Process.Start(path);
            }
            catch (Exception ex)
            {
                this.showErrorMessage("Error", ex.Message);
            }
        }

        void lb_MouseUpVersion(object sender, MouseButtonEventArgs e)
        {

            System.Windows.Controls.Label lb = (System.Windows.Controls.Label)sender;

            String Version = (String)lb.Content;
            //aprire un folder picker, per scegliere dove ripristinare 
            FolderBrowserDialog folderBrowserDialog1 = new System.Windows.Forms.FolderBrowserDialog();
            // Set the help text description for the FolderBrowserDialog.
            folderBrowserDialog1.Description =
                "Select the directory in which you want to restore the version";

            // Do not allow the user to create new files via the FolderBrowserDialog.
            folderBrowserDialog1.ShowNewFolderButton = false;
            folderBrowserDialog1.ShowDialog();
            //if (folderBrowserDialog1.ShowDialog() == DialogResult.HasValue)
            //{
            string path = folderBrowserDialog1.SelectedPath;
            Debug.WriteLine("restore path: "+path);
            this.status_label.Content = "Trying to restore version"+ Version + "in path:" + path;
        }

        private void RestoreButton_Click(object sender, RoutedEventArgs e)
        {
            if (core == null)
                core = new StorageClientCore(this);

            if (!this.core.issueRequest(new GetVerRequest(this.username, this.password)))
                this.onRestoreError("Restore temporary not available! Try later.");
        }

        private void folder_testbox_MouseUp(object sender, MouseButtonEventArgs e)
        {
            //se clicco nella text box mi apre il folder picker
            FolderBrowserDialog folderBrowserDialog1 = new System.Windows.Forms.FolderBrowserDialog();
            // Set the help text description for the FolderBrowserDialog.
            folderBrowserDialog1.Description =
                "Select the directory that you want to use as the default.";

            // Do not allow the user to create new files via the FolderBrowserDialog.
            folderBrowserDialog1.ShowNewFolderButton = false;
            folderBrowserDialog1.ShowDialog();
            //if (folderBrowserDialog1.ShowDialog() == DialogResult.HasValue)
            //{
            string path = folderBrowserDialog1.SelectedPath;
            Debug.WriteLine(path);
            this.folder_testbox.Text = path;
        }

        private void ProgressBar_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {

        }

    }
}
