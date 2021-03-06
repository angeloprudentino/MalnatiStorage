/*
 * Authors: Angelo Prudentino & Daniele Testa
 * Date: 14/09/2016
 * File: StorageClientCore.cpp
 * Description: this file contains all classes implementing
 *              client-side core logic
 *
 */
#include <boost\bind.hpp>
#include <boost/filesystem.hpp>
#include <boost/thread/thread.hpp>
#include "StorageClient.h"

#define MAX_TRY 5

using namespace System;
using namespace std;
using namespace boost;
using namespace boost::asio;
using namespace boost::asio::ip;
using namespace boost::filesystem;


//////////////////////////////////
//       TStorageClient	        //
//////////////////////////////////
#pragma region "TStorageClient"
void TStorageClient::onLoginSuccess(String^ aPath){
#ifdef _DEBUG
	logToFile("TStorageClient", "onLoginSuccess", marshalString(aPath));
#endif

	if (!System::Object::ReferenceEquals(this->fCallbackObj, nullptr))
		this->fCallbackObj->onLoginSuccess(aPath);
	else {
		errorToFile("TStorageClient", "onLoginError", "Callback objest is null!");
		this->disconnect();
	}
}

void TStorageClient::onLoginError(String^ aMsg){
#ifdef _DEBUG
	logToFile("TStorageClient", "onLoginError", marshalString(aMsg));
#endif
	if (!System::Object::ReferenceEquals(this->fCallbackObj, nullptr))
		this->fCallbackObj->onLoginError(aMsg);
	else
		errorToFile("TStorageClient", "onLoginError", "Callback objest is null!");

	this->disconnect();
}

void TStorageClient::onRegistrationSucces(){
#ifdef _DEBUG
	logToFile("TStorageClient", "onRegistrationSucces", "onRegistrationSucces");
#endif

	if (!System::Object::ReferenceEquals(this->fCallbackObj, nullptr))
		this->fCallbackObj->onRegistrationSucces();
	else {
		errorToFile("TStorageClient", "onRegistrationSucces", "Callback objest is null!");
		this->disconnect();
	}
}

void TStorageClient::onRegistrationError(String^ aMsg){
#ifdef _DEBUG
	logToFile("TStorageClient", "onRegistrationError", marshalString(aMsg));
#endif

	if (!System::Object::ReferenceEquals(this->fCallbackObj, nullptr))
		this->fCallbackObj->onRegistrationError(aMsg);
	else
		errorToFile("TStorageClient", "onRegistrationError", "Callback objest is null!");

	this->disconnect();
}

void TStorageClient::onUpdateStart(String^ aToken){
#ifdef _DEBUG
	logToFile("TStorageClient", "onUpdateStart", marshalString(aToken));
#endif

	if (!System::Object::ReferenceEquals(this->fCallbackObj, nullptr))
		this->fCallbackObj->onUpdateStart(aToken);
	else {
		errorToFile("TStorageClient", "onUpdateStart", "Callback objest is null!");
		this->disconnect();
	}
}

void TStorageClient::onUpdateSuccess(List<UserFile^>^ aFileList, const int aVersion, String^ aVersionDate){
#ifdef _DEBUG
	logToFile("TStorageClient", "onUpdateSuccess", "Version " + to_string(aVersion) + " @ " + marshalString(aVersionDate));
#endif

	if (!System::Object::ReferenceEquals(this->fCallbackObj, nullptr))
		this->fCallbackObj->onUpdateSuccess(aFileList, aVersion, aVersionDate);
	else {
		errorToFile("TStorageClient", "onUpdateSuccess", "Callback objest is null!");
		this->disconnect();
	}
}

void TStorageClient::onUpdateError(String^ aMsg){
#ifdef _DEBUG
	logToFile("TStorageClient", "onUpdateError", marshalString(aMsg));
#endif

	if (!System::Object::ReferenceEquals(this->fCallbackObj, nullptr))
		this->fCallbackObj->onUpdateError(aMsg);
	else{
		errorToFile("TStorageClient", "onUpdateError", "Callback objest is null!");
		this->disconnect();
	}
}

void TStorageClient::onRestoreStart(String^ aToken, const bool aStoreOnLocalDB){
#ifdef _DEBUG
	logToFile("TStorageClient", "onRestoreStart", marshalString(aToken));
#endif

	if (!System::Object::ReferenceEquals(this->fCallbackObj, nullptr)){
		if (!aStoreOnLocalDB)
			this->fCallbackObj->onRestoreStart(aToken);
		else
			this->onUpdateStart(aToken);
	}
	else {
		errorToFile("TStorageClient", "onRestoreStart", "Callback objest is null!");
		this->disconnect();
	}
}

void TStorageClient::onRestoreSuccess(const int aVersion, String^ aVersionDate, const bool aStoreOnLocalDB, List<UserFile^>^ aFileList){
#ifdef _DEBUG
	logToFile("TStorageClient", "onRestoreSuccess", "Version " + to_string(aVersion) + " @ " + marshalString(aVersionDate));
#endif

	if (!System::Object::ReferenceEquals(this->fCallbackObj, nullptr)) {
		if (!aStoreOnLocalDB)
			this->fCallbackObj->onRestoreSuccess(aFileList, aVersion, aVersionDate); 
		else
			this->onUpdateSuccess(aFileList, (-aVersion), aVersionDate);
	}
	else {
		errorToFile("TStorageClient", "onRestoreSuccess", "Callback objest is null!");
		this->disconnect();
	}
}

void TStorageClient::onRestoreError(String^ aMsg, const bool aStoreOnLocalDB){
#ifdef _DEBUG
	logToFile("TStorageClient", "onRestoreError", marshalString(aMsg));
#endif

	if (!System::Object::ReferenceEquals(this->fCallbackObj, nullptr)) {
		if (!aStoreOnLocalDB)
			this->fCallbackObj->onRestoreError(aMsg);
		else
			this->onUpdateError(aMsg);
	}
	else{
		errorToFile("TStorageClient", "onRestoreError", "Callback objest is null!");
		this->disconnect();
	}
}

