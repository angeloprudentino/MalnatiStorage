/*
 * Author: Angelo Prudentino
 * Date: 02/10/2015
 * File: StorageServer.cpp
 * Description: This is the main class of the server
 *
 */

#include "StorageServer.h"
#include "Utility.h"

TStorageServer::TStorageServer(int AServerPort, IManagedServerSockController^ aCallback){
	this->fServerPort = AServerPort;
	this->fCallbackObj = aCallback;
	onServerSockLog("TStorageServer", "constructor", "creating TServerSockController object...");
	this->fSockController = new TServerSockController(this->fServerPort, this);
	onServerSockLog("TStorageServer", "constructor", "TServerSockController object created");
}

TStorageServer::~TStorageServer(){
	if (this->fSockController != nullptr){
		onServerSockLog("TStorageServer", "destructor", "deleting TServerSockController object...");
		delete this->fSockController;
		onServerSockLog("TStorageServer", "destructor", "TServerSockController object deleted");
	}
	this->fSockController = nullptr;
	this->fCallbackObj = nullptr;
}

void TStorageServer::StartServer(){
	onServerSockLog("TStorageServer", "StartServer", "starting the internal server socket...");
	this->fSockController->StartSocket();
}

void TStorageServer::onServerSockCreate(){
	onServerSockLog("TStorageServer", "onServerSockCreate", "internal server socket correctly started");
	if (!System::Object::ReferenceEquals(this->fCallbackObj, nullptr))
		this->fCallbackObj->onServerSockCreate();
}

void TStorageServer::onServerSockLog(string aClassName, string aFuncName, string aMsg){
	if (!System::Object::ReferenceEquals(this->fCallbackObj, nullptr))
		this->fCallbackObj->onServerSockLog(aClassName, aFuncName, aMsg);
}

void TStorageServer::onServerSockError(string aClassName, string aFuncName, string aMsg){
	if (!System::Object::ReferenceEquals(this->fCallbackObj, nullptr))
		this->fCallbackObj->onServerSockError(aClassName, aFuncName, aMsg);
}

void TStorageServer::onServerSockCriticalError(string aClassName, string aFuncName, string aMsg){
	if (!System::Object::ReferenceEquals(this->fCallbackObj, nullptr))
		this->fCallbackObj->onServerSockError(aClassName, aFuncName, aMsg);
}

void TStorageServer::onServerSockAccept(TConnectionHandle aConnection){

}

void TStorageServer::onServerSockRead(TConnectionHandle aConnection, string aMsg){

}