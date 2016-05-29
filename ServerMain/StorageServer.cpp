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
		this->fExeController->enqueueMessageToSend(move_TMessageContainer_ptr(aMsg));
}

void TStorageServer::processRegistrationRequest(TConnectionHandle aConnection, TUserRegistrReqMessage_ptr& aMsg){
	if (aMsg != nullptr){
		string u = aMsg->getUser();
		string p = aMsg->getPass();

		//Log the message
		this->onServerLog("TStorageServer", "processRegistrationRequest", "####################################");
		this->onServerLog("TStorageServer", "processRegistrationRequest", "####################################");
		this->onServerLog("TStorageServer", "processRegistrationRequest", "## UserRegistrReqMessage ");
		this->onServerLog("TStorageServer", "processRegistrationRequest", "## ");
		this->onServerLog("TStorageServer", "processRegistrationRequest", "## user: " + u);
		this->onServerLog("TStorageServer", "processRegistrationRequest", "## coded pass: " + p);
		this->onServerLog("TStorageServer", "processRegistrationRequest", "## ");
		this->onServerLog("TStorageServer", "processRegistrationRequest", "####################################");
		this->onServerLog("TStorageServer", "processRegistrationRequest", "####################################");

		//TODO: check if user already registred; if so enqueue negative response

		//TODO: store user in DB and send back a positive response
		TUserRegistrReplyMessage_ptr reply = new_TUserRegistrReplyMessage_ptr(true);
		TMessageContainer_ptr replyContainer = new_TMessageContainer_ptr((TBaseMessage_ptr&)reply, aConnection); //reply is moved
		this->enqueueMessageToSend(replyContainer); //replyContainer is moved
	}
	else{
		this->onServerError("TStorageServer", "processRegistrationRequest", "received an empty message; skipped.");
	}
}

void TStorageServer::processUpdateStartRequest(TConnectionHandle aConnection, TUpdateStartReqMessage_ptr& aMsg){
	if (aMsg != nullptr){
		string u = aMsg->getUser();
		string p = aMsg->getPass();

		//Log the message
		this->onServerLog("TStorageServer", "processUpdateStartRequest", "####################################");
		this->onServerLog("TStorageServer", "processUpdateStartRequest", "####################################");
		this->onServerLog("TStorageServer", "processUpdateStartRequest", "## UpdateStartReqMessage ");
		this->onServerLog("TStorageServer", "processUpdateStartRequest", "## ");
		this->onServerLog("TStorageServer", "processUpdateStartRequest", "## user: " + u);
		this->onServerLog("TStorageServer", "processUpdateStartRequest", "## coded pass: " + p);
		this->onServerLog("TStorageServer", "processUpdateStartRequest", "## ");
		this->onServerLog("TStorageServer", "processUpdateStartRequest", "####################################");
		this->onServerLog("TStorageServer", "processUpdateStartRequest", "####################################");

		//TODO: check if no sessions for this user are already active


		//TODO: generate a new token and start a new session
		string token;
		try{
			string_ptr token_ptr = getUniqueToken(u);
			token = (token_ptr.release())->c_str();
		}
		catch (EOpensslException e){
			this->onServerError("TStorageServer", "processUpdateStartRequest", "EOpensslException creating a new token: " + e.getMessage() + " ; skipped.");
			return;
		}

		TUpdateStartReplyMessage_ptr reply = new_TUpdateStartReplyMessage_ptr(true, token);
		TMessageContainer_ptr replyContainer = new_TMessageContainer_ptr((TBaseMessage_ptr&)reply, aConnection); //reply is moved
		this->enqueueMessageToSend(replyContainer); //replyContainer is moved
	}
	else{
		this->onServerError("TStorageServer", "processUpdateStartRequest", "received an empty message; skipped.");
	}
}

