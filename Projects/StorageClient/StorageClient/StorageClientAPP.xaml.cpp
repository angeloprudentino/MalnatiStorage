//
// StorageClientAPP.xaml.cpp
// Implementazione della classe StorageClientAPP
//

#include "pch.h"
#include "StorageClientAPP.xaml.h"
//#include <boost\filesystem.hpp>
#include <iostream>
#include <string>
#include <array>
#include <list>

using namespace StorageClient;

//using namespace boost::filesystem;

using namespace std;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace concurrency;
using namespace Windows::Storage;
using namespace Windows::Storage::Pickers;
using namespace Windows::Storage::AccessCache;
using namespace Windows::Storage::Search;
using namespace Windows::UI::Popups;
using namespace Windows::System;




// Il modello di elemento per la pagina base è documentato all'indirizzo http://go.microsoft.com/fwlink/?LinkId=234237
String^ Mypath;
StorageFolder^ Myfolder;
//<StorageFile^,100> FileNow;
map<String^, StorageFile^> FileNow;
map<String^, StorageFolder^> FolderNow;

//StorageFolderQueryResult^ queryResult;
//timer
//DispatcherTimer^ dt;

StorageClientAPP::StorageClientAPP()
{
	InitializeComponent();
	SetValue(_defaultViewModelProperty, ref new Map<String^, Object^>(std::less<String^>()));
	auto navigationHelper = ref new Common::NavigationHelper(this);
	SetValue(_navigationHelperProperty, navigationHelper);
	navigationHelper->LoadState += ref new Common::LoadStateEventHandler(this, &StorageClientAPP::LoadState);
	navigationHelper->SaveState += ref new Common::SaveStateEventHandler(this, &StorageClientAPP::SaveState);
}

DependencyProperty^ StorageClientAPP::_defaultViewModelProperty =
DependencyProperty::Register("DefaultViewModel",
TypeName(IObservableMap<String^, Object^>::typeid), TypeName(StorageClientAPP::typeid), nullptr);

/// <summary>
/// utilizzata come semplice modello di visualizzazione.
/// </summary>
IObservableMap<String^, Object^>^ StorageClientAPP::DefaultViewModel::get()
{
	return safe_cast<IObservableMap<String^, Object^>^>(GetValue(_defaultViewModelProperty));
}

DependencyProperty^ StorageClientAPP::_navigationHelperProperty =
DependencyProperty::Register("NavigationHelper",
TypeName(Common::NavigationHelper::typeid), TypeName(StorageClientAPP::typeid), nullptr);

/// <summary>
/// Ottiene un'implementazione di <see cref="NavigationHelper"/> progettata per essere
/// utilizzata come semplice modello di visualizzazione.
/// </summary>
Common::NavigationHelper^ StorageClientAPP::NavigationHelper::get()
{
	return safe_cast<Common::NavigationHelper^>(GetValue(_navigationHelperProperty));
}

#pragma region Navigation support

/// I metodi forniti in questa sezione vengono utilizzati per consentire a
/// NavigationHelper di rispondere ai metodi di navigazione della pagina.
/// 
/// La logica specifica della pagina deve essere inserita nel gestore eventi per  
/// <see cref="NavigationHelper::LoadState"/>
/// e <see cref="NavigationHelper::SaveState"/>.
/// Il parametro di navigazione è disponibile nel metodo LoadState 
/// oltre allo stato della pagina conservato durante una sessione precedente.

void StorageClientAPP::OnNavigatedTo(NavigationEventArgs^ e)
{
	//Mypath = (String^)e->Parameter;
	Myfolder = (StorageFolder^)e->Parameter;
	NavigationHelper->OnNavigatedTo(e);
	//this->Messages->Text = Myfolder->Path;
	//dt = ref new DispatcherTimer();
	//dt->Tick += ref new Windows::Foundation::EventHandler<Platform::Object ^>(this, &StorageClient::StorageClientAPP::First_read_folder);
	////dt->Tick += ref new Windows::Foundation::EventHandler<Platform::Object ^>(this, &StorageClient::StorageClientAPP::OnTick);
	//TimeSpan t;
	//t.Duration = 10000;
	//dt->Interval = t;
	//dt->Start();
	//	First_read_folder();
	//auto queryResult = localFolder->CreateFileQueryWithOptions(queryOptions);

	//gestisco l' evento se viene modificata la cartella
	//queryResult = Myfolder->CreateFileQuery();
	//queryResult->ContentsChanged += ref new Windows::Foundation::TypedEventHandler<Windows::Storage::Search::IStorageQueryResultBase ^, Platform::Object ^>(this, &StorageClient::StorageClientAPP::OnLocalAppDataChanged);
	//create_task(queryResult->GetFilesAsync()).then([this](IVectorView<StorageFile^>^ files)
	//{
	//	String^ outputText = "";
	//	//output how many files that match the query were found
	//	if (files->Size == 0)
	//	{
	//		outputText += "No files found for '";
	//	}
	//	else if (files->Size == 1)
	//	{
	//		outputText += files->Size.ToString() + " file found:\n\n";
	//	}
	//	else
	//	{
	//		outputText += files->Size.ToString() + " files found:\n\n";
	//	}
	//	//output the name of each file that matches the query
	//	for (unsigned int i = 0; i < files->Size; i++)
	//	{
	//		outputText += files->GetAt(i)->Name + "\n";
	//	}
	//	Messages->Text = outputText;
	//});
}

