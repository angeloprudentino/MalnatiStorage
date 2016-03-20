/*
* Author: Angelo Prudentino
* Date: 30/09/2015
* File: ServerForm.h
*
*/
#pragma once

#include <string>
#include "Utility.h"
#include "StorageServer.h"

namespace Server_test {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Threading;

	/// <summary>
	/// Summary for TServerForm
	/// </summary>
	public ref class TServerForm : public System::Windows::Forms::Form, IManagedServerSockController
	{
	public:
		TServerForm(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~TServerForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Button^  btnStart;
	protected:
	private: System::Windows::Forms::TextBox^  tbPort;
	private: System::Windows::Forms::RichTextBox^  rtbLog;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Label^  ServerStatusLabel;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->btnStart = (gcnew System::Windows::Forms::Button());
			this->tbPort = (gcnew System::Windows::Forms::TextBox());
			this->rtbLog = (gcnew System::Windows::Forms::RichTextBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->ServerStatusLabel = (gcnew System::Windows::Forms::Label());
			this->SuspendLayout();
			// 
			// btnStart
			// 
			this->btnStart->BackColor = System::Drawing::SystemColors::Control;
			this->btnStart->FlatStyle = System::Windows::Forms::FlatStyle::System;
			this->btnStart->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->btnStart->ForeColor = System::Drawing::Color::LimeGreen;
			this->btnStart->Location = System::Drawing::Point(40, 28);
			this->btnStart->Name = L"btnStart";
			this->btnStart->Size = System::Drawing::Size(144, 31);
			this->btnStart->TabIndex = 0;
			this->btnStart->Text = L"Start";
			this->btnStart->UseVisualStyleBackColor = false;
			this->btnStart->Click += gcnew System::EventHandler(this, &TServerForm::btnStart_Click);
			// 
			// tbPort
			// 
			this->tbPort->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->tbPort->Location = System::Drawing::Point(747, 33);
			this->tbPort->Name = L"tbPort";
			this->tbPort->Size = System::Drawing::Size(142, 26);
			this->tbPort->TabIndex = 1;
			this->tbPort->Text = L"4700";
			this->tbPort->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// rtbLog
			// 
			this->rtbLog->BackColor = System::Drawing::Color::White;
			this->rtbLog->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->rtbLog->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->rtbLog->Location = System::Drawing::Point(40, 144);
			this->rtbLog->Name = L"rtbLog";
			this->rtbLog->ReadOnly = true;
			this->rtbLog->Size = System::Drawing::Size(849, 454);
			this->rtbLog->TabIndex = 2;
			this->rtbLog->Text = L"";
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label1->Location = System::Drawing::Point(634, 35);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(84, 24);
			this->label1->TabIndex = 3;
			this->label1->Text = L"TCP port";
			// 
			// ServerStatusLabel
			// 
			this->ServerStatusLabel->AutoSize = true;
			this->ServerStatusLabel->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 15.75F, System::Drawing::FontStyle::Regular,
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			this->ServerStatusLabel->ForeColor = System::Drawing::Color::Red;
			this->ServerStatusLabel->Location = System::Drawing::Point(379, 107);
			this->ServerStatusLabel->Name = L"ServerStatusLabel";
			this->ServerStatusLabel->Size = System::Drawing::Size(180, 25);
			this->ServerStatusLabel->TabIndex = 4;
			this->ServerStatusLabel->Text = L"Server is stopped";
			// 
			// TServerForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::SystemColors::ControlDark;
			this->ClientSize = System::Drawing::Size(931, 622);
			this->Controls->Add(this->ServerStatusLabel);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->rtbLog);
			this->Controls->Add(this->tbPort);
			this->Controls->Add(this->btnStart);
			this->Name = L"TServerForm";
			this->Text = L"TServerForm";
			this->FormClosing += gcnew System::Windows::Forms::FormClosingEventHandler(this, &TServerForm::ServerForm_FormClosing);
			this->Load += gcnew System::EventHandler(this, &TServerForm::ServerForm_Load);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion

#pragma region MyCode

	delegate void afterStartServerDelegate();
	delegate void LogDelegate(String^ strToLog);

	private: Thread^ NetworkThread;
	private: bool server_is_started = false;
	private: TStorageServer* serverEngine = nullptr;

	private: System::Void ServerForm_Load(System::Object^  sender, System::EventArgs^  e) {
				 server_is_started = false;
	}

	private: System::Void ServerForm_FormClosing(System::Object^  sender, System::Windows::Forms::FormClosingEventArgs^  e) {
				 if (this->server_is_started){
					 this->beforeStopServer();
					 this->Log("TServerForm", "ServerForm_FormClosing", "server is going to stop");
					 this->dismissTCPserver();
					 this->Log("TServerForm", "ServerForm_FormClosing", "server is stopped");
					 this->afterStopServer();
				 }
	}

	private: System::Void btnStart_Click(System::Object^  sender, System::EventArgs^  e) {
				 if (!this->server_is_started){
					 this->beforeStartServer();
					 this->Log("TServerForm", "btnStart_Click", "server is going to start");
					 this->NetworkThread = gcnew Thread(gcnew ParameterizedThreadStart(this, &TServerForm::initTCPserver));
					 this->NetworkThread->Start(tbPort->Text);
				 }
				 else{
					 this->beforeStopServer();
					 this->Log("TServerForm", "btnStart_Click", "server is going to stop");
					 this->dismissTCPserver();
					 this->Log("TServerForm", "btnStart_Click", "server is stopped");
					 this->afterStopServer();
				 }
	}

	private: void beforeStartServer(){
				this->ServerStatusLabel->Text = "Server is starting";
				this->ServerStatusLabel->ForeColor = System::Drawing::Color::Yellow;
				this->btnStart->Enabled = false;
				this->tbPort->Enabled = false;
	}

	private: void afterStartServer(){
				 if (this->ServerStatusLabel->InvokeRequired){
					 afterStartServerDelegate^ d = gcnew afterStartServerDelegate(this, &TServerForm::afterStartServer);
					 this->Invoke(d);
				 }
				 else {
					 this->ServerStatusLabel->Text = "Server is started";
					 this->ServerStatusLabel->ForeColor = System::Drawing::Color::Green;
					 this->btnStart->Text = "Stop";
					 this->btnStart->Enabled = true;
					 this->tbPort->Enabled = false;
					 this->server_is_started = true;
				 }
	}

	private: void beforeStopServer(){
				this->ServerStatusLabel->Text = "Server is stopping";
				this->ServerStatusLabel->ForeColor = System::Drawing::Color::Yellow;
				this->btnStart->Enabled = false;
				this->tbPort->Enabled = false;
	}

	private: void afterStopServer(){
				this->ServerStatusLabel->Text = "Server is stopped";
				this->ServerStatusLabel->ForeColor = System::Drawing::Color::Red;
				this->btnStart->Text = "Start";
				this->tbPort->Enabled = true;
				this->btnStart->Enabled = true;
				this->server_is_started = false;
	}

	private: void Log(std::string className, std::string funcName, std::string msg){
				 System::Threading::Monitor::Enter(rtbLog);
				 std::string toLog = currentDateTime();
				 toLog.append(" ").append(className).append("::").append(funcName).append(" => ").append(msg).append("\n");
				 this->LogDelegateMethod(gcnew String(toLog.c_str()));
				 System::Threading::Monitor::Exit(rtbLog);
	}

	private: void LogDelegateMethod(String^ strToLog);

	private: void initTCPserver(Object^ data);
	private: void dismissTCPserver();

	//Server socket Callbacks 
	public: void onServerSockCreate() override;
	public: void onServerSockLog(string aClassName, string aFuncName, string aMsg) override;
	public: void onServerSockError(string aClassName, string aFuncName, string aMsg) override;
	public: void onServerSockCriticalError(string aClassName, string aFuncName, string aMsg) override;
#pragma endregion
};

	[STAThread]

	void main() {

		Application::EnableVisualStyles();
		Application::SetCompatibleTextRenderingDefault(false);
		TServerForm form;
		Application::Run(%form);
	}
}