void TStorageServer::processAddNewFile(TConnectionHandle aConnection, TAddNewFileMessage_ptr& aMsg){
	if (aMsg != nullptr){
		string t = aMsg->getToken();
		string fp = aMsg->getFilePath();
		string c = aMsg->getChecksum();
		string fd = formatFileDate(aMsg->getFileDate()); 

		//Log the message
		this->onServerLog("TStorageServer", "processAddNewFile", "####################################");
		this->onServerLog("TStorageServer", "processAddNewFile", "####################################");
		this->onServerLog("TStorageServer", "processAddNewFile", "## AddNewFileMessage ");
		this->onServerLog("TStorageServer", "processAddNewFile", "## ");
		this->onServerLog("TStorageServer", "processAddNewFile", "## token: " + t);
		this->onServerLog("TStorageServer", "processAddNewFile", "## file path: " + fp);
		this->onServerLog("TStorageServer", "processAddNewFile", "## file checksum: " + c);
		this->onServerLog("TStorageServer", "processAddNewFile", "## file date: " + fd);
		this->onServerLog("TStorageServer", "processAddNewFile", "## ");
		this->onServerLog("TStorageServer", "processAddNewFile", "####################################");
		this->onServerLog("TStorageServer", "processAddNewFile", "####################################");

		//TODO: check if token is associated to a valid session


		//TODO: add file in proper location and send a positive response

		TFileAckMessage_ptr reply = new_TFileAckMessage_ptr(true, fp);
		TMessageContainer_ptr replyContainer = new_TMessageContainer_ptr((TBaseMessage_ptr&)reply, aConnection); //reply is moved
		this->enqueueMessageToSend(replyContainer); //replyContainer is moved
	}
	else{
		this->onServerError("TStorageServer", "processAddNewFile", "received an empty message; skipped.");
	}
}

void TStorageServer::processUpdateFile(TConnectionHandle aConnection, TUpdateFileMessage_ptr& aMsg){
	if (aMsg != nullptr){
		string t = aMsg->getToken();
		string fp = aMsg->getFilePath();
		string c = aMsg->getChecksum();
		string fd = formatFileDate(aMsg->getFileDate());

		//Log the message
		this->onServerLog("TStorageServer", "processUpdateFile", "####################################");
		this->onServerLog("TStorageServer", "processUpdateFile", "####################################");
		this->onServerLog("TStorageServer", "processUpdateFile", "## UpdateFileMessage ");
		this->onServerLog("TStorageServer", "processUpdateFile", "## ");
		this->onServerLog("TStorageServer", "processUpdateFile", "## token: " + t);
		this->onServerLog("TStorageServer", "processUpdateFile", "## file path: " + fp);
		this->onServerLog("TStorageServer", "processUpdateFile", "## file checksum: " + c);
		this->onServerLog("TStorageServer", "processUpdateFile", "## file date: " + fd);
		this->onServerLog("TStorageServer", "processUpdateFile", "## ");
		this->onServerLog("TStorageServer", "processUpdateFile", "####################################");
		this->onServerLog("TStorageServer", "processUpdateFile", "####################################");

		//TODO: check if token is associated to a valid session


		//TODO: update file in proper location and send a positive response

		TFileAckMessage_ptr reply = new_TFileAckMessage_ptr(true, fp);
		TMessageContainer_ptr replyContainer = new_TMessageContainer_ptr((TBaseMessage_ptr&)reply, aConnection); //reply is moved
		this->enqueueMessageToSend(replyContainer); //replyContainer is moved
	}
	else{
		this->onServerError("TStorageServer", "processUpdateFile", "received an empty message; skipped.");
	}
}

void TStorageServer::processRemoveFile(TConnectionHandle aConnection, TRemoveFileMessage_ptr& aMsg){
	if (aMsg != nullptr){
		string t = aMsg->getToken();
		string fp = aMsg->getFilePath();

		//Log the message
		this->onServerLog("TStorageServer", "processRemoveFile", "####################################");
		this->onServerLog("TStorageServer", "processRemoveFile", "####################################");
		this->onServerLog("TStorageServer", "processRemoveFile", "## RemoveFileMessage ");
		this->onServerLog("TStorageServer", "processRemoveFile", "## ");
		this->onServerLog("TStorageServer", "processRemoveFile", "## token: " + t);
		this->onServerLog("TStorageServer", "processRemoveFile", "## file path: " + fp);
		this->onServerLog("TStorageServer", "processRemoveFile", "## ");
		this->onServerLog("TStorageServer", "processRemoveFile", "####################################");
		this->onServerLog("TStorageServer", "processRemoveFile", "####################################");

		//TODO: check if token is associated to a valid session


		//TODO: remove file and send a positive response

		TFileAckMessage_ptr reply = new_TFileAckMessage_ptr(true, fp);
		TMessageContainer_ptr replyContainer = new_TMessageContainer_ptr((TBaseMessage_ptr&)reply, aConnection); //reply is moved
		this->enqueueMessageToSend(replyContainer); //replyContainer is moved
	}
	else{
		this->onServerError("TStorageServer", "processRemoveFile", "received an empty message; skipped.");
	}
}