void TStorageClient::onGetVersionsSuccess(List<UserVersion^>^ aVersionsList){
#ifdef _DEBUG
	logToFile("TStorageClient", "onGetVersionsSuccess", "onGetVersionsSuccess");
#endif

	if (!System::Object::ReferenceEquals(this->fCallbackObj, nullptr))
		this->fCallbackObj->onGetVersionsSuccess(aVersionsList);
	else {
		errorToFile("TStorageClient", "onGetVersionsSuccess", "Callback objest is null!");
		this->disconnect();
	}
}

void TStorageClient::onGetVersionsError(String^ aMsg){
#ifdef _DEBUG
	logToFile("TStorageClient", "onGetVersionsError", marshalString(aMsg));
#endif

	if (!System::Object::ReferenceEquals(this->fCallbackObj, nullptr))
		this->fCallbackObj->onGetVersionsError(aMsg);
	else{
		errorToFile("TStorageClient", "onGetVersionsError", "Callback objest is null!");
		this->disconnect();
	}
}

void TStorageClient::connect(const string& aHost, int aPort){
	if (aHost.empty()){
		errorToFile("TStorageClient", "connect", "host to connect is empty!");
		return;
	}
	if (aPort < 0){
		errorToFile("TStorageClient", "connect", "port to connect is not valid!");
		return;
	}

	if (this->fSock == nullptr)
		this->fSock = new tcp::socket(this->fMainIoService);
		
	if (!this->fSock->is_open()){
		tcp::endpoint ep(ip::address::from_string(aHost), aPort);
		this->fSock->connect(ep);
	}
}

void TStorageClient::disconnect(){
	if (this->fSock != nullptr){
		try{
			if (this->fSock->is_open()){
				this->fSock->close();
			}
		}
		catch (const boost::system::system_error& e){
			errorToFile("TStorageClient", "disconnect", string("disconnection error: ") + e.what());
		}
	}
}

const bool TStorageClient::sendMsg(TBaseMessage_ptr& aMsg){
	if (aMsg == nullptr){
		errorToFile("TStorageClient", "sendMsg", "msg to send is null!");
		return false;
	}

	string_ptr msg = aMsg->encodeMessage();
	msg->append("\n");

	int count = 2;
	bool result = true;
	while (count > 0){
		try{
			this->connect(DEFAULT_HOST, DEFAULT_PORT);
			boost::asio::write(*(this->fSock), boost::asio::buffer(*msg));
			break;
		}
		catch (const boost::system::system_error& e){
			this->disconnect();
			count--;
			string err = "impossible to send msg: ";
			if (count == 0){
				errorToFile("TStorageClient", "sendMsg", err + e.what() + "; exit.");
				result = false;
			}
			else{
				errorToFile("TStorageClient", "sendMsg", err + e.what() + "; retry.");
				//wait 2 seconds, then retry
				boost::this_thread::sleep_for(boost::chrono::seconds(2));
			}
		}
	}

	msg.reset();
	aMsg.reset();
	return result;
}

string_ptr TStorageClient::readMsg(){
	boost::asio::streambuf* buf = new boost::asio::streambuf();
	istream* is = new istream(buf);
	string_ptr msg = new_string_ptr();

	int count = 2;
	while (count > 0){
		try{
			this->connect(DEFAULT_HOST, DEFAULT_PORT);
			boost::asio::read_until(*(this->fSock), *buf, END_MSG + string("\n"));
			getline(*is, *msg);
			break;
		}
		catch (const boost::system::system_error& e){
			this->disconnect();
			count--;
			string err = "impossible to read msg: ";
			if (count == 0){
				errorToFile("TStorageClient", "readMsg", err + e.what() + "; exit.");
				msg.reset();
			}
			else{
				errorToFile("TStorageClient", "readMsg", err + e.what() + "; retry.");
				//wait 2 seconds, then retry
				boost::this_thread::sleep_for(boost::chrono::seconds(2));
			}
		}
	}

	delete buf;
	delete is;
	return move_string_ptr(msg);
}

TBaseMessage_ptr TStorageClient::filterPingMsgs(const string& aCaller){
	string_ptr msg = nullptr;
	TBaseMessage_ptr bm = nullptr;
	int kind = NO_ID;
	do{
		msg = this->readMsg();
		if (msg == nullptr)
			return nullptr;

		bm = new_TBaseMessage_ptr(msg);
		kind = bm->getID();
		if (kind == PING_REPLY_ID){
			logToFile("TStorageClient", aCaller, "Received ping reply from server");
			bm.reset();
		}
	} while (kind == PING_REPLY_ID);

	return move_TBaseMessage_ptr(bm);
}

const bool TStorageClient::processDirectory(const int aVersion, const string& aToken, const path& aRootPath, const path& aDirPath, List<UserFile^>^ aFileList, TUserFileList_ptr& aSqliteFileList){
	bool is_ok = true;
	for (directory_entry& x : directory_iterator(aDirPath)){
		path f = x.path();
		if (is_regular_file(f))
			is_ok = this->processFile(aVersion, aToken, aRootPath, f, aFileList, aSqliteFileList);
		else if (is_directory(f))
			is_ok = this->processDirectory(aVersion, aToken, aRootPath, f, aFileList, aSqliteFileList);

		if (!is_ok)
			return false;
	}

	return true;
}

