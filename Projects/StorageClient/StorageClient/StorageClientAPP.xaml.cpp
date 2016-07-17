//
// StorageClientAPP.xaml.cpp
// Implementazione della classe StorageClientAPP
//

#include "pch.h"
#include "StorageClientAPP.xaml.h"
//#include <boost\filesystem.hpp>

using namespace StorageClient;

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




// Il modello di elemento per la pagina base è documentato all'indirizzo http://go.microsoft.com/fwlink/?LinkId=234237
String^ path;
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
	path = (String^)e->Parameter;
	NavigationHelper->OnNavigatedTo(e);
	this->Messages->Text = path;
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
	//PRIMA PROVA
	//	Messages->Text = "";
	//	StorageFolder^ folder;
	//	folder->GetFolderFromPathAsync(path);

	//	
	//		create_task(folder->GetFoldersAsync()).then([this](IVectorView<StorageFolder^>^ folders)
	//		{
	//			StorageFolder^ folder;
	//			folder->GetFolderFromPathAsync(path);

	//			create_task(folder->GetFilesAsync()).then([this, folders](IVectorView<StorageFile^>^ files)
	//			{
	//				//auto count = folders->Size + files->Size;
	//				String^ outputText = ref new String();
	//				//outputText = KnownFolders::PicturesLibrary->Name + " (" + count.ToString() + ")\n\n";
	//				StorageFolder^ folder;
	//				folder->GetFolderFromPathAsync(path);
	//				outputText = folder->Name;// +" (" + count.ToString() + ")\n\n";
	//				std::for_each(begin(folders), end(folders), [this, &outputText](StorageFolder^ folder)
	//				{
	//					outputText += "    " + folder->DisplayName + "\\\n";
	//				});
	//				std::for_each(begin(files), end(files), [this, &outputText](StorageFile^ file)
	//				{
	//					outputText += "    " + file->Name + "\n";
	//				});
	//				Messages->Text = outputText;
	//			});
	//		});
	//}

	////LETTURA DA KNOWNFOLDER
	//Messages->Text= "";

	//create_task(KnownFolders::PicturesLibrary->GetFoldersAsync()).then([this](IVectorView<StorageFolder^>^ folders)
	//{
	//	create_task(KnownFolders::PicturesLibrary->GetFilesAsync()).then([this, folders](IVectorView<StorageFile^>^ files)
	//	{
	//		auto count = folders->Size + files->Size;
	//		String^ outputtext = ref new String();
	//		outputtext = KnownFolders::PicturesLibrary->Name + " (" + count.ToString() + ")\n\n";
	//		std::for_each(begin(folders), end(folders), [this, &outputtext](StorageFolder^ folder)
	//		{
	//			outputtext += "    " + folder->DisplayName + "\\\n";
	//		});
	//		std::for_each(begin(files), end(files), [this, &outputtext](StorageFile^ file)
	//		{
	//			outputtext += "    " + file->Name + "\n";
	//		});
	//		Messages->Text = outputtext;
	//	});
	//});




}
