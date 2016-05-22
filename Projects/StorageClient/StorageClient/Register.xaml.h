//
// Register.xaml.h
// Dichiarazione della classe Register
//

#pragma once

#include "Register.g.h"
#include "Common\NavigationHelper.h"
#include <stdio.h>
#include <iostream>
#include <string>
#include <string.h>
#include <time.h>
#include <ctime>



using namespace std;

namespace StorageClient
{
	/// <summary>
	/// Pagina base che fornisce caratteristiche comuni alla maggior parte delle applicazioni.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class Register sealed
	{
	public:
		Register();

		/// <summary>
		/// È possibile sostituirlo con un modello di visualizzazione fortemente tipizzato.
		/// </summary>
		property Windows::Foundation::Collections::IObservableMap<Platform::String^, Platform::Object^>^ DefaultViewModel
		{
			Windows::Foundation::Collections::IObservableMap<Platform::String^, Platform::Object^>^  get();
		}

		/// <summary>
		/// NavigationHelper viene utilizzato in oggi pagina per favorire la navigazione e 
		/// la gestione del ciclo di vita dei processi
		/// </summary>
		property Common::NavigationHelper^ NavigationHelper
		{
			Common::NavigationHelper^ get();
		}

	protected:
		virtual void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;
		virtual void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e) override;

	private:
		void LoadState(Platform::Object^ sender, Common::LoadStateEventArgs^ e);
		void SaveState(Platform::Object^ sender, Common::SaveStateEventArgs^ e);
		void SetResult(Windows::Security::Credentials::UI::CredentialPickerResults^ res);
		static Windows::UI::Xaml::DependencyProperty^ _defaultViewModelProperty;
		static Windows::UI::Xaml::DependencyProperty^ _navigationHelperProperty;
		void Register_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void backButton_Copy_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	};
}