void TStorageServer::processUpdateStopRequest(TConnectionHandle aConnection, TUpdateStopReqMessage_ptr& aMsg){
	if (aMsg != nullptr){
		string t = aMsg->getToken();

		//Log the message
		this->onServerLog("TStorageServer", "processRemoveFile", "####################################");
		this->onServerLog("TStorageServer", "processRemoveFile", "####################################");
		this->onServerLog("TStorageServer", "processRemoveFile", "## UpdateStopReqMessage ");
		this->onServerLog("TStorageServer", "processRemoveFile", "## ");
		this->onServerLog("TStorageServer", "processRemoveFile", "## token: " + t);
		this->onServerLog("TStorageServer", "processRemoveFile", "## ");
		this->onServerLog("TStorageServer", "processRemoveFile", "####################################");
		this->onServerLog("TStorageServer", "processRemoveFile", "####################################");

		//TODO: check if token is associated to a valid session


		//TODO: end update session and store modifictions permanently

		// send back info about the newly created version
		//TUpdateStopReplyMessage reply = new_TUpdateStopReplyMessage_ptr();
		//TMessageContainer_ptr replyContainer = new_TMessageContainer_ptr((TBaseMessage_ptr&)reply, aConnection); //reply is moved
		//this->enqueueMessageToSend(replyContainer); //replyContainer is moved
	}
	else{
		this->onServerError("TStorageServer", "processRemoveFile", "received an empty message; skipped.");
	}
}

void TStorageServer::processGetVersions(TConnectionHandle aConnection, TGetVersionsReqMessage_ptr& aMsg){
	if (aMsg != nullptr){
		string u = aMsg->getUser();
		string p = aMsg->getPass();

		//Log the message
		this->onServerLog("TStorageServer", "processGetVersions", "####################################");
		this->onServerLog("TStorageServer", "processGetVersions", "####################################");
		this->onServerLog("TStorageServer", "processGetVersions", "## GetVersionsReqMessage ");
		this->onServerLog("TStorageServer", "processGetVersions", "## ");
		this->onServerLog("TStorageServer", "processGetVersions", "## user: " + u);
		this->onServerLog("TStorageServer", "processGetVersions", "## coded pass: " + p);
		this->onServerLog("TStorageServer", "processGetVersions", "## ");
		this->onServerLog("TStorageServer", "processGetVersions", "####################################");
		this->onServerLog("TStorageServer", "processGetVersions", "####################################");

		//TODO: check if username and password are valid


		//TODO: get all versions of this user and send back the response

		//TGetVersionsReplyMessage_ptr reply = new_TGetVersionsReplyMessage_ptr();
		//TMessageContainer_ptr replyContainer = new_TMessageContainer_ptr((TBaseMessage_ptr&)reply, aConnection); //reply is moved
		//this->enqueueMessageToSend(replyContainer); //replyContainer is moved
	}
	else{
		this->onServerError("TStorageServer", "processGetVersions", "received an empty message; skipped.");
	}
}

