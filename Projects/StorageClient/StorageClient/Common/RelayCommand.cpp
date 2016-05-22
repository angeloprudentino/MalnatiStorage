//
// RelayCommand.cpp
// Implementazione di RelayCommand e classi associate
//

#include "pch.h"
#include "RelayCommand.h"
#include "NavigationHelper.h"

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
using namespace Windows::UI::Xaml::Navigation;

/// <summary>
/// Determina se <see cref="RelayCommand"/> può essere eseguito nello stato corrente.
/// </summary>
/// <param name="parametro">
/// Dati utilizzati dal comando. Se il comando non richiede il passaggio di dati, è possibile impostare questo oggetto su null.
/// </param>
/// <returns>true se il comando può essere eseguito; in caso contrario, false.</returns>
bool RelayCommand::CanExecute(Object^ parameter)
{
	return (_canExecuteCallback) (parameter);
}

/// <summary>
/// Esegue <see cref="RelayCommand"/> sulla destinazione del comando corrente.
/// </summary>
/// <param name="parametro">
/// Dati utilizzati dal comando. Se il comando non richiede il passaggio di dati, è possibile impostare questo oggetto su null.
/// </param>
void RelayCommand::Execute(Object^ parameter)
{
	(_executeCallback) (parameter);
}

/// <summary>
/// Il metodo utilizzato per generare l'evento <see cref="CanExecuteChanged"/>
/// per indicare che il valore restituito di <see cref="CanExecute"/>
/// il metodo è cambiato.
/// </summary>
void RelayCommand::RaiseCanExecuteChanged()
{
	CanExecuteChanged(this, nullptr);
}

/// <summary>
/// Distruttore della classe RelayCommand.
/// </summary>
RelayCommand::~RelayCommand()
{
	_canExecuteCallback = nullptr;
	_executeCallback = nullptr;
};

/// <summary>
/// Crea un nuovo comando che può essere sempre eseguito.
/// </summary>
/// <param name="canExecuteCallback">La logica dello stato di esecuzione.</param>
/// <param name="executeCallback">La logica di esecuzione.</param>
RelayCommand::RelayCommand(std::function<bool(Platform::Object^)> canExecuteCallback,
	std::function<void(Platform::Object^)> executeCallback) :
	_canExecuteCallback(canExecuteCallback),
	_executeCallback(executeCallback)
	{
	}
