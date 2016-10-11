using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Windows;
using System.Text;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;
using Windows.UI.Core;
using Windows.Storage;
using System.Diagnostics;
using System.Threading.Tasks;
using Windows.UI.Popups;
using SQLite;
using UniversalSqlLite.Model;

// Il modello di elemento per la pagina vuota è documentato all'indirizzo http://go.microsoft.com/fwlink/?LinkId=234238

namespace StorageClientCS
{
    static class Constants
    {
        public const string MSG_SEP = "$";
        public const string MSG_SEP_ESC = "&#36";
        public const string MSG_INVALID = "invalid";
        public const string END_MSG = "END_MSG";

    }
    /// <summary>
    /// Pagina vuota che può essere utilizzata autonomamente oppure esplorata all'interno di un frame.
    /// </summary>
    public sealed partial class Register : Page
    {
        Windows.Networking.Sockets.StreamSocket socket;
        Windows.Networking.HostName serverHost;
        string serverPort = "4700";
        public Register()
        {
            this.InitializeComponent();
            socket = new Windows.Networking.Sockets.StreamSocket();
            serverHost = new Windows.Networking.HostName("localhost");
            this.ConnectWithServer();
        }

        private async Task ConnectWithServer()
        {
            await socket.ConnectAsync(serverHost, serverPort);
        }

        private void RegisterButton_Click(object sender, RoutedEventArgs e)
        {
                Button _button = (Button)sender;
                _button.IsEnabled = false;
            	String pass = this.passwordbox.Password;
	            String pass2 = this.passwordbox2.Password;
                String user = this.User.Text;
	            
                this.Prova.Text = "user: "+ user + " pass: " + pass;
                
	            int res = String.CompareOrdinal(pass, pass2);
	        if (res != 0){
		//se le password sono diverse, finestra di dialogo con l' errore
		    MessageDialog messageDialog = new MessageDialog("The two password must be equal, try again", "Invalid Password");

		// Set the command that will be invoked by default
		    messageDialog.DefaultCommandIndex = 1;

		    // Show the message dialog
		    messageDialog.ShowAsync();
            }
            else
            {
                //controlla se l' utente è già regustrati, in base alla risposta del server
                this.Connect(user,pass);
            }
            _button.IsEnabled = true;
        }

        private async Task Connect(string user, string pass)
        {

            Debug.WriteLine("dovrei essere connesso col server");
            Stream streamOut = socket.OutputStream.AsStreamForWrite();
            StreamWriter writer = new StreamWriter(streamOut);
            //string request = "USER_REG_REQ$Pippo$pippo$END_MSG\n";

            //provo a comporre io il messaggio
            Message send_mex = new Message();

            //USER_REG_REQ
            send_mex.setType(0); //user_reg_req
            send_mex.addItem(user);
            send_mex.addItem(pass);

            Debug.WriteLine("creato messaggio, tipo = " + send_mex.ToSend());
            string req = send_mex.ToSend();

            await writer.WriteLineAsync(req);
            await writer.FlushAsync();

            Debug.WriteLine("ricezione risposta\n");

            //lettura della risposta.
            Stream streamIn = socket.InputStream.AsStreamForRead();
            StreamReader reader = new StreamReader(streamIn);
            string response = await reader.ReadLineAsync();
            Debug.WriteLine("stringa ricevuta: \n" + response);
            Message resp_mex = new Message();
            if (resp_mex.Parse(response) == false)
            {
                Debug.WriteLine("risposta NON CORRETTA\n");
            }
            List<string> items_resp = resp_mex.getItems();
            //foreach (string a in items_resp) Debug.WriteLine(a);

            //prendo la risposta true/false;
            string resp = items_resp[0];
            int res = String.CompareOrdinal(resp,"false");
            if (res == 0)
            {
                //ho ricevuto false dal server -> utente già registrato
                MessageDialog messageDialog = new MessageDialog("Username already registered, please choose another username", "Invalid User");

                // Set the command that will be invoked by default
                messageDialog.DefaultCommandIndex = 1;

                // Show the message dialog
                messageDialog.ShowAsync();
            }
            else
            {
                //ricevuto true, registrazione andata a buon fine,
                Debug.WriteLine("registrazione riuscita per l' utente: "+ user);
                var myList = new List<Object>()
                    {
                       user,
                       pass,
                       socket,
                    };
                //passo lo user e la pass come parametro-> serviranno per i successivi messaggi
                this.Frame.Navigate(typeof(StorageClientAPP),myList);
            }

        }
    }
}
