//
// SuspensionManager.h
// Dichiarazione della classe SuspensionManager
//

#pragma once

namespace StorageClient
{
	namespace Common
	{
		/// <summary>
		/// SuspensionManager acquisisce lo stato sessione globale per semplificare la gestione del ciclo di vita dei processi
		/// per un'applicazione.  Si noti che lo stato sessione verrà cancellato automaticamente in diverse
		/// condizioni e deve essere utilizzato solo per archiviare le informazioni utili da
		/// conservare tra le sessioni, ma che deve essere eliminato in caso di arresto anomale dell'applicazione o di
		/// aggiornamento.
		/// </summary>
		class SuspensionManager sealed
		{
		public:
			static void RegisterFrame(Windows::UI::Xaml::Controls::Frame^ frame, Platform::String^ sessionStateKey, Platform::String^ sessionBaseKey = nullptr);
			static void UnregisterFrame(Windows::UI::Xaml::Controls::Frame^ frame);
			static concurrency::task<void> SaveAsync();
			static concurrency::task<void> RestoreAsync(Platform::String^ sessionBaseKey = nullptr);
			static Windows::Foundation::Collections::IMap<Platform::String^, Platform::Object^>^ SessionState();
			static Windows::Foundation::Collections::IMap<Platform::String^, Platform::Object^>^ SessionStateForFrame(
				Windows::UI::Xaml::Controls::Frame^ frame);

		private:
			static void RestoreFrameNavigationState(Windows::UI::Xaml::Controls::Frame^ frame);
			static void SaveFrameNavigationState(Windows::UI::Xaml::Controls::Frame^ frame);

			static Platform::Collections::Map<Platform::String^, Platform::Object^>^ _sessionState;
			static const wchar_t* sessionStateFilename;

			static std::vector<Platform::WeakReference> _registeredFrames;
			static Windows::UI::Xaml::DependencyProperty^ FrameSessionStateKeyProperty;
			static Windows::UI::Xaml::DependencyProperty^ FrameSessionBaseKeyProperty;
			static Windows::UI::Xaml::DependencyProperty^ FrameSessionStateProperty;
		};
	}
}
