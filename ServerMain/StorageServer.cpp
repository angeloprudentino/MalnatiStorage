/*
 * Author: Angelo Prudentino
 * Date: 02/10/2015
 * File: StorageServer.cpp
 * Description: This is the main class of the server
 *
 */

#include <boost\bind.hpp>
#include "StorageServer.h"
#include "DBManagerMSDE.h"
#include "Utility.h"

#define ADD_FILE 0
#define UPDATE_FILE 1
#define REMOVE_FILE 2

#define DEADLINE 300
#define MILLI 1000


////////////////////////////////////
//        TStorageServer	      //
////////////////////////////////////
#pragma region "TStorageServer"
const bool TStorageServer::newSession(const string& aUser, const string& aToken, const int aSessionType){
	TSession_ptr s = nullptr;
	if (aSessionType == UPDATE_SESSION)
		s = new_TSession_ptr(UPDATE_SESSION, aToken);
	else if (aSessionType == RESTORE_SESSION)
		s = new_TSession_ptr(RESTORE_SESSION, aToken);
	else{
		this->onServerError("TStorageServer", "newSession", "Invalid session type!");
		return false;
	}

	//load initial data abaout the session from DB
	if (this->fDBManager != nullptr){
		try{
			TVersion_ptr v = this->fDBManager->getLastVersion(aUser, true);
			if (v != nullptr)
				s->setVersion(move_TVersion_ptr(v));
		}
		catch (EDBException e){
			this->onServerError("TStorageServer", "newSession", "Unable to load last version for " + aUser + ": " + e.getMessage());
			return false;
		}
	}
	else{
		this->onServerError("TStorageServer", "newSession", "DBManager is null!");
		return false;
	}

	//access map in mutual exclusion to avoid 2 thread inserting the same session in the table
	unique_lock<mutex> lock(this->fSessionsMutex);
	this->fSessions->emplace(aUser, s);

	////init session cleaner timer if necessary
	//if (this->fSessionsCleaner == nullptr){
	//	this->onServerLog("TStorageServer", "newSession", "creating sessions cleaner timer...");
	//	this->fSessionsCleaner = new deadline_timer(this->fMainIoService, boost::posix_time::seconds(DEADLINE));
	//	this->fSessionsCleaner->async_wait(bind(&TStorageServer::checkAndCleanSessions, this, boost::asio::placeholders::error));
	//	this->onServerLog("TStorageServer", "newSession", "sessions cleaner timer created");
	//}

	return true;
}

TSession_ptr TStorageServer::isThereASessionFor(const string& aUser){
	//access map in mutual exclusion to avoid 2 thread inserting the same session in the table
	unique_lock<mutex> lock(this->fSessionsMutex);

	auto pos = this->fSessions->find(aUser);
	TSession_ptr res = nullptr;
	if (pos != this->fSessions->end())
		res = pos->second;

	return res;
}

TSession_ptr TStorageServer::isThereAnUpdateSessionFor(const string& aUser){
	//access map in mutual exclusion to avoid 2 thread inserting the same session in the table
	unique_lock<mutex> lock(this->fSessionsMutex);

	auto pos = this->fSessions->find(aUser);
	TSession_ptr res = nullptr;
	if (pos != this->fSessions->end())
		if (pos->second->getKind() == UPDATE_SESSION)
			res = pos->second;

	return res;
}

TSession_ptr TStorageServer::isThereARestoreSessionFor(const string& aUser){
	//access map in mutual exclusion to avoid 2 thread inserting the same session in the table
	unique_lock<mutex> lock(this->fSessionsMutex);

	auto pos = this->fSessions->find(aUser);
	TSession_ptr res = nullptr;
	if (pos != this->fSessions->end())
		if (pos->second->getKind() == RESTORE_SESSION)
			res = pos->second;
	
	return res;
}

void TStorageServer::removeSession(const string& aUser){
	//access map in mutual exclusion to avoid 2 thread inserting the same session in the table
	unique_lock<mutex> lock(this->fSessionsMutex);

	this->fSessions->erase(aUser);
}

void TStorageServer::checkAndCleanSessions(const boost::system::error_code& aErr){
	if (!aErr){
		this->onServerLog("TStorageServer", "checkAndCleanSessions", "start sessions cleaning");
		if (this->fSessions != nullptr){
			TSessions::iterator it = this->fSessions->begin();
			while (it != this->fSessions->end()){
				if (time(nullptr) - it->second->getLastPing() > SESSION_TIMEOUT){
					this->onServerWarning("TStorageServer", "checkAndCleanSessions", it->first + "'s session was pending so it has been cleaned");
					this->removeSession(it->first);
					//TODO: remove files on disk
				}
				else
					it++;
			}
		}
	}
	else{
		this->onServerError("TStorageServer", "checkAndCleanSessions", aErr.message());
	}

	//re-init session cleaner timer
	delete this->fSessionsCleaner;
	this->onServerLog("TStorageServer", "checkAndCleanSessions", "re-creating sessions cleaner timer...");
	this->fSessionsCleaner = new deadline_timer(this->fMainIoService, boost::posix_time::seconds(DEADLINE));
	this->fSessionsCleaner->async_wait(bind(&TStorageServer::checkAndCleanSessions, this, boost::asio::placeholders::error));
	this->onServerLog("TStorageServer", "checkAndCleanSessions", "sessions cleaner timer re-created");
}

const bool TStorageServer::userExists(const string& aUser){
	if (this->fDBManager != nullptr)
		return this->fDBManager->checkIfUserExists(aUser);
	else{
		this->onServerError("TStorageServer", "userExists", "DBManager is null!");
		return false;
	}
}

string_ptr TStorageServer::checkUserCredential(const string& aUser, const string& aPass){
	if (this->fDBManager != nullptr)
		return move_string_ptr(this->fDBManager->verifyUserCredentials(aUser, aPass));
	else{
		this->onServerError("TStorageServer", "checkUserCredential", "DBManager is null!");
		return nullptr;
	}
}


TStorageServer::TStorageServer(int AServerPort, IManagedServerController^ aCallbackObj){
	initCrypto();

	this->fServerPort = AServerPort;
	this->fCallbackObj = aCallbackObj;
	
	try{
		unique_lock<mutex> lock(this->fLogMutex);

		this->onServerLog("TStorageServer", "constructor", "########################################");
		this->onServerLog("TStorageServer", "constructor", "########################################");
		this->onServerLog("TStorageServer", "constructor", "## ");
		this->onServerLog("TStorageServer", "constructor", "##             SERVER START");
		this->onServerLog("TStorageServer", "constructor", "##");
		this->onServerLog("TStorageServer", "constructor", "########################################");
		this->onServerLog("TStorageServer", "constructor", "########################################");
	}
	catch (...){}

	this->onServerLog("TStorageServer", "constructor", "creating TServerSockController object...");

	this->fSockController = new TServerSockController(this->fServerPort, &(this->fMainIoService), this);
	if (!(this->fExeController = dynamic_cast<IBaseExecutorController*>(this->fSockController))){
		this->fExeController = nullptr;
		this->onServerWarning("TStorageServer", "constructor", "socket controller does NOT implement the IBaseExecutorController interface!");
	}

	this->onServerLog("TStorageServer", "constructor", "TServerSockController object created");

	this->onServerLog("TStorageServer", "constructor", "creating TSessions object...");
	this->fSessions = new TSessions();
	this->onServerLog("TStorageServer", "constructor", "TSessions object created");

	this->onServerLog("TStorageServer", "constructor", "creating TDBManagerMSDE object...");
	this->fDBManager = new TDBManagerMSDE(DEFAULT_DB_HOST, DEFAULT_DB_NAME);
	this->onServerLog("TStorageServer", "constructor", "TDBManagerMSDE object created");
}

