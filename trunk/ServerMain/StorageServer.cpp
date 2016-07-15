/*
 * Author: Angelo Prudentino
 * Date: 02/10/2015
 * File: StorageServer.cpp
 * Description: This is the main class of the server
 *
 */

#include "StorageServer.h"
#include "Utility.h"

#define ADD_FILE 0
#define UPDATE_FILE 1
#define REMOVE_FILE 2


////////////////////////////////////
//        TStorageServer	      //
////////////////////////////////////
#pragma region "TStorageServer"
const bool TStorageServer::isThereASessionFor(const string aUser){
	//access map in mutual exclusion to avoid 2 thread inserting the same session in the table
	unique_lock<mutex> lock(this->fSessionsMutex);

	auto pos = this->fSessions->find(aUser);
	return pos != this->fSessions->end();
}

const bool TStorageServer::isThereAnUpdateSessionFor(const string aUser){
	//access map in mutual exclusion to avoid 2 thread inserting the same session in the table
	unique_lock<mutex> lock(this->fSessionsMutex);

	auto pos = this->fSessions->find(aUser);
	bool res = pos != this->fSessions->end();
	if (res == true){
		res = pos->second->getKind() == UPDATE_SESSION;
	}

	return res;
}

const bool TStorageServer::isThereARestoreSessionFor(const string aUser){
	//access map in mutual exclusion to avoid 2 thread inserting the same session in the table
	unique_lock<mutex> lock(this->fSessionsMutex);

	auto pos = this->fSessions->find(aUser);
	bool res = pos != this->fSessions->end();
	if (res == true){
		res = pos->second->getKind() == RESTORE_SESSION;
	}

	return res;
}

void TStorageServer::newUpdateSession(const string aUser, const string aToken){
	//access map in mutual exclusion to avoid 2 thread inserting the same session in the table
	unique_lock<mutex> lock(this->fSessionsMutex);

	this->fSessions->emplace(aUser, new_TSession_ptr(UPDATE_SESSION, aToken));
	//TODO: load data abaout the session from DB
}

void TStorageServer::newRestoreSession(const string aUser, const string aToken){
	//access map in mutual exclusion to avoid 2 thread inserting the same session in the table
	unique_lock<mutex> lock(this->fSessionsMutex);

	this->fSessions->emplace(aUser, new_TSession_ptr(RESTORE_SESSION, aToken));
	//TODO: load data abaout the session from DB

}

void TStorageServer::updateSessionWithFile(const string aUser, TFile_ptr& aFile, int aOperation){
	//access map in mutual exclusion to avoid 2 thread inserting the same session in the table
	unique_lock<mutex> lock(this->fSessionsMutex);

	auto session = this->fSessions->find(aUser);
	if (session != this->fSessions->end()){
		switch (aOperation){
			case ADD_FILE:
				session->second->addFile(move_TFile_ptr(aFile));
				break;
			
			case UPDATE_FILE:
				session->second->updateFile(move_TFile_ptr(aFile));
				break;

			case REMOVE_FILE:
				session->second->removeFile(move_TFile_ptr(aFile));
				break;

			default:
				break;
		}
	}
}

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

	this->onServerLog("TStorageServer", "constructor", "creating TSessions object...");
	this->fSessions = new TSessions();
	this->onServerLog("TStorageServer", "constructor", "TSessions object created");

	this->onServerLog("TStorageServer", "constructor", "creating TDBManager object...");
	this->fDBManager = new TDBManager(DEFAULT_DB_HOST, DEFAULT_DB_NAME);
	this->onServerLog("TStorageServer", "constructor", "TDBManager object created");
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

	if (this->fSessions != nullptr){
		this->onServerLog("TStorageServer", "destructor", "deleting TSessions object...");
		for (TSessions::iterator it = this->fSessions->begin(); it != this->fSessions->end(); it++){
			it->second.reset();
		}
		this->fSessions->clear();
		delete this->fSessions;
		this->fSessions = nullptr;
		this->onServerLog("TStorageServer", "destructor", "TSessions object deleted");
	}

	if (this->fDBManager != nullptr){
		this->onServerLog("TStorageServer", "destructor", "deleting TDBManager object...");
		delete this->fDBManager;
		this->fDBManager = nullptr;
		this->onServerLog("TStorageServer", "destructor", "TDBManager object deleted");
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

		//TODO: store user in DB 
		
		//send back a positive response
		TUserRegistrReplyMessage_ptr reply = new_TUserRegistrReplyMessage_ptr(true);
		TMessageContainer_ptr replyContainer = new_TMessageContainer_ptr((TBaseMessage_ptr&)reply, aConnection); //reply is moved
		this->enqueueMessageToSend(replyContainer); //replyContainer is moved
	}
	else{
		this->onServerError("TStorageServer", "processRegistrationRequest", "received an empty message; skipped.");
	}
}

