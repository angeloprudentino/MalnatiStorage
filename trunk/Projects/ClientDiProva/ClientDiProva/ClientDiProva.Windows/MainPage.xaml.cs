using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Windows;
using System.Text;
using System.Security;
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
using Windows.Security.Cryptography;
using Windows.Security.Cryptography.Core;
using Windows.Storage.Streams;
//#include "openssl/rand.h"
//#include "openssl/evp.h"
//#include "openssl/sha.h"
//#include "openssl/buffer.h"
//#include "openssl/bio.h" // BIO objects for I/O
//#include "openssl/ssl.h" // SSL and SSL_CTX for SSL connections
//#include "openssl/err.h" // Error reporting



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
              //  string request = "USER_REG_REQ$Pippo$pippo$END_MSG\n";

                //provo a comporre io il messaggio
              //  Message send_mex1 = new Message();

              ////  ////USER_REG_REQ
              //  send_mex1.setType(0); //user_reg_req
              //  send_mex1.addItem("Pippo");
              //  send_mex1.addItem("pippo");
              //  Debug.WriteLine("creato messaggio, tipo = " + send_mex1.ToSend());
              //  string req1 = send_mex1.ToSend();
              //  await writer.WriteLineAsync(req1);
              //  //  await writer.WriteLineAsync(request);
              //  await writer.FlushAsync();

              //  Debug.WriteLine("ricezione risposta\n");
                
              //  Stream streamIn1 = socket.InputStream.AsStreamForRead();
              //  StreamReader reader1 = new StreamReader(streamIn1);
              //  string response1 = await reader1.ReadLineAsync();
              //  Debug.WriteLine("stringa ricevuta: \n" + response1);
              //  Message resp_mex1 = new Message();
              //  if (resp_mex1.Parse(response1) == false)
              //  {
              //      Debug.WriteLine("risposta NON CORRETTA\n");
              //  }

                //  //UPDATE_START_REQ
                Message send_mex = new Message();
                send_mex.setType(2); //user_reg_req
                send_mex.addItem("Pippo");
                send_mex.addItem("pippo");

                Debug.WriteLine("creato messaggio, tipo = " + send_mex.ToSend());
                string req = send_mex.ToSend();

                await writer.WriteLineAsync(req);
                //  await writer.WriteLineAsync(request);
                await writer.FlushAsync();

                Debug.WriteLine("ricezione risposta\n");

              ////  //Read data from the echo server.
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

               List<string> items_resp = resp_mex.getItems();
               foreach (string a in items_resp) Debug.WriteLine(a);

               //prendo il token, secondo item ricevuto
               string token = items_resp[1];
              // string token = "M3Q1OWRaajhJVkhVOHNlUEhSZFlWSU5YYnFVPSRQaXBwbyQxNDcyMDQ2MjUyJDN0NTlkWmo4SVZIVThzZVBIUmRZVklOWGJxVT0=";

                //richiedo di aggiungere un file
                /*
                * item[0] -> msg name
                * item[1] -> token
                * item[2] -> file path
                * item[3] -> file checksum
                * item[4] -> file date
                * item[5] -> file content
                */
               Message send_mex2 = new Message();
               // item[0] -> msg name
               send_mex2.setType(4); //add new file
               //item[1] -> token
               send_mex2.addItem(token);
               
               StorageFolder fold = KnownFolders.PicturesLibrary;
               String file_name = "prova.txt";
               StorageFile file = await fold.GetFileAsync(file_name);
               //item[2] -> file path
               send_mex2.addItem(file.Path);

               // item[3] -> file checksum
               string text = await Windows.Storage.FileIO.ReadTextAsync(file);
               string checksum = Base64Checksum(text);
               send_mex2.addItem(checksum);
               //item[4] -> file date
               Windows.Storage.FileProperties.BasicProperties basicProperties = await file.GetBasicPropertiesAsync();
               string dateMod = basicProperties.DateModified.ToString();
               send_mex2.addItem(dateMod);
               //content in base 64item[5] -> file content in base 64
               send_mex2.addItem(Base64Encode(text));

                //preparo il messaggio all' invio
               req = send_mex2.ToSend();

               await writer.WriteLineAsync(req);
               //  await writer.WriteLineAsync(request);
               await writer.FlushAsync();

               //l' ACK del server
               string response2 = await reader.ReadLineAsync();
               Debug.WriteLine("stringa ricevuta: \n" + response2);
               Message resp_mex2 = new Message();
               if (resp_mex2.Parse(response) == false)
               {
                   Debug.WriteLine("risposta NON CORRETTA\n");
               }
                
               //// //update stop req
               //Message send_mex3 = new Message();
               //send_mex3.setType(8); //update stop req
               //send_mex3.addItem(token);

               // req = send_mex3.ToSend();

               //await writer.WriteLineAsync(req);
               ////  await writer.WriteLineAsync(request);
               //await writer.FlushAsync();

               ////update stop reply
               //response = await reader.ReadLineAsync();
               //Debug.WriteLine("stringa ricevuta: \n" + response);
               //Message resp_mex3 = new Message();
               //if (resp_mex3.Parse(response) == false)
               //{
               //    Debug.WriteLine("risposta NON CORRETTA\n");
               //}
                
            }
            catch (Exception e)
            {
                Debug.WriteLine("Eccezione CLIENT: " + e.Message);           
            }
        }
        public static string Base64Encode(string plainText)
        {
            var plainTextBytes = System.Text.Encoding.UTF8.GetBytes(plainText);
            return System.Convert.ToBase64String(plainTextBytes);
        }
        public static string Base64Decode(string base64EncodedData)
        {
            var base64EncodedBytes = System.Convert.FromBase64String(base64EncodedData);
            return System.Text.Encoding.UTF8.GetString(base64EncodedBytes,0,base64EncodedBytes.Length);
        }

        public static string Base64Checksum(string text)
        {
            //tratto da
            //http://stackoverflow.com/questions/26969469/cannot-find-type-system-security-cryptography-sha256-on-windows-phone-8-1
       
            HashAlgorithmProvider hap = HashAlgorithmProvider.OpenAlgorithm(HashAlgorithmNames.Sha256);
            CryptographicHash ch = hap.CreateHash();
            // read in bytes from file then append with ch.Append(data)           
            byte[] ByteArray = Encoding.UTF8.GetBytes(text);
            IBuffer buffer = ByteArray.AsBuffer();
            ch.Append(buffer);

            Windows.Storage.Streams.IBuffer b_hash = ch.GetValueAndReset();       // hash it
            //string hash_string = CryptographicBuffer.EncodeToHexString(b_hash);  
            // encode it to a hex string for easy reading
            return b_hash.ToString();
           // return hash_string;
        }
    }

}