TStorageServer::~TStorageServer(){
	if (this->fExecutor != nullptr){
		this->onServerLog("TStorageServer", "destructor", "deleting TMessageExecutor object...");
		delete this->fExecutor;
		this->onServerLog("TStorageServer", "destructor", "TMessageExecutor object deleted");
		this->fExecutor = nullptr;
	}

	if (this->fSockController != nullptr){
		this->onServerLog("TStorageServer", "destructor", "deleting TServerSockController object...");
		delete this->fSockController;
		this->onServerLog("TStorageServer", "destructor", "TServerSockController object deleted");
		this->fSockController = nullptr;
		this->fExeController = nullptr;
	}

	if (this->fSessions != nullptr){
		unique_lock<mutex> lock(this->fSessionsMutex);
		this->onServerLog("TStorageServer", "destructor", "deleting TSessions object...");
		//for (TSessions::iterator it = this->fSessions->begin(); it != this->fSessions->end(); it++){
		//	it->second.reset();
		//}
		this->fSessions->clear();
		delete this->fSessions;
		this->fSessions = nullptr;
		this->onServerLog("TStorageServer", "destructor", "TSessions object deleted");
	}

	if (this->fSessionsCleaner != nullptr){
		this->onServerLog("TStorageServer", "destructor", "deleting sessions cleaner thread...");
		boost::system::error_code ec;
		this->fSessionsCleaner->cancel(ec);
		if (ec)
			this->onServerLog("TStorageServer", "destructor", "error stopping sessions cleaner timer: " + ec.message());
		delete this->fSessionsCleaner;
		this->onServerLog("TStorageServer", "destructor", "sessions cleaner thread deleted");
		this->fSessionsCleaner = nullptr;
	}

	if (this->fDBManager != nullptr){
		this->onServerLog("TStorageServer", "destructor", "deleting TDBManager object...");
		delete this->fDBManager;
		this->fDBManager = nullptr;
		this->onServerLog("TStorageServer", "destructor", "TDBManager object deleted");
	}
	//this->fMainIoService.stop();

	this->fCallbackObj = nullptr;
}

void TStorageServer::startServer(){
	this->onServerLog("TStorageServer", "startServer", "starting the internal server socket...");
	if (this->fSockController->startSocket()){
		this->onServerLog("TStorageServer", "startServer", "internal server socket started");

		this->onServerLog("TStorageServer", "startServer", "creating TMessageExecutor object...");
		this->fExecutor = new TMessageExecutor(this);
		this->onServerLog("TStorageServer", "startServer", "TMessageExecutor object created");

		this->onServerReady(true);
		this->fMainIoService.run();
	}
	else{
		this->onServerLog("TStorageServer", "startServer", "internal server socket cannot be started");
		this->onServerReady(false);
	}
}

void TStorageServer::stopServer(){
	this->fMainIoService.stop();

	this->onServerLog("TStorageServer", "stopServer", "stopping server executors...");
	this->fExecutor->stopExecutors();
	this->onServerLog("TStorageServer", "stopServer", "server executors stopped");

	this->onServerLog("TStorageServer", "stopServer", "stopping the internal server socket for incoming messages...");
	this->fSockController->stopSocket();
	this->onServerLog("TStorageServer", "stopServer", "internal server socket for incoming messages stopped");
}

#pragma region "IServerBaseController implementation"
void TStorageServer::onServerReady(const bool aReadyState){
	if (!System::Object::ReferenceEquals(this->fCallbackObj, nullptr))
		this->fCallbackObj->onServerReady(aReadyState);
}

void TStorageServer::onServerLog(const string& aClassName, const string& aFuncName, const string& aMsg){
#ifdef _DEBUG
	if (!System::Object::ReferenceEquals(this->fCallbackObj, nullptr))
		this->fCallbackObj->onServerLog(aClassName, aFuncName, aMsg);

	logToFile(aClassName, aFuncName, aMsg);
#endif
}

void TStorageServer::onServerWarning(const string& aClassName, const string& aFuncName, const string& aMsg){
	unique_lock<mutex> lock(this->fLogMutex);

	if (!System::Object::ReferenceEquals(this->fCallbackObj, nullptr))
		this->fCallbackObj->onServerWarning(aClassName, aFuncName, aMsg);

	warningToFile(aClassName, aFuncName, aMsg);
}

void TStorageServer::onServerError(const string& aClassName, const string& aFuncName, const string& aMsg){
	unique_lock<mutex> lock(this->fLogMutex);

	if (!System::Object::ReferenceEquals(this->fCallbackObj, nullptr))
		this->fCallbackObj->onServerError(aClassName, aFuncName, aMsg);

	errorToFile(aClassName, aFuncName, aMsg);
}

void TStorageServer::onServerCriticalError(const string& aClassName, const string& aFuncName, const string& aMsg){
	unique_lock<mutex> lock(this->fLogMutex);

	if (!System::Object::ReferenceEquals(this->fCallbackObj, nullptr))
		this->fCallbackObj->onServerCriticalError(aClassName, aFuncName, aMsg);

	criticalErrorToFile(aClassName, aFuncName, aMsg);
}
#pragma endregion


#pragma region "IServerExecutorController implementation"
bool TStorageServer::isMessageQueueEmpty(){
	if (this->fExeController != nullptr)
		return this->fExeController->isMessageQueueEmpty();
	else
		return true;
}

TMessageContainer_ptr TStorageServer::getMessageToProcess(){
	TMessageContainer_ptr result;
	if (this->fExeController != nullptr)
		result = this->fExeController->getMessageToProcess();

	return std::move(result);
}

void TStorageServer::sendMessage(TMessageContainer_ptr& aMsg, const bool aCloseAfterSend){
	if (this->fExeController != nullptr)
		this->fExeController->sendMessage(move_TMessageContainer_ptr(aMsg), aCloseAfterSend);
}

void TStorageServer::processRegistrationRequest(TConnection_ptr& aConnection, TUserRegistrReqMessage_ptr& aMsg){
	if (aMsg != nullptr){
		string u = aMsg->getUser();
		string p = aMsg->getPass();
		string path = aMsg->getPath();

		try{
			unique_lock<mutex> lock(this->fLogMutex);

			//Log the message
			this->onServerLog("TStorageServer", "processRegistrationRequest", "<= <= <= <= <= <= <= <= <= <= <= <= <= ");
			this->onServerLog("TStorageServer", "processRegistrationRequest", "<= <= <= <= <= <= <= <= <= <= <= <= <= ");
			this->onServerLog("TStorageServer", "processRegistrationRequest", "<=  ");
			this->onServerLog("TStorageServer", "processRegistrationRequest", "<=   UserRegistrReqMessage ");
			this->onServerLog("TStorageServer", "processRegistrationRequest", "<=  ");
			this->onServerLog("TStorageServer", "processRegistrationRequest", "<=   user: " + u);
			this->onServerLog("TStorageServer", "processRegistrationRequest", "<=   coded pass: " + p);
			this->onServerLog("TStorageServer", "processRegistrationRequest", "<=   root path: " + path);
			this->onServerLog("TStorageServer", "processRegistrationRequest", "<=  ");
			this->onServerLog("TStorageServer", "processRegistrationRequest", "<= <= <= <= <= <= <= <= <= <= <= <= <= ");
			this->onServerLog("TStorageServer", "processRegistrationRequest", "<= <= <= <= <= <= <= <= <= <= <= <= <= ");
		}
		catch (...){ }

		TBaseMessage_ptr reply = nullptr;
		bool close = false;
		try{
			//check if user already registred
			if (!userExists(u)) {
				//store user in DB
				if (this->fDBManager != nullptr){
					this->fDBManager->insertNewUser(u, p, path);
				}
				else{
					this->onServerError("TStorageServer", "processRegistrationRequest", "DBManager is null!");
					reply = new_TUserRegistrReplyMessage_ptr(false);
					close = true;
				}
			}
			else{
				this->onServerWarning("TStorageServer", "processRegistrationRequest", "User " + u + " is already registred");
				reply = new_TUserRegistrReplyMessage_ptr(false);
				close = true;
			}
		}
		catch (EDBException& e){
			this->onServerError("TStorageServer", "processRegistrationRequest", "Unable to insert user in the DB: " + e.getMessage());
			reply = new_TUserRegistrReplyMessage_ptr(false);
			close = true;
		}

		if (reply == nullptr){
			//send back a positive response
			reply = new_TUserRegistrReplyMessage_ptr(true);
		}

		TMessageContainer_ptr replyContainer = new_TMessageContainer_ptr(move_TBaseMessage_ptr(reply), aConnection); //reply is moved
		this->sendMessage(move_TMessageContainer_ptr(replyContainer), close); //replyContainer is moved
	}
	else{
		this->onServerError("TStorageServer", "processRegistrationRequest", "received an empty message; skipped.");
	}
}

