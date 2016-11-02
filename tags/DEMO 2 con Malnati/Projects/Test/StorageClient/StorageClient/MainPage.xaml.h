//
// MainPage.xaml.h
// Dichiarazione della classe MainPage.
//

#pragma once

#include "MainPage.g.h"
using namespace Windows::Security::Credentials::UI;

namespace StorageClient
{
	/// <summary>
	/// Pagina vuota che può essere utilizzata autonomamente oppure esplorata all'interno di un frame.
	/// </summary>
	public ref class MainPage sealed
	{
	public:
		MainPage();
		property Windows::UI::Xaml::Controls::Frame^ InputFrame
		{
			Windows::UI::Xaml::Controls::Frame^ get() { return _inputFrame; }
		}
		property Windows::UI::Xaml::Controls::Frame^ OutputFrame
		{
			Windows::UI::Xaml::Controls::Frame^ get() { return _outputFrame; }
		}

	private:
		Windows::UI::Xaml::Controls::Frame^ _inputFrame;
		Windows::UI::Xaml::Controls::Frame^ _outputFrame;
		void HyperlinkButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void HyperlinkButton_Click_1(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void Register_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void SetResultRegister(CredentialPickerResults^ result);
	};
}