const bool TStorageClient::processFile(const int aVersion, const string& aToken, const path& aRootPath, const path& aFilePath, List<UserFile^>^ aFileList, TUserFileList_ptr& aSqliteFileList){
	TBaseMessage_ptr bm = nullptr;
	TSystemErrorMessage_ptr sysErr = nullptr;
	TFileAckMessage_ptr ack = nullptr;
	string_ptr fileContent = nullptr;
	string_ptr checksum = nullptr;
	bool ackResp = false;
	bool isAckExpected = false;
	bool is_ok = true;

	logToFile("TStorageClient", "processFile", "Sending file: " + aFilePath.string());
	try{
		//check if file is changed
		if (aSqliteFileList != nullptr && !aSqliteFileList->empty()){
			for (TUserFileList::iterator it = aSqliteFileList->begin(); it != aSqliteFileList->end(); it++){
				if ((*it)->getFilePath() == aFilePath.string()){
					logToFile("TStorageClient", "processFile", aFilePath.string() + " is already stored.");
					(*it)->setToRemove(false);
					fileContent = readFile(aFilePath);
					if (fileContent != nullptr)
						checksum = opensslB64FileChecksum(*fileContent);

					if ((fileContent == nullptr && checksum == nullptr) || (fileContent != nullptr && checksum != nullptr && (*it)->getFileChecksum() == *checksum)){
						logToFile("TStorageClient", "processFile", aFilePath.string() + " is not changed.");
						//file is not changed
						aFileList->Add(gcnew UserFile(aVersion, unmarshalString(aFilePath.string())));
						(*it)->updateVersion();
						fileContent.reset();
						checksum.reset();
						return true;
					}
					else{
						fileContent.reset();
						checksum.reset();
						(*it).reset();
						aSqliteFileList->erase(it);
						break;
					}
				}
			}
		}

		int count = 0;
		do{
			TAddNewFileMessage_ptr m = new_TAddNewFileMessage_ptr(aToken, aFilePath.string());
			time_t fileDate = m->getFileDate();
			string checksum = m->getFileChecksum();
			string relPath = aFilePath.string().substr(aRootPath.string().length());
			m->setFilePath(relPath);
			
			if (!this->sendMsg((TBaseMessage_ptr&)move_TBaseMessage_ptr(m)))
				return false;

			bm = this->filterPingMsgs("processFile");
			if (bm == nullptr)
				return false;

			int kind = bm->getID();

			if (kind == SYSTEM_ERR_ID){
				sysErr = make_TSystemErrorMessage_ptr(bm);
				errorToFile("TStorageClient", "processFile", sysErr->getDetail());
				sysErr.reset();
				is_ok = false;
			}
			else if (kind == FILE_ACK_ID){
				ack = make_TFileAckMessage_ptr(bm);
				ackResp = ack->getResp();
				string file = ack->getFilePath();
				isAckExpected = relPath == file;
				ack.reset();
				if (isAckExpected){
					if (ackResp){
						aFileList->Add(gcnew UserFile(aVersion, unmarshalString(aFilePath.string())));
						string u;
						try{
							u = getUserFromToken(aToken);
							TUserFile_ptr fptr = new_TUserFile_ptr(u, aVersion, aFilePath.string(), checksum, fileDate, false);
							if (aSqliteFileList == nullptr)
								aSqliteFileList = new_TUserFileList_ptr();
							aSqliteFileList->push_back(move_TUserFile_ptr(fptr));
						}
						catch (EOpensslException e){
							errorToFile("TStorageClient", "processFile", "Unable to store file info on local DB!");
							is_ok = false;
						}
					}
					else{
						warningToFile("TStorageClient", "processFile", "Received bad ack for file: " + file);
						count++;
						if (count == MAX_TRY){
							errorToFile("TStorageClient", "processFile", "Stop processing file: " + file + " after " + to_string(count) + " attempts");
							is_ok = false;
						}
					}
				}
				else
					warningToFile("TStorageClient", "processFile", "Received ack for file: " + file + "but expected for file: " + relPath);
			}
			else{
				errorToFile("TStorageClient", "processFile", getMessageName(kind) + " is invalid!");
				is_ok = false;
			}

			bm.reset();
		} while ((!ackResp || !isAckExpected) && is_ok);
	}
	catch (EBaseException& e){
		errorToFile("TStorageClient", "processFile", e.getMessage());
		is_ok = false;
		bm.reset();
		sysErr.reset();
		ack.reset();
		fileContent.reset();
		checksum.reset();
	}

	return is_ok;
}

const bool TStorageClient::removeFiles(const string& aToken, const path& aRootPath, TUserFileList_ptr& aSqliteFileList){
	for (TUserFileList::iterator it = aSqliteFileList->begin(); it != aSqliteFileList->end(); it++){
		if ((*it)->isToRemove()){
			TBaseMessage_ptr bm = nullptr;
			TSystemErrorMessage_ptr sysErr = nullptr;
			TFileAckMessage_ptr ack = nullptr;
			bool ackResp = false;
			bool isAckExpected = false;
			bool is_ok = true;

			try{
				int count = 0;
				logToFile("TStorageClient", "removeFiles", "Removing file: " + (*it)->getFilePath());
				do{
					string relPath = (*it)->getFilePath().substr(aRootPath.string().length());
					TRemoveFileMessage_ptr m = new_TRemoveFileMessage_ptr(aToken, relPath);
					if (!this->sendMsg((TBaseMessage_ptr&)move_TBaseMessage_ptr(m)))
						return false;

					bm = this->filterPingMsgs("removeFiles");
					if (bm == nullptr)
						return false;

					int kind = bm->getID();
					if (kind == SYSTEM_ERR_ID){
						sysErr = make_TSystemErrorMessage_ptr(bm);
						errorToFile("TStorageClient", "removeFiles", sysErr->getDetail());
						sysErr.reset();
						is_ok = false;
					}
					else if (kind == FILE_ACK_ID){
						ack = make_TFileAckMessage_ptr(bm);
						ackResp = ack->getResp();
						string file = ack->getFilePath();
						isAckExpected = relPath == file;
						ack.reset();
						if (isAckExpected){
							if (!ackResp){
								warningToFile("TStorageClient", "removeFiles", "Received bad ack for file: " + file);
								count++;
								if (count == MAX_TRY){
									errorToFile("TStorageClient", "removeFiles", "Stop processing file: " + file + " after " + to_string(count) + " attempts");
									is_ok = false;
								}
							}
						}
						else
							warningToFile("TStorageClient", "removeFiles", "Received ack for file: " + file + "but expected for file: " + relPath);
					}
					else{
						errorToFile("TStorageClient", "removeFiles", getMessageName(kind) + " is invalid!");
						is_ok = false;
					}

					bm.reset();
				} while ((!ackResp || !isAckExpected) && is_ok);
			}
			catch (EMessageException& e){
				errorToFile("TStorageClient", "removeFiles", e.getMessage());
				is_ok = false;
				bm.reset();
				sysErr.reset();
				ack.reset();
			}

			if (!is_ok)
				return false;
		}
	}

	return true;
}

