﻿/*
 * Authors: Angelo Prudentino & Daniele Testa
 * Date: 14/09/2016
 * File: MainWindow.xaml.cs
 * Description: Client application user interface
 *
 */
 
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
        private const int PING_INTERVAL = 60 * 1000;
        private const String UPDATE_FILE_DESC = "Synchronized files:";
        private const String RESTORE_FILE_DESC = "Restored files:";

        private string username;
        private string password;
        private string path;
        private string token;
        private List<UserFile> lastValidFileList;
        private List<UserVersion> lastValidVersionList;

        private StorageClientCore core;
        private Timer updateTimer;
        private Timer pingTimer;

        public MainWindow()
        {
            InitializeComponent();
            this.second_pass_label.Visibility = Visibility.Collapsed;
            this.repeat_pass.Visibility = Visibility.Collapsed;
            this.Register_button.Visibility = Visibility.Collapsed;
            this.back_button.Visibility = Visibility.Collapsed;
            this.folder_label.Visibility = Visibility.Collapsed;
            this.folder_testbox.Visibility = Visibility.Collapsed;
            this.LogOut.Visibility = Visibility.Collapsed;
            this.SoWriteGrid.Visibility = Visibility.Collapsed;
            this.RestoreButton.Visibility = Visibility.Collapsed;
            this.RestoreBackButton.Visibility = Visibility.Collapsed;
            this.folderPicker.Visibility = Visibility.Collapsed;
            this.SynchNow_Button.Visibility = Visibility.Collapsed;
            this.progressRing.IsActive = false;
        }

        public void onGetVersionsError(string aMsg)
        {
            if (this.Dispatcher.CheckAccess())
            {
                //update UI safely
                this.status_label.Foreground = new SolidColorBrush(Colors.Red);
                this.status_label.Content = aMsg;
                this.status_label.ToolTip = this.status_label.Content;
                this.showMainBtnBar(false);
                this.progressRing.IsActive = false;
                this.scroll.Visibility = Visibility.Collapsed;
                if (this.lastValidFileList != null)
                    this.DrawFileBottons(UPDATE_FILE_DESC, this.lastValidFileList);
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
                //stop update timer during restore
                this.stopUpdate();

                //update UI safely
                this.clearStatusLabel();
                this.lastValidVersionList = aVersionsList;
                this.DrawVersionBottons(aVersionsList);
                this.showMainBtnBar(true);
                this.progressRing.IsActive = false;
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
                this.showErrorMessage("Unexpected Error", aMsg);
                this.progressRing.IsActive = false;
                this.LoginButton.IsEnabled = true;
                this.label_reg.IsEnabled = true;
                this.label_reg.Foreground = new SolidColorBrush(Colors.Black);
            }
            else
            {
                Dispatcher.BeginInvoke((Action)(() => onLoginError(aMsg)));
            }
        }

        public void onLoginSuccess(string aPath)
        {
            if (this.Dispatcher.CheckAccess())
            {
                //update UI safely
                this.progressRing.IsActive = false;
                this.LoginButton.IsEnabled = true;
                this.label_reg.IsEnabled = true;
                this.label_reg.Foreground = new SolidColorBrush(Colors.Black);
                this.scroll.Visibility = Visibility.Collapsed;
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
                this.SoWriteGrid.Visibility = Visibility.Visible;
                this.showMainBtnBar(false);

                this.path = aPath;
                this.startUpdate(this, null);
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
                this.showErrorMessage("Unexpected Error", aMsg);
                this.progressRing.IsActive = false;
                this.Register_button.IsEnabled = true;
                this.back_button.IsEnabled = true;
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
                this.progressRing.IsActive = false;
                this.Register_button.IsEnabled = true;
                this.back_button.IsEnabled = true;
                this.scroll.Visibility = Visibility.Collapsed;
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
                this.folder_label.Visibility = Visibility.Collapsed;
                this.folder_testbox.Visibility = Visibility.Collapsed;
                this.LoginButton.Visibility = Visibility.Collapsed;
                this.folderPicker.Visibility = Visibility.Collapsed;
                this.SoWriteGrid.Visibility = Visibility.Visible;
                this.showMainBtnBar(false);

                this.startUpdate(this, null);
            }
            else
            {
                Dispatcher.BeginInvoke((Action)(() => onRegistrationSucces()));
            }
        }

        public void onRestoreStart(string aToken)
        {
            if (this.Dispatcher.CheckAccess())
            {
                //update UI safely
                this.progressRing.IsActive = true;
                this.hideMainBtnBar();
                this.LogOut.IsEnabled = false;
                this.clearRows();

                this.startPingTimer(aToken);
            }
            else
            {
                Dispatcher.BeginInvoke((Action)(() => onRestoreStart(aToken)));
            }
        }

        public void onRestoreError(string aMsg)
        {
            if (this.Dispatcher.CheckAccess())
            {
                //update UI safely
                this.status_label.Foreground = new SolidColorBrush(Colors.Red);
                this.status_label.Content = aMsg;
                this.status_label.ToolTip = this.status_label.Content;
                this.progressRing.IsActive = false;
                this.showMainBtnBar(true);

                if (this.lastValidVersionList != null)
                    this.DrawVersionBottons(this.lastValidVersionList);
                else
                    this.clearRows();

                this.stopPingTimer();
            }
            else
            {
                Dispatcher.BeginInvoke((Action)(() => onRestoreError(aMsg)));
            }
        }

        public void onRestoreSuccess(List<UserFile> aFileList, int aVersion, string aVersionDate)
        {
            if (this.Dispatcher.CheckAccess())
            {
                //update UI safely

                this.status_label.Foreground = new SolidColorBrush(Colors.Black);
                this.status_label.Content = "Restore of version " + aVersion + " [" + aVersionDate + "] completed";
                this.status_label.ToolTip = this.status_label.Content;
                this.progressRing.IsActive = false;
                this.showMainBtnBar(true);
                this.DrawFileBottons(RESTORE_FILE_DESC, aFileList);

                this.stopPingTimer();
            }
            else
            {
                Dispatcher.BeginInvoke((Action)(() => onRestoreSuccess(aFileList, aVersion, aVersionDate)));
            }
        }

        public void onUpdateError(string aMsg)
        {
            if (this.Dispatcher.CheckAccess())
            {
                //update UI safely
                this.status_label.Foreground = new SolidColorBrush(Colors.Red);
                this.status_label.Content = aMsg;
                this.status_label.ToolTip = this.status_label.Content;
                this.SynchNow_Button.IsEnabled = true;
                this.relaunchUpdate();
                this.progressRing.IsActive = false;
                this.stopPingTimer();
                this.showMainBtnBar(false);

                if (this.lastValidFileList != null)
                    this.DrawFileBottons(UPDATE_FILE_DESC, this.lastValidFileList);
                else
                    this.clearRows();
            }
            else
            {
                Dispatcher.BeginInvoke((Action)(() => onUpdateError(aMsg)));
            }
        }

        public void onUpdateStart(string aToken)
        {
            if (this.Dispatcher.CheckAccess())
            {
                //update UI safely
                this.progressRing.IsActive = true;
                this.scroll.Visibility = Visibility.Collapsed;
                this.hideMainBtnBar();
                this.clearRows();
                this.startPingTimer(aToken);
            }
            else
            {
                Dispatcher.BeginInvoke((Action)(() => onUpdateStart(aToken)));
            }
        }

        public void onUpdateSuccess(List<UserFile> aFileList, int aVersion, string aVersionDate)
        {
            if (this.Dispatcher.CheckAccess())
            {
                //update UI safely
                this.status_label.Foreground = new SolidColorBrush(Colors.Black);
                if (aVersion == 0)
                    this.status_label.Content = "Nothing changed since last synchronization";
                else if(aVersion < 0)
                    this.status_label.Content = "Synchronization finished: restored vesion " + (-aVersion) + " with date " + aVersionDate;
                else
                    this.status_label.Content = "Synchronization finished: stored vesion " + aVersion + " at " + aVersionDate;

                this.status_label.ToolTip = this.status_label.Content;
                this.relaunchUpdate();
                this.progressRing.IsActive = false;
                this.showMainBtnBar(false);
                
                this.stopPingTimer();
                this.lastValidFileList = aFileList;
                this.DrawFileBottons(UPDATE_FILE_DESC, aFileList);
            }
            else
            {
                Dispatcher.BeginInvoke((Action)(() => onUpdateSuccess(aFileList, aVersion, aVersionDate)));
            }
        }

        private void LoginButton_Click(object sender, RoutedEventArgs e)
        {
            this.username = this.user.Text;
            this.password = this.pass.Password;

            if (this.core == null)
                this.core = new StorageClientCore(this);

            if (!this.core.issueRequest(new LoginRequest(this.username, this.password)))
                this.onLoginError("Login temporarly not available! Try later.");
            else
            {
                this.progressRing.IsActive = true;
                this.LoginButton.IsEnabled = false;
                this.label_reg.IsEnabled = false;
                this.label_reg.Foreground = new SolidColorBrush(Colors.LightGray);
            }
        }

        private void relaunchUpdate()
        {
            if (this.updateTimer == null)
            {
                this.updateTimer = new Timer();
                this.updateTimer.Tick += new EventHandler(startUpdate);
                this.updateTimer.Interval = UPDATE_INTERVAL;
            }
            this.updateTimer.Start();
        }

        private void startUpdate(object sender, EventArgs e)
        {
            if (this.core == null)
                this.core = new StorageClientCore(this);

            if (!this.core.issueRequest(new UpdateRequest(this.username, this.password, this.path)))
                this.onUpdateError("Update temporarly not available! Try later.");
            else
            {
                this.status_label.Foreground = new SolidColorBrush(Colors.Black);
                this.status_label.Content = "Synchronizing with server";
                this.status_label.ToolTip = this.status_label.Content;
                this.stopUpdate();
                this.progressRing.IsActive = true;
                this.hideMainBtnBar();
            }
        }

        private void stopUpdate()
        {
            if (this.updateTimer != null)
                this.updateTimer.Stop();
        }

        private void startPingTimer(string aToken)
        {
            this.token = aToken;

            if (this.pingTimer == null)
            {
                this.pingTimer = new Timer();
                this.pingTimer.Tick += new EventHandler(pingSession);
                this.pingTimer.Interval = PING_INTERVAL;
            }
            this.pingTimer.Start();
        }

        private void stopPingTimer()
        {
            if (this.pingTimer != null)
                this.pingTimer.Stop();
        }

        private void pingSession(object sender, EventArgs e)
        {
            if (this.core != null)
                this.core.issueRequest(new PingRequest(this.token));
        }

        private void Window_Closed(object sender, EventArgs e)
        {
            //do clean-up operations
            if (this.core != null)
                this.core.Dispose();

            if (this.updateTimer != null)
                this.updateTimer.Dispose();

            if (this.pingTimer != null)
                this.pingTimer.Dispose();
        }

        //gestri eventi per la label "register" vicino al Login
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
            this.back_button.Visibility = Visibility.Visible;
            this.LoginButton.Visibility = Visibility.Visible;
            this.folder_label.Visibility = Visibility.Visible;
            this.folderPicker.Visibility = Visibility.Visible; 
            this.folder_testbox.Visibility = Visibility.Visible;
            this.LoginButton.Visibility = Visibility.Collapsed;
            this.label_reg.Visibility = Visibility.Collapsed;
            this.clearStatusLabel(); 

            //sistemare le textbox, svuotarle e settare a false il tasto cancella
            this.user.Text = "";
            this.user.SetValue(TextBoxHelper.ClearTextButtonProperty, false);
            this.pass.Password = "";
            this.pass.SetValue(TextBoxHelper.ClearTextButtonProperty, false);
            this.repeat_pass.Password = "";
            this.repeat_pass.SetValue(TextBoxHelper.ClearTextButtonProperty, false);
            this.folder_testbox.Text = "";
        }

        //si evidenzia la label quando ci finisci sopra
        private void cl_MouseEnter(object sender, System.Windows.Input.MouseEventArgs e)
        {
            this.label_reg.Foreground = this.progressRing.Foreground;
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
            this.folderPicker.Visibility = Visibility.Collapsed;
            this.folder_testbox.Visibility = Visibility.Collapsed;
            this.LoginButton.Visibility = Visibility.Visible;
            this.label_reg.Visibility = Visibility.Visible;
            this.back_button.Visibility = Visibility.Collapsed;
            this.LoginButton.IsEnabled = true;
            this.label_reg.IsEnabled = true;
            this.label_reg.Foreground = new SolidColorBrush(Colors.Black);

            //sistemare le textbox, svuotarle e settare a false il tasto cancella
            this.user.Text = "";
            this.user.SetValue(TextBoxHelper.ClearTextButtonProperty, false);
            this.pass.Password = "";
            this.pass.SetValue(TextBoxHelper.ClearTextButtonProperty, false);
            this.repeat_pass.Password = "";
            this.repeat_pass.SetValue(TextBoxHelper.ClearTextButtonProperty, false);

            this.clearStatusLabel();
        }

        private void LogOut_Click(object sender, RoutedEventArgs e)
        {
            this.user_label.FontWeight = FontWeights.Normal; 
            this.user_label.Text = "Username";
            this.user_label.Visibility = Visibility.Visible;
            this.user.Visibility = Visibility.Visible;
            this.pass_label.Visibility = Visibility.Visible;
            this.pass.Visibility = Visibility.Visible;
            this.LoginButton.Visibility = Visibility.Visible;
            this.label_reg.Visibility = Visibility.Visible;
            WriteGrid.RowDefinitions.Clear();
            WriteGrid.Children.Clear();
            this.SoWriteGrid.Visibility = Visibility.Collapsed;
            this.SynchNow_Button.Visibility = Visibility.Collapsed;
            this.clearStatusLabel();
            this.hideMainBtnBar();
            this.clearStatusLabel(); 

            this.stopUpdate();

            if (this.core != null)
                this.core.issueRequest(new LogoutRequest());

            core.Dispose();
            core = null;
        }

        private void Register_button_Click(object sender, RoutedEventArgs e)
        {
            string path = this.folder_testbox.Text; this.username = this.user.Text;
            this.password = this.pass.Password;
            string second_password = this.repeat_pass.Password;
            this.path = this.folder_testbox.Text;

            if (this.password.CompareTo(second_password) != 0)
            {
                this.showErrorMessage("Invalid Credentials", "Password and Repeat Password must be the same");
                return;
            }

            // get the file attributes for file or directory
            try
            {
                //cerca di prendere gli attributi del path
                FileAttributes attr = File.GetAttributes(path);

                //detect whether its a directory or file
                if ((attr & FileAttributes.Directory) == FileAttributes.Directory)
                {
                    if (core == null)
                        core = new StorageClientCore(this);

                    if (!this.core.issueRequest(new RegistrRequest(username, password, path)))
                        this.onRegistrationError("Registration temporary not available! Try later.");
                    else
                    {
                        this.progressRing.IsActive = true;
                        this.Register_button.IsEnabled = false;
                        this.back_button.IsEnabled = false;
                    }
                }
                else
                {
                    this.showErrorMessage("Unexpected Error", "Please select a valid directory");
                }
            }
            catch (Exception ex)
            {
                this.showErrorMessage("Unexpected Error", "Please select a valid directory");
            }

        }

        private void RestoreButton_Click(object sender, RoutedEventArgs e)
        {
            if (this.core == null)
                this.core = new StorageClientCore(this);

            if (!this.core.issueRequest(new GetVerRequest(this.username, this.password)))
                this.onRestoreError("Restore temporary not available! Try later.");
            else
            {
                this.status_label.Foreground = new SolidColorBrush(Colors.Black);
                this.status_label.Content = "Downloading version's list from server";
                this.status_label.ToolTip = this.status_label.Content;
                this.progressRing.IsActive = true;
                this.hideMainBtnBar();
                this.clearRows();
            }
        }

        private void clearRows(){
            WriteGrid.RowDefinitions.Clear();
            WriteGrid.Children.Clear();
            this.scroll.Visibility = Visibility.Collapsed;

            this.user_label.FontWeight = FontWeights.Normal;
            this.user_label.Text = "";
            this.user_label.Visibility = Visibility.Collapsed;
        }

        private void DrawFileBottons(String aMsg, List<UserFile> aFileList)
        {
            //prima pulisci tutto
            clearRows();
            WriteGrid.Height = 30 * 11;

            this.user_label.FontWeight = FontWeights.Bold;
            this.user_label.Text = aMsg;
            this.user_label.Visibility = Visibility.Visible;
            this.scroll.Visibility = Visibility.Visible;

            if (aFileList != null)
            {
                if (aFileList.Count > 12)
                    WriteGrid.Height += 30 * (aFileList.Count - 12);

                foreach (UserFile s in aFileList)
                {
                    RowDefinition row = new RowDefinition();
                    row.Height = new GridLength(30);
                    //grid_files_versions.RowDefinitions.Add(row);
                    WriteGrid.RowDefinitions.Add(row);
                    int i = WriteGrid.RowDefinitions.Count;
                    System.Windows.Controls.Label lb = new System.Windows.Controls.Label();
                    lb.Tag = s.getFilePath();
                    lb.ToolTip = "Click here to open file " + s.getFilePath();
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
        }

        private void DrawVersionBottons(List<UserVersion> aVersionsList)
        {
            //prima pulisci tutto
            clearRows();
            int tot = aVersionsList.Count;
            WriteGrid.Height = 30 * 11;

            this.user_label.FontWeight = FontWeights.Bold;
            this.user_label.Text = "Available versions: ";
            this.user_label.Visibility = Visibility.Visible;
            this.scroll.Visibility = Visibility.Visible;

            //ogni entry indica il numero di versione e la data

            if (aVersionsList.Count > 12)
                WriteGrid.Height += 30 * (tot - 12);

            foreach (UserVersion v in aVersionsList)
            {
                RowDefinition row = new RowDefinition();
                row.Height = new GridLength(30);
                //grid_files_versions.RowDefinitions.Add(row);
                WriteGrid.RowDefinitions.Add(row);
                int i = WriteGrid.RowDefinitions.Count;
                System.Windows.Controls.Label lb = new System.Windows.Controls.Label();
                lb.Tag = v.getVersionID();
                lb.ToolTip = "Click here to restore version " + v.getVersionID();
                lb.Content = "Version " + v.getVersionID() + " [" + v.getVersionDate() + "]";
                lb.MouseUp += lb_MouseUpVersion;
                lb.MouseEnter += lb_MouseEnter;
                lb.MouseLeave += lb_MouseLeave;


                StackPanel sp = new StackPanel();
                sp.Children.Clear();
                sp.SetValue(Grid.RowProperty, i - 1);

                sp.Children.Add(lb);

                ////grid_files_versions.Children.Add(sp);
                WriteGrid.Children.Add(sp);
                List<UserFile> files = v.getFileList();


                foreach (UserFile f in files)
                {
                    RowDefinition row2 = new RowDefinition();
                    row2.Height = new GridLength(30);
                    WriteGrid.RowDefinitions.Add(row2);
                    i = WriteGrid.RowDefinitions.Count;
                    lb = new System.Windows.Controls.Label();
                    TextElement.SetFontSize(lb, 10);
                    lb.Tag = f;
                    lb.ToolTip = "Click here to restore only " + f.getFilePath();
                    lb.Content = f.getFileName();
                    lb.MouseEnter += lb_MouseEnter;
                    lb.MouseLeave += lb_MouseLeave;
                    //metodo per fare la restore del singolo file
                    lb.MouseUp += lb_MouseUpFile;
                    

                    sp = new StackPanel();
                    sp.Children.Clear();
                    sp.SetValue(Grid.RowProperty, i - 1);

                    Thickness margin = sp.Margin;
                    margin.Left = 20;
                    sp.Margin = margin;

                    //ad ogni elemento aggiunto aggiorno il contatore
                    tot += 1;
                    if (tot > 12) WriteGrid.Height += 30;

                    sp.Children.Add(lb);

                    ////grid_files_versions.Children.Add(sp);
                    WriteGrid.Children.Add(sp);
                }
            
            }

        }

        private void lb_MouseUpFile(object sender, MouseButtonEventArgs e)
        {
            System.Windows.Controls.Label lb = (System.Windows.Controls.Label)sender;

            UserFile f = (UserFile)lb.Tag;
            int Version = f.getFileVersion();
            String file = f.getFilePath();
            
            //int ver = Int32.Parse((String)lb.ToolTip);
            string path = pathFromFolderPicker("Select the directory in which you want to restore the selected file");
            if (path.CompareTo("") == 0)
            {
                showErrorMessage("Error", "Select a valid folder to restore the file");
                return;
            }
            if (core == null)
                core = new StorageClientCore(this);

            if (!this.core.issueRequest(new RestoreRequest(this.username, this.password, Version,file, path)))
                this.onRestoreError("Restore temporary not available! Try later.");
            else
            {
            this.status_label.Foreground = new SolidColorBrush(Colors.Black);
            this.status_label.Content = "Restoring file " + file + " from server";
            this.status_label.ToolTip = this.status_label.Content;
            this.progressRing.IsActive = true;
            this.hideMainBtnBar();
            this.clearRows();
            }
        }
        private void lb_MouseLeave(object sender, System.Windows.Input.MouseEventArgs e)
        {
            System.Windows.Controls.Label lb = (System.Windows.Controls.Label)sender;
            lb.Background = new SolidColorBrush(Colors.White);
        }

        private void lb_MouseEnter(object sender, System.Windows.Input.MouseEventArgs e)
        {
            System.Windows.Controls.Label lb = (System.Windows.Controls.Label)sender;
            lb.Background = this.progressRing.Foreground;
        }

        private void lb_MouseUp(object sender, MouseButtonEventArgs e)
        {
            //evento per gestire l' apertura del file
            System.Windows.Controls.Label lb = (System.Windows.Controls.Label)sender;

            String path = (String)lb.Tag;
            try
            {
                System.Diagnostics.Process.Start(path);
            }
            catch (Exception ex)
            {
                this.showErrorMessage("Unexpected Error", "Cannot open the selected file");
            }
        }

        private void lb_MouseUpVersion(object sender, MouseButtonEventArgs e)
        {
            System.Windows.Controls.Label lb = (System.Windows.Controls.Label)sender;

            String Version = (String)lb.Content;
            int ver = (int)lb.Tag;
            string path = pathFromFolderPicker("Select the directory in which you want to restore the selected version");
            if (path.CompareTo("") == 0)
            {
                showErrorMessage("Error", "Select a valid folder to restore the version");
                return;
            }
            if (core == null)
                core = new StorageClientCore(this);

            if (!this.core.issueRequest(new RestoreRequest(this.username, this.password, ver,"", path)))
                this.onRestoreError("Restore temporary not available! Try later.");
            else
            {
                this.status_label.Foreground = new SolidColorBrush(Colors.Black);
                this.status_label.Content = "Restoring version " + Version + " from server";
                this.status_label.ToolTip = this.status_label.Content;
                this.progressRing.IsActive = true;
                this.hideMainBtnBar();
                this.clearRows();
            }
        }

        //per mostrare un errore all' utente
        private async Task showErrorMessage(string title, string message)
        {
            await this.ShowMessageAsync(title, message);
        }

        private void user_TextChanged(object sender, TextChangedEventArgs e)
        {
            try
            {
                this.user.SetValue(TextBoxHelper.ClearTextButtonProperty, true);
            }
            catch (Exception ex)
            {
                Debug.WriteLine(ex.Message);
            }
        }

        private void repeat_pass_PasswordChanged(object sender, RoutedEventArgs e)
        {
            try
            {
                this.repeat_pass.SetValue(TextBoxHelper.ClearTextButtonProperty, true);
            }
            catch (Exception ex)
            {
                Debug.WriteLine(ex.Message);
            }
        }

        private void pass_PasswordChanged(object sender, RoutedEventArgs e)
        {
            try
            {
                this.pass.SetValue(TextBoxHelper.ClearTextButtonProperty, true);
            }
            catch (Exception ex)
            {
                Debug.WriteLine(ex.Message);
            }
       }

        private void RestoreBackButton_Click(object sender, RoutedEventArgs e)
        {
            this.clearStatusLabel(); 
            this.progressRing.IsActive = false;
            this.showMainBtnBar(false);

            if (this.lastValidFileList != null)
                this.DrawFileBottons(UPDATE_FILE_DESC, this.lastValidFileList);
            else
                this.clearRows();

            this.relaunchUpdate();
        }

        private void folderPicker_MouseUp(object sender, MouseButtonEventArgs e)
        {
            this.folder_testbox.Text = pathFromFolderPicker("Select the directory that you want to synchronize.");
        }

        private string pathFromFolderPicker(string aMsg)
        {
            FolderBrowserDialog folderBrowserDialog = new System.Windows.Forms.FolderBrowserDialog();
            folderBrowserDialog.Description = aMsg;
            folderBrowserDialog.ShowNewFolderButton = true;
            folderBrowserDialog.ShowDialog();
            return folderBrowserDialog.SelectedPath;
        }

        private void SynchButton_Click(object sender, RoutedEventArgs e)
        {
            this.disableMainBtnBar();
            this.RestoreButton.Visibility = Visibility.Visible;
            this.RestoreBackButton.Visibility = Visibility.Collapsed;
            this.clearRows();
            this.clearStatusLabel();
            this.startUpdate(this, null);
        }

        private void hideMainBtnBar() 
        {
            this.RestoreButton.Visibility = Visibility.Collapsed;
            this.RestoreBackButton.Visibility = Visibility.Collapsed;
            this.LogOut.Visibility = Visibility.Collapsed;
            this.SynchNow_Button.Visibility = Visibility.Collapsed;
            this.disableMainBtnBar();
        }

        private void showMainBtnBar(bool isRestorePage)
        {
            if (isRestorePage)
            {
                this.RestoreButton.Visibility = Visibility.Collapsed;
                this.RestoreBackButton.Visibility = Visibility.Visible;
            }
            else
            {
                this.RestoreButton.Visibility = Visibility.Visible;
                this.RestoreBackButton.Visibility = Visibility.Collapsed;
            }
            this.LogOut.Visibility = Visibility.Visible;
            this.SynchNow_Button.Visibility = Visibility.Visible;
            this.enableMainBtnBar(isRestorePage);
        }

        private void disableMainBtnBar()
        {
            this.RestoreButton.IsEnabled = false;
            this.RestoreBackButton.IsEnabled = false;
            this.LogOut.IsEnabled = false;
            this.SynchNow_Button.IsEnabled = false;
        }

        private void enableMainBtnBar(bool isRestorePage)
        {
            if (isRestorePage)
            {
                this.RestoreButton.IsEnabled = false;
                this.RestoreBackButton.IsEnabled = true;
            }
            else
            {
                this.RestoreButton.IsEnabled = true;
                this.RestoreBackButton.IsEnabled = false;
            }
            this.LogOut.IsEnabled = true;
            this.SynchNow_Button.IsEnabled = true;
        }

        private void clearStatusLabel()
        {
            this.status_label.Foreground = new SolidColorBrush(Colors.Black);
            this.status_label.Content = null;
            this.status_label.ToolTip = this.status_label.Content;
        }
    }
}