void TStorageServer::processUpdateStart(TConnectionHandle aConnection, TUpdateStartReqMessage_ptr& aMsg){
	if (aMsg != nullptr){
		string u = aMsg->getUser();
		string p = aMsg->getPass();

		//Log the message
		this->onServerLog("TStorageServer", "processUpdateStart", "####################################");
		this->onServerLog("TStorageServer", "processUpdateStart", "####################################");
		this->onServerLog("TStorageServer", "processUpdateStart", "## UpdateStartReqMessage ");
		this->onServerLog("TStorageServer", "processUpdateStart", "## ");
		this->onServerLog("TStorageServer", "processUpdateStart", "## user: " + u);
		this->onServerLog("TStorageServer", "processUpdateStart", "## coded pass: " + p);
		this->onServerLog("TStorageServer", "processUpdateStart", "## ");
		this->onServerLog("TStorageServer", "processUpdateStart", "####################################");
		this->onServerLog("TStorageServer", "processUpdateStart", "####################################");

		//TODO: check if user already registred; if so no response

		//check if no session for this user is already active
		bool isSess = this->isThereASessionFor(u);
		if (isSess){
			this->onServerError("TStorageServer", "processUpdateStart", "A session is already active for user: " + u);

			//send back negative response
			TUpdateStartReplyMessage_ptr reply = new_TUpdateStartReplyMessage_ptr(false, "");
			TMessageContainer_ptr replyContainer = new_TMessageContainer_ptr((TBaseMessage_ptr&)reply, aConnection); //reply is moved
			this->enqueueMessageToSend(replyContainer); //replyContainer is moved
			return;
		}

		//generate a new token and start a new session
		string_ptr token_ptr;
		try{
			token_ptr = getUniqueToken(u);
		}
		catch (EOpensslException e){
			this->onServerError("TStorageServer", "processUpdateStart", "EOpensslException creating a new token: " + e.getMessage() + " ; skipped.");
			return;
		}
		this->newUpdateSession(u, token_ptr->c_str());

		//send back positive response
		TUpdateStartReplyMessage_ptr reply = new_TUpdateStartReplyMessage_ptr(true, token_ptr->c_str());
		TMessageContainer_ptr replyContainer = new_TMessageContainer_ptr((TBaseMessage_ptr&)reply, aConnection); //reply is moved
		this->enqueueMessageToSend(replyContainer); //replyContainer is moved

		token_ptr.reset();
	}
	else{
		this->onServerError("TStorageServer", "processUpdateStart", "received an empty message; skipped.");
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

		//check if token is associated to a valid session
		string u = getUserFromToken(t);
		bool isSess = this->isThereAnUpdateSessionFor(u);
		if (!isSess){
			this->onServerError("TStorageServer", "processAddNewFile", "There is no update session opened for user: " + u);

			//enqueue negative response
			TFileAckMessage_ptr reply = new_TFileAckMessage_ptr(false, fp);
			TMessageContainer_ptr replyContainer = new_TMessageContainer_ptr((TBaseMessage_ptr&)reply, aConnection); //reply is moved
			this->enqueueMessageToSend(replyContainer); //replyContainer is moved
		}

		//TODO: add file in proper location

		//update session object
		//this->updateSessionWithFile(u, /*file*/, ADD_FILE);

		//send a positive response
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

		//check if token is associated to a valid session
		string u = getUserFromToken(t);
		bool isSess = this->isThereAnUpdateSessionFor(u);
		if (!isSess){
			this->onServerError("TStorageServer", "processUpdateFile", "There is no update session opened for user: " + u);

			//enqueue negative response
			TFileAckMessage_ptr reply = new_TFileAckMessage_ptr(false, fp);
			TMessageContainer_ptr replyContainer = new_TMessageContainer_ptr((TBaseMessage_ptr&)reply, aConnection); //reply is moved
			this->enqueueMessageToSend(replyContainer); //replyContainer is moved
		}

		//TODO: add file in proper location 

		//update session object
		//this->updateSessionWithFile(u, /*file*/, ADD_FILE);

		//send a positive response
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

		//check if token is associated to a valid session
		string u = getUserFromToken(t);
		bool isSess = this->isThereAnUpdateSessionFor(u);
		if (!isSess){
			this->onServerError("TStorageServer", "processRemoveFile", "There is no update session opened for user: " + u);

			//enqueue negative response
			TFileAckMessage_ptr reply = new_TFileAckMessage_ptr(false, fp);
			TMessageContainer_ptr replyContainer = new_TMessageContainer_ptr((TBaseMessage_ptr&)reply, aConnection); //reply is moved
			this->enqueueMessageToSend(replyContainer); //replyContainer is moved
		}

		//TODO: remove file 

		//update session object
		//this->updateSessionWithFile(u, /*file*/, ADD_FILE);

		//send a positive response
		TFileAckMessage_ptr reply = new_TFileAckMessage_ptr(true, fp);
		TMessageContainer_ptr replyContainer = new_TMessageContainer_ptr((TBaseMessage_ptr&)reply, aConnection); //reply is moved
		this->enqueueMessageToSend(replyContainer); //replyContainer is moved
	}
	else{
		this->onServerError("TStorageServer", "processRemoveFile", "received an empty message; skipped.");
	}
}

