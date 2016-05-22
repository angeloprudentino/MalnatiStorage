//
// Register.xaml.cpp
// Implementazione della classe Register
//

#include "pch.h"
#include "Register.xaml.h"
#include "Windows.h"
#include <stdio.h>
#include <iostream>
#include <string>
#include <string.h>
#include <time.h>
#include <ctime>

using namespace StorageClient;

using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Graphics::Display;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Security::Credentials::UI;
using namespace concurrency;
using namespace Windows::Data;


using namespace std;






// Il modello di elemento per la pagina base è documentato all'indirizzo http://go.microsoft.com/fwlink/?LinkId=234237

Register::Register()
{
	InitializeComponent();
	SetValue(_defaultViewModelProperty, ref new Map<String^,Object^>(std::less<String^>()));
	auto navigationHelper = ref new Common::NavigationHelper(this);
	SetValue(_navigationHelperProperty, navigationHelper);
	navigationHelper->LoadState += ref new Common::LoadStateEventHandler(this, &Register::LoadState);
	navigationHelper->SaveState += ref new Common::SaveStateEventHandler(this, &Register::SaveState);
}

DependencyProperty^ Register::_defaultViewModelProperty =
	DependencyProperty::Register("DefaultViewModel",
		TypeName(IObservableMap<String^,Object^>::typeid), TypeName(Register::typeid), nullptr);

/// <summary>
/// utilizzata come semplice modello di visualizzazione.
/// </summary>
IObservableMap<String^, Object^>^ Register::DefaultViewModel::get()
{
	return safe_cast<IObservableMap<String^, Object^>^>(GetValue(_defaultViewModelProperty));
}

DependencyProperty^ Register::_navigationHelperProperty =
	DependencyProperty::Register("NavigationHelper",
		TypeName(Common::NavigationHelper::typeid), TypeName(Register::typeid), nullptr);

/// <summary>
/// Ottiene un'implementazione di <see cref="NavigationHelper"/> progettata per essere
/// utilizzata come semplice modello di visualizzazione.
/// </summary>
Common::NavigationHelper^ Register::NavigationHelper::get()
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

void Register::OnNavigatedTo(NavigationEventArgs^ e)
{
	NavigationHelper->OnNavigatedTo(e);
}

void Register::OnNavigatedFrom(NavigationEventArgs^ e)
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
void Register::LoadState(Object^ sender, Common::LoadStateEventArgs^ e)
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
void Register::SaveState(Object^ sender, Common::SaveStateEventArgs^ e){
	(void) sender;	// Parametro non utilizzato
	(void) e; // Parametro non utilizzato
}

void Register::SetResult(CredentialPickerResults^ result)
{
	auto domainName = result->CredentialDomainName;
	auto userName = result->CredentialUserName;
	auto password = result->CredentialPassword;
	auto savedByApi = result->CredentialSaved;
	auto saveOption = result->CredentialSaveOption;

	
	//Page^ outputFrame = (Page^)this->rootPage->OutputFrame->Content;
	//TextBox^ status = (TextBox^)(outputFrame->FindName("Status"));
	//status->Text = "OK";
	//TextBox^ domain = (TextBox^)outputFrame->FindName("Domain");
	//domain->Text = domainName;
	//TextBox^ username = (TextBox^)outputFrame->FindName("Username");
	//username->Text = userName;
	//TextBox^ passwordBox = (TextBox^)outputFrame->FindName("Password");
	//passwordBox->Text = password;
	//TextBox^ credsaved = (TextBox^)outputFrame->FindName("CredentialSaved");
	//credsaved->Text = (savedByApi ? "true" : "false");
	//TextBox^ checkboxState = (TextBox^)outputFrame->FindName("CheckboxState");
	//switch (result->CredentialSaveOption)
	//{
	//case Windows::Security::Credentials::UI::CredentialSaveOption::Hidden:
	//	checkboxState->Text = "Hidden";
	//	break;
	//case Windows::Security::Credentials::UI::CredentialSaveOption::Selected:
	//	checkboxState->Text = "Selected";
	//	break;
	//case Windows::Security::Credentials::UI::CredentialSaveOption::Unselected:
	//	checkboxState->Text = "Unselected";
	//	break;
	//}
}

void StorageClient::Register::Register_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	String^ user = this->User->Text;
	//Platform::String^ password = this->Pass->Text;
	
	String^ pass = this->Pass->Text;
	this->Prova->Text = pass;
	String^ prova_pass = "Pippo";
	//int res=String::CompareOrdinal(prova_pass,pass);

}





void StorageClient::Register::backButton_Copy_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{

	//va a cercare la cartella

}
