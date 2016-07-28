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
        int Actual_Version;
        
        
        public StorageClientAPP()
        {
            files = new ObservableCollection<string>();
           map_files = new Dictionary<string, StorageFile>();
            //apre se esiste, crea se non esiste

            this.InitializeComponent();

            this.CreateDatabase();
            Debug.WriteLine("cerco il numero di versione salvato nel db");
            this.UpdateVersionVariable();
            this.DisplayDB();
            this.Messages.Text = "Welcome, Press SynchNow to start";
         
            //this.Initialize();
            

        }

        private async void Button_Click(object sender, RoutedEventArgs e)
        {
          
            //synch now
            Button _button = (Button)sender;
            _button.IsEnabled = false;
            this.Actual_Version++;
            WriteGrid.RowDefinitions.Clear();
            WriteGrid.Children.Clear();
          //  StorageFolder fold = KnownFolders.PicturesLibrary;

            outputtext = "Files in " + fold.Name + "Version: " + this.Actual_Version + ": \n";

            try
            {
                var task = Task.Run(async () => { await this.GetFiles(fold); });
                task.Wait();
            }
            catch (Exception ecc)
            {
                Debug.WriteLine("Errore nel button_click: "+ ecc.Message);
            }
            //this.GetFiles(fold);

            Debug.WriteLine("numero di file in map: " + this.map_files.Count);

            Debug.WriteLine("contenuto del db dopo il click a synch now: ");
            this.DisplayDB();

            foreach (StorageFile f in this.map_files.Values)
            {
                outputtext += f.Path + ": \n";
            }
            this.Messages.Text = outputtext;
            this.DrawBottonsFiles();
            this.setListenerOnChanges();
            _button.IsEnabled = true;
        }

        //funzione chiamata all' avvio della app
        private async void Initialize()
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
                Debug.WriteLine("trovo il numero  della versione allo START");
                var task2 = Task.Run(async () => { await this.UpdateVersionVariable(); });
                task2.Wait();
                this.Messages.Text = "Starting now...\n";
            }
            catch (Exception ecc)
            {
                Debug.WriteLine("Errore nel disply del db allo start: " + ecc.Message);
            }
            
            //synch now
            SynchNow.IsEnabled = false;
            Versions.IsEnabled = false;
            WriteGrid.RowDefinitions.Clear();
            WriteGrid.Children.Clear();
            //  StorageFolder fold = KnownFolders.PicturesLibrary;

            outputtext = "Files in " + fold.Name + "Version: "+ this.Actual_Version +"\n";
            //outputText = new StringBuilder();
            //outputText.AppendLine("Seach in " + KnownFolders.PicturesLibrary.Name);
            // this.GetFiles(fold);
            this.map_files.Clear();

            try
            {
                var task = Task.Run(async () => { await this.GetFiles(fold); });
                task.Wait();
            }
            catch (Exception ecc)
            {
                Debug.WriteLine("Errore nel button_click: " + ecc.Message);
            }
            //this.GetFiles(fold);

            Debug.WriteLine("numero di file in map: " + this.map_files.Count);

            this.UpdateVersionVariable();
           

            foreach (StorageFile f in this.map_files.Values)
            {
                outputtext += f.Path + ": \n";
            }
            this.Messages.Text = outputtext;
            this.DrawBottonsFiles();
            this.setListenerOnChanges();
            SynchNow.IsEnabled = true;
            Versions.IsEnabled = true;
        }

        //funzione che cerca ricorsivamente nella cartella e salva gli elementi nel dictionary
        // e si confronta e aggiorna il db
        private async Task GetFiles(IStorageItem folder)
        {

          //  outputText.Append("entro in getFiles  \n");
            //per vedere se accede in memoria alla pictures
            // StorageFile f = await KnownFolders.PicturesLibrary.GetFileAsync("maxresdefault.jpg");
           // outputText.Append("trovato files: "+ f.Path);
            StorageFolder fold = (StorageFolder)folder;
            IReadOnlyList<IStorageItem> itemList = await fold.GetItemsAsync();
            
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



                    //vede se è presente nel db
                    var result = await connection.QueryAsync<FileDB>("SELECT * FROM Files WHERE Path = ?",path_db);
               
                    if(result.Count==1){
                        //era presente nel db
                        //Debug.WriteLine("file già presente nel db: "+ path_db);
                        foreach (var it in result)
                        {
                            string OldDateMod = it.DateMod;
                            it.Versione = this.Actual_Version;
                            if (OldDateMod.Equals(dateMod))
                            {
                                //il file non è stato modificato
                                //faccio l' update della versione
                                await connection.UpdateAsync(it);
                            }
                            else
                            {
                                //il file è stato modificato
                                //sarà da mandare al server
                                //aggiorno dateMod e versione
                                it.DateMod = dateMod;
                                await connection.UpdateAsync(it);
                            }
                        }

                    }
                    else
                    {
                        //non era presente nel db;
                        var File_db = new FileDB()
                        {
                            Path = path_db,
                            Name = name_db,
                            DateMod = dateMod,
                            Versione = this.Actual_Version
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
                     var task = Task.Run(async () => { await this.GetFiles(item); });
                     task.Wait();
                }
            }
           // this.Messages.Text = outputtext;
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



        private void setListenerOnChanges()
        {
            var options = new Windows.Storage.Search.QueryOptions
            {
                FolderDepth = Windows.Storage.Search.FolderDepth.Deep
            };
            var query = this.fold.CreateFileQueryWithOptions(options);
            query.ContentsChanged += query_ContentsChanged;
            var files = query.GetFilesAsync();
        }
        public async void OnChanges()
        {
            Messages.Text = "Update now...";

            SynchNow.IsEnabled = false;
            Versions.IsEnabled = false;
            WriteGrid.RowDefinitions.Clear();
            WriteGrid.Children.Clear();
            this.outputtext = "Update now...\nFiles in " + fold.Name + "Version: " + this.Actual_Version + "\n";
            this.map_files.Clear();
            try
            {
                var task = Task.Run(async () => { await this.GetFiles(fold); });
                task.Wait();
            }
            catch (Exception ecc)
            {
                Debug.WriteLine("Errore nell' ON CHANGE: " + ecc.Message);
            }
            //this.GetFiles(fold);

            Debug.WriteLine("numero di file in map AGGIORNATA: " + this.map_files.Count);

        }

        //evento invocato quando avviene un cambiamento nella cartella Pictures
       async void query_ContentsChanged(Windows.Storage.Search.IStorageQueryResultBase sender, object args)
        {
            Debug.WriteLine("contenuto cambiato: " + sender.Folder);
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () =>
            {
                this.Actual_Version++;
                //rootPage.NotifyUser("The toast encountered an error", NotifyType.ErrorMessage);
                this.OnChanges();
                 Debug.WriteLine("contenuto del db dopo il contenuto cambiato");
           //     this.UpdateVersionVariable();

                 this.DisplayDB();
                 foreach (StorageFile f in this.map_files.Values)
                 {
                     outputtext += f.Path + " \n";
                 }
                 this.Messages.Text = outputtext;
                 this.setListenerOnChanges();
                 this.DrawBottonsFiles();

                 //aggiorno la variabile con la versione corrente

                 SynchNow.IsEnabled = true;
                 Versions.IsEnabled = true;
            });

        }
        
        void tb_Click(object sender, RoutedEventArgs e)
        {
            //evento per gestire l' apertura del file
            Button _button = (Button)sender;
            
            String path = (String)_button.Tag;

            StorageFile f = this.map_files[path];
            Windows.System.Launcher.LaunchFileAsync(f);
        }


        private void Button_Click_1(object sender, RoutedEventArgs e)
        { //versioni
            this.Messages.Text = "Versions";
            Button _button = (Button)sender;
            _button.IsEnabled = false;
            WriteGrid.RowDefinitions.Clear();
            WriteGrid.Children.Clear();

            _button.IsEnabled = true;
        }
        public async Task CreateDatabase()
        {
            connection = new SQLiteAsyncConnection("Files.db");
            Debug.WriteLine("creo la tabella");
            await connection.CreateTableAsync<FileDB>();
       
        }

        public async Task UpdateVersionVariable()
        {
            var result = await connection.QueryAsync<FileDB>("SELECT * FROM Files");

            int MaxVers=0;
            foreach (var item in result)
            {
                if (item.Versione > MaxVers)
                {
                    Debug.WriteLine("aggiorno la versione, nuovo valore: " + item.Versione);
                    this.Actual_Version = item.Versione;
                    MaxVers = this.Actual_Version;
                }
            }
            Debug.WriteLine("versione più recente: " + this.Actual_Version);
        }

        public async Task DisplayDB()
        {
            Debug.WriteLine("DISPLAY_DB:contenuti del db");
            var result = await connection.QueryAsync<FileDB>("SELECT * FROM Files");

            foreach (var item in result)
            {
                Debug.WriteLine(item.Path + " dateMod: "+ item.DateMod + " version: "+ item.Versione);
            }

            Debug.WriteLine("FINE contenuti del db");
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

        public string Name { get; set; }

        public string DateMod { get; set; }

        public int Versione { get; set; }
    }
}