void TStorageServer::processUpdateStart(TConnection_ptr& aConnection, TUpdateStartReqMessage_ptr& aMsg){
	if (aMsg != nullptr){
		string u = aMsg->getUser();
		string p = aMsg->getPass();

		try{
			unique_lock<mutex> lock(this->fLogMutex);

			//Log the message
			this->onServerLog("TStorageServer", "processUpdateStart", "<= <= <= <= <= <= <= <= <= <= <= <= <= ");
			this->onServerLog("TStorageServer", "processUpdateStart", "<= <= <= <= <= <= <= <= <= <= <= <= <= ");
			this->onServerLog("TStorageServer", "processUpdateStart", "<=  ");
			this->onServerLog("TStorageServer", "processUpdateStart", "<=   UpdateStartReqMessage ");
			this->onServerLog("TStorageServer", "processUpdateStart", "<=  ");
			this->onServerLog("TStorageServer", "processUpdateStart", "<=   user: " + u);
			this->onServerLog("TStorageServer", "processUpdateStart", "<=   coded pass: " + p);
			this->onServerLog("TStorageServer", "processUpdateStart", "<=  ");
			this->onServerLog("TStorageServer", "processUpdateStart", "<= <= <= <= <= <= <= <= <= <= <= <= <= ");
			this->onServerLog("TStorageServer", "processUpdateStart", "<= <= <= <= <= <= <= <= <= <= <= <= <= ");
		}
		catch (...) { }

		TBaseMessage_ptr reply = nullptr;
		TMessageContainer_ptr replyContainer = nullptr;

		//check if username and password are valid
		try{
			if (!checkUserCredential(u, p)){
				reply = new_TSystemErrorMessage_ptr("Authentication failed; try again!");
			}
		}
		catch (EDBException e){
			this->onServerError("TStorageServer", "processUpdateStart", "Unable to verify credentials: " + e.getMessage());
			reply = new_TSystemErrorMessage_ptr("Unexpected System Error. Try later");
		}

		if (reply != nullptr){
			replyContainer = new_TMessageContainer_ptr(move_TBaseMessage_ptr(reply), aConnection); //reply is moved
			this->sendMessage(move_TMessageContainer_ptr(replyContainer), true); //replyContainer is moved
			return;
		}

		//check if no session for this user is already active
		TSession_ptr session = this->isThereASessionFor(u);
		if (session != nullptr){
			this->onServerError("TStorageServer", "processUpdateStart", "A session is already active for user: " + u);

			//send back negative response
			reply = new_TUpdateStartReplyMessage_ptr(false, "");
			replyContainer = new_TMessageContainer_ptr(move_TBaseMessage_ptr(reply), aConnection); //reply is moved
			this->sendMessage(move_TMessageContainer_ptr(replyContainer), true); //replyContainer is moved
			return;
		}

		//generate a new token and start a new session
		string_ptr token_ptr;
		try{
			token_ptr = getUniqueToken(u);
		}
		catch (EOpensslException e){
			this->onServerError("TStorageServer", "processUpdateStart", "EOpensslException creating a new token: " + e.getMessage() + " ; skipped.");
			token_ptr.reset();

			//system failure
			reply = new_TSystemErrorMessage_ptr("Unexpected System Error. Try later");
			replyContainer = new_TMessageContainer_ptr(move_TBaseMessage_ptr(reply), aConnection); //reply is moved
			this->sendMessage(move_TMessageContainer_ptr(replyContainer), true); //replyContainer is moved
			return;
		}

		bool close = false;
		if (this->newSession(u, token_ptr->c_str(), UPDATE_SESSION)){
			//send back positive response
			reply = new_TUpdateStartReplyMessage_ptr(true, token_ptr->c_str());
			token_ptr.reset();
		}
		else{
			//system failure
			close = true;
			reply = new_TSystemErrorMessage_ptr("Unexpected System Error. Try later");
		}
		replyContainer = new_TMessageContainer_ptr(move_TBaseMessage_ptr(reply), aConnection); //reply is moved
		this->sendMessage(move_TMessageContainer_ptr(replyContainer), close); //replyContainer is moved
	}
	else{
		this->onServerError("TStorageServer", "processUpdateStart", "received an empty message; skipped.");
	}
}