void StorageClientAPP::OnNavigatedFrom(NavigationEventArgs^ e)
{
	NavigationHelper->OnNavigatedFrom(e);
	//path = e->Parameter->ToString();
}

#pragma endregion

/// <summary>
/// Popola la pagina con il contenuto passato durante la navigazione. Vengono inoltre forniti eventuali stati
/// salvati durante la ricreazione di una pagina in una sessione precedente.
/// </summary>
/// <param name="sender">
/// Origine dell'evento. In genere <see cref="NavigationHelper"/>
/// </param>
/// <param name="e">Dati evento che forniscono il parametro di navigazione passato a
/// <see cref="Frame.Navigate(Type, Object)"/> quando la pagina è stata inizialmente richiesta e
/// un dizionario di stato mantenuto da questa pagina nel corso di una sessione
/// precedente. Lo stato è null la prima volta che viene visitata una pagina.</param>
void StorageClientAPP::LoadState(Object^ sender, Common::LoadStateEventArgs^ e)
{
	(void)sender;	// Parametro non utilizzato
	(void)e;	// Parametro non utilizzato
}

/// <summary>
/// Mantiene lo stato associato a questa pagina in caso di sospensione dell'applicazione o se la
/// viene scartata dalla cache di navigazione.  I valori devono essere conformi ai requisiti di
/// serializzazione di <see cref="SuspensionManager::SessionState"/>.
/// </summary>
/// <param name="sender">Origine dell'evento. In genere <see cref="NavigationHelper"/></param>
/// <param name="e">Dati di evento che forniscono un dizionario vuoto da popolare con
/// uno stato serializzabile.</param>
void StorageClientAPP::SaveState(Object^ sender, Common::SaveStateEventArgs^ e){
	(void)sender;	// Parametro non utilizzato
	(void)e; // Parametro non utilizzato
}


void StorageClient::StorageClientAPP::Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	//TENERE LA PARTE COMMENTATA PER LA GESTIONE STRINGHE 
	////String^ Mypath;

	////per prova

	////PROBLEMA CON I DIRITTI DI ACCESSO AI FILE
	////leggo con boost
	//std::wstring ws1(Mypath->Data());

	//// Assign the modified wstring back to str1. 
	//Mypath = ref new String(ws1.c_str());
	//
	//path p(ws1);

	////Messages->Text = "stampa il path: " + Mypath;
	////testo da stampare
	//String^ outputtext = ref new String();
	//outputtext = "fyles in: " + Mypath + " : \n";

	////conversione da string a String^
	//std::wstring p2 = p.wstring();
	//String^ pp= ref new String(p2.data());
	////outputtext += pp + " : \n";

	//LETTURA DA KNOWNFOLDER
	Messages->Text = "Folder: " + Myfolder->Path;
	//elimino gli elementi già presenti
	WriteGrid->RowDefinitions->Clear();
	WriteGrid->Children->Clear();
	//int size = WriteGrid->RowDefinitions->Size;
	//for (int j = 0; j < size; j++){
	//	 WriteGrid->RowDefinitions->RemoveAt(j);
	//}
	//KnownFolders::PicturesLibrary
	//Myfolder
	//queryResult = Myfolder->CreateFolderQuery();
	//queryResult->ContentsChanged += ref new Windows::Foundation::TypedEventHandler<Windows::Storage::Search::IStorageQueryResultBase ^, Platform::Object ^>(this, &StorageClient::StorageClientAPP::OnLocalAppDataChanged);
	//queryResult->ContentsChanged += ref new Windows::Foundation::TypedEventHandler<Windows::Storage::Search::IStorageQueryResultBase ^, Platform::Object ^>(this, &StorageClient::StorageClientAPP::OnLocalAppDataChanged);

	create_task(Myfolder->GetFoldersAsync()).then([this](IVectorView<StorageFolder^>^ folders)
	{

		create_task(Myfolder->GetFilesAsync()).then([this, folders](IVectorView<StorageFile^>^ files)
		{
			auto count = folders->Size + files->Size;
			String^ outputtext = ref new String();
			outputtext = Myfolder->Name + " (" + count.ToString() + ")\n\n";

			std::for_each(begin(folders), end(folders), [this, &outputtext](StorageFolder^ folder)
			{
				outputtext += "    " + folder->DisplayName + "\\\n";
				//per ogni cartella che trova
				RowDefinition^ row = ref new RowDefinition();
				row->Height = 40;
				//rimuovo il vecchio elemento in quella riga
				//int size = WriteGrid->RowDefinitions->Size;
				//if(size>0)WriteGrid->RowDefinitions->RemoveAt(size-1);
				//aggiungo il nuovo elemento
				WriteGrid->RowDefinitions->Append(row);
				int i = WriteGrid->RowDefinitions->Size;
				Button^ tb = ref new Button();
				tb->Content = folder->Name + "/";
				tb->Tag = folder->Path;

				FolderNow[folder->Path] = folder;
				tb->Click += ref new Windows::UI::Xaml::RoutedEventHandler(this, &StorageClient::StorageClientAPP::Button_Open_Folder);

				StackPanel^ sp = ref new StackPanel();
				sp->Children->Clear();
				sp->SetValue(WriteGrid->RowProperty, i - 1);

				sp->Children->Append(tb);

				WriteGrid->Children->Append(sp);
				/////
			});
			std::for_each(begin(files), end(files), [this, &outputtext](StorageFile^ file)
			{
				outputtext += "    " + file->Name + "\n";
				//aggiungo il bottone per il file
				RowDefinition^ row = ref new RowDefinition();
				row->Height = 40;
				//rimuovo il vecchio elemento in quella riga
				//int size = WriteGrid->RowDefinitions->Size;
				//if(size>0)WriteGrid->RowDefinitions->RemoveAt(size-1);
				//aggiungo il nuovo elemento
				WriteGrid->RowDefinitions->Append(row);
				int i = WriteGrid->RowDefinitions->Size;

				Button^ tb = ref new Button();
				tb->Content = file->Name;
				tb->Tag = file->Path;

				FileNow[file->Path] = file;
				tb->Click += ref new Windows::UI::Xaml::RoutedEventHandler(this, &StorageClient::StorageClientAPP::Button_Open_File);

				StackPanel^ sp = ref new StackPanel();
				sp->Children->Clear();
				sp->SetValue(WriteGrid->RowProperty, i - 1);

				sp->Children->Append(tb);

				WriteGrid->Children->Append(sp);
				FileProperties::BasicProperties^ prop; 
				

				create_task(file->GetBasicPropertiesAsync()).then([this, &outputtext](FileProperties::BasicProperties^ p){
					//Messages->Text = "size: " + p->Size.ToString() + " last mod: " + p->DateModified.UniversalTime;
					//p->SavePropertiesAsync();
					//p->DateModified.UniversalTime.
				});


			});
			Messages->Text = outputtext;
		});
	});
}

