//
// App.xaml.cpp
// Implementazione della classe App.
//

#include "pch.h"
#include "MainPage.xaml.h"

using namespace StorageClient;

using namespace Platform;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Activation;
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

// Il modello di applicazione vuota è documentato all'indirizzo http://go.microsoft.com/fwlink/?LinkId=234227

/// <summary>
/// Inizializza l'oggetto Application singleton.  Si tratta della prima riga del codice creato
/// eseguita e, come tale, corrisponde all'equivalente logico di main() o WinMain().
/// </summary>
App::App()
{
	InitializeComponent();
	Suspending += ref new SuspendingEventHandler(this, &App::OnSuspending);
}

/// <summary>
/// Richiamato quando l'applicazione viene avviata normalmente dall'utente.	All'avvio dell'applicazione
/// verranno utilizzati altri punti di ingresso per aprire un file specifico.
/// </summary>
/// <param name="e">Dettagli sulla richiesta e il processo di avvio.</param>
void App::OnLaunched(Windows::ApplicationModel::Activation::LaunchActivatedEventArgs^ e)
{

#if _DEBUG
		// Visualizza informazioni di profilatura delle immagini durante il debug.
		if (IsDebuggerPresent())
		{
			// Visualizza i contatori della frequenza dei fotogrammi corrente
			 DebugSettings->EnableFrameRateCounter = true;
		}
#endif

	auto rootFrame = dynamic_cast<Frame^>(Window::Current->Content);

	// Non ripetere l'inizializzazione dell'applicazione se la finestra già dispone di contenuto,
	// assicurarsi solo che la finestra sia attiva
	if (rootFrame == nullptr)
	{
		// Crea un frame che agisca come menu di scelta rapida di navigazione e associalo con una
		// chiave SuspensionManager
		rootFrame = ref new Frame();

		// Imposta la lingua predefinita
		rootFrame->Language = Windows::Globalization::ApplicationLanguages::Languages->GetAt(0);

		rootFrame->NavigationFailed += ref new Windows::UI::Xaml::Navigation::NavigationFailedEventHandler(this, &App::OnNavigationFailed);

		if (e->PreviousExecutionState == ApplicationExecutionState::Terminated)
		{
			// TODO: ripristinare lo stato della sessione salvata solo se appropriato, programmando
			// i passaggi di avvio finali al termine del ripristino

		}

		if (rootFrame->Content == nullptr)
		{
			// Quando lo stack di navigazione non viene ripristinato, esegui la navigazione alla prima pagina,
			// configurando la nuova pagina per passare le informazioni richieste come parametro di
			// navigazione
			rootFrame->Navigate(TypeName(MainPage::typeid), e->Arguments);
		}
		// Posizionare il frame nella finestra corrente
		Window::Current->Content = rootFrame;
		// Assicurarsi che la finestra corrente sia attiva
		Window::Current->Activate();
	}
	else
	{
		if (rootFrame->Content == nullptr)
		{
			// Quando lo stack di navigazione non viene ripristinato, esegui la navigazione alla prima pagina,
			// configurando la nuova pagina per passare le informazioni richieste come parametro di
			// navigazione
			rootFrame->Navigate(TypeName(MainPage::typeid), e->Arguments);
		}
		// Assicurarsi che la finestra corrente sia attiva
		Window::Current->Activate();
	}
}

/// <summary>
/// Richiamato quando l'esecuzione dell'applicazione viene sospesa.	Lo stato dell'applicazione viene salvato
/// senza che sia noto se l'applicazione verrà terminata o ripresa con il contenuto
/// della memoria ancora integro.
/// </summary>
/// <param name="sender">Origine della richiesta di sospensione.</param>
/// <param name="e">Dettagli relativi alla richiesta di sospensione.</param>
void App::OnSuspending(Object^ sender, SuspendingEventArgs^ e)
{
	(void) sender;	// Parametro non utilizzato
	(void) e;	// Parametro non utilizzato

	//TODO: Salvare lo stato dell'applicazione e interrompere qualsiasi attività in background
}

/// <summary>
/// Chiamato quando la navigazione a una determinata pagina ha esito negativo
/// </summary>
/// <param name="sender">Frame la cui navigazione non è riuscita</param>
/// <param name="e">Dettagli sull'errore di navigazione.</param>
void App::OnNavigationFailed(Platform::Object ^sender, Windows::UI::Xaml::Navigation::NavigationFailedEventArgs ^e)
{
	throw ref new FailureException("Failed to load Page " + e->SourcePageType.Name);
}