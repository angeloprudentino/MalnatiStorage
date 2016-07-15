//
// StorageClientAPP.xaml.h
// Dichiarazione della classe StorageClientAPP
//

#pragma once

#include "StorageClientAPP.g.h"
#include "Common\NavigationHelper.h"

namespace StorageClient
{
	/// <summary>
	/// Pagina base che fornisce caratteristiche comuni alla maggior parte delle applicazioni.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class StorageClientAPP sealed
	{
	public:
		StorageClientAPP();

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

		static Windows::UI::Xaml::DependencyProperty^ _defaultViewModelProperty;
		static Windows::UI::Xaml::DependencyProperty^ _navigationHelperProperty;
		void Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	};
}
