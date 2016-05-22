//
// App.xaml.h
// Dichiarazione della classe App.
//

#pragma once

#include "App.g.h"

namespace StorageClient
{
	/// <summary>
	/// Fornisci un comportamento specifico dell'applicazione in supplemento alla classe Application predefinita.
	/// </summary>
	ref class App sealed
	{
	protected:
		virtual void OnLaunched(Windows::ApplicationModel::Activation::LaunchActivatedEventArgs^ e) override;

	internal:
		App();

	private:
		void OnSuspending(Platform::Object^ sender, Windows::ApplicationModel::SuspendingEventArgs^ e);
		void OnNavigationFailed(Platform::Object ^sender, Windows::UI::Xaml::Navigation::NavigationFailedEventArgs ^e);
	};
}
