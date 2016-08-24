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
using System.Threading.Tasks;
using System.Diagnostics;

// Il modello di elemento per la pagina vuota è documentato all'indirizzo http://go.microsoft.com/fwlink/?LinkId=234238

namespace StorageClientCS
{
    /// <summary>
    /// Pagina vuota che può essere utilizzata autonomamente oppure esplorata all'interno di un frame.
    /// </summary>
    public sealed partial class Login : Page
    {
        Windows.Networking.Sockets.StreamSocket socket;
        Windows.Networking.HostName serverHost;
        string serverPort = "4700";
        List<string> items_resp;
        bool server_res;
        string token;
        
        public Login()
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

        private void LogInButton_Click(object sender, RoutedEventArgs e)
        {
            Button _button = (Button)sender;
            _button.IsEnabled = false;
            String user = this.User.Text;

            String pass = this.passwordbox.Password;

            //mando un messaggio di UPDATE START_REQ
            //per vedere se le credenziali sono giuste

            //DECOMMENTARE
            try
            {
                var task = Task.Run(async () => { await this.Connect(user, pass); });
                task.Wait();
            }
            catch (Exception ecc)
            {
                Debug.WriteLine("Errore nel primo scambio di messaggi: " + ecc.Message);
            }

            //String  prova_pass = "Pippo";
            //int res=String.CompareOrdinal(prova_pass,pass);
            if (this.server_res=false)
            {
                MessageDialog messageDialog =  new MessageDialog("Username or Password not correct, try again", "Invalid Credentials");
                messageDialog.DefaultCommandIndex = 1;
                messageDialog.ShowAsync();
            }
            else
            {
                //chiudo la update_start (usata solo per controllare le credenziali)
              //  this.CloseUpdate(token);

                //DECOMMENTARE-> chiude lo scambio utilizzato per verificare la PASSWORD
                try
                {
                    var task = Task.Run(async () => { await this.CloseUpdate(token); });
                    task.Wait();
                }
                catch (Exception ecc)
                {
                    Debug.WriteLine("Errore nel secondo scambio di messaggi: " + ecc.Message);
                }

                var myList = new List<string>()
                    {
                       user,
                       pass,
                    };
                this.Frame.Navigate(typeof(StorageClientAPP),myList);
            }
            _button.IsEnabled = true;
        }

        private async Task Connect(string user, string pass)
        {
            Debug.WriteLine("dovrei essere connesso col server");
            Stream streamOut = socket.OutputStream.AsStreamForWrite();
            StreamWriter writer = new StreamWriter(streamOut);

            //provo a comporre io il messaggio
            Message send_mex = new Message();

            //Update _start_req
            send_mex.setType(2); //user_reg_req
            send_mex.addItem(user);
            send_mex.addItem(pass);

            Debug.WriteLine("creato messaggio, tipo = " + send_mex.ToSend());
            string req = send_mex.ToSend();

            await writer.WriteLineAsync(req);
            await writer.FlushAsync();

            Stream streamIn = socket.InputStream.AsStreamForRead();
            StreamReader reader = new StreamReader(streamIn);
            string response = await reader.ReadLineAsync();
            Debug.WriteLine("stringa ricevuta: \n" + response);
            Message resp_mex = new Message();
            if (resp_mex.Parse(response) == false)
            {
                Debug.WriteLine("risposta NON CORRETTA\n");
            }
            this.items_resp = resp_mex.getItems();
            //foreach (string a in items_resp) Debug.WriteLine(a);

            //prendo la risposta true/false;
            string resp = items_resp[0];
            int res = String.CompareOrdinal(resp,"true");
            if (res == 0) this.server_res = true;
            else this.server_res = false;
            this.token = items_resp[1];

        }
       
        private async Task CloseUpdate(string token)
        {
            Stream streamOut = socket.OutputStream.AsStreamForWrite();
            StreamWriter writer = new StreamWriter(streamOut);

           // token = "UGRxVWZDKzdqZ1BHMDA4L2hqVEhGZ2FzVWtzPSRQaXBwbyQxNDcyMDQ1MjY0JFBkcVVmQys3amdQRzAwOC9oalRIRmdhc1Vrcz0=";
            //provo a comporre io il messaggio
            Message send_mex2 = new Message();

            //Update _stop_req
            send_mex2.setType(8);
            send_mex2.addItem(token);

            Debug.WriteLine("creato messaggio, tipo = " + send_mex2.ToSend());
            string req = send_mex2.ToSend();

            await writer.WriteLineAsync(req);
            await writer.FlushAsync();

            Stream streamIn = socket.InputStream.AsStreamForRead();
            StreamReader reader = new StreamReader(streamIn);
            string response = await reader.ReadLineAsync();
            Debug.WriteLine("stringa ricevuta: \n" + response);
            Message resp_mex2 = new Message();
            if (resp_mex2.Parse(response) == false)
            {
                Debug.WriteLine("risposta NON CORRETTA\n");
            }
            this.items_resp = resp_mex2.getItems();
            //foreach (string a in items_resp) Debug.WriteLine(a);


        }

    }
}
