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

namespace StorageClientWPF
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window, StorageClientController
    {
        private string username;
        private string password;

        public MainWindow()
        {
            InitializeComponent();
            this.user.Visibility = Visibility.Collapsed;
            this.user_label.Visibility = Visibility.Collapsed;
            this.pass.Visibility = Visibility.Collapsed;
            this.pass_label.Visibility = Visibility.Collapsed;
            this.LoginButton.Visibility = Visibility.Collapsed;
            this.second_pass_label.Visibility = Visibility.Collapsed;
            this.repeat_pass.Visibility = Visibility.Collapsed;
            this.Register_button.Visibility = Visibility.Collapsed;
        }

        private void Button_Click_1(object sender, RoutedEventArgs e)
        {
            //Click on Register
            Login.Visibility = Visibility.Collapsed;
            Register.Visibility = Visibility.Collapsed;
            this.user.Visibility = Visibility.Visible;
            this.user_label.Visibility = Visibility.Visible;
            this.pass.Visibility = Visibility.Visible;
            this.pass_label.Visibility = Visibility.Visible;
            this.repeat_pass.Visibility = Visibility.Visible;
            this.second_pass_label.Visibility = Visibility.Visible;
            this.Register_button.Visibility = Visibility.Visible;
        }

        private void Login_Click(object sender, RoutedEventArgs e)
        {
            //Click on Login
            Login.Visibility = Visibility.Collapsed;
            Register.Visibility = Visibility.Collapsed;
            this.user.Visibility = Visibility.Visible;
            this.user_label.Visibility = Visibility.Visible;
            this.pass.Visibility = Visibility.Visible;
            this.pass_label.Visibility = Visibility.Visible;
            this.LoginButton.Visibility = Visibility.Visible;
        }

        private void LoginButton_Click(object sender, RoutedEventArgs e)
        {
            this.username = this.user.Text;
            this.password = this.pass.Password;

            StorageClientCore core = new StorageClientCore(this);
            core.issueRequest(new LoginRequest("pippo", "pippo"));
        }

        public void onGetVersionsError(string aMsg)
        {

        }
        public void onGetVersionsSuccess()
        {

        }
        public void onLoginError(string aMsg)
        {
            this.text.Text = aMsg;
        }
        public void onLoginSuccess()
        {

        }
        public void onRegistrationError(string aMsg)
        {

        }
        public void onRegistrationSucces()
        {

        }
        public void onRestoreError(string aMsg)
        {

        }
        public void onRestoreSuccess()
        {

        }
        public void onUpdateError(string aMsg)
        {

        }
        public void onUpdateStart()
        {

        }
        public void onUpdateSuccess()
        {

        }

    }
}