void StorageClient::StorageClientAPP::First_read_folder(Platform::Object^ sender, Platform::Object^ e){
	//TimeSpan t = dt->Interval;
	////String^ time = ref new String(t.ToString);
	//this->Messages->Text = Myfolder->Path + " dentro la funzione; interval: " + t.ToString() ;
	////mettere la gestione di un timer
	//dt->Stop();
	//dt->Start();
	
}

void StorageClient::StorageClientAPP::OnLocalAppDataChanged(Windows::Storage::Search::IStorageQueryResultBase^ sender, Platform::Object^ args)
{
	Messages->Text = "Modificata la cartella: "+ Myfolder->Path;
}

void StorageClient::StorageClientAPP::Button_Click_1(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	WriteGrid->RowDefinitions->Clear();
	WriteGrid->Children->Clear();
	//per gestire le versioni
	Messages->Text = "Available Versions";


	int i;
	for (i = 0; i < 10; i++){

		RowDefinition^ row = ref new RowDefinition();
		row->Height = 40;

		WriteGrid->RowDefinitions->Append(row);

		//sp->Margin = 0, 10+i*40, 0, 0;
		//TextBox^ tb = ref new TextBox();
		//tb->Text = "prova " + i ;
		//tb->Height = 40;
		Button^ tb = ref new Button();
		tb->Content = "Version " + i;

		StackPanel^ sp = ref new StackPanel();
		sp->Children->Clear();

		sp->SetValue(WriteGrid->RowProperty, i);

		sp->Children->Append(tb);

		//sp->Children->InsertAt(i, tb);
		WriteGrid->Children->Append(sp);
		//WriteGrid->Children->InsertAt();
	}
}

void StorageClient::StorageClientAPP::Button_Open_File(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e){

	//provo ad aprire il file
	Button^ _button = (Button^)sender;
	Messages->Text = "file: " + _button->Tag;
	//messaggio di errore
	String^ path = (String^)_button->Tag;

	StorageFile^ file = FileNow[path];
	auto attr = file->Properties;
	//Windows::Storage::FileProperties::BasicProperties^ prop;
	//prop->DateModified;
	Messages->Text = "file: " + _button->Tag;
		//+ " prop:" +prop->DateModified + " size:" + prop->Size;
	
		//Uri^ uri = (Uri^)_button->Tag;
	//auto uri = ref new Windows::Foundation::Uri(path);
	Windows::System::Launcher::LaunchFileAsync(file);
	//Windows::System::Launcher::LaunchUriAsync(uri);
}

void StorageClient::StorageClientAPP::Button_Open_Folder(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e){

	//provo ad aprire il file
	Button^ _button = (Button^)sender;
	Messages->Text = "folder: " + _button->Tag + "/";
	//messaggio di errore
	String^ path = (String^)_button->Tag;

	StorageFolder^ folder = FolderNow[path];

	auto uri = ref new Windows::Foundation::Uri(path);

	Windows::System::Launcher::LaunchUriAsync(uri);
}