void TStorageServer::processAddNewFile(TConnection_ptr& aConnection, TAddNewFileMessage_ptr& aMsg){
	if (aMsg != nullptr){
		string t = aMsg->getToken();
		string fp = aMsg->getFilePath();
		time_t fd = aMsg->getFileDate(); 

		try{
			unique_lock<mutex> lock(this->fLogMutex);

			//Log the message
			this->onServerLog("TStorageServer", "processAddNewFile", "<= <= <= <= <= <= <= <= <= <= <= <= <= ");
			this->onServerLog("TStorageServer", "processAddNewFile", "<= <= <= <= <= <= <= <= <= <= <= <= <= ");
			this->onServerLog("TStorageServer", "processAddNewFile", "<=  ");
			this->onServerLog("TStorageServer", "processAddNewFile", "<=   AddNewFileMessage ");
			this->onServerLog("TStorageServer", "processAddNewFile", "<=  ");
			this->onServerLog("TStorageServer", "processAddNewFile", "<=   token: " + t);
			this->onServerLog("TStorageServer", "processAddNewFile", "<=   file path: " + fp);
			this->onServerLog("TStorageServer", "processAddNewFile", "<=   file date: " + formatFileDate(fd));
			this->onServerLog("TStorageServer", "processAddNewFile", "<=  ");
			this->onServerLog("TStorageServer", "processAddNewFile", "<= <= <= <= <= <= <= <= <= <= <= <= <= ");
			this->onServerLog("TStorageServer", "processAddNewFile", "<= <= <= <= <= <= <= <= <= <= <= <= <= ");
		}
		catch (...){ }

		TBaseMessage_ptr reply = nullptr;
		TMessageContainer_ptr replyContainer = nullptr;

		//check if token is associated to a valid session
		string u;
		try{
			u = getUserFromToken(t);
		}
		catch (EOpensslException e){
			this->onServerError("TStorageServer", "processAddNewFile", "Error in getUserFromToken(): " + e.getMessage());
			
			//send negative response
			reply = new_TFileAckMessage_ptr(false, fp);
			replyContainer = new_TMessageContainer_ptr(move_TBaseMessage_ptr(reply), aConnection); //reply is moved
			this->sendMessage(move_TMessageContainer_ptr(replyContainer), false); //replyContainer is moved
			return;
		}

		TSession_ptr session = this->isThereAnUpdateSessionFor(u);
		if (session == nullptr){
			this->onServerError("TStorageServer", "processAddNewFile", "There is no update session opened for user: " + u);
			
			//send negative response
			reply = new_TSystemErrorMessage_ptr("Unexpected System Error. Try later");
			replyContainer = new_TMessageContainer_ptr(move_TBaseMessage_ptr(reply), aConnection); //reply is moved
			this->sendMessage(move_TMessageContainer_ptr(replyContainer), true); //replyContainer is moved
			return;
		}

		//check if checksum is correct
		bool checksumMatches = false;
		try{
			checksumMatches = aMsg->verifyChecksum();
		}
		catch (EMessageException e){
			this->onServerError("TStorageServer", "processAddNewFile", "Checksum validation failed: " + e.getMessage());

			//send negative response
			reply = new_TSystemErrorMessage_ptr("Unexpected System Error. Try later");
			replyContainer = new_TMessageContainer_ptr((TBaseMessage_ptr&)move_TBaseMessage_ptr(reply), aConnection); //reply is moved
			this->sendMessage(move_TMessageContainer_ptr(replyContainer), true); //replyContainer is moved
			return;
		}

		bool close = false;
		if (!checksumMatches){
			this->onServerError("TStorageServer", "processAddNewFile", "Checksum mismatch for :" + fp);
			reply = new_TFileAckMessage_ptr(false, fp);
		}
		else{
			//update session object
			int v = session->getVersion() + 1; //to get the new version index
			TFile_ptr file = new_TFile_ptr(u, v, fp, fd);
			path p = file->getServerPathPrefix();
			p /= file->getClientRelativePath();

			//store file in proper location
			try{
				storeFile(p, move_string_ptr(aMsg->getFileContent()));
				session->addFile(move_TFile_ptr(file));
				reply = new_TFileAckMessage_ptr(true, fp);
			}
			catch (EFilesystemException e){
				this->onServerError("TStorageServer", "processAddNewFile", "Storage error: " + e.getMessage());
				reply = new_TSystemErrorMessage_ptr("Unexpected System Error. Try later");
				close = true;
			}
		}

		replyContainer = new_TMessageContainer_ptr((TBaseMessage_ptr&)move_TBaseMessage_ptr(reply), aConnection); //reply is moved
		this->sendMessage(move_TMessageContainer_ptr(replyContainer), close); //replyContainer is moved
	}
	else{
		this->onServerError("TStorageServer", "processAddNewFile", "received an empty message; skipped.");
	}
}

void TStorageServer::processUpdateFile(TConnection_ptr& aConnection, TUpdateFileMessage_ptr& aMsg){
	if (aMsg != nullptr){
		string t = aMsg->getToken();
		string fp = aMsg->getFilePath();
		time_t fd = aMsg->getFileDate();

		try{
			unique_lock<mutex> lock(this->fLogMutex);

			//Log the message
			this->onServerLog("TStorageServer", "processUpdateFile", "<= <= <= <= <= <= <= <= <= <= <= <= <= ");
			this->onServerLog("TStorageServer", "processUpdateFile", "<= <= <= <= <= <= <= <= <= <= <= <= <= ");
			this->onServerLog("TStorageServer", "processUpdateFile", "<=  ");
			this->onServerLog("TStorageServer", "processUpdateFile", "<=   UpdateFileMessage ");
			this->onServerLog("TStorageServer", "processUpdateFile", "<=  ");
			this->onServerLog("TStorageServer", "processUpdateFile", "<=   token: " + t);
			this->onServerLog("TStorageServer", "processUpdateFile", "<=   file path: " + fp);
			this->onServerLog("TStorageServer", "processUpdateFile", "<=   file date: " + formatFileDate(fd));
			this->onServerLog("TStorageServer", "processUpdateFile", "<=  ");
			this->onServerLog("TStorageServer", "processUpdateFile", "<= <= <= <= <= <= <= <= <= <= <= <= <= ");
			this->onServerLog("TStorageServer", "processUpdateFile", "<= <= <= <= <= <= <= <= <= <= <= <= <= ");
		}
		catch (...){ }

		TBaseMessage_ptr reply = nullptr;
		TMessageContainer_ptr replyContainer = nullptr;

		//check if token is associated to a valid session
		string u;
		try{
			u = getUserFromToken(t);
		}
		catch (EOpensslException e){
			this->onServerError("TStorageServer", "processUpdateFile", "Error in getUserFromToken(): " + e.getMessage());

			//send negative response
			reply = new_TSystemErrorMessage_ptr("Unexpected System Error. Try later");
			replyContainer = new_TMessageContainer_ptr(move_TBaseMessage_ptr(reply), aConnection); //reply is moved
			this->sendMessage(move_TMessageContainer_ptr(replyContainer), true); //replyContainer is moved
			return;
		}

		TSession_ptr session = this->isThereAnUpdateSessionFor(u);
		if (session == nullptr){
			this->onServerError("TStorageServer", "processUpdateFile", "There is no update session opened for user: " + u);

			//send negative response
			reply = new_TSystemErrorMessage_ptr("Unexpected System Error. Try later");
			replyContainer = new_TMessageContainer_ptr(move_TBaseMessage_ptr(reply), aConnection); //reply is moved
			this->sendMessage(move_TMessageContainer_ptr(replyContainer), true); //replyContainer is moved
			return;
		}

		//check if checksum is correct
		bool checksumMatches = false;
		try{
			checksumMatches = aMsg->verifyChecksum();
		}
		catch (EMessageException e){
			this->onServerError("TStorageServer", "processUpdateFile", "Checksum validation failed: " + e.getMessage());

			//send negative response
			reply = new_TSystemErrorMessage_ptr("Unexpected System Error. Try later");
			replyContainer = new_TMessageContainer_ptr(move_TBaseMessage_ptr(reply), aConnection); //reply is moved
			this->sendMessage(move_TMessageContainer_ptr(replyContainer), true); //replyContainer is moved
			return;
		}

		bool close = false;
		if (!checksumMatches){
			this->onServerError("TStorageServer", "processUpdateFile", "Checksum mismatch for :" + fp);
			reply = new_TFileAckMessage_ptr(false, fp);
		}
		else{
			//update session object
			int v = session->getVersion() + 1; //to get the new version index
			TFile_ptr file = new_TFile_ptr(u, v, fp, fd);
			path p = file->getServerPathPrefix();
			p /= file->getClientRelativePath();

			//store file in proper location
			try{
				storeFile(p, move_string_ptr(aMsg->getFileContent()));
				if(session->updateFile(move_TFile_ptr(file)))
					reply = new_TFileAckMessage_ptr(true, fp);
				else{
					reply = new_TFileAckMessage_ptr(false, fp);
				}
			}
			catch (EFilesystemException e){
				this->onServerError("TStorageServer", "processAddNewFile", "Storage error: " + e.getMessage());
				reply = new_TSystemErrorMessage_ptr("Unexpected System Error. Try later");
				close = true;
			}
		}

		replyContainer = new_TMessageContainer_ptr((TBaseMessage_ptr&)move_TBaseMessage_ptr(reply), aConnection); //reply is moved
		this->sendMessage(move_TMessageContainer_ptr(replyContainer), close); //replyContainer is moved
	}
	else{
		this->onServerError("TStorageServer", "processUpdateFile", "received an empty message; skipped.");
	}
}

