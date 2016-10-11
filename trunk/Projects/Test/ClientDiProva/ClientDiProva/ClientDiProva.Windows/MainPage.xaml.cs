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
        byte[] bytes;
        string text_file;
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
                send_mex.addItem("pip");
                send_mex.addItem("pip");

                Debug.WriteLine("creato messaggio, tipo = " + send_mex.ToSend());
                string req = send_mex.ToSend();


                await writer.WriteAsync(req);

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
                String file_name = "maxresdefault.jpg";
                StorageFile file = await fold.GetFileAsync(file_name);
                //item[2] -> file path
                send_mex2.addItem("maxresdefault.jpg");

                // item[3] -> file checksum

                //  string text = await Windows.Storage.FileIO.ReadTextAsync(file);


                try
                {
                    var task = Task.Run(async () =>
                    {
                        byte[] fileBytes = null;
                        using (IRandomAccessStreamWithContentType stream = await file.OpenReadAsync())
                        {
                            fileBytes = new byte[stream.Size];
                            using (DataReader Reader = new DataReader(stream))
                            {
                                await Reader.LoadAsync((uint)stream.Size);
                                Reader.ReadBytes(fileBytes);
                            }
                        }


                        //  text_file= Windows.Security.Cryptography.CryptographicBuffer.ConvertBinaryToString(BinaryStringEncoding.Utf8, CryptographicBuffer.CreateFromByteArray(fileBytes));
                        text_file = Base64EncodeBin(fileBytes);
                        // Create sample file; replace if exists.
                        //                   Windows.Storage.StorageFolder storageFolder =KnownFolders.PicturesLibrary;
                        //                   Windows.Storage.StorageFile sampleFile = await storageFolder.CreateFileAsync("sample.png",
                        //                           Windows.Storage.CreationCollisionOption.ReplaceExisting);
                        //                   var buffer = Windows.Security.Cryptography.CryptographicBuffer.ConvertStringToBinary(
                        //text_file, Windows.Security.Cryptography.BinaryStringEncoding.Utf8);

                        //                   await Windows.Storage.FileIO.WriteBufferAsync(sampleFile, buffer);

                    });
                    task.Wait();
                }
                catch (Exception ecc)
                {
                    Debug.WriteLine("Errore nell'aspettare: " + ecc.Message);
                }

                //   IBuffer b = await Windows.Storage.FileIO.ReadBufferAsync(file);
                //   byte[] ByteArray = b.ToArray();
                //   string text = System.Text.Encoding.UTF8.GetString(bytes,0,bytes.Length);

                //   text_file = Base64Encode(text_file);
                string checksum = Base64Checksum(text_file);

                send_mex2.addItem(checksum);
                //item[4] -> file date
                Windows.Storage.FileProperties.BasicProperties basicProperties = await file.GetBasicPropertiesAsync();
                string dateMod = basicProperties.DateModified.ToString();
                send_mex2.addItem(dateMod);
                //content in base 64item[5] -> file content in base 64
                send_mex2.addItem(text_file);

                //preparo il messaggio all' invio
                string req2 = send_mex2.ToSend();


                await writer.WriteAsync(req2);
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
                Message send_mex3 = new Message();
                send_mex3.setType(8); //update stop req
                send_mex3.addItem(token);

                req = send_mex3.ToSend();

                await writer.WriteAsync(req);
                ////  await writer.WriteLineAsync(request);
                await writer.FlushAsync();

                //update stop reply
                response = await reader.ReadLineAsync();
                Debug.WriteLine("stringa ricevuta: \n" + response);
                Message resp_mex3 = new Message();
                if (resp_mex3.Parse(response) == false)
                {
                    Debug.WriteLine("risposta NON CORRETTA\n");
                }
                



//                //RESTORE VERSION
//                Message send_mex = new Message();
//                send_mex.setType(14); //RESTORE VERSION request
//                send_mex.addItem("pippo");
//                send_mex.addItem("pippo");
//                send_mex.addItem("0");

//                Debug.WriteLine("creato messaggio, tipo = " + send_mex.ToSend());
//                string req = send_mex.ToSend();


//                await writer.WriteAsync(req);

//                await writer.FlushAsync();



