//
// NavigationHelper.cpp
// Implementazione di NavigationHelper e classi associate
//

#include "pch.h"
#include "NavigationHelper.h"
#include "RelayCommand.h"
#include "SuspensionManager.h"

using namespace StorageClient::Common;

using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::System;
using namespace Windows::UI::Core;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::UI::Xaml::Navigation;

#if WINAPI_FAMILY==WINAPI_FAMILY_PHONE_APP
using namespace Windows::Phone::UI::Input;
#endif

/// <summary>
/// Inizializza una nuova istanza della classe <see cref="LoadStateEventArgs"/>.
/// </summary>
/// <param name="navigationParameter">
/// Valore del parametro passato a <see cref="Frame->Navigate(Type, Object)"/> 
/// quando la pagina è stata inizialmente richiesta.
/// </param>
/// <param name="pageState">
/// Dizionario di stato mantenuto da questa pagina nel corso di una sessione
/// precedente.  Il valore è null la prima volta che viene visitata una pagina.
/// </param>
LoadStateEventArgs::LoadStateEventArgs(Object^ navigationParameter, IMap<String^, Object^>^ pageState)
{
	_navigationParameter = navigationParameter;
	_pageState = pageState;
}

/// <summary>
/// Ottiene la proprietà <see cref="NavigationParameter"/> della classe <see cref"LoadStateEventArgs"/>.
/// </summary>
Object^ LoadStateEventArgs::NavigationParameter::get()
{
	return _navigationParameter;
}

/// <summary>
/// Ottiene la proprietà <see cref="PageState"/> della classe <see cref"LoadStateEventArgs"/>.
/// </summary>
IMap<String^, Object^>^ LoadStateEventArgs::PageState::get()
{
	return _pageState;
}

/// <summary>
/// Inizializza una nuova istanza della classe <see cref="SaveStateEventArgs"/>.
/// </summary>
/// <param name="pageState">Dizionario vuoto da popolare con uno stato serializzabile.</param>
SaveStateEventArgs::SaveStateEventArgs(IMap<String^, Object^>^ pageState)
{
	_pageState = pageState;
}

/// <summary>
/// Ottiene la proprietà <see cref="PageState"/> della classe <see cref"SaveStateEventArgs"/>.
/// </summary>
IMap<String^, Object^>^ SaveStateEventArgs::PageState::get()
{
	return _pageState;
}

/// <summary>
/// Inizializza una nuova istanza della classe <see cref="NavigationHelper"/>.
/// </summary>
/// <param name="page">Riferimento alla pagina corrente utilizzata per la navigazione.  
/// Questo riferimento consente la manipolazione dei frame e di verificare che 
/// le richieste di navigazione tramite tastiera si verifichino solo quando la pagina occupa l'intera finestra.</param>
NavigationHelper::NavigationHelper(Page^ page, RelayCommand^ goBack, RelayCommand^ goForward) :
_page(page),
_goBackCommand(goBack),
_goForwardCommand(goForward)
{
	// Quando questa pagina è parte della struttura ad albero visuale, effettua due modifiche:
	// 1) Esegui il mapping dello stato di visualizzazione dell'applicazione allo stato di visualizzazione per la pagina
	// 2) Gestire le richieste di navigazione dell'hardware
	_loadedEventToken = page->Loaded += ref new RoutedEventHandler(this, &NavigationHelper::OnLoaded);

	//// Annullare le stesse modifiche quando la pagina non è più visibile
	_unloadedEventToken = page->Unloaded += ref new RoutedEventHandler(this, &NavigationHelper::OnUnloaded);
}

NavigationHelper::~NavigationHelper()
{
	delete _goBackCommand;
	delete _goForwardCommand;

	_page = nullptr;
}