void TStorageServer::processRemoveFile(TConnection_ptr& aConnection, TRemoveFileMessage_ptr& aMsg){
	if (aMsg != nullptr){
		string t = aMsg->getToken();
		string fp = aMsg->getFilePath();

		try{
			unique_lock<mutex> lock(this->fLogMutex);

			//Log the message
			this->onServerLog("TStorageServer", "processRemoveFile", "<= <= <= <= <= <= <= <= <= <= <= <= <= ");
			this->onServerLog("TStorageServer", "processRemoveFile", "<= <= <= <= <= <= <= <= <= <= <= <= <= ");
			this->onServerLog("TStorageServer", "processRemoveFile", "<=  ");
			this->onServerLog("TStorageServer", "processRemoveFile", "<=   RemoveFileMessage ");
			this->onServerLog("TStorageServer", "processRemoveFile", "<=  ");
			this->onServerLog("TStorageServer", "processRemoveFile", "<=   token: " + t);
			this->onServerLog("TStorageServer", "processRemoveFile", "<=   file path: " + fp);
			this->onServerLog("TStorageServer", "processRemoveFile", "<=  ");
			this->onServerLog("TStorageServer", "processRemoveFile", "<= <= <= <= <= <= <= <= <= <= <= <= <= ");
			this->onServerLog("TStorageServer", "processRemoveFile", "<= <= <= <= <= <= <= <= <= <= <= <= <= ");
		}
		catch (...){ }

		TBaseMessage_ptr reply = nullptr;
		TMessageContainer_ptr replyContainer = nullptr;

		//check if token is associated to a valid session
		string u;
		try{
			u = getUserFromToken(t);
		}
		catch (EOpensslException e){
			this->onServerError("TStorageServer", "processRemoveFile", "Error in getUserFromToken(): " + e.getMessage());
			
			//send negative response
			reply = new_TSystemErrorMessage_ptr("Unexpected System Error. Try later");
			replyContainer = new_TMessageContainer_ptr(move_TBaseMessage_ptr(reply), aConnection); //reply is moved
			this->sendMessage(move_TMessageContainer_ptr(replyContainer), true); //replyContainer is moved
			return;
		}

		TSession_ptr session = this->isThereAnUpdateSessionFor(u);
		if (session == nullptr){
			this->onServerError("TStorageServer", "processRemoveFile", "There is no update session opened for user: " + u);

			//send negative response
			reply = new_TSystemErrorMessage_ptr("Unexpected System Error. Try later");
			replyContainer = new_TMessageContainer_ptr(move_TBaseMessage_ptr(reply), aConnection); //reply is moved
			this->sendMessage(move_TMessageContainer_ptr(replyContainer), true); //replyContainer is moved
			return;
		}

		//update session object
		bool close = false;
		//TODO: aggiungere try-catch per errori filesystem
		int v = session->getVersion();
		TFile_ptr file = new_TFile_ptr(u, v, fp, time(NULL));
		session->removeFile(move_TFile_ptr(file));

		//send a positive response
		reply = new_TFileAckMessage_ptr(true, fp);
		replyContainer = new_TMessageContainer_ptr((TBaseMessage_ptr&)move_TBaseMessage_ptr(reply), aConnection); //reply is moved
		this->sendMessage(move_TMessageContainer_ptr(replyContainer), close); //replyContainer is moved
	}
	else{
		this->onServerError("TStorageServer", "processRemoveFile", "received an empty message; skipped.");
	}
}

void TStorageServer::processUpdateStop(TConnection_ptr& aConnection, TUpdateStopReqMessage_ptr& aMsg){
	if (aMsg != nullptr){
		string t = aMsg->getToken();

		try{
			unique_lock<mutex> lock(this->fLogMutex);

			//Log the message
			this->onServerLog("TStorageServer", "processUpdateStop", "<= <= <= <= <= <= <= <= <= <= <= <= <= ");
			this->onServerLog("TStorageServer", "processUpdateStop", "<= <= <= <= <= <= <= <= <= <= <= <= <= ");
			this->onServerLog("TStorageServer", "processUpdateStop", "<=  ");
			this->onServerLog("TStorageServer", "processUpdateStop", "<=   UpdateStopReqMessage ");
			this->onServerLog("TStorageServer", "processUpdateStop", "<=  ");
			this->onServerLog("TStorageServer", "processUpdateStop", "<=   token: " + t);
			this->onServerLog("TStorageServer", "processUpdateStop", "<=  ");
			this->onServerLog("TStorageServer", "processUpdateStop", "<= <= <= <= <= <= <= <= <= <= <= <= <= ");
			this->onServerLog("TStorageServer", "processUpdateStop", "<= <= <= <= <= <= <= <= <= <= <= <= <= ");
		}
		catch (...){ }

		TBaseMessage_ptr reply = nullptr;
		TMessageContainer_ptr replyContainer = nullptr;

		//check if token is associated to a valid session
		string u;
		try{
			u = getUserFromToken(t);
		}
		catch (EOpensslException e){
			this->onServerError("TStorageServer", "processUpdateStop", "Error in getUserFromToken(): " + e.getMessage());

			//send back negative response
			reply = new_TSystemErrorMessage_ptr("Unexpected System Error. Try later");
			replyContainer = new_TMessageContainer_ptr(move_TBaseMessage_ptr(reply), aConnection); //reply is moved
			this->sendMessage(move_TMessageContainer_ptr(replyContainer), true); //replyContainer is moved
			return;
		}

		TSession_ptr session = this->isThereAnUpdateSessionFor(u);
		if (session == nullptr){
			this->onServerError("TStorageServer", "processUpdateStop", "There is no update session opened for user: " + u);

			//send back negative response
			reply = new_TSystemErrorMessage_ptr("Unexpected System Error. Try later");
			replyContainer = new_TMessageContainer_ptr(move_TBaseMessage_ptr(reply), aConnection); //reply is moved
			this->sendMessage(move_TMessageContainer_ptr(replyContainer), true); //replyContainer is moved
			return;
		}

		//end update session and store modifictions permanently
		TVersion_ptr v = session->terminateWithSuccess(false);
		this->removeSession(u);
		int vID = v->getVersion();
		time_t vDate = v->getDate();
		bool ok = true;
		try{
			if (this->fDBManager != nullptr){
				this->fDBManager->InsertNewVersion(u, move_TVersion_ptr(v));
			}
			else{
				ok = false;
				this->onServerError("TStorageServer", "processUpdateStop", "DBManager is null!");
			}
		}
		catch (EDBException e){
			ok = false;
			this->onServerError("TStorageServer", "processUpdateStop", "Unable to create a new version: " + e.getMessage());
		}

		if (ok){
			// send back info about the newly created version
			reply = new_TUpdateStopReplyMessage_ptr(true, vID, vDate);
			replyContainer = new_TMessageContainer_ptr((TBaseMessage_ptr&)move_TBaseMessage_ptr(reply), aConnection); //reply is moved
			this->sendMessage(move_TMessageContainer_ptr(replyContainer), false); //replyContainer is moved
		}
		else{
			path p(buildServerPathPrefix(u, vID));
			try{
				removeDir(p);
			}
			catch (EFilesystemException e){
				this->onServerError("TStorageServer", "processUpdateStop", e.getMessage());
			}

			//send back negative response
			reply = new_TUpdateStopReplyMessage_ptr(false, NO_VERSION, time(nullptr));
			replyContainer = new_TMessageContainer_ptr((TBaseMessage_ptr&)move_TBaseMessage_ptr(reply), aConnection); //reply is moved
			this->sendMessage(move_TMessageContainer_ptr(replyContainer), true); //replyContainer is moved
		}
	}
	else{
		this->onServerError("TStorageServer", "processUpdateStop", "received an empty message; skipped.");
	}
}

