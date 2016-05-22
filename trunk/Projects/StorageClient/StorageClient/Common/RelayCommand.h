//
// NavigationHelper.cpp
// Dichiarazione di NavigationHelper e classi associate
//

#pragma once

// <summary>
// Un comando con l'unico scopo di affidarsi alla propria funzionalità 
// ad altri oggetti richiamando i delegati. 
// Il valore restituito predefinito per il metodo CanExecute è 'true'.
// <see cref="RaiseCanExecuteChanged"/> deve essere chiamato ogni volta che
// <see cref="CanExecute"/> è previsto che venga restituito un valore differente.
// </summary>


namespace StorageClient
{
	namespace Common
	{
		[Windows::Foundation::Metadata::WebHostHidden]
		public ref class RelayCommand sealed :[Windows::Foundation::Metadata::Default] Windows::UI::Xaml::Input::ICommand
		{
		public:
			virtual event Windows::Foundation::EventHandler<Object^>^ CanExecuteChanged;
			virtual bool CanExecute(Object^ parameter);
			virtual void Execute(Object^ parameter);
			virtual ~RelayCommand();

		internal:
			RelayCommand(std::function<bool(Platform::Object^)> canExecuteCallback,
				std::function<void(Platform::Object^)> executeCallback);
			void RaiseCanExecuteChanged();

		private:
			std::function<bool(Platform::Object^)> _canExecuteCallback;
			std::function<void(Platform::Object^)> _executeCallback;
		};
	}
}