void TStorageServer::processRestoreVersion(TConnectionHandle aConnection, TRestoreVerReqMessage_ptr& aMsg){
	if (aMsg != nullptr){
		string u = aMsg->getUser();
		string p = aMsg->getPass();
		string v = to_string(aMsg->getVersion());

		//Log the message
		this->onServerLog("TStorageServer", "processRestoreVersion", "####################################");
		this->onServerLog("TStorageServer", "processRestoreVersion", "####################################");
		this->onServerLog("TStorageServer", "processRestoreVersion", "## RestoreVerReqMessage ");
		this->onServerLog("TStorageServer", "processRestoreVersion", "## ");
		this->onServerLog("TStorageServer", "processRestoreVersion", "## user: " + u);
		this->onServerLog("TStorageServer", "processRestoreVersion", "## coded pass: " + p);
		this->onServerLog("TStorageServer", "processRestoreVersion", "## required version: " + v);
		this->onServerLog("TStorageServer", "processRestoreVersion", "## ");
		this->onServerLog("TStorageServer", "processRestoreVersion", "####################################");
		this->onServerLog("TStorageServer", "processRestoreVersion", "####################################");

		//TODO: check if no sessions for this user are already active

		//TODO: check if required version is available

		//TODO: generate a new token and start a new session
		string token;
		try{
			string_ptr token_ptr = getUniqueToken(u);
			token = (token_ptr.release())->c_str();
		}
		catch (EOpensslException e){
			this->onServerError("TStorageServer", "processRestoreVersion", "EOpensslException creating a new token: " + e.getMessage() + " ; skipped.");
			return;
		}

		TRestoreVerReplyMessage_ptr reply = new_TRestoreVerReplyMessage_ptr(true, token);
		TMessageContainer_ptr replyContainer = new_TMessageContainer_ptr((TBaseMessage_ptr&)reply, aConnection); //reply is moved
		this->enqueueMessageToSend(replyContainer); //replyContainer is moved

		//TODO: send the first file of the required version
	}
	else{
		this->onServerError("TStorageServer", "processRestoreVersion", "received an empty message; skipped.");
	}
}

void TStorageServer::processRestoreFileAck(TConnectionHandle aConnection, TRestoreFileAckMessage_ptr& aMsg){
	if (aMsg != nullptr){
		string t = aMsg->getToken();
		string fp = aMsg->getFilePath();
		bool r = aMsg->getResp();

		//Log the message
		this->onServerLog("TStorageServer", "processRemoveFile", "####################################");
		this->onServerLog("TStorageServer", "processRemoveFile", "####################################");
		this->onServerLog("TStorageServer", "processRemoveFile", "## RestoreFileAckMessage ");
		this->onServerLog("TStorageServer", "processRemoveFile", "## ");
		this->onServerLog("TStorageServer", "processRemoveFile", "## token: " + t);
		this->onServerLog("TStorageServer", "processRemoveFile", "## file path: " + fp);
		if (r)
			this->onServerLog("TStorageServer", "processRemoveFile", "## result: ok");
		else
			this->onServerLog("TStorageServer", "processRemoveFile", "## result: error");
		this->onServerLog("TStorageServer", "processRemoveFile", "## ");
		this->onServerLog("TStorageServer", "processRemoveFile", "####################################");
		this->onServerLog("TStorageServer", "processRemoveFile", "####################################");

		//TODO: check if token is associated to a valid session


		//TODO: if result is ok, send the next file, otherwise send this one again
		if (r){

		}
		else{

		}

		// send back info about the newly created version
		//TUpdateStopReplyMessage reply = new_TUpdateStopReplyMessage_ptr();
		//TMessageContainer_ptr replyContainer = new_TMessageContainer_ptr((TBaseMessage_ptr&)reply, aConnection); //reply is moved
		//this->enqueueMessageToSend(replyContainer); //replyContainer is moved
	}
	else{
		this->onServerError("TStorageServer", "processRemoveFile", "received an empty message; skipped.");
	}
}

void TStorageServer::processPingRequest(TConnectionHandle aConnection, TPingReqMessage_ptr& aMsg){
	if (aMsg != nullptr){
		string t = formatFileDate(aMsg->getTime());

		//Log the message
		this->onServerLog("TStorageServer", "processPingRequest", "####################################");
		this->onServerLog("TStorageServer", "processPingRequest", "####################################");
		this->onServerLog("TStorageServer", "processPingRequest", "## PingReqMessage ");
		this->onServerLog("TStorageServer", "processPingRequest", "## ");
		this->onServerLog("TStorageServer", "processPingRequest", "## time: " + t);
		this->onServerLog("TStorageServer", "processPingRequest", "## ");
		this->onServerLog("TStorageServer", "processPingRequest", "####################################");
		this->onServerLog("TStorageServer", "processPingRequest", "####################################");

		//send ping reply
		TPingReplyMessage_ptr reply = new_TPingReplyMessage_ptr();
		TMessageContainer_ptr replyContainer = new_TMessageContainer_ptr((TBaseMessage_ptr&)reply, aConnection); //reply is moved
		this->enqueueMessageToSend(replyContainer); //replyContainer is moved
	}
	else{
		this->onServerError("TStorageServer", "processPingRequest", "received an empty message; skipped.");
	}
}
#pragma endregion
#pragma endregion