TStorageClient::TStorageClient(StorageClientController^ aCallbackObj) : fMainIoService(){
	this->fCallbackObj = (StorageClientController^)aCallbackObj;
	this->fMustExit.store(false, boost::memory_order_release);
	this->fQueue = new RequestsQueue();
	this->fExecutor = new thread(bind(&TStorageClient::processRequest, this));
}

TStorageClient::~TStorageClient(){
	this->fMustExit.store(true, boost::memory_order_release);

	this->disconnect();
	if (this->fSock != nullptr)
		delete this->fSock;

	if (this->fQueue != nullptr)
		delete this->fQueue;

	try{
		if (this->fSqliteDB != nullptr)
			delete this->fSqliteDB;
		this->fSqliteDB = nullptr;
	}
	catch (ESqliteDBException& e){
		errorToFile("TStorageClient", "destructor", e.getMessage());
	}

	if (this->fExecutor != nullptr)
		this->fExecutor->join();
}

void TStorageClient::verifyUser(const string& aUser, const string& aPass){
	if (aUser.empty()){
		//this->fLoggedIn.store(false, boost::memory_order_release);
		this->onLoginError("Username cannot be empty!");
		return;
	}
	if (aPass.empty()){
		//this->fLoggedIn.store(false, boost::memory_order_release);
		this->onLoginError("Password cannot be empty!");
		return;
	}

	TBaseMessage_ptr bm = nullptr;
	TSystemErrorMessage_ptr sysErr = nullptr;

	//verify if user exists
	logToFile("TStorageClient", "verifyUser", "Verify if user " + aUser + " is valid");
	if (!this->sendMsg((TBaseMessage_ptr&)move_TBaseMessage_ptr(new_TVerifyCredReqMessage_ptr(aUser, aPass)))){
		//this->fLoggedIn.store(false, boost::memory_order_release);
		this->onLoginError("Login temporarly unavailable! Try later.");
		return;
	}
	bm = this->filterPingMsgs("verifyUser");
	if (bm == nullptr){
		//this->fLoggedIn.store(false, boost::memory_order_release);
		this->onLoginError("Login temporarly unavailable! Try later.");
		return;
	}

	TVerifyCredReplyMessage_ptr verReply = nullptr;
	try{
		verReply = make_TVerifyCredReplyMessage_ptr(bm);
		bm.reset();
	}
	catch (EMessageException& e){
		errorToFile("TStorageClient", "verifyUser", e.getMessage());

		bm.reset();
		verReply.reset();
		//this->fLoggedIn.store(false, boost::memory_order_release);
		this->onLoginError("Login temporarly unavailable! Try later.");
		return;
	}

	string p = verReply->getPath();
	if (verReply->getResp()){
		//this->fLoggedIn.store(true, boost::memory_order_release);
		this->onLoginSuccess(unmarshalString(p));
	}
	else{
		//this->fLoggedIn.store(false, boost::memory_order_release);
		this->onLoginError("Login failed! Try later.");
	}

	verReply.reset();
}

void TStorageClient::registerUser(const string& aUser, const string& aPass, const string& aRootPath){
	if (aUser.empty()){
		this->onRegistrationError("Username cannot be empty!");
		return;
	}
	if (aPass.empty()){
		this->onRegistrationError("Password cannot be empty!");
		return;
	}
	if (aRootPath.empty()){
		this->onRegistrationError("Directory to be synchronized cannot be empty!");
		return;
	}
	path p(aRootPath);
	if (!exists(p)){
		this->onRegistrationError("Directory to be synchronized must exist!");
		return;
	}

	bool result = false;
	TBaseMessage_ptr bm = nullptr;
	TSystemErrorMessage_ptr sysErr = nullptr;

	logToFile("TStorageClient", "registerUser", "Try to register a new user " + aUser);
	if (!this->sendMsg((TBaseMessage_ptr&)move_TBaseMessage_ptr(new_TUserRegistrReqMessage_ptr(aUser, aPass, aRootPath)))){
		this->onRegistrationError("Registration temporarly unavailable! Try later.");
		return;
	}
	bm = this->filterPingMsgs("registerUser");
	if (bm == nullptr){
		this->onRegistrationError("Registration temporarly unavailable! Try later.");
		return;
	}

	TUserRegistrReplyMessage_ptr regReply = nullptr;
	try{
		int kind = bm->getID();
		if (kind == SYSTEM_ERR_ID){
			sysErr = make_TSystemErrorMessage_ptr(bm);
			errorToFile("TStorageClient", "registerUser", sysErr->getDetail());
			this->onRegistrationError("Registration temporarly unavailable! Try later.");
			result = false;
			sysErr.reset();
		}
		else if (kind == USER_REG_REPLY_ID){
			regReply = make_TUserRegistrReplyMessage_ptr(bm);
			result = regReply->getResp();
			regReply.reset();
		}
		else{
			errorToFile("TStorageClient", "registerUser", getMessageName(kind) + " is invalid");
			this->onRegistrationError("Registration temporarly unavailable! Try later.");
			result = false;
		}
	}
	catch (EMessageException& e){
		errorToFile("TStorageClient", "registerUser", e.getMessage());
		this->onRegistrationError("Registration temporarly unavailable! Try later.");

		bm.reset();
		sysErr.reset();
		regReply.reset();
		return;
	}

	bm.reset();

	if (result){
		logToFile("TStorageClient", "registerUser", "User " + aUser + " registered");
		this->onRegistrationSucces();
	}
	else{
		warningToFile("TStorageClient", "registerUser", "User " + aUser + " cannot be registered");
		this->onRegistrationError("Registration failed! Try later.");
	}
}

