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
        public const int USER_REG_REQ_TOK_NUM = 4;
        public const int USER_REG_REPLY_TOK_NUM= 3;
        public const int UPDATE_START_REQ_TOK_NUM= 4;
        public const int UPDATE_START_REPLY_TOK_NUM= 4;
        public const int ADD_NEW_FILE_TOK_NUM= 7;
        public const int UPDATE_FILE_TOK_NUM= 7;
        public const int REMOVE_FILE_TOK_NUM= 4;
        public const int FILE_ACK_TOK_NUM =4;
        public const int UPDATE_STOP_REQ_TOK_NUM = 3;
        public const int UPDATE_STOP_REPLY_TOK_NUM = 5;
        public const int GET_VERSIONS_REQ_TOK_NUM = 4;
        public const int GET_VERSIONS_REPLY_MIN_TOK_NUM = 5;
        public const int RESTORE_VER_REQ_TOK_NUM = 5;
        public const int RESTORE_VER_REPLY_TOK_NUM = 4;
        public const int RESTORE_FILE_TOK_NUM = 6;
        public const int RESTORE_FILE_ACK_TOK_NUM = 5;
        public const int RESTORE_STOP_TOK_NUM = 4;
        public const int PING_TOK_NUM = 4;

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

                ////Write data to the echo server.
                //Stream streamOut = socket.OutputStream.AsStreamForWrite();
                //StreamWriter writer = new StreamWriter(streamOut);
                //string request = "test";
                //await writer.WriteLineAsync(request);
                //await writer.FlushAsync();

                //Read data from the echo server.
                Stream streamIn = socket.InputStream.AsStreamForRead();
                StreamReader reader = new StreamReader(streamIn);
                string response = await reader.ReadLineAsync();
                Debug.WriteLine("stringa ricevuta: \n" + response);
            }
            catch (Exception e)
            {
                Debug.WriteLine("Eccezione CLIENT: " + e.Message);           
            }
        }
    }

}
