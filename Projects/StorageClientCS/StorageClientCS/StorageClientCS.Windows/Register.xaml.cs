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
    public sealed partial class Register : Page
    {
        public Register()
        {
            this.InitializeComponent();
        }

        private void RegisterButton_Click(object sender, RoutedEventArgs e)
        {
            	String pass = this.passwordbox.Password;
	            String pass2 = this.passwordbox2.Password;
	            this.Prova.Text = pass;

	            int res = String.CompareOrdinal(pass, pass2);
	        if (res != 0){
		//messaggio di errore, bisogna scegliere una cartella
		    MessageDialog messageDialog = new MessageDialog("The two password must be equal, try again", "Invalid Password");

		// Set the command that will be invoked by default
		    messageDialog.DefaultCommandIndex = 1;

		    // Show the message dialog
		    messageDialog.ShowAsync();
            }
            else
            {
                this.Frame.Navigate(typeof(StorageClientAPP));
            }
        }
    }
}