int TStorageClient::getLastVersion(const string& aUser, const string& aPass){
	TBaseMessage_ptr bm = nullptr;
	TSystemErrorMessage_ptr sysErr = nullptr;

	int version = 0;
	if (!this->sendMsg((TBaseMessage_ptr&)move_TBaseMessage_ptr(new_TGetLastVerReqMessage_ptr(aUser, aPass))))
		return -1;
	bm = this->filterPingMsgs("getLastVersion");
	if (bm == nullptr)
		return -1;

	TGetLastVerReplyMessage_ptr lastVer = nullptr;
	try{
		int kind = bm->getID();
		if (kind == SYSTEM_ERR_ID){
			sysErr = make_TSystemErrorMessage_ptr(bm);
			errorToFile("TStorageClient", "getLastVersion", sysErr->getDetail());
			version = -1;
		}
		else if (kind == GET_LAST_VERSION_REPLY_ID){
			lastVer = make_TGetLastVerReplyMessage_ptr(bm);
			version = lastVer->getVersion();
		}
		else{
			errorToFile("TStorageClient", "getLastVersion", getMessageName(kind) + " is invalid");
			version = -1;
		}
	}
	catch (EMessageException& e){
		errorToFile("TStorageClient", "getLastVersion", e.getMessage());
		version = -1;
	}
	bm.reset();
	sysErr.reset();
	lastVer.reset();

	if (version == 0)
		warningToFile("TStorageClient", "getLastVersion", "User " + aUser + " has no version to restore");

	return version;
}

void TStorageClient::updateCurrentVersion(const string& aUser, const string& aPass, const string& aRootPath){
	if (aUser.empty()){
		this->onUpdateError("Username cannot be empty!");
		return;
	}
	if (aPass.empty()){
		this->onUpdateError("Password cannot be empty!");
		return;
	}
	if (aRootPath.empty()){
		this->onUpdateError("Directory to be synchronized cannot be empty!");
		return;
	}
	path root(aRootPath);
	if (exists(root) && !is_directory(root)){
		this->onUpdateError("Directory to be synchronized must be valid!");
		return;
	}

	int lastServer = this->getLastVersion(aUser, aPass);
	if (lastServer == -1){
		this->onUpdateError("Temporary error! Update session cannot be started.");
		return;
	}

	int lastLocal = 0;
	if (lastServer > 0){
		try{
			if (this->fSqliteDB == nullptr)
				this->fSqliteDB = new TSqliteDB(SQLITE_DEFAULT_DB_NAME);

			lastLocal = this->fSqliteDB->getLastVersion(aUser);
		}
		catch (ESqliteDBException& e){
			errorToFile("TStorageClient", "updateCurrentVersion", e.getMessage());
			this->onUpdateError("Temporary error! Update session cannot be started.");
			return;
		}

		if (lastServer > lastLocal){
			try{
				if (!exists(root))
					createDir(root);

				path local(root / path("LOCAL_NO_SYNCH"));
				moveAllFiles(root, local);
				if (!this->restoreVersion(aUser, aPass, lastServer, "", aRootPath, true)){
					moveAllFiles(local, root);
					removeDir(local);
				}
			}
			catch (EFilesystemException& e){
				errorToFile("TStorageClient", "updateCurrentVersion", e.getMessage());
				onUpdateError("Temporary error! Update session cannot be started.");
			}
			return;
		}
	}

	if (!exists(root)){
		if (lastServer == 0)
			//no previous versions
			this->onUpdateError(unmarshalString("Create directory " + aRootPath + " and put some files in it to start synchronization"));
		else
			this->onUpdateError(unmarshalString("Synchronization interrupted because " + aRootPath + " does not exist"));

		return;
	}

	if (isDirectoryEmpty(root)){
		if (lastServer == 0)
			//no previous versions
			this->onUpdateError(unmarshalString("Put some files in " + aRootPath + " to start synchronization"));
		else
			this->onUpdateError(unmarshalString("Synchronization interrupted because " + aRootPath + " is empty"));

		return;
	}

	TBaseMessage_ptr bm = nullptr;
	TSystemErrorMessage_ptr sysErr = nullptr;

	//verify if an update session could be started
	logToFile("TStorageClient", "updateCurrentVersion", "Verify if an update session could be started for user " + aUser);
	if (!this->sendMsg((TBaseMessage_ptr&)move_TBaseMessage_ptr(new_TUpdateStartReqMessage_ptr(aUser, aPass)))){
		this->onUpdateError("Temporary error! Update session cannot be started.");
		return;
	}
	bm = this->filterPingMsgs("updateCurrentVersion");
	if (bm == nullptr){
		this->onUpdateError("Temporary error! Update session cannot be started.");
		return;
	}

	TUpdateStartReplyMessage_ptr updReply = nullptr;
	bool result = false;
	try{
		int kind = bm->getID();
		if (kind == SYSTEM_ERR_ID){
			sysErr = make_TSystemErrorMessage_ptr(bm);
			errorToFile("TStorageClient", "updateCurrentVersion", sysErr->getDetail());
			result = false;
			sysErr.reset();
		}
		else if (kind == UPDATE_START_REPLY_ID){
			updReply = make_TUpdateStartReplyMessage_ptr(bm);
			result = updReply->getResp();
		}
		else{
			errorToFile("TStorageClient", "updateCurrentVersion", getMessageName(kind) + " is invalid");
			result = false;
		}
	}
	catch (EMessageException& e){
		errorToFile("TStorageClient", "updateCurrentVersion", e.getMessage());

		updReply.reset();
		result = false;
	}
	bm.reset();
	sysErr.reset();

	string token = "";
	if (result){
		token = updReply->getToken();
		updReply.reset();

		logToFile("TStorageClient", "updateCurrentVersion", "Start sending files");
		bool is_ok = true;
		TUserFileList_ptr fl = nullptr;
		try{
			if (this->fSqliteDB == nullptr)
				this->fSqliteDB = new TSqliteDB(SQLITE_DEFAULT_DB_NAME);

			fl = this->fSqliteDB->getFileList(aUser, lastLocal);
		}
		catch (ESqliteDBException e){
			errorToFile("TStorageClient", "updateCurrentVersion", e.getMessage());
			this->onUpdateError("Temporary error! Update session cannot be started.");
			fl.reset();
			return;
		}

		List<UserFile^>^ flist = gcnew List<UserFile^>();
		this->onUpdateStart(unmarshalString(token));
		try{
			if (is_regular_file(root)){
				is_ok = this->processFile(lastServer + 1, token, root, root, flist, fl);
			}
			else if (is_directory(root)){
				is_ok = this->processDirectory(lastServer + 1, token, root, root, flist, fl);
			}

			if (!is_ok){
				this->onUpdateError("Temporary error! Update session interrupted.");
				fl->clear();
				fl.reset();
				return;
			}

			//remove from server file marked as to be removed
			this->removeFiles(token, root, fl);
			if (!is_ok){
				this->onUpdateError("Temporary error! Update session interrupted.");
				fl->clear();
				fl.reset();
				return;
			}

			//update is finished; send stop
			if (!this->sendMsg((TBaseMessage_ptr&)move_TBaseMessage_ptr(new_TUpdateStopReqMessage_ptr(token)))){
				this->onUpdateError("Temporary error! Update session interrupted.");
				return;
			}

			bm = this->filterPingMsgs("updateCurrentVersion");
			if (bm == nullptr){
				this->onUpdateError("Temporary error! Update session interrupted.");
				return;
			}

			int kind = bm->getID();
			TUpdateStopReplyMessage_ptr updstopReply = nullptr;
			try{
				is_ok = true;
				if (kind == SYSTEM_ERR_ID){
					sysErr = make_TSystemErrorMessage_ptr(bm);
					errorToFile("TStorageClient", "updateCurrentVersion", sysErr->getDetail());
					sysErr.reset();
					is_ok = false;
				}
				else if (kind == UPDATE_STOP_REPLY_ID){
					updstopReply = make_TUpdateStopReplyMessage_ptr(bm);
					if (updstopReply->getResp()){
						string vdate = formatFileDate(updstopReply->getTime());
						int v = updstopReply->getVersion();
						logToFile("TStorageClient", "updateCurrentVersion", "Version " + to_string(v) + " at " + vdate);
						try{
							if (this->fSqliteDB == nullptr)
								this->fSqliteDB = new TSqliteDB(SQLITE_DEFAULT_DB_NAME);

							this->fSqliteDB->removeFileList(aUser);
							this->fSqliteDB->insertFileList(fl);
							if (fl != nullptr){
								fl->clear();
								fl.reset();
							}
						}
						catch (ESqliteDBException& e){
							errorToFile("TStorageClient", "updateCurrentVersion", "Unable to store information on local SQLite DB: " + e.getMessage());
							if (fl != nullptr){
								fl->clear();
								fl.reset();
							}
						}

						this->onUpdateSuccess(flist, v, unmarshalString(vdate));
					}
					else{
						warningToFile("TStorageClient", "updateCurrentVersion", "Update session refused by the server!");
						this->onUpdateSuccess(flist, 0, "");
					}

					updstopReply.reset();
				}
				else{
					errorToFile("TStorageClient", "updateCurrentVersion", getMessageName(kind) + " is invalid");
					this->onUpdateError("Temporary error! Update session interrupted.");
					is_ok = false;
				}
			}
			catch (EMessageException& e){
				errorToFile("TStorageClient", "updateCurrentVersion", e.getMessage());
				this->onUpdateError("Temporary error! Update session interrupted.");
				bm.reset();
				sysErr.reset();
				updstopReply.reset();
			}
			bm.reset();
		}
		catch (const filesystem_error& e){
			errorToFile("TStorageClient", "updateCurrentVersion", e.what());
			this->onUpdateError("Temporary error! Update session interrupted.");
		}
	}
	else{
		errorToFile("TStorageClient", "updateCurrentVersion", "User " + aUser + " cannot start an update session.");
		this->onUpdateError("Temporary error! Update session cannot be started.");
	}
}