/// <summary>
/// Richiamato quando la pagina fa parte dell'albero visuale
/// </summary>
/// <param name="sender">Istanza che ha generato l'evento.</param>
/// <param name="e">Dati evento che descrivono le condizioni che hanno determinato l'evento.</param>
void NavigationHelper::OnLoaded(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
#if WINAPI_FAMILY==WINAPI_FAMILY_PHONE_APP
	_backPressedEventToken = HardwareButtons::BackPressed +=
		ref new EventHandler<BackPressedEventArgs^>(this,
		&NavigationHelper::HardwareButton_BackPressed);
#else
	Page ^page = _page.Resolve<Page>();

	// La navigazione mediante tastiera e mouse è applicabile solo quando la finestra viene occupata per intero
	if (page != nullptr &&
		page->ActualHeight == Window::Current->Bounds.Height &&
		page->ActualWidth == Window::Current->Bounds.Width)
	{
		// Ascolta la finestra direttamente, in modo che non ne sia richiesto lo stato attivo
		_acceleratorKeyEventToken = Window::Current->CoreWindow->Dispatcher->AcceleratorKeyActivated +=
			ref new TypedEventHandler<CoreDispatcher^, AcceleratorKeyEventArgs^>(this,
			&NavigationHelper::CoreDispatcher_AcceleratorKeyActivated);

		_pointerPressedEventToken = Window::Current->CoreWindow->PointerPressed +=
			ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this,
			&NavigationHelper::CoreWindow_PointerPressed);

		_navigationShortcutsRegistered = true;
	}
#endif
}

/// <summary>
/// Richiamato quando la pagina è rimossa dall'albero visuale
/// </summary>
/// <param name="sender">Istanza che ha generato l'evento.</param>
/// <param name="e">Dati evento che descrivono le condizioni che hanno determinato l'evento.</param>
void NavigationHelper::OnUnloaded(Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
#if WINAPI_FAMILY==WINAPI_FAMILY_PHONE_APP
	HardwareButtons::BackPressed -= _backPressedEventToken;
#else
	if (_navigationShortcutsRegistered)
	{
		Window::Current->CoreWindow->Dispatcher->AcceleratorKeyActivated -= _acceleratorKeyEventToken;
		Window::Current->CoreWindow->PointerPressed -= _pointerPressedEventToken;
		_navigationShortcutsRegistered = false;
	}
#endif

	// Rimuovere il gestore e rilasciare il riferimento alla pagina
	Page ^page = _page.Resolve<Page>();
	if (page != nullptr)
	{
		page->Loaded -= _loadedEventToken;
		page->Unloaded -= _unloadedEventToken;
		delete _goBackCommand;
		delete _goForwardCommand;
		_goForwardCommand = nullptr;
		_goBackCommand = nullptr;
	}
}

#pragma region Navigation support

/// <summary>
/// Metodo utilizzato dalla proprietà <see cref="GoBackCommand"/>
/// per determinare se <see cref="Frame"/> può spostarsi all'indietro.
/// </summary>
/// <returns>
/// true se <see cref="Frame"/> contiene almeno una voce 
/// nella cronologia di navigazione all'indietro.
/// </returns>
bool NavigationHelper::CanGoBack()
{
	Page ^page = _page.Resolve<Page>();
	if (page != nullptr)
	{
		auto frame = page->Frame;
		return (frame != nullptr && frame->CanGoBack);
	}

	return false;
}

/// <summary>
/// Metodo utilizzato dalla proprietà <see cref="GoBackCommand"/>
/// per richiamare il metodo <see cref="Windows::UI::Xaml::Controls::Frame::GoBack"/>.
/// </summary>
void NavigationHelper::GoBack()
{
	Page ^page = _page.Resolve<Page>();
	if (page != nullptr)
	{
		auto frame = page->Frame;
		if (frame != nullptr && frame->CanGoBack)
		{
			frame->GoBack();
		}
	}
}

/// <summary>
/// Metodo utilizzato dalla proprietà <see cref="GoForwardCommand"/>
/// per determinare se <see cref="Frame"/> può spostarsi in avanti.
/// </summary>
/// <returns>
/// true se <see cref="Frame"/> contiene almeno una voce 
/// nella cronologia di navigazione in avanti.
/// </returns>
bool NavigationHelper::CanGoForward()
{
	Page ^page = _page.Resolve<Page>();
	if (page != nullptr)
	{
		auto frame = page->Frame;
		return (frame != nullptr && frame->CanGoForward);
	}

	return false;
}

