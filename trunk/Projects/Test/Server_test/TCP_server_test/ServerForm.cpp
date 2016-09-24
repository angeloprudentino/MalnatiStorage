/*
 * Author: Angelo Prudentino
 * Date: 30/09/2015
 * File: ServerForm.cpp
 * Description: This is the UI class of the server
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

	try{
		this->Log("TServerForm", "initTCPserver", "start creating TStorageServer object...");
		this->serverEngine = new TStorageServer(port, this);
		this->Log("TServerForm", "initTCPserver", "TStorageServer object created; starting the server...");
		this->serverEngine->startServer();
	}
	catch (EBaseException e){
		this->onServerError("TServerForm", "initTCPserver", e.getMessage());
		afterStopServer();
	}

}

void TServerForm::dismissTCPserver(){
	if (this->serverEngine != nullptr){
		beforeStopServer();

		this->Log("TServerForm", "dismissTCPserver", "stopping serverEngine object...");
		this->serverEngine->stopServer();
		this->Log("TServerForm", "dismissTCPserver", "serverEngine stopped");

		this->Log("TServerForm", "dismissTCPserver", "start deleting TStorageServer object...");
		delete this->serverEngine;
		this->serverEngine = nullptr;
		this->Log("TServerForm", "dismissTCPserver", "TStorageServer object deleted");
	}

	//wait for all secondary threads to terminate
	ServerThread->Join();
}

void TServerForm::onServerReady(const bool aReadyState){
	if (aReadyState)
		afterStartServer();
	else
		afterStopServer();
}

void TServerForm::onServerLog(const string& aClassName, const string& aFuncName, const string& aMsg){
	this->Log(aClassName, aFuncName, aMsg);
}

void TServerForm::onServerWarning(const string& aClassName, const string& aFuncName, const string& aMsg){
	this->Log(aClassName, aFuncName, "wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww");
	this->Log(aClassName, aFuncName, "w  " + aMsg);
	this->Log(aClassName, aFuncName, "wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww");
}

void TServerForm::onServerError(const string& aClassName, const string& aFuncName, const string& aMsg){
	this->Log(aClassName, aFuncName, "************************************************");
	this->Log(aClassName, aFuncName, "************************************************");
	this->Log(aClassName, aFuncName, "** ");
	this->Log(aClassName, aFuncName, "**  " + aMsg);
	this->Log(aClassName, aFuncName, "** ");
	this->Log(aClassName, aFuncName, "************************************************");
	this->Log(aClassName, aFuncName, "************************************************");
}

void TServerForm::onServerCriticalError(const string& aClassName, const string& aFuncName, const string& aMsg){
	this->onServerError(aClassName, aFuncName, aMsg);
	this->dismissTCPserver();
	this->afterStopServer();
}