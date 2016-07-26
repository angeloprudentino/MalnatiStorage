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
using Windows.Storage;
using System.Diagnostics;
using System.Threading.Tasks;
using SQLite;
using UniversalSqlLite.Model;


// Il modello di elemento per la pagina vuota è documentato all'indirizzo http://go.microsoft.com/fwlink/?LinkId=234238

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
        
        
        public StorageClientAPP()
        {
            files = new ObservableCollection<string>();
           map_files = new Dictionary<string, StorageFile>();
            //apre se esiste, crea se non esiste

            this.InitializeComponent();
            
           // this.CreateDatabase();
            this.Messages.Text = "Starting now...\n Press SynchNow to Start";
            this.Initialize();

        }

        private async void Button_Click(object sender, RoutedEventArgs e)
        {
          
            //synch now
            Button _button = (Button)sender;
            _button.IsEnabled = false;
            WriteGrid.RowDefinitions.Clear();
            WriteGrid.Children.Clear();
          //  StorageFolder fold = KnownFolders.PicturesLibrary;
            
            outputtext = "Files in " + fold.Name + ": \n";
            //outputText = new StringBuilder();
            //outputText.AppendLine("Seach in " + KnownFolders.PicturesLibrary.Name);
           // this.GetFiles(fold);
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

            foreach (StorageFile f in this.map_files.Values)
            {
                outputtext += f.Path + ": \n";
            }
            this.Messages.Text = outputtext;
            this.DrawBottonsFiles();
            this.setListenerOnChanges();
            _button.IsEnabled = true;
        }

        private async void Initialize()
        {
            //synch now
            SynchNow.IsEnabled = false;
            Versions.IsEnabled = false;
            WriteGrid.RowDefinitions.Clear();
            WriteGrid.Children.Clear();
            //  StorageFolder fold = KnownFolders.PicturesLibrary;

            outputtext = "Files in " + fold.Name + ": \n";
            //outputText = new StringBuilder();
            //outputText.AppendLine("Seach in " + KnownFolders.PicturesLibrary.Name);
            // this.GetFiles(fold);
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

        //modificare per fargli ritornare direttamente la mappa di oggetti
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

                    Debug.WriteLine(item.Path + ",created: " + item.DateCreated + ",size: " + fileSize + ",modified: " + dateMod);

                }else if(item is StorageFolder){
                     var task = Task.Run(async () => { await this.GetFiles(item); });
                     task.Wait();
                }
            }
           // this.Messages.Text = outputtext;

        }

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

        void query_ContentsChanged(Windows.Storage.Search.IStorageQueryResultBase sender, object args)
        {
            Messages.Text = "Update now...";

            SynchNow.IsEnabled=false;
            Versions.IsEnabled=false;
            Debug.WriteLine("contenuto cambiato: "+sender.Folder);
            this.outputtext = "";
            var task = Task.Run(async () => { await this.GetFiles(fold); });
            task.Wait();
            //  Debug.WriteLine("numero di file in map: " + this.map_files.Count);

            foreach (StorageFile f in this.map_files.Values)
            {
                outputtext += f.Path + " \n";
            }
            this.setListenerOnChanges();
            this.DrawBottonsFiles();

           // this.SynchNow.IsEnabled=true;
          //  this.Versions.IsEnabled=true;
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
        public async void CreateDatabase()
        {
            connection = new SQLiteAsyncConnection("Files.db");
            Debug.WriteLine("creo la tabella");
            await connection.CreateTableAsync<Files>();

        }
    }
}

namespace UniversalSqlLite.Model
{
    [Table("Files")]
    public class Files
    {
        [PrimaryKey]
        public string Name { get; set; }
       
        [PrimaryKey]
        public string Path { get; set; }


        public string DateMod { get; set; }

        public string Version { get; set; }
    }
}