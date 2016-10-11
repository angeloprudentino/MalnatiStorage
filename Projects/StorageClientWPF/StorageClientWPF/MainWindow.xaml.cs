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
        private const int UPDATE_INTERVAL = /*300*/60 * 1000;
        private const int PING_INTERVAL = 60 * 1000;

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
            // this.folder_picker.Visibility = Visibility.Collapsed;
            this.folder_testbox.Visibility = Visibility.Collapsed;
            this.LogOut.Visibility = Visibility.Collapsed;
            this.SoWriteGrid.Visibility = Visibility.Collapsed;
            this.RestoreButton.Visibility = Visibility.Collapsed;
            this.RestoreBackButton.Visibility = Visibility.Collapsed;
            this.folderPicker.Visibility = Visibility.Collapsed;
            this.progressRing.IsActive = false;
        }

        public void onGetVersionsError(string aMsg)
        {
            if (this.Dispatcher.CheckAccess())
            {
                //update UI safely
                this.showErrorMessage("Unexpected Error", aMsg);
                this.progressRing.IsActive = false;
                this.scroll.Visibility = Visibility.Collapsed;
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
                this.lastValidVersionList = aVersionsList;
                this.DrawVersionBottons(aVersionsList);
                this.RestoreButton.Visibility = Visibility.Collapsed;
                this.RestoreBackButton.Visibility = Visibility.Visible;
                this.RestoreBackButton.IsEnabled = true;
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
                this.path = aPath;
                this.startUpdate(this, null);

                this.progressRing.IsActive = false;
                this.LoginButton.IsEnabled = true;
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
                this.LogOut.Visibility = Visibility.Visible;
                this.RestoreButton.Visibility = Visibility.Visible;
                this.RestoreBackButton.Visibility = Visibility.Collapsed;
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
                this.showErrorMessage("Unexpected Error", aMsg);
                this.progressRing.IsActive = false;
                this.Register_button.IsEnabled = true;
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

                this.startUpdate(this, null);

                //aggiorno la grafica
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
                this.label_reg.Visibility = Visibility.Collapsed;
                this.folder_label.Visibility = Visibility.Collapsed;
                this.folder_testbox.Visibility = Visibility.Collapsed;
                this.LoginButton.Visibility = Visibility.Collapsed;
                this.label_reg.Visibility = Visibility.Collapsed;
                this.folderPicker.Visibility = Visibility.Collapsed;
                this.LogOut.Visibility = Visibility.Visible;
                this.RestoreButton.Visibility = Visibility.Visible;
                this.RestoreBackButton.Visibility = Visibility.Collapsed;
                this.SoWriteGrid.Visibility = Visibility.Visible;
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
                //stop update timer during restore
                if (this.updateTimer != null)
                    this.updateTimer.Stop();

                //update UI safely
                this.status_label.Foreground = new SolidColorBrush(Colors.Black);
                this.status_label.Content = "Restoring files from server";
                this.status_label.ToolTip = this.status_label.Content;
                this.progressRing.IsActive = true;
                this.LogOut.IsEnabled = false;
                this.RestoreButton.IsEnabled = false;
                this.RestoreButton.Visibility = Visibility.Collapsed;
                this.RestoreBackButton.IsEnabled = false;
                this.RestoreBackButton.Visibility = Visibility.Visible;
                this.clearRows();

                this.token = aToken;
                if (this.pingTimer == null)
                {
                    this.pingTimer = new Timer();
                    this.pingTimer.Tick += new EventHandler(pingSession);
                    this.pingTimer.Interval = PING_INTERVAL;
                }
                this.pingTimer.Start();


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
                this.LogOut.IsEnabled = true;
                this.RestoreButton.IsEnabled = false;
                this.RestoreButton.Visibility = Visibility.Collapsed;
                this.RestoreBackButton.IsEnabled = true;
                this.RestoreBackButton.Visibility = Visibility.Visible;

                if (this.lastValidVersionList != null)
                    this.DrawVersionBottons(this.lastValidVersionList);
                else
                    this.clearRows();

                if (this.pingTimer != null)
                    this.pingTimer.Stop();
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
                this.LogOut.IsEnabled = true;
                this.RestoreButton.IsEnabled = false;
                this.RestoreButton.Visibility = Visibility.Collapsed;
                this.RestoreBackButton.IsEnabled = true;
                this.RestoreBackButton.Visibility = Visibility.Visible;
                this.DrawFileBottons(aFileList);

                if (this.pingTimer != null)
                    this.pingTimer.Stop();
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
                this.relaunchUpdate();
                this.progressRing.IsActive = false;
                if (this.pingTimer != null)
                    this.pingTimer.Stop();

                this.RestoreButton.IsEnabled = true;
                this.RestoreButton.Visibility = Visibility.Visible;
                this.LogOut.IsEnabled = true;
                if (this.lastValidFileList != null)
                    this.DrawFileBottons(this.lastValidFileList);
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
                this.status_label.Foreground = new SolidColorBrush(Colors.Black);
                this.status_label.Content = "Synchronizing with server";
                this.status_label.ToolTip = this.status_label.Content;
                this.progressRing.IsActive = true;
                this.scroll.Visibility = Visibility.Collapsed;
                this.token = aToken;

                if (this.pingTimer == null)
                {
                    this.pingTimer = new Timer();
                    this.pingTimer.Tick += new EventHandler(pingSession);
                    this.pingTimer.Interval = PING_INTERVAL;
                }
                this.pingTimer.Start();

                //disabilito i bottoni
                this.RestoreButton.IsEnabled = false;
                this.RestoreBackButton.IsEnabled = false;
                this.LogOut.IsEnabled = false;
                this.clearRows();

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
                if (this.pingTimer != null)
                    this.pingTimer.Stop();

                this.lastValidFileList = aFileList;
                this.DrawFileBottons(aFileList);
                //riabilito i bottoni
                this.RestoreButton.IsEnabled = true;
                this.LogOut.IsEnabled = true;
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
            if (this.updateTimer != null)
                this.updateTimer.Stop();

            if (this.core != null)
            {
                if (!this.core.issueRequest(new UpdateRequest(this.username, this.password, this.path)))
                    this.onUpdateError("Update temporarly not available! Try later.");
                else
                    this.progressRing.IsActive = true;
            }
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
            this.label_reg.Visibility = Visibility.Collapsed;
            this.back_button.Visibility = Visibility.Visible;
            this.LoginButton.Visibility = Visibility.Visible;
            this.label_reg.Visibility = Visibility.Visible;
            this.folder_label.Visibility = Visibility.Visible;
            this.folderPicker.Visibility = Visibility.Visible; 
            this.folder_testbox.Visibility = Visibility.Visible;
            this.LoginButton.Visibility = Visibility.Collapsed;
            this.label_reg.Visibility = Visibility.Collapsed;
            this.status_label.Content = null;
            this.status_label.ToolTip = this.status_label.Content;

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
            //this.label_reg.Foreground = this.Resources["Color_023"] as SolidColorBrush;
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

            //sistemare le textbox, svuotarle e settare a false il tasto cancella
            this.user.Text = "";
            this.user.SetValue(TextBoxHelper.ClearTextButtonProperty, false);
            this.pass.Password = "";
            this.pass.SetValue(TextBoxHelper.ClearTextButtonProperty, false);
            this.repeat_pass.Password = "";
            this.repeat_pass.SetValue(TextBoxHelper.ClearTextButtonProperty, false);

            this.status_label.Content = null;
            this.status_label.ToolTip = this.status_label.Content;
            this.progressRing.IsActive = false;
        }

        private void LogOut_Click(object sender, RoutedEventArgs e)
        {
            this.user_label.Text = "Username";
            this.user_label.Visibility = Visibility.Visible;
            this.user.Visibility = Visibility.Visible;
            this.pass_label.Visibility = Visibility.Visible;
            this.pass.Visibility = Visibility.Visible;
            this.LoginButton.Visibility = Visibility.Visible;
            this.label_reg.Visibility = Visibility.Visible;
            this.LogOut.Visibility = Visibility.Collapsed;
            this.RestoreButton.Visibility = Visibility.Collapsed;
            this.RestoreBackButton.Visibility = Visibility.Collapsed;
            WriteGrid.RowDefinitions.Clear();
            WriteGrid.Children.Clear();
            this.SoWriteGrid.Visibility = Visibility.Collapsed;
            this.status_label.Content = null;
            this.status_label.ToolTip = this.status_label.Content;
            this.progressRing.IsActive = false;

            if (this.updateTimer != null)
                this.updateTimer.Stop();

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
                    }
                }
                else
                {
                    this.showErrorMessage("Unexpected Error", "Please select a valid folder");
                }
            }
            catch (Exception ex)
            {
                this.showErrorMessage("Unexpected Error", "Please select a valid folder");
            }

        }

        private void RestoreButton_Click(object sender, RoutedEventArgs e)
        {
            if (this.core == null)
                this.core = new StorageClientCore(this);

            if (!this.core.issueRequest(new GetVerRequest(this.username, this.password)))
                this.onRestoreError("Restore temporary not available! Try later.");
        }

        private void clearRows(){
            WriteGrid.RowDefinitions.Clear();
            WriteGrid.Children.Clear();
            this.scroll.Visibility = Visibility.Collapsed;

            this.user_label.Text = "";
            this.user_label.Visibility = Visibility.Collapsed;
        }

        private void DrawFileBottons(List<UserFile> aFileList)
        {
            //prima pulisci tutto
            clearRows();

            this.user_label.Text = "Files in folder: ";
            this.user_label.Visibility = Visibility.Visible;
            this.scroll.Visibility = Visibility.Visible;

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
            clearRows();

            this.user_label.Text = "Available versions: ";
            this.user_label.Visibility = Visibility.Visible;
            this.scroll.Visibility = Visibility.Visible;

            //ogni entry indica il numero di versione e la data

            if (aVersionsList.Count > 12)
                WriteGrid.Height += 30 * (aVersionsList.Count - 12);

            foreach (UserVersion v in aVersionsList)
            {
                RowDefinition row = new RowDefinition();
                row.Height = new GridLength(30);
                //grid_files_versions.RowDefinitions.Add(row);
                WriteGrid.RowDefinitions.Add(row);
                int i = WriteGrid.RowDefinitions.Count;
                System.Windows.Controls.Label lb = new System.Windows.Controls.Label();
                lb.ToolTip = v.getVersionID().ToString();
                lb.Content = "Version " + v.getVersionID() + ": " + v.getVersionDate();
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

            String path = (String)lb.ToolTip;
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
            int ver = Int32.Parse((String)lb.ToolTip);
            //aprire un folder picker, per scegliere dove ripristinare 
            FolderBrowserDialog folderBrowserDialog1 = new System.Windows.Forms.FolderBrowserDialog();
            // Set the help text description for the FolderBrowserDialog.
            folderBrowserDialog1.Description =
                "Select the directory in which you want to restore the version";

            // Do not allow the user to create new files via the FolderBrowserDialog.
            folderBrowserDialog1.ShowNewFolderButton = false;
            folderBrowserDialog1.ShowDialog();
            string path = folderBrowserDialog1.SelectedPath;
            Debug.WriteLine("restore path: " + path);
            this.status_label.Foreground = new SolidColorBrush(Colors.Black);
            this.status_label.Content = "Trying to restore version " + Version;
            this.status_label.ToolTip = this.status_label.Content;

            if (core == null)
                core = new StorageClientCore(this);

            if (!this.core.issueRequest(new RestoreRequest(this.username, this.password, ver, path)))
                this.onRestoreError("Restore temporary not available! Try later.");
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
            Debug.WriteLine("testo cambiato");
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
            Debug.WriteLine("password cambiato");
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
            Debug.WriteLine("password cambiato");
        }

        private void RestoreBackButton_Click(object sender, RoutedEventArgs e)
        {
            this.status_label.Foreground = new SolidColorBrush(Colors.Black);
            this.status_label.Content = null;
            this.status_label.ToolTip = this.status_label.Content;
            this.progressRing.IsActive = false;
            this.LogOut.IsEnabled = true;
            this.RestoreButton.IsEnabled = true;
            this.RestoreButton.Visibility = Visibility.Visible;
            this.RestoreBackButton.IsEnabled = false;
            this.RestoreBackButton.Visibility = Visibility.Collapsed;
            if (this.lastValidFileList != null)
                this.DrawFileBottons(this.lastValidFileList);
            else
                this.clearRows();

            this.relaunchUpdate();
        }

        private void folderPicker_MouseUp(object sender, MouseButtonEventArgs e)
        {
            //se clicco nella text box mi apre il folder picker
            FolderBrowserDialog folderBrowserDialog1 = new System.Windows.Forms.FolderBrowserDialog();
            // Set the help text description for the FolderBrowserDialog.
            folderBrowserDialog1.Description =
                "Select the directory that you want to synchronize.";

            // Do not allow the user to create new files via the FolderBrowserDialog.
            folderBrowserDialog1.ShowNewFolderButton = false;
            folderBrowserDialog1.ShowDialog();
            string path = folderBrowserDialog1.SelectedPath;
            Debug.WriteLine(path);
            this.folder_testbox.Text = path;
        }
    }
}