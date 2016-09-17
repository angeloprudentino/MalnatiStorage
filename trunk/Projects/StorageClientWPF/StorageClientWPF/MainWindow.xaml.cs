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


namespace StorageClientWPF
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window//, StorageClientController
    {
        private string username;
        private string password;
        private StorageClientCore core;

        public MainWindow()
        {
            try
            {
                InitializeComponent();
            }catch(Exception e)
            {
                Debug.WriteLine("eccezione: " + e.Message);
            }

           // core = new StorageClientCore(this);

            this.second_pass_label.Visibility = Visibility.Collapsed;
            this.repeat_pass.Visibility = Visibility.Collapsed;
            this.Register_button.Visibility = Visibility.Collapsed;
            this.back_button.Visibility = Visibility.Collapsed;
            this.folder_label.Visibility = Visibility.Collapsed;
            this.folder_picker.Visibility = Visibility.Collapsed;
            this.folder_testbox.Visibility = Visibility.Collapsed;
            this.LogOut.Visibility = Visibility.Collapsed;
            this.SoWriteGrid.Visibility = Visibility.Collapsed;
            this.RestoreButton.Visibility = Visibility.Collapsed;
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

        //private void Login_Click(object sender, RoutedEventArgs e)
        //{
        //    //Click on Login
        //    this.user.Visibility = Visibility.Visible;
        //    this.user_label.Visibility = Visibility.Visible;
        //    this.pass.Visibility = Visibility.Visible;
        //    this.pass_label.Visibility = Visibility.Visible;
        //    this.LoginButton.Visibility = Visibility.Visible;
        //    this.LogOut.Visibility = Visibility.Visible;
        //    this.back_button.Visibility = Visibility.Collapsed;
            
        //}

        private void LoginButton_Click(object sender, RoutedEventArgs e)
        {
            //ha fatto login,
            //eliminare tutto ciò che è visualizzato e stampare la grid
            this.username = this.user.Text;
            this.password = this.pass.Password;

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
            this.folder_picker.Visibility = Visibility.Collapsed;
            this.folder_testbox.Visibility = Visibility.Collapsed;
            this.LogOut.Visibility = Visibility.Visible;
            this.RestoreButton.Visibility = Visibility.Visible;

            this.status_label.Content = "Utente Loggato";
            this.SoWriteGrid.Visibility = Visibility.Visible;
            //PER PROVA
            this.DrawFileBottons();

            //if (!this.core.issueRequest(new LoginRequest(username, password)))
            //    this.onLoginError("not Called");
        }

        public void onGetVersionsError(string aMsg)
        {
            if (this.Dispatcher.CheckAccess())
            {
                //update UI safely

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
                //this.text.Text = aMsg;
            }
            else
            {
                Dispatcher.BeginInvoke((Action)(() => onLoginError(aMsg)));
            }
        }

        public void onLoginSuccess()
        {
            if (this.Dispatcher.CheckAccess())
            {
                //update UI safely

            }
            else
            {
                Dispatcher.BeginInvoke((Action)(() => onLoginSuccess()));
            }
        }

        public void onRegistrationError(string aMsg)
        {
            if (this.Dispatcher.CheckAccess())
            {
                //update UI safely

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

            }
            else
            {
                Dispatcher.BeginInvoke((Action)(() => onUpdateSuccess(aFileList, aVersion, aVersionDate)));
            }
        }

        private void Window_Closed(object sender, EventArgs e)
        {   
            //do clean-up operations
            this.core.Dispose();
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
            this.folder_picker.Visibility = Visibility.Visible;
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
            this.folder_picker.Visibility = Visibility.Collapsed;
            this.folder_testbox.Visibility = Visibility.Collapsed;
            this.LoginButton.Visibility = Visibility.Visible;
            this.label_reg.Visibility = Visibility.Visible;
            this.back_button.Visibility = Visibility.Collapsed;

            this.status_label.Content = "";
        }

        private void folder_picker_Click(object sender, RoutedEventArgs e)
        {
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
            //}
        }

        private void LogOut_Click(object sender, RoutedEventArgs e)
        {
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
            this.folder_picker.Visibility = Visibility.Collapsed;
            this.folder_testbox.Visibility = Visibility.Collapsed;
            this.LoginButton.Visibility = Visibility.Collapsed;
            this.label_reg.Visibility = Visibility.Collapsed;
            this.LogOut.Visibility = Visibility.Visible;
            this.RestoreButton.Visibility = Visibility.Visible;

            this.SoWriteGrid.Visibility = Visibility.Visible;
            this.status_label.Content = "Utente registrato";



        }

        private void DrawFileBottons()
        {
            //prima pulisci tutto
            WriteGrid.RowDefinitions.Clear();
            WriteGrid.Children.Clear();
            
            
            //file di prova
            string path = "C:/Users/Daniele/Pictures/word_prova.docx";
          
            RowDefinition row = new RowDefinition();
            row.Height = new GridLength(40);
            //grid_files_versions.RowDefinitions.Add(row);
            SoWriteGrid.RowDefinitions.Add(row);
            int i = SoWriteGrid.RowDefinitions.Count;
            System.Windows.Controls.Label lb = new System.Windows.Controls.Label();
            lb.Content = path;
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

        private void DrawVersionBottons()
        {
            //prima pulisci tutto
            WriteGrid.RowDefinitions.Clear();
            WriteGrid.Children.Clear();


            ////file di prova
            //string path = "C:/Users/Daniele/Pictures/word_prova.docx";

            //RowDefinition row = new RowDefinition();
            //row.Height = new GridLength(40);
            ////grid_files_versions.RowDefinitions.Add(row);
            //SoWriteGrid.RowDefinitions.Add(row);
            //int i = SoWriteGrid.RowDefinitions.Count;
            //System.Windows.Controls.Label lb = new System.Windows.Controls.Label();
            //lb.Content = path;
            //lb.MouseUp += lb_MouseUp;
            //lb.MouseEnter += lb_MouseEnter;
            //lb.MouseLeave += lb_MouseLeave;
            ////System.Windows.Controls.Button lb = new System.Windows.Controls.Button();

            //StackPanel sp = new StackPanel();
            //sp.Children.Clear();
            //sp.SetValue(Grid.RowProperty, i - 1);

            //sp.Children.Add(lb);

            ////grid_files_versions.Children.Add(sp);
            //WriteGrid.Children.Add(sp);

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

            String path = (String)lb.Content;
            System.Diagnostics.Process.Start(path);
        }

        private void RestoreButton_Click(object sender, RoutedEventArgs e)
        {
            //stampare le label per le versioni e gestire restore
            this.DrawVersionBottons();
        }
    }
}
