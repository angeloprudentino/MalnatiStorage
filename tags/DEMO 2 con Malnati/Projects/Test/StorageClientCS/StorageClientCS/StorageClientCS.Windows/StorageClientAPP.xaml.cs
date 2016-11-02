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
using SQLite;
using UniversalSqlLite.Model;
using Windows.Security.Cryptography;
using Windows.Security.Cryptography.Core;
using Windows.Storage.Streams;


// Il modello di elemento per la pagina vuota è documentato all'indirizzo http://go.microsoft.com/fwlink/?LinkId=234238

//TO DO: nella funzione getFiles, gestione dell' inserimento nel DB
//la politica è descritta lì dove va implementata


namespace StorageClientCS
{
    /// <summary>
    /// Pagina vuota che può essere utilizzata autonomamente oppure esplorata all'interno di un frame.
    /// </summary>
    /// 
    public sealed partial class StorageClientAPP : Page
    {

        ObservableCollection<string> files;
        String outputtext;
        StringBuilder outputText;
        Dictionary<String, StorageFile> map_files;
        StorageFolder fold = KnownFolders.PicturesLibrary;
        SQLiteAsyncConnection connection;
        int Actual_Version_Client;
        int Actual_Version_Server;
        string user,pass;
        Windows.Networking.Sockets.StreamSocket socket;
        Windows.Networking.HostName serverHost;
        string serverPort = "4700";
        Stream streamOut;
        StreamWriter writer;
        Stream streamIn;
        StreamReader reader;

        byte[] bytes;
        string text_file;
        string server_response;
       
        
        public StorageClientAPP()
        {
            files = new ObservableCollection<string>();
           map_files = new Dictionary<string, StorageFile>();
            //apre se esiste, crea se non esiste

            this.InitializeComponent();


            //connessione al server
            //socket = new Windows.Networking.Sockets.StreamSocket();
            //serverHost = new Windows.Networking.HostName("192.168.0.105");
            //this.ConnectWithServer();

            //sono state inserite nell' initialize
          //  this.CreateDatabase();
           // Debug.WriteLine("cerco il numero di versione salvato nel db");
          //  this.UpdateVersionVariable();
          //  this.DisplayDB();
         
           //this.Initialize();
            

        }

        //connessione al server
        //private async Task ConnectWithServer()
        //{
        //    await socket.ConnectAsync(serverHost, serverPort);
        //}

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            var myList = e.Parameter as List<Object>;
            this.user =(string)myList[0];
            this.pass =(string)myList[1];
            this.socket = (Windows.Networking.Sockets.StreamSocket)myList[2];
            this.Messages.Text = "Welcome " + user + "   , Press SynchNow to start";
            //inizializzo gli stream di in e out
            this.streamOut = this.socket.OutputStream.AsStreamForWrite();
            this.writer = new StreamWriter(this.streamOut);

            this.streamIn = this.socket.InputStream.AsStreamForRead();
            this.reader = new StreamReader(this.streamIn);
        }