void TStorageServer::processUpdateStop(TConnectionHandle aConnection, TUpdateStopReqMessage_ptr& aMsg){
	if (aMsg != nullptr){
		string t = aMsg->getToken();

		//Log the message
		this->onServerLog("TStorageServer", "processUpdateStop", "####################################");
		this->onServerLog("TStorageServer", "processUpdateStop", "####################################");
		this->onServerLog("TStorageServer", "processUpdateStop", "## UpdateStopReqMessage ");
		this->onServerLog("TStorageServer", "processUpdateStop", "## ");
		this->onServerLog("TStorageServer", "processUpdateStop", "## token: " + t);
		this->onServerLog("TStorageServer", "processUpdateStop", "## ");
		this->onServerLog("TStorageServer", "processUpdateStop", "####################################");
		this->onServerLog("TStorageServer", "processUpdateStop", "####################################");

		//check if token is associated to a valid session
		string u = getUserFromToken(t);
		bool isSess = this->isThereAnUpdateSessionFor(u);
		if (!isSess){
			this->onServerError("TStorageServer", "processUpdateStop", "There is no update session opened for user: " + u);

			//enqueue negative response
			TUpdateStopReplyMessage_ptr reply = new_TUpdateStopReplyMessage_ptr(false, NO_VERSION, time(nullptr));
			TMessageContainer_ptr replyContainer = new_TMessageContainer_ptr((TBaseMessage_ptr&)reply, aConnection); //reply is moved
			this->enqueueMessageToSend(replyContainer); //replyContainer is moved
		}


		//TODO: end update session and store modifictions permanently

		// send back info about the newly created version
		TUpdateStopReplyMessage_ptr reply = new_TUpdateStopReplyMessage_ptr(true, /*NO_VERSION*/, time(nullptr));
		TMessageContainer_ptr replyContainer = new_TMessageContainer_ptr((TBaseMessage_ptr&)reply, aConnection); //reply is moved
		this->enqueueMessageToSend(replyContainer); //replyContainer is moved
	}
	else{
		this->onServerError("TStorageServer", "processUpdateStop", "received an empty message; skipped.");
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

		//check if no session for this user is already active
		bool isSess = this->isThereASessionFor(u);
		if (isSess){
			this->onServerError("TStorageServer", "processRestoreVersion", "A session is already active for user: " + u);

			//send back negative response
			TRestoreVerReplyMessage_ptr reply = new_TRestoreVerReplyMessage_ptr(false, "");
			TMessageContainer_ptr replyContainer = new_TMessageContainer_ptr((TBaseMessage_ptr&)reply, aConnection); //reply is moved
			this->enqueueMessageToSend(replyContainer); //replyContainer is moved
			return;
		}

		//TODO: check if required version is available

		//generate a new token and start a new session
		string_ptr token_ptr;
		try{
			token_ptr = getUniqueToken(u);
		}
		catch (EOpensslException e){
			this->onServerError("TStorageServer", "processRestoreVersion", "EOpensslException creating a new token: " + e.getMessage() + " ; skipped.");
			return;
		}
		this->newRestoreSession(u, token_ptr->c_str());

		//send back positive response
		TRestoreVerReplyMessage_ptr reply = new_TRestoreVerReplyMessage_ptr(true, token_ptr->c_str());
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
		this->onServerLog("TStorageServer", "processRestoreFileAck", "####################################");
		this->onServerLog("TStorageServer", "processRestoreFileAck", "####################################");
		this->onServerLog("TStorageServer", "processRestoreFileAck", "## RestoreFileAckMessage ");
		this->onServerLog("TStorageServer", "processRestoreFileAck", "## ");
		this->onServerLog("TStorageServer", "processRestoreFileAck", "## token: " + t);
		this->onServerLog("TStorageServer", "processRestoreFileAck", "## file path: " + fp);
		if (r)
			this->onServerLog("TStorageServer", "processRestoreFileAck", "## result: ok");
		else
			this->onServerLog("TStorageServer", "processRestoreFileAck", "## result: error");
		this->onServerLog("TStorageServer", "processRestoreFileAck", "## ");
		this->onServerLog("TStorageServer", "processRestoreFileAck", "####################################");
		this->onServerLog("TStorageServer", "processRestoreFileAck", "####################################");

		//check if token is associated to a valid session
		string u = getUserFromToken(t);
		bool isSess = this->isThereARestoreSessionFor(u);
		if (!isSess){
			this->onServerError("TStorageServer", "processRestoreFileAck", "There is no restore session opened for user: " + u);
		}


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
		this->onServerError("TStorageServer", "processRestoreFileAck", "received an empty message; skipped.");
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