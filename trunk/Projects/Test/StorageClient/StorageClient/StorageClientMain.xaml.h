//
// StorageClient.xaml.h
// Dichiarazione della classe StorageClient
//

#pragma once

#include "StorageClient.g.h"
#include "Common\NavigationHelper.h"

namespace StorageClient
{
	/// <summary>
	/// Pagina in cui è visualizzato un titolo gruppo, un elenco di elementi all'interno del gruppo e i dettagli relativi
	/// all'elemento correntemente selezionato.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class StorageClient sealed
	{
	public:
		StorageClient();

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
		void SaveState(Object^ sender, Common::SaveStateEventArgs^ e);
		bool CanGoBack();
		void GoBack();

#pragma region Logical page navigation

		// La pagina divisa viene disegnata in modo che quando la finestra dispone dello spazio sufficiente per visualizzare
		// l'elenco e i dettagli, viene visualizzato un solo riquadro per volta.
		//
		// Ciò è interamente implementato mediante una singola pagina fisica che può rappresentare due pagine
		// logiche.  Nel codice seguente viene raggiunto questo obiettivo senza che l'utente si renda conto della
		// distinzione.

		void Window_SizeChanged(Platform::Object^ sender, Windows::UI::Core::WindowSizeChangedEventArgs^ e);
		void ItemListView_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);
		bool UsingLogicalPageNavigation();
		void InvalidateVisualState();
		Platform::String^ DetermineVisualState();

#pragma endregion

		static Windows::UI::Xaml::DependencyProperty^ _defaultViewModelProperty;
		static Windows::UI::Xaml::DependencyProperty^ _navigationHelperProperty;
		static const int MinimumWidthForSupportingTwoPanes = 768;
	};
}
