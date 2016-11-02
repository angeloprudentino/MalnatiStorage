//
// NavigationHelper.h
// Dichiarazione di NavigationHelper e classi associate
//

#pragma once

#include "RelayCommand.h"

namespace StorageClient
{
	namespace Common
	{
		/// <summary>
		/// Classe utilizzata per memorizzare i dati degli eventi necessari quando una pagina tenta di caricare uno stato.
		/// </summary>
		public ref class LoadStateEventArgs sealed
		{
		public:

			/// <summary>
			/// Valore del parametro passato a <see cref="Frame->Navigate(Type, Object)"/> 
			/// quando la pagina è stata inizialmente richiesta.
			/// </summary>
			property Platform::Object^ NavigationParameter
			{
				Platform::Object^ get();
			}

			/// <summary>
			/// Dizionario di stato mantenuto da questa pagina nel corso di una sessione
			/// precedente.  Il valore è null la prima volta che viene visitata una pagina.
			/// </summary>
			property Windows::Foundation::Collections::IMap<Platform::String^, Platform::Object^>^ PageState
			{
				Windows::Foundation::Collections::IMap<Platform::String^, Platform::Object^>^ get();
			}

		internal:
			LoadStateEventArgs(Platform::Object^ navigationParameter, Windows::Foundation::Collections::IMap<Platform::String^, Platform::Object^>^ pageState);

		private:
			Platform::Object^ _navigationParameter;
			Windows::Foundation::Collections::IMap<Platform::String^, Platform::Object^>^ _pageState;
		};

		/// <summary>
		/// Rappresenta il metodo che gestirà l'evento <see cref="NavigationHelper->LoadState"/>
		/// </summary>
		public delegate void LoadStateEventHandler(Platform::Object^ sender, LoadStateEventArgs^ e);

		/// <summary>
		/// Classe utilizzata per memorizzare i dati degli eventi necessari quando una pagina tenta di salvare uno stato.
		/// </summary>
		public ref class SaveStateEventArgs sealed
		{
		public:

			/// <summary>
			/// Dizionario vuoto da popolare con uno stato serializzabile.
			/// </summary>
			property Windows::Foundation::Collections::IMap<Platform::String^, Platform::Object^>^ PageState
			{
				Windows::Foundation::Collections::IMap<Platform::String^, Platform::Object^>^ get();
			}

		internal:
			SaveStateEventArgs(Windows::Foundation::Collections::IMap<Platform::String^, Platform::Object^>^ pageState);

		private:
			Windows::Foundation::Collections::IMap<Platform::String^, Platform::Object^>^ _pageState;
		};

		/// <summary>
		/// Rappresenta il metodo che gestirà l'evento <see cref="NavigationHelper->SaveState"/>
		/// </summary>
		public delegate void SaveStateEventHandler(Platform::Object^ sender, SaveStateEventArgs^ e);