//                //restore version reply
//                Stream streamIn4 = socket.InputStream.AsStreamForRead();
//                StreamReader reader4 = new StreamReader(streamIn4);
//               string response4 = await reader4.ReadLineAsync();
//                Debug.WriteLine("stringa ricevuta: \n" + response4);
//                Message resp_mex4 = new Message();
//                if (resp_mex4.Parse(response4) == false)
//                {
//                    Debug.WriteLine("risposta NON CORRETTA\n");
//                }

//                List<string> items_resp = resp_mex4.getItems();
//                foreach (string a in items_resp) Debug.WriteLine(a);

//                           //prendo il token, secondo item ricevuto
//                string token = items_resp[1];
//                //nel primo c'è un booleano per vedere se la restore può avere successo

//                //mi manda una sequenza di restore_file message
//                //in questo caso 1
//                string response5 = await reader4.ReadLineAsync();
//                Debug.WriteLine("stringa ricevuta: \n" + response5);
//                Message resp_mex5 = new Message();
//                if (resp_mex5.Parse(response5) == false)
//                {
//                    Debug.WriteLine("risposta NON CORRETTA\n");
//                }

//                //[0]path
//                //[1]checksum
//                //[2] timestamp ultima modifica
//                //[3] file content
                
//                List<string> items_resp5 = resp_mex5.getItems();
//                foreach (string a in items_resp5) Debug.WriteLine(a);

//                string path = items_resp5[0];
//                string file_content = items_resp5[3];
//                file_content = Base64Decode(file_content);


//                // Create sample file; replace if exists.
//                Windows.Storage.StorageFolder storageFolder = KnownFolders.PicturesLibrary;
//                Windows.Storage.StorageFile sampleFile = await storageFolder.CreateFileAsync("Version_0\\" + path,
//                        Windows.Storage.CreationCollisionOption.ReplaceExisting);
//                var buffer = Windows.Security.Cryptography.CryptographicBuffer.ConvertStringToBinary(
//file_content, Windows.Security.Cryptography.BinaryStringEncoding.Utf8);

//                await Windows.Storage.FileIO.WriteBufferAsync(sampleFile, buffer);


//                //mando una ACK per ogni file
//                Message send_mex6 = new Message();
//                send_mex6.setType(17); //RESTORE_FILE_ACK
//                send_mex6.addItem(token);
//                send_mex6.addItem("true");
//                send_mex6.addItem(path);

//                Debug.WriteLine("creato messaggio, tipo = " + send_mex6.ToSend());
//                string req6 = send_mex6.ToSend();


//                await writer.WriteAsync(req6);

//                await writer.FlushAsync();

//                //ricevere la STOP
//                StreamReader reader7 = new StreamReader(streamIn4);
//                string response7 = await reader4.ReadLineAsync();
//                Debug.WriteLine("stringa ricevuta: \n" + response7);
//                Message resp_mex7 = new Message();
//                if (resp_mex7.Parse(response7) == false)
//                {
//                    Debug.WriteLine("risposta NON CORRETTA\n");
//                }

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

        public static string Base64EncodeBin(byte[] plainTextBytes)
        {
            return System.Convert.ToBase64String(plainTextBytes);
        }
        public static string Base64DecodeBin(byte[] base64EncodedBytes)
        {
            return System.Text.Encoding.UTF8.GetString(base64EncodedBytes, 0, base64EncodedBytes.Length);
        }
       
        public static string Base64Checksum(string text)
        {
            //tratto da
            //http://stackoverflow.com/questions/26969469/cannot-find-type-system-security-cryptography-sha256-on-windows-phone-8-1
       
            HashAlgorithmProvider hap = HashAlgorithmProvider.OpenAlgorithm(HashAlgorithmNames.Md5);
            CryptographicHash ch = hap.CreateHash();
            // read in bytes from file then append with ch.Append(data)           
           byte[] ByteArray = Encoding.UTF8.GetBytes(text);
           // byte[] ByteArray = Encoding.Unicode.GetBytes(text);
            IBuffer buffer = ByteArray.AsBuffer();
            ch.Append(buffer);

            Windows.Storage.Streams.IBuffer b_hash = ch.GetValueAndReset();       // hash it
            string hash_string = CryptographicBuffer.EncodeToBase64String(b_hash);  // encode it to a hex string for easy reading

           return hash_string;


            //
        }
    }

}
