/*
 * Author: Angelo Prudentino
 * Date: 02/10/2015
 * File: StorageServer.cpp
 * Description: This is the main class of the server
 *
 */

#include "StorageServer.h"
#include "Utility.h"

////////////////////////////////////
//        TStorageServer	      //
////////////////////////////////////
#pragma region "TStorageServer"
TStorageServer::TStorageServer(int AServerPort, IManagedServerSockController^ aCallbackObj){
	initCrypto();

	this->fServerPort = AServerPort;
	this->fCallbackObj = aCallbackObj;
	
	this->onServerLog("TStorageServer", "constructor", "creating TServerSockController object...");

	this->fSockController = new TServerSockController(this->fServerPort, this);
	if (!(this->fExeController = dynamic_cast<IBaseExecutorController*>(this->fSockController))){
		this->fExeController = nullptr;
		this->onServerWarning("TStorageServer", "constructor", "socket controller does not implement the IBaseExecutorController interface!");
	}

	this->onServerLog("TStorageServer", "constructor", "TServerSockController object created");
}

TStorageServer::~TStorageServer(){
	if (this->fSockController != nullptr){
		this->onServerLog("TStorageServer", "destructor", "deleting TServerSockController object...");
		delete this->fSockController;
		this->onServerLog("TStorageServer", "destructor", "TServerSockController object deleted");
		this->fSockController = nullptr;
		this->fExeController = nullptr;
	}

	if (this->fExecutor != nullptr){
		this->onServerLog("TStorageServer", "destructor", "deleting TMessageExecutor object...");
		delete this->fExecutor;
		this->onServerLog("TStorageServer", "destructor", "TMessageExecutor object deleted");
		this->fExecutor = nullptr;
	}

	this->fCallbackObj = nullptr;
}

void TStorageServer::startServer(){
	this->onServerLog("TStorageServer", "StartServer", "starting the internal server socket...");
	this->fSockController->startSocket();
	this->onServerLog("TStorageServer", "StartServer", "internal server socket started");

	this->onServerLog("TStorageServer", "StartServer", "creating TMessageExecutor object...");
	this->fExecutor = new TMessageExecutor(this);
	this->onServerLog("TStorageServer", "StartServer", "TMessageExecutor object created");
}

#pragma region "IServerBaseController implementation"
void TStorageServer::onServerLog(string aClassName, string aFuncName, string aMsg){
	if (!System::Object::ReferenceEquals(this->fCallbackObj, nullptr))
		this->fCallbackObj->onServerLog(aClassName, aFuncName, aMsg);
}

void TStorageServer::onServerWarning(string aClassName, string aFuncName, string aMsg){
	if (!System::Object::ReferenceEquals(this->fCallbackObj, nullptr))
		this->fCallbackObj->onServerWarning(aClassName, aFuncName, aMsg);
}

void TStorageServer::onServerError(string aClassName, string aFuncName, string aMsg){
	if (!System::Object::ReferenceEquals(this->fCallbackObj, nullptr))
		this->fCallbackObj->onServerError(aClassName, aFuncName, aMsg);
}

void TStorageServer::onServerCriticalError(string aClassName, string aFuncName, string aMsg){
	if (!System::Object::ReferenceEquals(this->fCallbackObj, nullptr))
		this->fCallbackObj->onServerError(aClassName, aFuncName, aMsg);
}
#pragma endregion

#pragma region "IServerSockController implementation"
void TStorageServer::onServerSockCreate(){
	this->onServerLog("TStorageServer", "onServerSockCreate", "internal server socket correctly started");
	if (!System::Object::ReferenceEquals(this->fCallbackObj, nullptr))
		this->fCallbackObj->onServerSockCreate();
}

void TStorageServer::onServerSockAccept(TConnectionHandle aConnection){}

void TStorageServer::onServerSockRead(TConnectionHandle aConnection, string_ptr aMsg){}

void TStorageServer::onServerSockWrite(){}
#pragma endregion

#pragma region "IServerExecutorController implementation"
TMessageContainer TStorageServer::getMessageToProcess(){
	TMessageContainer result;
	if (this->fExeController != nullptr)
		result = this->fExeController->getMessageToProcess();

	return result;
}

void TStorageServer::enqueueMessageToSend(TMessageContainer aMsg){
	if (this->fExeController != nullptr)
		this->fExeController->enqueueMessageToSend(aMsg);
}

void TStorageServer::processRegistrationRequest(TConnectionHandle aConnection, TUserRegistrReqMessage aMsg){

}

void TStorageServer::processUpdateStartRequest(TConnectionHandle aConnection, TUpdateStartReqMessage aMsg){

}

void TStorageServer::processAddNewFile(TConnectionHandle aConnection, TAddNewFileMessage aMsg){

}

void TStorageServer::processUpdateFile(TConnectionHandle aConnection, TUpdateFileMessage aMsg){

}

void TStorageServer::processRemoveFile(TConnectionHandle aConnection, TRemoveFileMessage aMsg){

}

void TStorageServer::processUpdateStopRequest(TConnectionHandle aConnection, TUpdateStopReqMessage aMsg){

}

void TStorageServer::processGetVersions(TConnectionHandle aConnection, TGetVersionsReqMessage aMsg){

}

void TStorageServer::processRestoreVersion(TConnectionHandle aConnection, TRestoreVerReqMessage aMsg){

}

void TStorageServer::processRestoreFileAck(TConnectionHandle aConnection, TRestoreFileAckMessage aMsg){

}

void TStorageServer::processPingRequest(TConnectionHandle aConnection, TPingReqMessage aMsg){

}
#pragma endregion
#pragma endregion