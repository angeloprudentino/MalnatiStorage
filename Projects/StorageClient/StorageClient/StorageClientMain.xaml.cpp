//
// StorageClient.xaml.cpp
// Implementazione della classe StorageClient
//

#include "pch.h"

using namespace StorageClientMain;
using namespace StorageClientMain::Common;

using namespace Platform;
using namespace Platform::Collections;
using namespace concurrency;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::ViewManagement;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

// Il modello di elemento per la pagina divisa è documentato all'indirizzo http://go.microsoft.com/fwlink/?LinkId=234234

StorageClient::StorageClient()
{
	InitializeComponent();
	SetValue(_defaultViewModelProperty, ref new Map<String^,Object^>(std::less<String^>()));
	auto navigationHelper = ref new Common::NavigationHelper(this,
		ref new Common::RelayCommand(
		[this](Object^) -> bool
	{
		return CanGoBack();
	},
		[this](Object^) -> void
	{
		GoBack();
	}
	)
		);
	SetValue(_navigationHelperProperty, navigationHelper);
	navigationHelper->LoadState += ref new Common::LoadStateEventHandler(this, &StorageClient::LoadState);
	navigationHelper->SaveState += ref new Common::SaveStateEventHandler(this, &StorageClient::SaveState);

	itemListView->SelectionChanged += ref new SelectionChangedEventHandler(this, &StorageClient::ItemListView_SelectionChanged);
	Window::Current->SizeChanged += ref new WindowSizeChangedEventHandler (this, &StorageClient::Window_SizeChanged);
	InvalidateVisualState();

}

DependencyProperty^ StorageClient::_defaultViewModelProperty =
	DependencyProperty::Register("DefaultViewModel",
		TypeName(IObservableMap<String^,Object^>::typeid), TypeName(StorageClient::typeid), nullptr);

/// <summary>
/// utilizzata come semplice modello di visualizzazione.
/// </summary>
IObservableMap<String^, Object^>^ StorageClient::DefaultViewModel::get()
{
	return safe_cast<IObservableMap<String^, Object^>^>(GetValue(_defaultViewModelProperty));
}

DependencyProperty^ StorageClient::_navigationHelperProperty =
	DependencyProperty::Register("NavigationHelper",
		TypeName(Common::NavigationHelper::typeid), TypeName(StorageClient::typeid), nullptr);

/// <summary>
/// Ottiene un'implementazione di <see cref="NavigationHelper"/> progettata per essere
/// utilizzata come semplice modello di visualizzazione.
/// </summary>
Common::NavigationHelper^ StorageClient::NavigationHelper::get()
{
	//	return _navigationHelper;
	return safe_cast<Common::NavigationHelper^>(GetValue(_navigationHelperProperty));
}

#pragma region Page state management

/// <summary>
/// Popola la pagina con il contenuto passato durante la navigazione.  Vengono inoltre forniti eventuali stati
/// salvati durante la ricreazione di una pagina in una sessione precedente.
/// </summary>
/// <param name="navigationParameter">Valore del parametro passato a
/// <see cref="Frame::Navigate(Type, Object)"/> quando la pagina è stata inizialmente richiesta.
/// </param>
/// <param name="pageState">Mappa di stato mantenuto da questa pagina nel corso di una sessione
/// precedente.  Il valore è null la prima volta che viene visitata una pagina.</param>
void StorageClient::LoadState(Platform::Object^ sender, Common::LoadStateEventArgs^ e)
{
	// TODO: impostare un gruppo associabile utilizzando DefaultViewModel->Inserire("Group", <valore>)
	// TODO: impostare una raccolta di elementi associabili utilizzando DefaultViewModel->Inserire("Items", <valore>)

	if (e->PageState == nullptr)
	{
		// Se si tratta di una nuova pagina, selezionare il primo elemento automaticamente, a meno che non sia in uso
		// la navigazione all'interno di pagine logiche (vedere l'istruzione #region più avanti relativa alla navigazione all'interno di pagine logiche.)
		if (!UsingLogicalPageNavigation() && itemsViewSource->View != nullptr)
		{
			itemsViewSource->View->MoveCurrentToFirst();
		}
	}
	else
	{
		// Ripristinare lo stato salvato in precedenza con questa pagina
		if (e->PageState->HasKey("SelectedItem") && itemsViewSource->View != nullptr)
		{
			// TODO: richiamare itemsViewSource->View->MoveCurrentTo() con l'elemento
			//       selezionato come specificato dal valore di pageState->Lookup("SelectedItem")
		}
	}
}

/// <summary>
/// Mantiene lo stato associato a questa pagina in caso di sospensione dell'applicazione o se la
/// viene scartata dalla cache di navigazione.  I valori devono essere conformi ai requisiti di
/// serializzazione di <see cref="SuspensionManager::SessionState"/>.
/// </summary>
/// <param name="sender">Origine dell'evento. In genere <see cref="NavigationHelper"/></param>
/// <param name="e">Dati di evento che forniscono un dizionario vuoto da popolare con
/// uno stato serializzabile.</param>
void StorageClient::SaveState(Platform::Object^ sender, Common::SaveStateEventArgs^ e)
{
	if (itemsViewSource->View != nullptr)
	{
		auto selectedItem = itemsViewSource->View->CurrentItem;
		// TODO: derivare un parametro di navigazione serializzabile e passarlo a
		//       pageState->Insert("SelectedItem", <valore>)
	}
}

