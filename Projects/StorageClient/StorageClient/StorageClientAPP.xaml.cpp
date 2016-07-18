//
// StorageClientAPP.xaml.cpp
// Implementazione della classe StorageClientAPP
//

#include "pch.h"
#include "StorageClientAPP.xaml.h"
#include <boost\filesystem.hpp>
#include <iostream>
#include <string>

using namespace StorageClient;

using namespace boost::filesystem;

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




// Il modello di elemento per la pagina base è documentato all'indirizzo http://go.microsoft.com/fwlink/?LinkId=234237
String^ Mypath;
StorageFolder^ Myfolder;

StorageClientAPP::StorageClientAPP()
{
	InitializeComponent();
	SetValue(_defaultViewModelProperty, ref new Map<String^,Object^>(std::less<String^>()));
	auto navigationHelper = ref new Common::NavigationHelper(this);
	SetValue(_navigationHelperProperty, navigationHelper);
	navigationHelper->LoadState += ref new Common::LoadStateEventHandler(this, &StorageClientAPP::LoadState);
	navigationHelper->SaveState += ref new Common::SaveStateEventHandler(this, &StorageClientAPP::SaveState);
}

DependencyProperty^ StorageClientAPP::_defaultViewModelProperty =
	DependencyProperty::Register("DefaultViewModel",
		TypeName(IObservableMap<String^,Object^>::typeid), TypeName(StorageClientAPP::typeid), nullptr);

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
	this->Messages->Text = Mypath;
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
	(void) sender;	// Parametro non utilizzato
	(void) e;	// Parametro non utilizzato
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
	(void) sender;	// Parametro non utilizzato
	(void) e; // Parametro non utilizzato
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


	//try
	//{
	//	if (exists(p))    // does p actually exist?
	//	{
	//		if (is_regular_file(p)) {       // is p a regular file?   
	//			cout << p << " size is " << file_size(p) << '\n';
	//			//traformo p in String^
	//			p2 = p.wstring();
	//			pp = ref new String(p2.data());
	//			outputtext += "    " + pp + "\\\n";
	//		}
	//		else if (is_directory(p))      // is p a directory?
	//		{
	//			cout << p << " is a directory containing:\n";
	//			p2 = p.wstring();
	//			pp = ref new String(p2.data());
	//			outputtext += "Directory: " + pp + "\\\n";
	//			copy(directory_iterator(p), directory_iterator(), // directory_iterator::value_type
	//				ostream_iterator<directory_entry>(cout, "\n")); // is directory_entry, which is
	//			// converted to a path by the
	//			// path stream inserter
	//		}

	//		else
	//			cout << p << " exists, but is neither a regular file nor a directory\n";
	//	}
	//	else
	//		cout << p << " does not exist\n";
	//		p2 = p.wstring();
	//		pp = ref new String(p2.data());
	//		outputtext += pp + "does not exists \\\n";
	//}

	//catch (const filesystem_error& ex)
	//{
	//	cout << ex.what() << '\n'; 
	//	string str = ex.what();
	//	//convertire da string a wstring
	//	std::wstring wsTmp;
	//	wsTmp.assign(str.begin(),str.end());
	//	pp = ref new String(wsTmp.data());
	//	outputtext += pp;
	//	//pp = ref new String();

	//}

	//Messages->Text = outputtext;


	//LETTURA DA KNOWNFOLDER
	Messages->Text= "";
	//KnownFolders::PicturesLibrary
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
				//per ogni cartella che trova, gli faccio cercare anche i file

				/////
			});
			std::for_each(begin(files), end(files), [this, &outputtext](StorageFile^ file)
			{
				outputtext += "    " + file->Name + "\n";
			});
			Messages->Text = outputtext;
		});
	});




}

void StorageClient::StorageClientAPP::Button_Click_1(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	//per gestire le versioni
	Messages->Text = "Gestione Versioni";
	//tb->Margin = "59,10,0,0";
	int i = 0;
	for (i = 0; i < 10; i++){
		StackPanel^ sp = ref new StackPanel();
		sp->Margin = 0, 10 + i * 40, 0, 0;
		TextBox^ tb = ref new TextBox();
		tb->Text = "prova" + i;
		tb->Height = 40;
		sp->Children->Append(tb);
		WriteGrid->Children->Append(sp);
	}
}