        //bottone SYNCH NOW, sarà da togliere
        private async void Button_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                var task = Task.Run(async () => { await this.Initialize(); });
                task.Wait();
            }
            catch (Exception ecc)
            {
                Debug.WriteLine("Errore nel button_click: " + ecc.Message);
            }
          //  //synch now
          //  Button _button = (Button)sender;
          //  _button.IsEnabled = false;
          // // this.Actual_Version++;
          //  WriteGrid.RowDefinitions.Clear();
          //  WriteGrid.Children.Clear();
          ////  StorageFolder fold = KnownFolders.PicturesLibrary;

          // // outputtext = "Files in " + fold.Name + "Version: " + this.Actual_Version + ": \n";

          //  try
          //  {
          //      var task = Task.Run(async () => { await this.GetFiles(fold); });
          //      task.Wait();
          //  }
          //  catch (Exception ecc)
          //  {
          //      Debug.WriteLine("Errore nel button_click: "+ ecc.Message);
          //  }
          //  //this.GetFiles(fold);

          //  Debug.WriteLine("numero di file in map: " + this.map_files.Count);

          //  Debug.WriteLine("contenuto del db dopo il click a synch now: ");
          //  this.DisplayDB();

          //  foreach (StorageFile f in this.map_files.Values)
          //  {
          //      outputtext += f.Path + ": \n";
          //  }
          //  this.Messages.Text = outputtext;
          //  this.DrawBottonsFiles();
          //  this.setListenerOnChanges();
          //  _button.IsEnabled = true;
        }

        //funzione chiamata all' avvio della app
        private async Task Initialize()
        {
           //operazioni preliminari, devono essere eseguite in ordine

            try
            {
                var task = Task.Run(async () => { await this.CreateDatabase(); });
                task.Wait();
            }
            catch (Exception ecc)
            {
                Debug.WriteLine("Errore nella creazione del db allo start: " + ecc.Message);
            }

            try
            {
                Debug.WriteLine("contenuti del db allo start");
                var task = Task.Run(async () => { await this.DisplayDB(); });
                task.Wait();
                Debug.WriteLine("trovo il numero  della versione allo START nel client");
                var task2 = Task.Run(async () => { await this.UpdateVersionVariable(); });
                task2.Wait();
               // this.Messages.Text = "Starting now...\n";
            }
            catch (Exception ecc)
            {
                Debug.WriteLine("Errore nel disply del db allo start: " + ecc.Message);
            }
            
            //synch now
           // SynchNow.IsEnabled = false;
           // Versions.IsEnabled = false;


            //SCAMBIO DI MESSAGGI COL SERVER PER VERIFICARE SE HO L' ULTIMA VERSIONE
            //
            //  get last version request
            //Stream streamOut = socket.OutputStream.AsStreamForWrite();
            //StreamWriter writer = new StreamWriter(streamOut);
            Message send_mex = new Message();
            send_mex.setType(12); 
            send_mex.addItem(this.user);
            send_mex.addItem(this.pass);

            Debug.WriteLine("creato messaggio, tipo = " + send_mex.ToSend());
            string req = send_mex.ToSend();

            await writer.WriteAsync(req);
            await writer.FlushAsync();

           // risposta GET_LAST_VERSION_REPLY
           
//           Stream streamIn = socket.InputStream.AsStreamForRead();
//           StreamReader reader = new StreamReader(streamIn);
       
            string response = await reader.ReadLineAsync();
           Debug.WriteLine("stringa ricevuta: \n" + response);
           Message resp_mex = new Message();
           if (resp_mex.Parse(response) == false)
           {
               Debug.WriteLine("risposta NON CORRETTA\n");
           }

            List<string> items_resp = resp_mex.getItems();
            foreach (string a in items_resp) Debug.WriteLine(a);

            //prendo il token, secondo item ricevuto
             string versione_server = items_resp[0];
             this.Actual_Version_Server = int.Parse(versione_server);


            //gestire la restore -> DECOMMENTARE
             if (this.Actual_Version_Server != this.Actual_Version_Client && this.Actual_Version_Server!=-1)
             {
                 //devo fare il restore se la versione del client è più vecchia del server
                 //!= -1 per vedere che ci sia almeno una versione
                 //RESTORE VERSION

                 //SISTEMAREEE con il while


                 ///
                 send_mex = new Message();
                 send_mex.setType(14); //RESTORE VERSION request
                 send_mex.addItem(this.user);
                 send_mex.addItem(this.pass);
                 send_mex.addItem(this.Actual_Version_Server.ToString());

                 Debug.WriteLine("creato messaggio, tipo = " + send_mex.ToSend());
                 req = send_mex.ToSend();


                 await writer.WriteAsync(req);
                 await writer.FlushAsync();


                 //restore version reply
                 //                streamIn = socket.InputStream.AsStreamForRead();
                 //                 reader = new StreamReader(streamIn);
                 response = await reader.ReadLineAsync();
                 Debug.WriteLine("stringa ricevuta: \n" + response);
                 resp_mex = new Message();
                 if (resp_mex.Parse(response) == false)
                 {
                     Debug.WriteLine("risposta NON CORRETTA\n");
                 }

                 items_resp = resp_mex.getItems();
                 foreach (string a in items_resp) Debug.WriteLine(a);

                 //prendo il token, secondo item ricevuto
                 string token = items_resp[1];
                 //nel primo c'è un booleano per vedere se la restore può avere successo


                 // WHILE PER OGNI FILE CHE DEVO RICEVERE -> GESTIONE TIPI DI MESSAGGI
                 //mi manda una sequenza di restore_file message
                 //in questo caso 1
                 streamIn = socket.InputStream.AsStreamForRead();
                 reader = new StreamReader(streamIn);
                 response = await reader.ReadLineAsync();
                 Debug.WriteLine("stringa ricevuta: \n" + response);
                 resp_mex = new Message();
                 if (resp_mex.Parse(response) == false)
                 {
                     Debug.WriteLine("risposta NON CORRETTA\n");
                 }

                 //[0]path
                 //[1]checksum
                 //[2] timestamp ultima modifica
                 //[3] file content

                 items_resp = resp_mex.getItems();
                 foreach (string a in items_resp) Debug.WriteLine(a);

                 string path = items_resp[0];
                 string file_content = items_resp[3];
                 file_content = Base64Decode(file_content);


                 // Create sample file; replace if exists.
                 Windows.Storage.StorageFolder storageFolder = KnownFolders.PicturesLibrary;
                 Windows.Storage.StorageFile sampleFile = await storageFolder.CreateFileAsync(path,
                         Windows.Storage.CreationCollisionOption.ReplaceExisting);
                 var buffer = Windows.Security.Cryptography.CryptographicBuffer.ConvertStringToBinary(
 file_content, Windows.Security.Cryptography.BinaryStringEncoding.Utf8);

                 await Windows.Storage.FileIO.WriteBufferAsync(sampleFile, buffer);


                 //mando una ACK per ogni file
                 send_mex = new Message();
                 send_mex.setType(17); //RESTORE_FILE_ACK
                 send_mex.addItem(token);
                 send_mex.addItem("true");
                 send_mex.addItem(path);

                 Debug.WriteLine("creato messaggio, tipo = " + send_mex.ToSend());
                 req = send_mex.ToSend();


                 await writer.WriteAsync(req);

                 await writer.FlushAsync();


                 //ricevere la STOP
                 reader = new StreamReader(streamIn);
                 response = await reader.ReadLineAsync();
                 Debug.WriteLine("stringa ricevuta: \n" + response);
                 resp_mex = new Message();
                 if (resp_mex.Parse(response) == false)
                 {
                     Debug.WriteLine("risposta NON CORRETTA\n");
                 }


             }


            //////

            //lancio la funzione che scandisce la cartella e fa i controlli
            //se non rileverò cambiamenti, la versione resterà quella del server
           
            //DECOMMENTARE
           // WriteGrid.RowDefinitions.Clear();
           // WriteGrid.Children.Clear();
            
            this.map_files.Clear();

////per prova
//            IAsyncAction asyncAction = Windows.System.Threading.ThreadPool.RunAsync(
//    (workItem) =>
//{
//    this.ProvaSend();
//});



            ///eliminare
            try
            {
                var task = Task.Run(async () => { await this.GetFiles(fold); });
                task.Wait();
            }
            catch (Exception ecc)
            {
                Debug.WriteLine("Errore nel button_click: " + ecc.Message);
            }

            Debug.WriteLine("numero di file in map: " + this.map_files.Count);

           // this.UpdateVersionVariable();
           

            foreach (StorageFile f in this.map_files.Values)
            {
                outputtext += f.Path + ": \n";
            }
            this.Messages.Text = outputtext;
            //disegna i bottoni per i file, con la loro apertura
            this.DrawBottonsFiles();

            //NON SETTIAMO IL LISTENER, verifichiamo ogni tot minuti
          //  this.setListenerOnChanges();
            SynchNow.IsEnabled = true;
            Versions.IsEnabled = true;
        }

        //funzione che cerca ricorsivamente nella cartella e salva gli elementi nel dictionary
        // e si confronta e aggiorna il db
        private async Task GetFiles(IStorageItem folder)
        {

            //APRIRE UNA SESSIONE di update
//            Stream streamOut = socket.OutputStream.AsStreamForWrite();
//            StreamWriter writer = new StreamWriter(streamOut);



            ///
            //decommentare tutta la parte qui sotto
             ///////
             StorageFolder fold = (StorageFolder)folder;
             IReadOnlyList<IStorageItem> itemList = await fold.GetItemsAsync();

             //file trovati nella cartella folder
             foreach (var item in itemList)
             {
                 if (item is StorageFile)
                 {
                     // outputText.Append("file " +file.Name + "\n");
                  //   outputtext += item.Path + "\n";

                     StorageFile f = (StorageFile)item;
                     map_files.Add(item.Path,f);
                   //  Debug.WriteLine("numero di file in map: " + this.map_files.Count);


                     Windows.Storage.FileProperties.BasicProperties basicProperties = await f.GetBasicPropertiesAsync();
                     string fileSize = string.Format("{0:n0}", basicProperties.Size);
                    // string dateMod = string.Format("{0:n0}", basicProperties.DateModified);
                     string  dateMod = basicProperties.DateModified.ToString();
                     string path_db = item.Path;
                     string name_db = item.Name;
                     Debug.WriteLine(item.Path + ",created: " + item.DateCreated + ",size: " + fileSize + ",modified: " + dateMod);



                     //vede se è presente nel db, con la versione uguale al server
                     var result = await connection.QueryAsync<FileDB>("SELECT * FROM Files WHERE Path = ? AND Versione = ?",path_db,this.Actual_Version_Server);

                     if(result.Count==1){
                         //era presente nel db, nella versione del server
                         Debug.WriteLine("file già presente nel db, nella versione del server: "+ path_db);
                         Debug.WriteLine("verifico se è stato modificato dall' ultima accensione");
                         foreach (var it in result)
                         {
                             string OldDateMod = it.DateMod;

                             //se la data di ultima modifica trovata tramite properties è uguale a quella trovata nel db
                             if (OldDateMod.Equals(dateMod))
                             {
                                 //il file non è stato modificato
                                 //faccio l' update della versione
                                 //await connection.UpdateAsync(it);

                                 //NON tocco il file nel mio db: la versione del server e la mia sono uguali
                                 Debug.WriteLine("file non modificato: "+ item.Path);
                             }
                             else
                             {
                                 //il file è stato modificato
                                 //sarà da mandare al server

                                 //MANDARE AL SERVER



                                 ///
                                 //aggiorno dateMod e versione
                                 it.DateMod = dateMod;
                                 await connection.UpdateAsync(it);
                             }
                         }

                     }
                     else
                     {
                         //non era presente nel db;
                         //file aggiunto 
                         var File_db = new FileDB()
                         {
                             Path = path_db,
                             Name = name_db,
                             DateMod = dateMod,
                         //    Versione = this.Actual_Version
                         };
                         await connection.InsertAsync(File_db);
                     }

                     //DA DECOMMENTARE
                     //prima di file la insert, vedere se il file era già presente
                     //se non era presente, inserisci (con numero di versione +1)
                     //(potrebbe anche solo essere stato spostato, ma considero una nuova entry)
                     //se sì, verificare se il dateMod è uguale
                     // se sì,cambiare il numero di versione (+1),se no
                     //se i dateMod non coincidono, elimina la entry vecchia e inserisci la nuova (versione +1 e datemod nuovo)



                 }else if(item is StorageFolder){
                     //ricorsione nel caso sia una cartella
                      var task = Task.Run(async () => { await this.GetFiles(item); });
                      task.Wait();
                 }
             }
            // this.Messages.Text = outputtext;

           //CHIUDERE LA SESSIONE COL SERVER//
            //// //update stop req

           // 
        }

        //disegna i bottoni con i link ai file
        private void DrawBottonsFiles()
        {
            foreach (StorageFile f in this.map_files.Values)
            {
                //aggiungo il bottone

                RowDefinition row = new RowDefinition();
                row.Height = new GridLength(40);
                WriteGrid.RowDefinitions.Add(row);
                int i = WriteGrid.RowDefinitions.Count;
                Button tb = new Button();
                tb.Content = f.Name;
                tb.Tag = f.Path;
                tb.Click += tb_Click;

                StackPanel sp = new StackPanel();
                sp.Children.Clear();
                sp.SetValue(Grid.RowProperty, i - 1);
                // sp.SetValue(WriteGrid.ro)
                sp.Children.Add(tb);

                WriteGrid.Children.Add(sp);
            }
        }

        //permette di aprire i file dal bottone
        void tb_Click(object sender, RoutedEventArgs e)
        {
            //evento per gestire l' apertura del file
            Button _button = (Button)sender;

            String path = (String)_button.Tag;

            StorageFile f = this.map_files[path];
            Windows.System.Launcher.LaunchFileAsync(f);
        }


        ////setta i listener per i cambiamenti
        //private void setListenerOnChanges()
        //{
        //    var options = new Windows.Storage.Search.QueryOptions
        //    {
        //        FolderDepth = Windows.Storage.Search.FolderDepth.Deep
        //    };
        //    var query = this.fold.CreateFileQueryWithOptions(options);
        //    query.ContentsChanged += query_ContentsChanged;
        //    var files = query.GetFilesAsync();
        //}
        //public async void OnChanges()
        //{
        //    Messages.Text = "Update now...";

        //    SynchNow.IsEnabled = false;
        //    Versions.IsEnabled = false;
        //    WriteGrid.RowDefinitions.Clear();
        //    WriteGrid.Children.Clear();
        // //   this.outputtext = "Update now...\nFiles in " + fold.Name + "Version: " + this.Actual_Version + "\n";
        //    this.map_files.Clear();
        //    try
        //    {
        //        var task = Task.Run(async () => { await this.GetFiles(fold); });
        //        task.Wait();
        //    }
        //    catch (Exception ecc)
        //    {
        //        Debug.WriteLine("Errore nell' ON CHANGE: " + ecc.Message);
        //    }
        //    //this.GetFiles(fold);

        //    Debug.WriteLine("numero di file in map AGGIORNATA: " + this.map_files.Count);

        //}

       // //evento invocato quando avviene un cambiamento nella cartella Pictures
       //async void query_ContentsChanged(Windows.Storage.Search.IStorageQueryResultBase sender, object args)
       // {
            
       //     // se sta sincronizzando , fallo attendere (usare una variabile globale)
       //    //oppure una condition variable
       //    Debug.WriteLine("contenuto cambiato: " + sender.Folder);
       //     await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
       //     {
       //    //     this.Actual_Version++;
       //         //rootPage.NotifyUser("The toast encountered an error", NotifyType.ErrorMessage);
       //         this.OnChanges();
       //          Debug.WriteLine("contenuto del db dopo il contenuto cambiato");
       //    //     this.UpdateVersionVariable();

       //          this.DisplayDB();
       //          foreach (StorageFile f in this.map_files.Values)
       //          {
       //              outputtext += f.Path + " \n";
       //          }
       //          this.Messages.Text = outputtext;
       //          this.setListenerOnChanges();
       //          this.DrawBottonsFiles();

       //          //aggiorno la variabile con la versione corrente

       //          SynchNow.IsEnabled = true;
       //          Versions.IsEnabled = true;
       //     });

       // }
        


        //pulsante per le versioni
       private async void Button_Click_1(object sender, RoutedEventArgs e)
        { //versioni

           //SERVE UN MODO PER ACCEDERE ALL' ELENCO DELLE VERSIONI, CON DATA
           //per scegliere di quale fare il restore

           //
            this.Messages.Text = "Versions";
            Button _button = (Button)sender;
            _button.IsEnabled = false;
            //CHIEDERE AL SERVER LE VERSIONI
            //get versions request
 //           Stream streamOut = socket.OutputStream.AsStreamForWrite();
 //           StreamWriter writer = new StreamWriter(streamOut);

            Message send_mex1 = new Message();
            send_mex1.setType(10); //get_version_request
            send_mex1.addItem(this.user);
            send_mex1.addItem(this.pass);
            Debug.WriteLine("creato messaggio, tipo = " + send_mex1.ToSend());
            string req1 = send_mex1.ToSend();
            await writer.WriteAsync(req1);
            await writer.FlushAsync();

            Debug.WriteLine("ricezione risposta\n");

            //get version reply
//            Stream streamIn4 = socket.InputStream.AsStreamForRead();
//            StreamReader reader4 = new StreamReader(streamIn4);
            string response4 = await reader.ReadLineAsync();
            Debug.WriteLine("stringa ricevuta: \n" + response4);
            Message resp_mex4 = new Message();
            if (resp_mex4.Parse(response4) == false)
            {
                Debug.WriteLine("risposta NON CORRETTA\n");
            }

            List<string> items_resp = resp_mex4.getItems();
            foreach (string a in items_resp) Debug.WriteLine(a);

            //prendo il token, secondo item ricevuto
            string token = items_resp[1];


            //

            WriteGrid.RowDefinitions.Clear();
            WriteGrid.Children.Clear();

            //disegnare i ta
            _button.IsEnabled = true;
        }


        public async Task CreateDatabase()
        {
            //connessione con il database e creazione tabella
            connection = new SQLiteAsyncConnection("Files.db");
            Debug.WriteLine("creo la tabella");
            await connection.CreateTableAsync<FileDB>();
       
        }

        public async Task UpdateVersionVariable()
        {
            var result = await connection.QueryAsync<FileDB>("SELECT * FROM Files WHERE User = ?",this.user);

            int MaxVers=-1;
            foreach (var item in result)
            {
                if (item.Versione > MaxVers)
                {
                    Debug.WriteLine("aggiorno la versione, nuovo valore: " + item.Versione);
                    this.Actual_Version_Client = item.Versione;
                    MaxVers = this.Actual_Version_Client;
                }
            }
            Debug.WriteLine("versione più recente nel client per utente: "+ this.user +"= " + this.Actual_Version_Client);
        }

        public async Task DisplayDB()
        {
            Debug.WriteLine("DISPLAY_DB:contenuti del db per l' utente "+ this.user);
            var result = await connection.QueryAsync<FileDB>("SELECT * FROM Files WHERE User = ?",this.user);

            foreach (var item in result)
            {
                Debug.WriteLine(item.Path + " dateMod: "+ item.DateMod + " version: "+ item.Versione);
            }

            Debug.WriteLine("FINE contenuti del db");
        }

        public static string Base64Encode(string plainText)
        {
            var plainTextBytes = System.Text.Encoding.UTF8.GetBytes(plainText);
            return System.Convert.ToBase64String(plainTextBytes);
        }
        public static string Base64Decode(string base64EncodedData)
        {
            var base64EncodedBytes = System.Convert.FromBase64String(base64EncodedData);
            return System.Text.Encoding.UTF8.GetString(base64EncodedBytes, 0, base64EncodedBytes.Length);
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


        //FUNZIONI DI PROVA
        private async Task ProvaSend()
        {
            try
            {

                ////Create the StreamSocket and establish a connection to the echo server.
                //Windows.Networking.Sockets.StreamSocket socket = new Windows.Networking.Sockets.StreamSocket();

                ////The server hostname that we will be establishing a connection to. We will be running the server and client locally,
                ////so we will use localhost as the hostname.
                //Windows.Networking.HostName serverHost = new Windows.Networking.HostName("localhost");
                ////  Windows.Networking.HostName serverHost = new Windows.Networking.HostName("127.0.0.1");
                ////Every protocol typically has a standard port number. For example HTTP is typically 80, FTP is 20 and 21, etc.
                ////For the echo server/client application we will use a random port 1337.
                //string serverPort = "4700";

                ////await socket.ConnectAsync(serverHost, serverPort);
                //await socket.ConnectAsync(serverHost, serverPort);


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

    }
}

namespace UniversalSqlLite.Model
{
    [Table("Files")]
    public class FileDB
    {       
        [PrimaryKey]
        public string Path { get; set; }

        //considerare l' utente!!
        public string User { get; set; }

        public string Name { get; set; }

        public string DateMod { get; set; }

        public int Versione { get; set; }
    }

    //tabella delle versioni lato server
    //con nome versione e data
    //-> recupero la versione come massimo valore contenuto nella mia tabela


    //inserire una tabella utenti
}