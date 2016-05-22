//
// MainPage.xaml.cpp
// Implementazione della classe MainPage.
//

#include "pch.h"
#include "MainPage.xaml.h"
#include "LogIn.xaml.h"
#include "Register.xaml.h"

using namespace StorageClient;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Security::Credentials::UI;
using namespace concurrency;
using namespace std;

// Il modello di elemento per la pagina vuota è documentato all'indirizzo http://go.microsoft.com/fwlink/?LinkId=234238

MainPage::MainPage()
{
	InitializeComponent();
}


void StorageClient::MainPage::HyperlinkButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	this->Frame->Navigate(Windows::UI::Xaml::Interop::TypeName(LogIn::typeid));;
}


void StorageClient::MainPage::HyperlinkButton_Click_1(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	this->Frame->Navigate(Windows::UI::Xaml::Interop::TypeName(Register::typeid));;
}


void StorageClient::MainPage::Register_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	//if ((Message->Text == nullptr) || (Caption->Text == nullptr) || (Target->Text == nullptr))
	//{
	//	return;
	//}

	CredentialPickerOptions^ credPickerOptions = ref new CredentialPickerOptions();
	credPickerOptions->AlwaysDisplayDialog = true;
	credPickerOptions->Message = "insert username and password";
	credPickerOptions->Caption =  "StorageClient";
	credPickerOptions->TargetName = "target";
	//if (Protocol->SelectedItem == nullptr)
	//{
	//	credPickerOptions->AuthenticationProtocol = Windows::Security::Credentials::UI::AuthenticationProtocol::Negotiate;
	//}
	//else
	//{
	//	String^ protocolName = ((ComboBoxItem^)Protocol->SelectedItem)->Content->ToString();
	//	if (protocolName->Equals("Negotiate"))
	//	{
	//		credPickerOptions->AuthenticationProtocol = Windows::Security::Credentials::UI::AuthenticationProtocol::Negotiate;
	//	}
	//	else if (protocolName->Equals("NTLM"))
	//	{
	//		credPickerOptions->AuthenticationProtocol = Windows::Security::Credentials::UI::AuthenticationProtocol::Ntlm;
	//	}
	//	else if (protocolName->Equals("Kerberos"))
	//	{
	//		credPickerOptions->AuthenticationProtocol = Windows::Security::Credentials::UI::AuthenticationProtocol::Kerberos;
	//	}
	//	else if (protocolName->Equals("CredSsp"))
	//	{
	//		credPickerOptions->AuthenticationProtocol = Windows::Security::Credentials::UI::AuthenticationProtocol::CredSsp;
	//	}
	//	else if (protocolName->Equals("Basic"))
	//	{
	//		credPickerOptions->AuthenticationProtocol = Windows::Security::Credentials::UI::AuthenticationProtocol::Basic;
	//	}
	//	else if (protocolName->Equals("Digest"))
	//	{
	//		credPickerOptions->AuthenticationProtocol = Windows::Security::Credentials::UI::AuthenticationProtocol::Digest;
	//	}
	//	else if (protocolName->Equals("Custom"))
	//	{
	//		credPickerOptions->AuthenticationProtocol = Windows::Security::Credentials::UI::AuthenticationProtocol::Custom;
	//		credPickerOptions->CustomAuthenticationProtocol = CustomProtocol->Text;
	//	}
	//	else
	//	{
	//		rootPage->NotifyUser("Unknown Protocol", NotifyType::ErrorMessage);
	//	}
	//}
	//if (CheckboxState->SelectedItem != nullptr)
	//{
	//	String^ checkboxState = ((ComboBoxItem^)CheckboxState->SelectedItem)->Content->ToString();
	//	if (checkboxState->Equals("Hidden"))
	//	{
	//		credPickerOptions->CredentialSaveOption = Windows::Security::Credentials::UI::CredentialSaveOption::Hidden;
	//	}
	//	else if (checkboxState->Equals("Selected"))
	//	{
	//		credPickerOptions->CredentialSaveOption = Windows::Security::Credentials::UI::CredentialSaveOption::Selected;
	//	}
	//	else if (checkboxState->Equals("Unselected"))
	//	{
	//		credPickerOptions->CredentialSaveOption = Windows::Security::Credentials::UI::CredentialSaveOption::Unselected;
	//	}
	//	else
	//	{
	//		rootPage->NotifyUser("Unknown Checkbox state", NotifyType::ErrorMessage);
	//	}
	//}
	create_task(CredentialPicker::PickAsync(credPickerOptions))
		.then([this](CredentialPickerResults^ credPickerResult)
	{
		SetResultRegister(credPickerResult);
	});

}

void MainPage::SetResultRegister(CredentialPickerResults^ result)
{
	auto domainName = result->CredentialDomainName;
	auto userName = result->CredentialUserName;
	auto password = result->CredentialPassword;
	auto savedByApi = result->CredentialSaved;
	auto saveOption = result->CredentialSaveOption;
	Page^ outputFrame = (Page^)this->OutputFrame->Content;
	TextBox^ status = (TextBox^)outputFrame->FindName("Status");
	status->Text = "OK";
	TextBox^ domain = (TextBox^)outputFrame->FindName("Domain");
	domain->Text = domainName;
	TextBox^ username = (TextBox^)outputFrame->FindName("Username");
	username->Text = userName;
	TextBox^ passwordBox = (TextBox^)outputFrame->FindName("Password");
	passwordBox->Text = password;
	TextBox^ credsaved = (TextBox^)outputFrame->FindName("CredentialSaved");
	credsaved->Text = (savedByApi ? "true" : "false");
	TextBox^ checkboxState = (TextBox^)outputFrame->FindName("CheckboxState");
	switch (result->CredentialSaveOption)
	{
	case Windows::Security::Credentials::UI::CredentialSaveOption::Hidden:
		checkboxState->Text = "Hidden";
		break;
	case Windows::Security::Credentials::UI::CredentialSaveOption::Selected:
		checkboxState->Text = "Selected";
		break;
	case Windows::Security::Credentials::UI::CredentialSaveOption::Unselected:
		checkboxState->Text = "Unselected";
		break;
	}

}