void TStorageServer::processGetVersions(TConnection_ptr& aConnection, TGetVersionsReqMessage_ptr& aMsg){
	if (aMsg != nullptr){
		string u = aMsg->getUser();
		string p = aMsg->getPass();

		try{
			unique_lock<mutex> lock(this->fLogMutex);

			//Log the message
			this->onServerLog("TStorageServer", "processGetVersions", "<= <= <= <= <= <= <= <= <= <= <= <= <= ");
			this->onServerLog("TStorageServer", "processGetVersions", "<= <= <= <= <= <= <= <= <= <= <= <= <= ");
			this->onServerLog("TStorageServer", "processGetVersions", "<=  ");
			this->onServerLog("TStorageServer", "processGetVersions", "<=   GetVersionsReqMessage ");
			this->onServerLog("TStorageServer", "processGetVersions", "<=  ");
			this->onServerLog("TStorageServer", "processGetVersions", "<=   user: " + u);
			this->onServerLog("TStorageServer", "processGetVersions", "<=   coded pass: " + p);
			this->onServerLog("TStorageServer", "processGetVersions", "<=  ");
			this->onServerLog("TStorageServer", "processGetVersions", "<= <= <= <= <= <= <= <= <= <= <= <= <= ");
			this->onServerLog("TStorageServer", "processGetVersions", "<= <= <= <= <= <= <= <= <= <= <= <= <= ");
		}
		catch (...){ }

		TBaseMessage_ptr reply = nullptr;
		TMessageContainer_ptr replyContainer = nullptr;

		//check if username and password are valid
		try{
			if (checkUserCredential(u, p) == nullptr){
				reply = new_TSystemErrorMessage_ptr("Authentication failed; try again!");
			}
		}
		catch (EDBException e){
			this->onServerError("TStorageServer", "processGetVersions", "Unable to verify credentials: " + e.getMessage());
			reply = new_TSystemErrorMessage_ptr("Unexpected System Error. Try later");
		}

		if (reply != nullptr){
			replyContainer = new_TMessageContainer_ptr(move_TBaseMessage_ptr(reply), aConnection); //reply is moved
			this->sendMessage(move_TMessageContainer_ptr(replyContainer), true); //replyContainer is moved
			return;
		}

		//get all versions of this user and send back the response
		TVersionList_ptr vers;
		try{
			if (this->fDBManager != nullptr)
				vers = move_TVersionList_ptr(this->fDBManager->getAllVersions(u));
			else{
				reply = new_TSystemErrorMessage_ptr("Unexpected System Error. Try later");
				this->onServerError("TStorageServer", "processGetVersions", "DBManager is null!");
			}
		}
		catch (EDBException& e){
			this->onServerError("TStorageServer", "processGetVersions", "Unable to get versions for this user: " + e.getMessage());
			reply = new_TSystemErrorMessage_ptr("Unexpected System Error. Try later");
		}

		if (reply != nullptr){
			replyContainer = new_TMessageContainer_ptr(move_TBaseMessage_ptr(reply), aConnection); //reply is moved
			this->sendMessage(move_TMessageContainer_ptr(replyContainer), true); //replyContainer is moved
			return;
		}

		int totVers = 0;
		int oldest = 0;
		int last = 0;
		if (vers != nullptr){
			totVers = (int)vers->size();
			oldest = 1;
			last = totVers;
		}

		reply = new_TGetVersionsReplyMessage_ptr(totVers, oldest, last, vers);
		replyContainer = new_TMessageContainer_ptr(move_TBaseMessage_ptr(reply), aConnection); //reply is moved
		this->sendMessage(move_TMessageContainer_ptr(replyContainer), false); //replyContainer is moved
	}
	else{
		this->onServerError("TStorageServer", "processGetVersions", "received an empty message; skipped.");
	}
}

void TStorageServer::processGetLastVersion(TConnection_ptr& aConnection, TGetLastVerReqMessage_ptr& aMsg){
	if (aMsg != nullptr){
		string u = aMsg->getUser();
		string p = aMsg->getPass();

		try{
			unique_lock<mutex> lock(this->fLogMutex);

			//Log the message
			this->onServerLog("TStorageServer", "processGetLastVersion", "<= <= <= <= <= <= <= <= <= <= <= <= <= ");
			this->onServerLog("TStorageServer", "processGetLastVersion", "<= <= <= <= <= <= <= <= <= <= <= <= <= ");
			this->onServerLog("TStorageServer", "processGetLastVersion", "<=  ");
			this->onServerLog("TStorageServer", "processGetLastVersion", "<=   GetLastVersionReqMessage ");
			this->onServerLog("TStorageServer", "processGetLastVersion", "<=  ");
			this->onServerLog("TStorageServer", "processGetLastVersion", "<=   user: " + u);
			this->onServerLog("TStorageServer", "processGetLastVersion", "<=   coded pass: " + p);
			this->onServerLog("TStorageServer", "processGetLastVersion", "<=  ");
			this->onServerLog("TStorageServer", "processGetLastVersion", "<= <= <= <= <= <= <= <= <= <= <= <= <= ");
			this->onServerLog("TStorageServer", "processGetLastVersion", "<= <= <= <= <= <= <= <= <= <= <= <= <= ");
		}
		catch (...){ }

		TBaseMessage_ptr reply = nullptr;
		TMessageContainer_ptr replyContainer = nullptr;

		//check if username and password are valid
		try{
			if (checkUserCredential(u, p) == nullptr){
				reply = new_TSystemErrorMessage_ptr("Authentication failed; try again!");
			}
		}
		catch (EDBException e){
			this->onServerError("TStorageServer", "processGetLastVersion", "Unable to verify credentials: " + e.getMessage());
			reply = new_TSystemErrorMessage_ptr("Unexpected System Error. Try later");
		}

		if (reply != nullptr){
			//send back an empty response
			replyContainer = new_TMessageContainer_ptr((TBaseMessage_ptr&)move_TBaseMessage_ptr(reply), aConnection); //reply is moved
			this->sendMessage(move_TMessageContainer_ptr(replyContainer), false); //replyContainer is moved
			return;
		}

		//get last version for this user and send back the response
		bool err = false;
		TVersion_ptr ver;
		try{
			if (this->fDBManager != nullptr)
				ver = move_TVersion_ptr(this->fDBManager->getLastVersion(u, false));
			else{
				err = true;
				this->onServerError("TStorageServer", "processGetLastVersion", "DBManager is null!");
			}
		}
		catch (EDBException& e){
			this->onServerError("TStorageServer", "processGetLastVersion", "Unable to get versions for this user: " + e.getMessage());
			err = true;
		}

		bool close = false;
		if (!err && ver != nullptr){
			//positive response
			reply = new_TGetLastVerReplyMessage_ptr(ver->getVersion(), ver->getDate());
			ver.reset();
		}
		else{
			if (err) {
				close = true;
				reply = new_TSystemErrorMessage_ptr("Unexpected System Error. Try later");
			}
			else {
				//empty response
				reply = new_TGetLastVerReplyMessage_ptr(-1, time(NULL));
			}
		}

		replyContainer = new_TMessageContainer_ptr(move_TBaseMessage_ptr(reply), aConnection); //reply is moved
		this->sendMessage(move_TMessageContainer_ptr(replyContainer), close); //replyContainer is moved
	}
	else{
		this->onServerError("TStorageServer", "processGetLastVersion", "received an empty message; skipped.");
	}
}

