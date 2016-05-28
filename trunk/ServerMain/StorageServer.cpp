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
		this->onServerWarning("TStorageServer", "constructor", "socket controller does NOT implement the IBaseExecutorController interface!");
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
	this->onServerLog("TStorageServer", "startServer", "starting the internal server socket...");
	this->fSockController->startSocket();
	this->onServerLog("TStorageServer", "startServer", "internal server socket started");

	this->onServerLog("TStorageServer", "startServer", "creating TMessageExecutor object...");
	this->fExecutor = new TMessageExecutor(this);
	this->onServerLog("TStorageServer", "startServer", "TMessageExecutor object created");
}

void TStorageServer::stopServer(){
	this->onServerLog("TStorageServer", "stopServer", "stopping the internal server socket for incoming messages...");
	this->fSockController->stopSocketIn();
	this->onServerLog("TStorageServer", "stopServer", "internal server socket for incoming messages stopped");

	this->onServerLog("TStorageServer", "stopServer", "stopping server executors...");
	this->fExecutor->stopExecutors();
	this->onServerLog("TStorageServer", "stopServer", "server executors stopped");
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
		this->fCallbackObj->onServerCriticalError(aClassName, aFuncName, aMsg);
}
#pragma endregion


#pragma region "IServerSockController implementation"
void TStorageServer::onServerSockCreate(){
	this->onServerLog("TStorageServer", "onServerSockCreate", "internal server socket correctly started");
	if (!System::Object::ReferenceEquals(this->fCallbackObj, nullptr))
		this->fCallbackObj->onServerSockCreate();
}

void TStorageServer::onServerSockAccept(TConnectionHandle aConnection){}

void TStorageServer::onServerSockRead(TConnectionHandle aConnection, string_ptr& aMsg){}

void TStorageServer::onServerSockWrite(){}
#pragma endregion


#pragma region "IServerExecutorController implementation"
bool TStorageServer::isInQueueEmpty(){
	if (this->fExeController != nullptr)
		return this->fExeController->isInQueueEmpty();
	else
		return true;
}

TMessageContainer_ptr TStorageServer::getMessageToProcess(){
	TMessageContainer_ptr result;
	if (this->fExeController != nullptr)
		result = this->fExeController->getMessageToProcess();

	return std::move(result);
}

void TStorageServer::enqueueMessageToSend(TMessageContainer_ptr& aMsg){
	if (this->fExeController != nullptr)
		this->fExeController->enqueueMessageToSend(aMsg);
}

void TStorageServer::processRegistrationRequest(TConnectionHandle aConnection, TUserRegistrReqMessage_ptr& aMsg){

}

void TStorageServer::processUpdateStartRequest(TConnectionHandle aConnection, TUpdateStartReqMessage_ptr& aMsg){

}

void TStorageServer::processAddNewFile(TConnectionHandle aConnection, TAddNewFileMessage_ptr& aMsg){

}

void TStorageServer::processUpdateFile(TConnectionHandle aConnection, TUpdateFileMessage_ptr& aMsg){

}

void TStorageServer::processRemoveFile(TConnectionHandle aConnection, TRemoveFileMessage_ptr& aMsg){

}

void TStorageServer::processUpdateStopRequest(TConnectionHandle aConnection, TUpdateStopReqMessage_ptr& aMsg){

}

void TStorageServer::processGetVersions(TConnectionHandle aConnection, TGetVersionsReqMessage_ptr& aMsg){

}

void TStorageServer::processRestoreVersion(TConnectionHandle aConnection, TRestoreVerReqMessage_ptr& aMsg){

}

void TStorageServer::processRestoreFileAck(TConnectionHandle aConnection, TRestoreFileAckMessage_ptr& aMsg){

}

void TStorageServer::processPingRequest(TConnectionHandle aConnection, TPingReqMessage_ptr& aMsg){

}
#pragma endregion
#pragma endregion