#pragma endregion

#pragma region Logical page navigation

// La gestione dello stato di visualizzazione in genere rispecchia direttamente i quattro stati di visualizzazione dell'applicazione (Portrait e Landscape
// a schermo intero più le visualizzazioni Snapped e Filled). La pagina divisa viene disegnata in modo che
// gli stati di visualizzazione Snapped e Portrait dispongano ognuno di due sottostati distinti: viene visualizzato solo l'elenco
// di elementi oppure solo i dettagli ma non entrambi allo stesso tempo.
//
// Ciò è interamente implementato mediante una singola pagina fisica che può rappresentare due pagine logiche.
// Nel codice seguente viene raggiunto questo obiettivo senza che l'utente si renda conto della distinzione.

/// <summary>
/// Richiamato per determinare se la pagina deve funzionare come una singola pagina logica o come due pagine.
/// </summary>
/// <returns>True quando lo stato di visualizzazione corrente è Portrait o Snapped, false
/// in caso contrario.</returns>
bool StorageClient::CanGoBack()
{
	if (UsingLogicalPageNavigation() && itemListView->SelectedItem != nullptr)
	{
		return true;
	}
	else
	{
		return NavigationHelper->CanGoBack();
	}
}

void StorageClient::GoBack()
{
	if (UsingLogicalPageNavigation() && itemListView->SelectedItem != nullptr)
	{
		// Quando è attiva la navigazione all'interno di pagine logiche e vi è un elemento selezionato, vengono
		// visualizzati i dettagli di tale elemento.  La cancellazione della selezione comporterà il ritorno
		// all'elenco di elementi.  Dal punto di vista dell'utente, si tratta di un'operazione di navigazione logica
		// a ritroso.
		itemListView->SelectedItem = nullptr;
	}
	else
	{
		NavigationHelper->GoBack();
	}
}

/// <summary>
/// Richiamato con le modifiche alle dimensioni della finestra
/// </summary>
/// <param name="sender">Finestra corrente</param>
/// <param name="e">Dati dell'evento in cui vengono descritte le nuove dimensioni della finestra</param>
void StorageClient::Window_SizeChanged(Platform::Object^ sender, Windows::UI::Core::WindowSizeChangedEventArgs^ e)
{
	InvalidateVisualState();
}

/// <summary>
/// Richiamato quando un elemento all'interno dell'elenco è selezionato.
/// </summary>
/// <param name="sender"> GridView in cui viene visualizzato l'elemento selezionato.</param>
/// <param name="e">Dati dell'evento in cui è descritto in che modo è stata modificata la selezione.</param>
void StorageClient::ItemListView_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e)
{
	if (UsingLogicalPageNavigation())
	{
		InvalidateVisualState();
	}
}

/// <summary>
/// Richiamato per determinare se la pagina deve funzionare come una singola pagina logica o come due pagine.
/// </summary>
/// <returns>True se la finestra deve agire come una sola pagina logica, false
/// in caso contrario.</returns>
bool StorageClient::UsingLogicalPageNavigation()
{
	return Windows::UI::Xaml::Window::Current->Bounds.Width < MinimumWidthForSupportingTwoPanes;
}

void StorageClient::InvalidateVisualState()
{
	auto visualState = DetermineVisualState();
	Windows::UI::Xaml::VisualStateManager::GoToState(this, visualState, false);
	NavigationHelper->GoBackCommand->RaiseCanExecuteChanged();
}

/// <summary>
/// Richiamato per determinare lo stato di visualizzazione corrispondente a quello di
/// un'applicazione.
/// </summary>
/// <returns>Nome dello stato di visualizzazione desiderato.  È lo stesso nome utilizzato per lo
/// stato di visualizzazione, eccetto quando un elemento è selezionato nelle visualizzazioni Portrait e Snapped, nei cui casi
/// questa pagina logica aggiuntiva viene rappresentata aggiungendo un suffisso _Detail.</returns>
Platform::String^ StorageClient::DetermineVisualState()
{
	if (!UsingLogicalPageNavigation())
		return "PrimaryView";

	// Aggiorna lo stato abilitato del pulsante Indietro quando viene modificato lo stato di visualizzazione
	auto logicalPageBack = UsingLogicalPageNavigation() && itemListView->SelectedItem != nullptr;

	return logicalPageBack ? "SinglePane_Detail" : "SinglePane";
}

#pragma endregion

#pragma region Navigation support

/// I metodi forniti in questa sezione vengono utilizzati per consentire a
/// NavigationHelper di rispondere ai metodi di navigazione della pagina.
/// 
/// La logica specifica della pagina deve essere inserita nel gestore eventi per  
/// <see cref="NavigationHelper::LoadState"/>
/// e <see cref="NavigationHelper::SaveState"/>.
/// Il parametro di navigazione è disponibile nel metodo LoadState 
/// oltre allo stato della pagina conservato durante una sessione precedente.

void StorageClient::OnNavigatedTo(NavigationEventArgs^ e)
{
	NavigationHelper->OnNavigatedTo(e);
}

void StorageClient::OnNavigatedFrom(NavigationEventArgs^ e)
{
	NavigationHelper->OnNavigatedFrom(e);

}
#pragma endregion