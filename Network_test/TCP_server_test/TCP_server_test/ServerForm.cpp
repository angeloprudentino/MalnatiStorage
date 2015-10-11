/*
 * Author: Angelo Prudentino
 * Date: 30/09/2015
 * File: ServerForm.cpp
 *
 */

#include "ServerForm.h"

using namespace TCP_server_test;

void ServerForm::LogDelegateMethod(String^ strToLog){
	//thread-safe logging function
	if (this->rtbLog->InvokeRequired){
		LogDelegate^ d = gcnew LogDelegate(this, &ServerForm::LogDelegateMethod);
		this->Invoke(d, strToLog);
	}
	else{
		this->rtbLog->AppendText(strToLog);
	}
}

void ServerForm::initTCPserver(Object^ data){
	int port = int::Parse((System::String^)data);

	this->Log("ServerForm", "initTCPserver", "start creating StorageServer object...");
	this->serverEngine = new StorageServer(port, this);
	this->Log("ServerForm", "initTCPserver", "StorageServer object created; starting the server...");
	this->serverEngine->StartServer();
}

void ServerForm::dismissTCPserver(){

	if (this->serverEngine != nullptr){
		this->Log("ServerForm", "dismissTCPserver", "start deleting StorageServer object...");
		delete this->serverEngine;
		this->Log("ServerForm", "dismissTCPserver", "StorageServer object deleted");
	}
	this->serverEngine = nullptr;

	//wait for all secondary threads to terminate
	NetworkThread->Join();
}

void ServerForm::onServerSockCreate(){

	this->Log("ServerForm", "onServerSockCreate", "Main TCP Server has been succesfully created");
	this->afterStartServer();
}

void ServerForm::onServerSockLog(std::string className, std::string funcName, std::string msg){
	this->Log(className, funcName, msg);
}

void ServerForm::onServerSockError(std::string className, std::string funcName, std::string msg){
	this->Log(className, funcName, "************************************************");
	this->Log(className, funcName, "* " + msg);
	this->Log(className, funcName, "************************************************");
	
	this->dismissTCPserver();
	this->afterStopServer();
}