/// <summary>
/// Metodo utilizzato dalla proprietà <see cref="GoForwardCommand"/>
/// per richiamare il metodo <see cref="Windows::UI::Xaml::Controls::Frame::GoBack"/>.
/// </summary>
void NavigationHelper::GoForward()
{
	Page ^page = _page.Resolve<Page>();
	if (page != nullptr)
	{
		auto frame = page->Frame;
		if (frame != nullptr && frame->CanGoForward)
		{
			frame->GoForward();
		}
	}
}

/// <summary>
/// Ottiene la proprietà <see cref="GoBackCommand"/> della classe <see cref"NavigationHelper"/>.
/// </summary>
RelayCommand^ NavigationHelper::GoBackCommand::get()
{
	if (_goBackCommand == nullptr)
	{
		_goBackCommand = ref new RelayCommand(
			[this](Object^) -> bool
		{
			return CanGoBack();
		},
			[this](Object^) -> void
		{
			GoBack();
		}
		);
	}
	return _goBackCommand;
}

/// <summary>
/// Ottiene la proprietà <see cref="GoForwardCommand"/> della classe <see cref"NavigationHelper"/>.
/// </summary>
RelayCommand^ NavigationHelper::GoForwardCommand::get()
{
	if (_goForwardCommand == nullptr)
	{
		_goForwardCommand = ref new RelayCommand(
			[this](Object^) -> bool
		{
			return CanGoForward();
		},
			[this](Object^) -> void
		{
			GoForward();
		}
		);
	}
	return _goForwardCommand;
}

#if WINAPI_FAMILY==WINAPI_FAMILY_PHONE_APP
/// <summary>
/// Gestisce l'utilizzo del pulsante Indietro e naviga nella cronologia del frame radice.
/// </summary>
void NavigationHelper::HardwareButton_BackPressed(Object^ sender, BackPressedEventArgs^ e)
{
	if (this->GoBackCommand->CanExecute(nullptr))
	{
		e->Handled = true;
		this->GoBackCommand->Execute(nullptr);
	}
}
#else
/// <summary>
/// Richiamato per ciascuna sequenza di tasti, compresi i tasti di sistema quali combinazioni con il tasto ALT, quando
/// questa pagina è attiva e occupa l'intera finestra.  Utilizzato per il rilevamento della navigazione da tastiera
/// tra pagine, anche quando la pagina stessa no dispone dello stato attivo.
/// </summary>
/// <param name="sender">Istanza che ha generato l'evento.</param>
/// <param name="e">Dati evento che descrivono le condizioni che hanno determinato l'evento.</param>
void NavigationHelper::CoreDispatcher_AcceleratorKeyActivated(CoreDispatcher^ sender,
	AcceleratorKeyEventArgs^ e)
{
	sender; // Parametro non utilizzato
	auto virtualKey = e->VirtualKey;

	// Esegui ulteriori controlli solo se vengono premuti i tasti Freccia SINISTRA, Freccia DESTRA o i tasti dedicati Precedente
	// o successivo
	if ((e->EventType == CoreAcceleratorKeyEventType::SystemKeyDown ||
		e->EventType == CoreAcceleratorKeyEventType::KeyDown) &&
		(virtualKey == VirtualKey::Left || virtualKey == VirtualKey::Right ||
		virtualKey == VirtualKey::GoBack || virtualKey == VirtualKey::GoForward))
	{
		auto coreWindow = Window::Current->CoreWindow;
		auto downState = Windows::UI::Core::CoreVirtualKeyStates::Down;
		bool menuKey = (coreWindow->GetKeyState(VirtualKey::Menu) & downState) == downState;
		bool controlKey = (coreWindow->GetKeyState(VirtualKey::Control) & downState) == downState;
		bool shiftKey = (coreWindow->GetKeyState(VirtualKey::Shift) & downState) == downState;
		bool noModifiers = !menuKey && !controlKey && !shiftKey;
		bool onlyAlt = menuKey && !controlKey && !shiftKey;

		if ((virtualKey == VirtualKey::GoBack && noModifiers) ||
			(virtualKey == VirtualKey::Left && onlyAlt))
		{
			// Quando viene premuto il tasto Precedente o ALT+Freccia SINISTRA, torna indietro
			e->Handled = true;
			GoBackCommand->Execute(this);
		}
		else if ((virtualKey == VirtualKey::GoForward && noModifiers) ||
			(virtualKey == VirtualKey::Right && onlyAlt))
		{
			// Quando viene premuto il tasto Successivo o ALT+Freccia DESTRA, vai avanti
			e->Handled = true;
			GoForwardCommand->Execute(this);
		}
	}
}