const bool TStorageClient::restoreVersion(const string& aUser, const string& aPass, const int aVersion, const string& aFile, const string& aDestPath, const bool aStoreOnLocalDB){
	path root(aDestPath);
	bool is_err = false;
	TBaseMessage_ptr bm = nullptr;
	TSystemErrorMessage_ptr sysErr = nullptr;

	if (aUser.empty()){
		this->onRestoreError("Username cannot be empty!", aStoreOnLocalDB);
		return false;
	}
	if (aPass.empty()){
		this->onRestoreError("Password cannot be empty!", aStoreOnLocalDB);
		return false;
	}
	if (aDestPath.empty()){
		this->onRestoreError("Destination directory cannot be empty!", aStoreOnLocalDB);
		return false;
	}
	path p(aDestPath);
	if (!exists(p)){
		this->onRestoreError("Destination directory must exist!", aStoreOnLocalDB);
		return false;
	}
	else if (!is_directory(p)){
		this->onRestoreError("Restore destination must be a valid directory!", aStoreOnLocalDB);
		return false;
	}

	if (aVersion <= 0){
		errorToFile("TStorageClient", "restoreVersion", "Required version cannot be lower than 0");
		this->onRestoreError("Required version cannot be lower than 0", aStoreOnLocalDB);
		return false;
	}

	//verify if an update session could be started
	logToFile("TStorageClient", "restoreVersion", "Verify if a restore session could be started for user " + aUser);
	if (!this->sendMsg((TBaseMessage_ptr&)move_TBaseMessage_ptr(new_TRestoreVerReqMessage_ptr(aUser, aPass, aVersion, aFile)))){
		this->onRestoreError("Temporary error! Restore of version " + aVersion + " cannot be started.", aStoreOnLocalDB);
		return false;
	}
	bm = this->filterPingMsgs("restoreVersion");
	if (bm == nullptr){
		this->onRestoreError("Temporary error! Restore of version " + aVersion + " cannot be started.", aStoreOnLocalDB);
		return false;
	}

	TRestoreVerReplyMessage_ptr restReply = nullptr;
	bool result = false;
	try{
		int kind = bm->getID();
		if (kind == SYSTEM_ERR_ID){
			sysErr = make_TSystemErrorMessage_ptr(bm);
			errorToFile("TStorageClient", "restoreVersion", sysErr->getDetail());
			result = false;
			sysErr.reset();
		}
		else if (kind == RESTORE_VER_REPLY_ID){
			restReply = make_TRestoreVerReplyMessage_ptr(bm);
			result = restReply->getResp();
		}
		else{
			errorToFile("TStorageClient", "restoreVersion", getMessageName(kind) + " is invalid");
			result = false;
		}
	}
	catch (EMessageException& e){
		errorToFile("TStorageClient", "restoreVersion", e.getMessage());
		result = false;
		restReply.reset();
	}
	bm.reset();
	sysErr.reset();

	string token = "";
	if (result){
		token = restReply->getToken();
		restReply.reset();
		this->onRestoreStart(unmarshalString(token), aStoreOnLocalDB);

		TUserFileList_ptr fl = nullptr;
		List<UserFile^>^ flist = nullptr;

		logToFile("TStorageClient", "restoreVersion", "Start receiving files");
		bool exit = false;
		while (!exit){
			bm = this->filterPingMsgs("updateCurrentVersion");
			if (bm == nullptr){
				is_err = true;
				break;
			}

			int kind = bm->getID();
			TRestoreFileMessage_ptr restFile = nullptr;
			TRestoreStopMessage_ptr restStop = nullptr;
			result = false;
			try{
				if (kind == SYSTEM_ERR_ID){
					sysErr = make_TSystemErrorMessage_ptr(bm);
					errorToFile("TStorageClient", "restoreVersion", sysErr->getDetail());
					exit = is_err = true;
				}
				else if (kind == RESTORE_FILE_ID){
					restFile = make_TRestoreFileMessage_ptr(bm);
					path f(restFile->getFilePath());
					logToFile("TStorageClient", "restoreVersion", "received file " + f.string());

					try{
						if (restFile->verifyChecksum()){
							try{
								storeFile(root / f, restFile->getFileContent());

								if (flist == nullptr)
									flist = gcnew List<UserFile^>();
								flist->Add(gcnew UserFile(aVersion, unmarshalString((root / f).string())));

								if (aStoreOnLocalDB){
									if (fl == nullptr)
										fl = new_TUserFileList_ptr();

									TUserFile_ptr fptr = new_TUserFile_ptr(aUser, aVersion, (root / f).string(), restFile->getFileChecksum(), restFile->getFileDate(), false);
									fl->push_back(move_TUserFile_ptr(fptr));
								}

								if (!this->sendMsg((TBaseMessage_ptr&)move_TBaseMessage_ptr(new_TRestoreFileAckMessage_ptr(true, token, f.string()))))
									exit = is_err = true;
							}
							catch (EFilesystemException& e){
								errorToFile("TStorageClient", "restoreVersion", e.getMessage());
								if (!this->sendMsg((TBaseMessage_ptr&)move_TBaseMessage_ptr(new_TRestoreFileAckMessage_ptr(false, token, f.string()))))
									exit = is_err = true;
							}
						}
					}
					catch (EMessageException e){
						errorToFile("TStorageClient", "restoreVersion", e.getMessage());
						if (!this->sendMsg((TBaseMessage_ptr&)move_TBaseMessage_ptr(new_TRestoreFileAckMessage_ptr(false, token, f.string()))))
							exit = is_err = true;
					}
				}
				else if (kind == RESTORE_STOP_ID){
					restStop = make_TRestoreStopMessage_ptr(bm);
					string vdate = formatFileDate(restStop->getVersionDate());
					if (restStop->getVersion() > 0)
						logToFile("TStorageClient", "restoreVersion", "Version " + to_string(restStop->getVersion()) + " [" + vdate + "] completely restored.");

					if (aStoreOnLocalDB){
						try{
							if (this->fSqliteDB == nullptr)
								this->fSqliteDB = new TSqliteDB(SQLITE_DEFAULT_DB_NAME);

							this->fSqliteDB->removeFileList(aUser);
							this->fSqliteDB->insertFileList(fl);
							if (fl != nullptr){
								fl->clear();
								fl.reset();
							}

							if (restStop->getVersion() > 0)
								this->onRestoreSuccess(restStop->getVersion(), unmarshalString(vdate), aStoreOnLocalDB, flist);
							else
								is_err = true;
							exit = true;
						}
						catch (ESqliteDBException& e){
							this->onRestoreError("Unable to store information on local SQLite DB: " + unmarshalString(e.getMessage()), aStoreOnLocalDB);
							if (fl != nullptr){
								fl->clear();
								fl.reset();
							}
							exit = is_err = true;
						}
					}
					else{
						if (restStop->getVersion() > 0)
							this->onRestoreSuccess(restStop->getVersion(), unmarshalString(vdate), aStoreOnLocalDB, flist);
						else
							is_err = true;
						exit = true;
					}
				}
				else{
					errorToFile("TStorageClient", "restoreVersion", getMessageName(kind) + " is invalid");
					exit = is_err = true;
				}
			}
			catch (EMessageException& e){
				errorToFile("TStorageClient", "restoreVersion", e.getMessage());
				exit = is_err = true;
			}

			bm.reset();
			restFile.reset();
			restStop.reset();
		}

		if (is_err){
			this->onRestoreError("Temporary error! Restore of version " + aVersion + " interrupted.", aStoreOnLocalDB);
			if (fl != nullptr){
				fl->clear();
				fl.reset();
			}
			return false;
		}
		else
			return true;
	}
	else{
		errorToFile("TStorageClient", "restoreVersion", "User " + aUser + " cannot start a restore session.");
		this->onRestoreError("Temporary error! Restore of version " + aVersion + " cannot be started.", aStoreOnLocalDB);
	}

	return false;
}