void TStorageServer::processRestoreVersion(TConnection_ptr& aConnection, TRestoreVerReqMessage_ptr& aMsg){
	if (aMsg != nullptr){
		string u = aMsg->getUser();
		string p = aMsg->getPass();
		int v = aMsg->getVersion();

		try{
			unique_lock<mutex> lock(this->fLogMutex);

			//Log the message
			this->onServerLog("TStorageServer", "processRestoreVersion", "<= <= <= <= <= <= <= <= <= <= <= <= <= ");
			this->onServerLog("TStorageServer", "processRestoreVersion", "<= <= <= <= <= <= <= <= <= <= <= <= <= ");
			this->onServerLog("TStorageServer", "processRestoreVersion", "<=  ");
			this->onServerLog("TStorageServer", "processRestoreVersion", "<=   RestoreVerReqMessage ");
			this->onServerLog("TStorageServer", "processRestoreVersion", "<=  ");
			this->onServerLog("TStorageServer", "processRestoreVersion", "<=   user: " + u);
			this->onServerLog("TStorageServer", "processRestoreVersion", "<=   coded pass: " + p);
			this->onServerLog("TStorageServer", "processRestoreVersion", "<=   required version: " + to_string(v));
			this->onServerLog("TStorageServer", "processRestoreVersion", "<=  ");
			this->onServerLog("TStorageServer", "processRestoreVersion", "<= <= <= <= <= <= <= <= <= <= <= <= <= ");
			this->onServerLog("TStorageServer", "processRestoreVersion", "<= <= <= <= <= <= <= <= <= <= <= <= <= ");
		}
		catch (...){ }

		TBaseMessage_ptr reply = nullptr;
		TMessageContainer_ptr replyContainer = nullptr;

		//check if username and password are valid
		try{
			if (checkUserCredential(u, p) == nullptr){
				reply = new_TSystemErrorMessage_ptr("Authentication failed; try again!");
			}
		}
		catch (EDBException e){
			this->onServerError("TStorageServer", "processRestoreVersion", "Unable to verify credentials: " + e.getMessage());
			reply = new_TSystemErrorMessage_ptr("Unexpected System Error. Try later");
		}

		if (reply != nullptr){
			replyContainer = new_TMessageContainer_ptr(move_TBaseMessage_ptr(reply), aConnection); //reply is moved
			this->sendMessage(move_TMessageContainer_ptr(replyContainer), false); //replyContainer is moved
			return;
		}

		//check if no session for this user is already active
		TSession_ptr session = this->isThereASessionFor(u);
		if (session != nullptr){
			this->onServerError("TStorageServer", "processRestoreVersion", "A session is already active for user: " + u);

			reply = new_TSystemErrorMessage_ptr("Unexpected System Error. Try later");
			TMessageContainer_ptr replyContainer = new_TMessageContainer_ptr(move_TBaseMessage_ptr(reply), aConnection); //reply is moved
			this->sendMessage(move_TMessageContainer_ptr(replyContainer), true); //replyContainer is moved
			return;
		}

		//check if required version is available
		TVersion_ptr ver;
		try{
			if (this->fDBManager != nullptr)
				ver = this->fDBManager->getVersion(u, v);
			else{
				reply = new_TSystemErrorMessage_ptr("Unexpected System Error. Try later");
				this->onServerError("TStorageServer", "processRestoreVersion", "DBManager is null!");
			}
		}
		catch (EDBException& e){
			this->onServerError("TStorageServer", "processRestoreVersion", "required version for user " + u + " is not available: " + e.getMessage());
			reply = new_TSystemErrorMessage_ptr("Unexpected System Error. Try later");
		}

		if (ver != nullptr && reply == nullptr){
			ver.reset();

			//generate a new token and start a new session
			string_ptr token_ptr;
			try{
				token_ptr = getUniqueToken(u);
			}
			catch (EOpensslException e){
				this->onServerError("TStorageServer", "processRestoreVersion", "EOpensslException creating a new token: " + e.getMessage() + " ; skipped.");
				reply = new_TSystemErrorMessage_ptr("Unexpected System Error. Try later");
			}

			bool close = true;
			bool newSess = false;
			if (reply == nullptr){
				newSess = this->newSession(u, token_ptr->c_str(), RESTORE_SESSION);
				if (newSess){
					//send back positive response
					reply = new_TRestoreVerReplyMessage_ptr(true, token_ptr->c_str());
					token_ptr.reset();
					close = false;
				}
				else{
					//send back negative response
					reply = new_TSystemErrorMessage_ptr("Unexpected System Error. Try later");
				}
			}

			TMessageContainer_ptr replyContainer = new_TMessageContainer_ptr(move_TBaseMessage_ptr(reply), aConnection); //reply is moved
			this->sendMessage(move_TMessageContainer_ptr(replyContainer), close); //replyContainer is moved

			if (newSess){
				session = this->isThereARestoreSessionFor(u);
				//send the first file of the required version
				TFile_ptr f = session->getNextFileToSend();
				if (f != nullptr){
					TRestoreFileMessage_ptr first = new_TRestoreFileMessage_ptr(f->getServerPathPrefix(), f->getClientRelativePath(), f->getLastMod());
					TMessageContainer_ptr firstContainer = new_TMessageContainer_ptr((TBaseMessage_ptr&)move_TBaseMessage_ptr(first), aConnection);
					this->sendMessage(move_TMessageContainer_ptr(firstContainer), false);
				}
			}
		}
		else{
			ver.reset();
			if (reply == nullptr) //it means no version for the user
				reply = new_TRestoreVerReplyMessage_ptr(false, "");

			replyContainer = new_TMessageContainer_ptr((TBaseMessage_ptr&)move_TBaseMessage_ptr(reply), aConnection); //reply is moved
			this->sendMessage(move_TMessageContainer_ptr(replyContainer), true); //replyContainer is moved
		}
	}
	else{
		this->onServerError("TStorageServer", "processRestoreVersion", "received an empty message; skipped.");
	}
}

