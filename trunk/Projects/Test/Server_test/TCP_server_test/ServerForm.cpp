/*
 * Author: Angelo Prudentino
 * Date: 30/09/2015
 * File: ServerForm.cpp
 *
 */

#include "ServerForm.h"

using namespace Server_test;

void TServerForm::LogDelegateMethod(String^ strToLog){
	//thread-safe logging function
	if (this->rtbLog->InvokeRequired){
		LogDelegate^ d = gcnew LogDelegate(this, &TServerForm::LogDelegateMethod);
		this->Invoke(d, strToLog);
	}
	else{
		this->rtbLog->AppendText(strToLog);
	}
}

void TServerForm::initTCPserver(Object^ data){
	int port = int::Parse((System::String^)data);

	this->Log("TServerForm", "initTCPserver", "start creating TStorageServer object...");
	this->serverEngine = new TStorageServer(port, this);
	this->Log("TServerForm", "initTCPserver", "TStorageServer object created; starting the server...");
	this->serverEngine->StartServer();
}

void TServerForm::dismissTCPserver(){

	if (this->serverEngine != nullptr){
		this->Log("TServerForm", "dismissTCPserver", "start deleting TStorageServer object...");
		delete this->serverEngine;
		this->Log("TServerForm", "dismissTCPserver", "TStorageServer object deleted");
	}
	this->serverEngine = nullptr;

	//wait for all secondary threads to terminate
	NetworkThread->Join();
}

void TServerForm::onServerSockCreate(){

	this->Log("TServerForm", "onServerSockCreate", "Main TCP Server has been succesfully created");
	this->afterStartServer();
}

void TServerForm::onServerSockLog(std::string aClassName, std::string aFuncName, std::string aMsg){
	this->Log(aClassName, aFuncName, aMsg);
}

void TServerForm::onServerSockError(std::string aClassName, std::string aFuncName, std::string aMsg){
	this->Log(aClassName, aFuncName, "************************************************");
	this->Log(aClassName, aFuncName, "* " + aMsg);
	this->Log(aClassName, aFuncName, "************************************************");
}

void TServerForm::onServerSockCriticalError(string aClassName, string aFuncName, string aMsg){
	this->onServerSockError(aClassName, aFuncName, aMsg);
	this->dismissTCPserver();
	this->afterStopServer();
}