void TStorageClient::getAllVersions(const string& aUser, const string& aPass){
	if (aUser.empty()){
		this->onGetVersionsError("Username cannot be empty!");
		return;
	}
	if (aPass.empty()){
		this->onGetVersionsError("Password cannot be empty!");
		return;
	}

	TBaseMessage_ptr bm = nullptr;
	TSystemErrorMessage_ptr sysErr = nullptr;

	logToFile("TStorageClient", "getAllVersions", "Try to get all versions of user " + aUser);
	if (!this->sendMsg((TBaseMessage_ptr&)move_TBaseMessage_ptr(new_TGetVersionsReqMessage_ptr(aUser, aPass)))){
		this->onGetVersionsError("Temporary Error! Version list cannot be retrived.");
		return;
	}
	bm = this->filterPingMsgs("getAllVersions");
	if (bm == nullptr){
		this->onGetVersionsError("Temporary Error! Version list cannot be retrived.");
		return;
	}

	TGetVersionsReplyMessage_ptr verReply = nullptr;
	try{
		int kind = bm->getID();
		if (kind == SYSTEM_ERR_ID){
			sysErr = make_TSystemErrorMessage_ptr(bm);
			errorToFile("TStorageClient", "getAllVersions", sysErr->getDetail());
			this->onGetVersionsError("Temporary Error! Version list cannot be retrived.");
		}
		else if (kind == GET_VERSIONS_REPLY_ID){
			verReply = make_TGetVersionsReplyMessage_ptr(bm);
			int tot = verReply->getTotVersions();
			if (tot > 0){
				logToFile("TStorageClient", "getAllVersions", "Versions of user " + aUser + ":");
				List<UserVersion^>^ result = gcnew List<UserVersion^>();
				TVersionList_ptr vers = verReply->getVersions();
				for (int i = 0; i < tot; i++){
					int id = vers->at(i)->getVersion();
					string vd = formatFileDate(vers->at(i)->getDate());
					logToFile("TStorageClient", "getAllVersions", "Version " + to_string(id) + " @ " + vd);
					int totF = vers->at(i)->getFileNum();

					//files for this version
					List<UserFile^>^ files = gcnew List<UserFile^>();
					for (int j = 0; j < totF; j++){
						TFile_ptr file = vers->at(i)->getNextFile();
						vers->at(i)->updateNext();
						files->Add(gcnew UserFile(id, unmarshalString(file->getClientRelativePath())));
						file.reset();
					}

					result->Add(gcnew UserVersion(unmarshalString(vd), id, files));
				}
				vers.reset();
				this->onGetVersionsSuccess(result);
			}
			else
				this->onGetVersionsError("No versions available!");
		}
		else{
			errorToFile("TStorageClient", "getAllVersions", getMessageName(kind) + " is invalid");
			this->onGetVersionsError("Temporary Error! Version list cannot be retrived.");
		}
	}
	catch (EMessageException& e){
		errorToFile("TStorageClient", "getAllVersions", e.getMessage());
		this->onGetVersionsError("Temporary Error! Version list cannot be retrived.");
	}
	bm.reset();
	sysErr.reset();
	verReply.reset();
}