		/// <summary>
		/// NavigationHelper favorisce la navigazione tra le pagine.  Fornisce i comandi utilizzati per 
		/// spostarsi avanti e indietro nonché esegue la registrazione delle scelte rapide standard da tastiera e con il mouse 
		/// tasti di scelta rapida utilizzati per andare avanti e indietro in Windows e pulsante Indietro dell'hardware in
		/// Windows Phone. Integra inoltre SuspensionManger per utilizzare la gestione della durata
		/// e dello stato dei processi quando si naviga tra le pagine.
		/// </summary>
		/// <example>
		/// Per utilizzare NavigationHelper, seguire i due passaggi seguenti o
		/// iniziare con un elemento BasicPage o un altro modello di elemento per la pagina diverso da BlankPage.
		/// 
		/// 1) Creare un'istanza di NavigationHelper in qualsiasi punto, ad esempio nel 
		///		costruttore della pagina e registrare un callback per gli eventi LoadState e 
		///		SaveState.
		/// <code>
		///		MyPage::MyPage()
		///		{
		///			InitializeComponent();
		///			auto navigationHelper = ref new Common::NavigationHelper(this);
		///			navigationHelper->LoadState += ref new Common::LoadStateEventHandler(this, &MyPage::LoadState);
		///			navigationHelper->SaveState += ref new Common::SaveStateEventHandler(this, &MyPage::SaveState);
		///		}
		///		
		///		void MyPage::LoadState(Object^ sender, Common::LoadStateEventArgs^ e)
		///		{ }
		///		void MyPage::SaveState(Object^ sender, Common::SaveStateEventArgs^ e)
		///		{ }
		/// </code>
		/// 
		/// 2) Registrare la pagina per chiamare un elemento di NavigationHelper ogni volta che la pagina partecipa 
		///		alla navigazione eseguendo l'override degli eventi <see cref="Windows::UI::Xaml::Controls::Page::OnNavigatedTo"/> 
		///		e <see cref="Windows::UI::Xaml::Controls::Page::OnNavigatedFrom"/> events.
		/// <code>
		///		void MyPage::OnNavigatedTo(NavigationEventArgs^ e)
		///		{
		///			NavigationHelper->OnNavigatedTo(e);
		///		}
		///
		///		void MyPage::OnNavigatedFrom(NavigationEventArgs^ e)
		///		{
		///			NavigationHelper->OnNavigatedFrom(e);
		///		}
		/// </code>
		/// </example>
		[Windows::Foundation::Metadata::WebHostHidden]
		[Windows::UI::Xaml::Data::Bindable]
		public ref class NavigationHelper sealed
		{
		public:
			/// <summary>
			/// <see cref="RelayCommand"/> utilizzato per l'associazione alla proprietà Comando del pulsante Indietro
			/// per spostarsi all'elemento più recente nella cronologia di navigazione all'indietro, se un frame
			/// gestisce la propria cronologia di navigazione.
			/// 
			/// <see cref="RelayCommand"/> viene configurato per utilizzare il metodo virtuale <see cref="GoBack"/>
			/// come l'azione Esegui e <see cref="CanGoBack"/> per CanExecute.
			/// </summary>
			property RelayCommand^ GoBackCommand
			{
				RelayCommand^ get();
			}

			/// <summary>
			/// <see cref="RelayCommand"/> utilizzato per spostarsi all'elemento più recente nella 
			/// cronologia di navigazione in avanti, se un frame gestisce la propria cronologia di navigazione.
			/// 
			/// <see cref="RelayCommand"/> viene configurato per utilizzare il metodo virtuale <see cref="GoForward"/>
			/// come l'azione Esegui e <see cref="CanGoForward"/> per CanExecute.
			/// </summary>
			property RelayCommand^ GoForwardCommand
			{
				RelayCommand^ get();
			}

		internal:
			NavigationHelper(Windows::UI::Xaml::Controls::Page^ page,
				RelayCommand^ goBack = nullptr,
				RelayCommand^ goForward = nullptr);

			bool CanGoBack();
			void GoBack();
			bool CanGoForward();
			void GoForward();

			void OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e);
			void OnNavigatedFrom(Windows::UI::Xaml::Navigation::NavigationEventArgs^ e);

			event LoadStateEventHandler^ LoadState;
			event SaveStateEventHandler^ SaveState;

		private:
			Platform::WeakReference _page;

			RelayCommand^ _goBackCommand;
			RelayCommand^ _goForwardCommand;

#if WINAPI_FAMILY==WINAPI_FAMILY_PHONE_APP
			Windows::Foundation::EventRegistrationToken _backPressedEventToken;

			void HardwareButton_BackPressed(Platform::Object^ sender,
				Windows::Phone::UI::Input::BackPressedEventArgs^ e);
#else
			bool _navigationShortcutsRegistered;
			Windows::Foundation::EventRegistrationToken _acceleratorKeyEventToken;
			Windows::Foundation::EventRegistrationToken _pointerPressedEventToken;

			void CoreDispatcher_AcceleratorKeyActivated(Windows::UI::Core::CoreDispatcher^ sender,
				Windows::UI::Core::AcceleratorKeyEventArgs^ e);
			void CoreWindow_PointerPressed(Windows::UI::Core::CoreWindow^ sender,
				Windows::UI::Core::PointerEventArgs^ e);
#endif

			Platform::String^ _pageKey;
			Windows::Foundation::EventRegistrationToken _loadedEventToken;
			Windows::Foundation::EventRegistrationToken _unloadedEventToken;
			void OnLoaded(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
			void OnUnloaded(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);

			~NavigationHelper();
		};
	}
}
