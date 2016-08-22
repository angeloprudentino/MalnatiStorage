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




// Il modello di elemento per la pagina vuota è documentato all'indirizzo http://go.microsoft.com/fwlink/?LinkId=234238

namespace ClientDiProva
{
    static class Constants
    {
        public const double Pi = 3.14159;
        public const int SpeedOfLight = 300000; // km per sec.
        public const string MSG_SEP="$";
        public const string MSG_SEP_ESC= "&#36";
        public const string MSG_INVALID = "invalid";
        public const string END_MSG = "END_MSG";

    }
    
    /// <summary>
    /// Pagina vuota che può essere utilizzata autonomamente oppure esplorata all'interno di un frame.
    /// </summary>
    public sealed partial class MainPage : Page
    {
        public MainPage()
        {
            this.InitializeComponent();
            this.Send();
        }

        private async Task Send()
        {
            try
            {
                
                //Create the StreamSocket and establish a connection to the echo server.
                Windows.Networking.Sockets.StreamSocket socket = new Windows.Networking.Sockets.StreamSocket();

                //The server hostname that we will be establishing a connection to. We will be running the server and client locally,
                //so we will use localhost as the hostname.
              Windows.Networking.HostName serverHost = new Windows.Networking.HostName("localhost");
              //  Windows.Networking.HostName serverHost = new Windows.Networking.HostName("127.0.0.1");
                //Every protocol typically has a standard port number. For example HTTP is typically 80, FTP is 20 and 21, etc.
                //For the echo server/client application we will use a random port 1337.
                string serverPort = "4700";
                                                              
                //await socket.ConnectAsync(serverHost, serverPort);
               await socket.ConnectAsync(serverHost,serverPort );

                Debug.WriteLine("dovrei essere connesso");

                //creo un messaggio
                
                //Write data to the echo server.
                Stream streamOut = socket.OutputStream.AsStreamForWrite();
                StreamWriter writer = new StreamWriter(streamOut);
                string request = "USER_REG_REQ$Pippo$pippo$END_MSG\n";

                //provo a comporre io il messaggio
                Message send_mex = new Message();

                ////USER_REG_REQ
                //send_mex.setType(0); //user_reg_req
                //send_mex.addItem("Pippo");
                //send_mex.addItem("pippo");

                //UPDATE_START_REQ
                send_mex.setType(2); //user_reg_req
                send_mex.addItem("Pippo");
                send_mex.addItem("pippo");

                Debug.WriteLine("creato messaggio, tipo = " + send_mex.ToSend());
                string req = send_mex.ToSend();

               await writer.WriteLineAsync(req);
              //  await writer.WriteLineAsync(request);
                await writer.FlushAsync();

                Debug.WriteLine("ricezione risposta\n");

                //Read data from the echo server.
                Stream streamIn = socket.InputStream.AsStreamForRead();
                StreamReader reader = new StreamReader(streamIn);
                string response = await reader.ReadLineAsync();
                Debug.WriteLine("stringa ricevuta: \n" + response);
                Message resp_mex = new Message();
                if (resp_mex.Parse(response) == false)
                {
                    Debug.WriteLine("risposta NON CORRETTA\n");
                }
                //string response = await reader.ReadToEndAsync();
                
            }
            catch (Exception e)
            {
                Debug.WriteLine("Eccezione CLIENT: " + e.Message);           
            }
        }
    }

}
