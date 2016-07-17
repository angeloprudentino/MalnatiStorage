//
// LogIn.xaml.cpp
// Implementazione della classe LogIn
//

#include "pch.h"
#include "LogIn.xaml.h"
#include "StorageClientAPP.xaml.h"
#include "ClientMain.h"


using namespace StorageClient;

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
using namespace Windows::UI::Popups;

// Il modello di elemento per la pagina base è documentato all'indirizzo http://go.microsoft.com/fwlink/?LinkId=234237

LogIn::LogIn()
{
	InitializeComponent();
	SetValue(_defaultViewModelProperty, ref new Map<String^,Object^>(std::less<String^>()));
	auto navigationHelper = ref new Common::NavigationHelper(this);
	SetValue(_navigationHelperProperty, navigationHelper);
	navigationHelper->LoadState += ref new Common::LoadStateEventHandler(this, &LogIn::LoadState);
	navigationHelper->SaveState += ref new Common::SaveStateEventHandler(this, &LogIn::SaveState);
}

DependencyProperty^ LogIn::_defaultViewModelProperty =
	DependencyProperty::Register("DefaultViewModel",
		TypeName(IObservableMap<String^,Object^>::typeid), TypeName(LogIn::typeid), nullptr);

/// <summary>
/// utilizzata come semplice modello di visualizzazione.
/// </summary>
IObservableMap<String^, Object^>^ LogIn::DefaultViewModel::get()
{
	return safe_cast<IObservableMap<String^, Object^>^>(GetValue(_defaultViewModelProperty));
}

DependencyProperty^ LogIn::_navigationHelperProperty =
	DependencyProperty::Register("NavigationHelper",
		TypeName(Common::NavigationHelper::typeid), TypeName(LogIn::typeid), nullptr);

/// <summary>
/// Ottiene un'implementazione di <see cref="NavigationHelper"/> progettata per essere
/// utilizzata come semplice modello di visualizzazione.
/// </summary>
Common::NavigationHelper^ LogIn::NavigationHelper::get()
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

void LogIn::OnNavigatedTo(NavigationEventArgs^ e)
{
	NavigationHelper->OnNavigatedTo(e);
}

void LogIn::OnNavigatedFrom(NavigationEventArgs^ e)
{
	NavigationHelper->OnNavigatedFrom(e);
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
void LogIn::LoadState(Object^ sender, Common::LoadStateEventArgs^ e)
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
void LogIn::SaveState(Object^ sender, Common::SaveStateEventArgs^ e){
	(void) sender;	// Parametro non utilizzato
	(void) e; // Parametro non utilizzato
}


void StorageClient::LogIn::LogInButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	String^ user = this->User->Text;
	//Platform::String^ password = this->Pass->Text;

	ClientMain* c = ClientMain::getInstance();

	String^ pass = this->passwordbox->Password;
	String^ prova_pass = "Pippo";

	//bisogna convertire in string
	//c->PasswordCheck(user,pass);

	int res=String::CompareOrdinal(prova_pass,pass);
	if (res != 0){
		//messaggio di errore
		// Create the message dialog and set its content and title
		auto messageDialog = ref new MessageDialog("Username or Password not correct, try again", "Invalid Credentials");
		// Add commands and set their callbacks
		messageDialog->Commands->Append(ref new UICommand("Try Again", ref new UICommandInvokedHandler([this](IUICommand^ command)
		{
			//rootPage->NotifyUser("The 'Don't install' command has been selected.", NotifyType::StatusMessage);
		})));

		// Set the command that will be invoked by default
		messageDialog->DefaultCommandIndex = 1;

		// Show the message dialog
		messageDialog->ShowAsync();
	
	}
	else{
		//user e password corretti, vai a schermata applicazione
		this->Frame->Navigate(Windows::UI::Xaml::Interop::TypeName(StorageClientAPP::typeid));;
	}

}