const bool TStorageClient::pingServer(const string& aToken){
	//send ping to server
	logToFile("TStorageClient", "pingServer", "Send ping to server fof token " + aToken);
	if (this->sendMsg((TBaseMessage_ptr&)move_TBaseMessage_ptr(new_TPingReqMessage_ptr(aToken)))){
		logToFile("TStorageClient", "pingServer", "Ping sent for token " + aToken);
		return true;
	}
	else{
		errorToFile("TStorageClient", "pingServer", "Unable to send ping message.");
		return false;
	}
}

void TStorageClient::processRequest(){
	while (!this->fMustExit.load(boost::memory_order_acquire) && !System::Object::ReferenceEquals(this->fCallbackObj, nullptr) && this->fQueue != nullptr){
		UserRequest^ req = this->fQueue->popRequest();
		int kind = req->getID();
		switch (kind){
			case REGISTR_REQ:{
				RegistrRequest^ rr = (RegistrRequest^)req;
				this->registerUser(marshalString(rr->getUser()), marshalString(rr->getPass()), marshalString(rr->getPath()));
				break;
			}
			case LOGIN_REQ:{
				LoginRequest^ lr = (LoginRequest^)req;
				this->verifyUser(marshalString(lr->getUser()), marshalString(lr->getPass()));
				break;
			}
			case LOGOUT_REQ:{
				this->disconnect();
				break;
			}
			case UPDATE_REQ:{
				UpdateRequest^ ur = (UpdateRequest^)req;
				this->updateCurrentVersion(marshalString(ur->getUser()), marshalString(ur->getPass()), marshalString(ur->getPath()));
				break;
			}
			case GET_VERSIONS_REQ:{
				GetVerRequest^ gvr = (GetVerRequest^)req;
				this->getAllVersions(marshalString(gvr->getUser()), marshalString(gvr->getPass()));
				break;
			}
			case RESTORE_REQ:{
				RestoreRequest^ rr = (RestoreRequest^)req;
				this->restoreVersion(marshalString(rr->getUser()), marshalString(rr->getPass()), rr->getVersion(), marshalString(rr->getFile()), marshalString(rr->getDestPath()), false);
				break;
			}
			default:{
				warningToFile("TStorageClient", "processRequest", "invalid request ignored");
			}
		}
	}
}

const bool TStorageClient::issueRequest(UserRequest^ aRequest){
	if (aRequest->getID() == PING_REQ)
		return this->pingServer(marshalString(((PingRequest^)aRequest)->getToken()));

	if (this->fQueue != nullptr){
		this->fQueue->pushRequest(aRequest);
		return true;
	}
	else
		return false;
}