void TStorageServer::processRestoreFileAck(TConnection_ptr& aConnection, TRestoreFileAckMessage_ptr& aMsg){
	if (aMsg != nullptr){
		string t = aMsg->getToken();
		string fp = aMsg->getFilePath();
		bool r = aMsg->getResp();

		try{
			unique_lock<mutex> lock(this->fLogMutex);

			//Log the message
			this->onServerLog("TStorageServer", "processRestoreFileAck", "<= <= <= <= <= <= <= <= <= <= <= <= <= ");
			this->onServerLog("TStorageServer", "processRestoreFileAck", "<= <= <= <= <= <= <= <= <= <= <= <= <= ");
			this->onServerLog("TStorageServer", "processRestoreFileAck", "<=  ");
			this->onServerLog("TStorageServer", "processRestoreFileAck", "<=   RestoreFileAckMessage ");
			this->onServerLog("TStorageServer", "processRestoreFileAck", "<=  ");
			this->onServerLog("TStorageServer", "processRestoreFileAck", "<=   token: " + t);
			this->onServerLog("TStorageServer", "processRestoreFileAck", "<=   file path: " + fp);
			if (r)
				this->onServerLog("TStorageServer", "processRestoreFileAck", "<=   result: ok");
			else
				this->onServerLog("TStorageServer", "processRestoreFileAck", "<=   result: error");
			this->onServerLog("TStorageServer", "processRestoreFileAck", "<=  ");
			this->onServerLog("TStorageServer", "processRestoreFileAck", "<= <= <= <= <= <= <= <= <= <= <= <= <= ");
			this->onServerLog("TStorageServer", "processRestoreFileAck", "<= <= <= <= <= <= <= <= <= <= <= <= <= ");
		}
		catch (...){ }

		TBaseMessage_ptr reply = nullptr;
		TMessageContainer_ptr replyContainer = nullptr;

		//check if token is associated to a valid session
		string u;
		try{
			u = getUserFromToken(t);
		}
		catch (EOpensslException e){
			this->onServerError("TStorageServer", "processRestoreFileAck", "Error in getUserFromToken(): " + e.getMessage());
			reply = new_TSystemErrorMessage_ptr("Unexpected System Error. Try later");
		}

		TSession_ptr session = this->isThereARestoreSessionFor(u);
		if (session == nullptr){
			this->onServerError("TStorageServer", "processRestoreFileAck", "There is no restore session opened for user: " + u);
			reply = new_TSystemErrorMessage_ptr("Unexpected System Error. Try later");
		}

		if (reply != nullptr){
			replyContainer = new_TMessageContainer_ptr(move_TBaseMessage_ptr(reply), aConnection); //reply is moved
			this->sendMessage(move_TMessageContainer_ptr(replyContainer), false); //replyContainer is moved
			return;
		}

		//if result is ok, send the next file, otherwise send this one again
		TFile_ptr f;
		if (r){
			f = session->updateNextFileToSend();
		}
		else
			f = session->getNextFileToSend();

		if (f != nullptr){
			TRestoreFileMessage_ptr next = new_TRestoreFileMessage_ptr(f->getServerPathPrefix(), f->getClientRelativePath(), f->getLastMod());
			TMessageContainer_ptr nextContainer = new_TMessageContainer_ptr((TBaseMessage_ptr&)move_TBaseMessage_ptr(next), aConnection);
			this->sendMessage(move_TMessageContainer_ptr(nextContainer), false);

			f.reset();
		}
		else{
			TVersion_ptr v = session->terminateWithSuccess(true);
			this->removeSession(u);
			TRestoreStopMessage_ptr next = new_TRestoreStopMessage_ptr(v->getVersion(), v->getDate());
			TMessageContainer_ptr nextContainer = new_TMessageContainer_ptr((TBaseMessage_ptr&)move_TBaseMessage_ptr(next), aConnection);
			this->sendMessage(move_TMessageContainer_ptr(nextContainer), true);
		}
	}
	else{
		this->onServerError("TStorageServer", "processRestoreFileAck", "received an empty message; skipped.");
	}
}

void TStorageServer::processPingRequest(TConnection_ptr& aConnection, TPingReqMessage_ptr& aMsg){
	if (aMsg != nullptr){
		string t = formatFileDate(aMsg->getTime());
		string tok = aMsg->getToken();

		try{
			unique_lock<mutex> lock(this->fLogMutex);

			//Log the message
			this->onServerLog("TStorageServer", "processPingRequest", "<= <= <= <= <= <= <= <= <= <= <= <= <= ");
			this->onServerLog("TStorageServer", "processPingRequest", "<= <= <= <= <= <= <= <= <= <= <= <= <= ");
			this->onServerLog("TStorageServer", "processPingRequest", "<=  ");
			this->onServerLog("TStorageServer", "processPingRequest", "<=   PingReqMessage ");
			this->onServerLog("TStorageServer", "processPingRequest", "<=  ");
			this->onServerLog("TStorageServer", "processPingRequest", "<=   time: " + t);
			this->onServerLog("TStorageServer", "processPingRequest", "<=   token: " + tok);
			this->onServerLog("TStorageServer", "processPingRequest", "<=  ");
			this->onServerLog("TStorageServer", "processPingRequest", "<= <= <= <= <= <= <= <= <= <= <= <= <= ");
			this->onServerLog("TStorageServer", "processPingRequest", "<= <= <= <= <= <= <= <= <= <= <= <= <= ");
		}
		catch (...){ }

		TBaseMessage_ptr reply = nullptr;
		TMessageContainer_ptr replyContainer = nullptr;

		//check if token is associated to a valid session
		string u;
		try{
			u = getUserFromToken(tok);
		}
		catch (EOpensslException e){
			this->onServerError("TStorageServer", "processPingRequest", "Error in getUserFromToken(): " + e.getMessage());

			//system failure
			reply = new_TSystemErrorMessage_ptr("Unexpected System Error. Try later");
			replyContainer = new_TMessageContainer_ptr(move_TBaseMessage_ptr(reply), aConnection); //reply is moved
			this->sendMessage(move_TMessageContainer_ptr(replyContainer), true); //replyContainer is moved
			return;
		}

		TSession_ptr session = this->isThereASessionFor(u);
		if (session != nullptr){
			session->setLastPing(aMsg->getTime());

			//send ping reply
			reply = new_TPingReplyMessage_ptr(tok);
			TMessageContainer_ptr replyContainer = new_TMessageContainer_ptr(move_TBaseMessage_ptr(reply), aConnection); //reply is moved
			this->sendMessage(move_TMessageContainer_ptr(replyContainer), false); //replyContainer is moved
		}
		else
			this->onServerError("TStorageServer", "processPingRequest", "There is no session opened for user: " + u);
	}
	else{
		this->onServerError("TStorageServer", "processPingRequest", "received an empty message; skipped.");
	}
}

void TStorageServer::processVerifyCred(TConnection_ptr& aConnection, TVerifyCredReqMessage_ptr& aMsg){
	if (aMsg != nullptr){
		string u = aMsg->getUser();
		string p = aMsg->getPass();

		try{
			unique_lock<mutex> lock(this->fLogMutex);

			//Log the message
			this->onServerLog("TStorageServer", "processRegistrationRequest", "<= <= <= <= <= <= <= <= <= <= <= <= <= ");
			this->onServerLog("TStorageServer", "processRegistrationRequest", "<= <= <= <= <= <= <= <= <= <= <= <= <= ");
			this->onServerLog("TStorageServer", "processRegistrationRequest", "<=  ");
			this->onServerLog("TStorageServer", "processRegistrationRequest", "<=   VerifyCredReqMessage ");
			this->onServerLog("TStorageServer", "processRegistrationRequest", "<=  ");
			this->onServerLog("TStorageServer", "processRegistrationRequest", "<=   user: " + u);
			this->onServerLog("TStorageServer", "processRegistrationRequest", "<=   coded pass: " + p);
			this->onServerLog("TStorageServer", "processRegistrationRequest", "<=  ");
			this->onServerLog("TStorageServer", "processRegistrationRequest", "<= <= <= <= <= <= <= <= <= <= <= <= <= ");
			this->onServerLog("TStorageServer", "processRegistrationRequest", "<= <= <= <= <= <= <= <= <= <= <= <= <= ");
		}
		catch (...){ }

		TBaseMessage_ptr reply = nullptr;
		bool close = false;
		string_ptr path = nullptr;
		//check if username and password are valid
		try{
			path = checkUserCredential(u, p);
			if (path == nullptr){
				reply = new_TVerifyCredReplyMessage_ptr(false, "");
				close = true;
			}
		}
		catch (EDBException e){
			this->onServerError("TStorageServer", "processGetLastVersion", "Unable to verify credentials: " + e.getMessage());
			reply = new_TVerifyCredReplyMessage_ptr(false, "");
			close = true;
		}

		if (reply == nullptr){
			//send back a positive response
			reply = new_TVerifyCredReplyMessage_ptr(true, path->c_str());
		}
		path.reset();

		TMessageContainer_ptr replyContainer = new_TMessageContainer_ptr(move_TBaseMessage_ptr(reply), aConnection); //reply is moved
		this->sendMessage(move_TMessageContainer_ptr(replyContainer), close); //replyContainer is moved
	}
	else{
		this->onServerError("TStorageServer", "processVerifyCred", "received an empty message; skipped.");
	}
}
#pragma endregion
#pragma endregion