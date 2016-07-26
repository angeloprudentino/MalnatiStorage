using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;
using Windows.UI.Popups;

// Il modello di elemento per la pagina vuota è documentato all'indirizzo http://go.microsoft.com/fwlink/?LinkId=234238

namespace StorageClientCS
{
    /// <summary>
    /// Pagina vuota che può essere utilizzata autonomamente oppure esplorata all'interno di un frame.
    /// </summary>
    public sealed partial class Login : Page
    {
        public Login()
        {
            this.InitializeComponent();
        }

        private void LogInButton_Click(object sender, RoutedEventArgs e)
        {
            String user = this.User.Text;

            String pass = this.passwordbox.Password;
            String  prova_pass = "Pippo";
            int res=String.CompareOrdinal(prova_pass,pass);
            if (res != 0)
            {
                MessageDialog messageDialog =  new MessageDialog("Username or Password not correct, try again", "Invalid Credentials");
                messageDialog.DefaultCommandIndex = 1;
                messageDialog.ShowAsync();
            }
            else
            {
                this.Frame.Navigate(typeof(StorageClientAPP));
            }
        }
    }
}