/// <summary>
/// Richiamato per ciascun clic del mouse, tocco del touch screen o interazione equivalente quando la
/// pagina è attiva e occupa per intero la finestra.  Utilizzato per il rilevamento del clic del mouse sui pulsanti di tipo browser
/// Precedente e Successivo per navigare tra pagine.
/// </summary>
/// <param name="sender">Istanza che ha generato l'evento.</param>
/// <param name="e">Dati evento che descrivono le condizioni che hanno determinato l'evento.</param>
void NavigationHelper::CoreWindow_PointerPressed(CoreWindow^ sender, PointerEventArgs^ e)
{
	auto properties = e->CurrentPoint->Properties;

	// Ignora combinazioni di pulsanti con i pulsanti sinistro destro e centrale
	if (properties->IsLeftButtonPressed ||
		properties->IsRightButtonPressed ||
		properties->IsMiddleButtonPressed)
	{
		return;
	}

	// Se viene premuto Precedente o Successivo (ma non entrambi) naviga come appropriato
	bool backPressed = properties->IsXButton1Pressed;
	bool forwardPressed = properties->IsXButton2Pressed;
	if (backPressed ^ forwardPressed)
	{
		e->Handled = true;
		if (backPressed)
		{
			if (GoBackCommand->CanExecute(this))
			{
				GoBackCommand->Execute(this);
			}
		}
		else
		{
			if (GoForwardCommand->CanExecute(this))
			{
				GoForwardCommand->Execute(this);
			}
		}
	}
}
#endif

#pragma endregion

#pragma region Process lifetime management

/// <summary>
/// Richiamato quando la pagina sta per essere visualizzata in un Frame.
/// </summary>
/// <param name="e">Dati dell'evento in cui vengono descritte le modalità con cui la pagina è stata raggiunta.  La proprietà
/// Parameter fornisce il gruppo da visualizzare.</param>
void NavigationHelper::OnNavigatedTo(NavigationEventArgs^ e)
{
	Page ^page = _page.Resolve<Page>();
	if (page != nullptr)
	{
		auto frameState = SuspensionManager::SessionStateForFrame(page->Frame);
		_pageKey = "Page-" + page->Frame->BackStackDepth;

		if (e->NavigationMode == NavigationMode::New)
		{
			// Cancella lo stato esistente per la navigazione in avanti quando si aggiunge una nuova pagina allo
			// stack di navigazione
			auto nextPageKey = _pageKey;
			int nextPageIndex = page->Frame->BackStackDepth;
			while (frameState->HasKey(nextPageKey))
			{
				frameState->Remove(nextPageKey);
				nextPageIndex++;
				nextPageKey = "Page-" + nextPageIndex;
			}

			// Passa il parametro di navigazione alla nuova pagina
			LoadState(this, ref new LoadStateEventArgs(e->Parameter, nullptr));
		}
		else
		{
			// Passa il parametro di navigazione e lo stato della pagina mantenuto, utilizzando
			// la stessa strategia per caricare lo stato sospeso e ricreare le pagine scartate
			// dalla cache
			LoadState(this, ref new LoadStateEventArgs(e->Parameter, safe_cast<IMap<String^, Object^>^>(frameState->Lookup(_pageKey))));
		}
	}
}

/// <summary>
/// Richiamato quando questa pagina non verrà più visualizzata in un frame.
/// </summary>
/// <param name="e">Dati dell'evento in cui vengono descritte le modalità con cui la pagina è stata raggiunta.  La proprietà
/// Parameter fornisce il gruppo da visualizzare.</param>
void NavigationHelper::OnNavigatedFrom(NavigationEventArgs^ e)
{
	Page ^page = _page.Resolve<Page>();
	if (page != nullptr)
	{
		auto frameState = SuspensionManager::SessionStateForFrame(page->Frame);
		auto pageState = ref new Map<String^, Object^>();
		SaveState(this, ref new SaveStateEventArgs(pageState));
		frameState->Insert(_pageKey, pageState);
	}
}
#pragma endregion