/*
 * Author: Angelo Prudentino
 * Date: 14/09/2016
 * File: StorageClientCore.cpp
 * Description: this file contains all classes implementing
 *              client-side core logic
 *
 */
#include <boost\bind.hpp>
#include <boost/filesystem.hpp>
#include "StorageClient.h"

using namespace System;
using namespace std;
using namespace boost;
using namespace boost::asio;
using namespace boost::asio::ip;
using namespace boost::filesystem;

#define PING_INTERVAL 60


//////////////////////////////////
//       TStorageClient	        //
//////////////////////////////////
#pragma region "TStorageClient"
void TStorageClient::connect(const string& aHost, int aPort){
	try{
		if (this->fSock == nullptr)
			this->fSock = new tcp::socket(this->fMainIoService);

		tcp::endpoint ep(ip::address::from_string(aHost), aPort);
		this->fSock->connect(ep);
	}
	catch (...){

	}
}

void TStorageClient::disconnect(){
	if (this->fSock != nullptr){
		try{
			this->fSock->shutdown(socket_base::shutdown_both);
			this->fSock->close();
		}
		catch (...){

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
		catch (...){
			this->disconnect();
			count--;
			if (count == 0){
				result = false;
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
		catch (...){
			this->disconnect();
			count--;
			if (count == 0){
				msg.reset();
			}
		}
	}

	delete buf;
	delete is;
	return move_string_ptr(msg);
}

const bool TStorageClient::processDirectory(const string& aToken, const path& aRootPath, const path& aDirPath, List<UserFile^>^ aFileList){
	bool is_ok = true;
	for (directory_entry& x : directory_iterator(aDirPath)){
		path f = x.path();
		if (is_regular_file(f)){
			is_ok = this->processFile(aToken, aRootPath, f, aFileList);
		}
		else if (is_directory(f))
			is_ok = this->processDirectory(aToken, aRootPath, f, aFileList);

		if (!is_ok)
			return false;
	}

	return true;
}

const bool TStorageClient::processFile(const string& aToken, const path& aRootPath, const path& aFilePath, List<UserFile^>^ aFileList){
	string_ptr msg = nullptr;
	TBaseMessage_ptr bm = nullptr;
	TSystemErrorMessage_ptr sysErr = nullptr;
	TFileAckMessage_ptr ack = nullptr;
	bool ackResp = false;
	bool isAckExpected = false;
	bool is_ok = true;

	logToFile("TStorageClient", "processFile", "Sending file: " + aFilePath.string());
	try{
		do{
			TAddNewFileMessage_ptr m = new_TAddNewFileMessage_ptr(aToken, aFilePath.string());
			string relPath = aFilePath.string().substr(aRootPath.string().length());
			m->setFilePath(relPath);
			if (!this->sendMsg((TBaseMessage_ptr&)move_TBaseMessage_ptr(m)))
				return false;

			msg = this->readMsg();
			if (msg == nullptr)
				return false;

			bm = new_TBaseMessage_ptr(msg);
			int kind = bm->getID();
			if (kind == SYSTEM_ERR_ID){
				sysErr = new_TSystemErrorMessage_ptr(bm);
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
						aFileList->Add(gcnew UserFile(unmarshalString(aFilePath.string())));
					}
					else
						warningToFile("TStorageClient", "processFile", "Received bad ack for file: " + file);
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
	catch (EMessageException& e){
		errorToFile("TStorageClient", "processFile", e.getMessage());
		is_ok = false;
		bm.reset();
		sysErr.reset();
		ack.reset();
	}

	return is_ok;
}

TStorageClient::TStorageClient(StorageClientController^ aCallbackObj) : fMainIoService(){
	this->fCallbackObj = aCallbackObj;
	this->fMustExit.store(false, boost::memory_order_release);
	this->fQueue = new RequestsQueue();
	this->fExecutor = new thread(bind(&TStorageClient::processRequest, this));
}

TStorageClient::~TStorageClient(){
	this->fMustExit.store(true, boost::memory_order_release);
	
	//this->disconnect();
	if (this->fSock != nullptr)
		delete this->fSock;

	if (this->fQueue != nullptr)
		delete this->fQueue;

	if (this->fExecutor != nullptr)
		this->fExecutor->join();
}

void TStorageClient::verifyUser(const string& aUser, const string& aPass){
	if (aUser.empty()){
		this->fCallbackObj->onLoginError("Username cannot be empty!");
		return;
	}
	if (aPass.empty()){
		this->fCallbackObj->onLoginError("Password cannot be empty!");
		return;
	}

	string_ptr msg = nullptr;
	TBaseMessage_ptr bm = nullptr;
	TSystemErrorMessage_ptr sysErr = nullptr;

	//verify if user exists
	logToFile("TStorageClient", "verifyUser", "Verify if user " + aUser + " is valid");
	if (!this->sendMsg((TBaseMessage_ptr&)move_TBaseMessage_ptr(new_TVerifyCredReqMessage_ptr(aUser, aPass)))){
		this->fCallbackObj->onLoginError("Temporary error! Try again.");
		return;
	}
	msg = this->readMsg();
	if (msg == nullptr){
		this->fCallbackObj->onLoginError("Temporary error! Try again.");
		return;
	}

	TVerifyCredReplyMessage_ptr verReply = nullptr;
	try{
		bm = new_TBaseMessage_ptr(msg);
		verReply = make_TVerifyCredReplyMessage_ptr(bm);
		bm.reset();
	}
	catch (EMessageException& e){
		errorToFile("TStorageClient", "verifyUser", e.getMessage());

		bm.reset();
		verReply.reset();
		this->fCallbackObj->onLoginError("Temporary error! Try again.");
		return;
	}

	string p = verReply->getPath();
	if (verReply->getResp())
		this->fCallbackObj->onLoginSuccess(unmarshalString(p));
	else
		this->fCallbackObj->onLoginError("Login failed! Try again.");
	
	verReply.reset();
}

void TStorageClient::registerUser(const string& aUser, const string& aPass, const string& aRootPath){
	if (aUser.empty()){
		this->fCallbackObj->onRegistrationError("Username cannot be empty!");
		return;
	}
	if (aPass.empty()){
		this->fCallbackObj->onRegistrationError("Password cannot be empty!");
		return;
	}
	if (aRootPath.empty()){
		this->fCallbackObj->onRegistrationError("Directory to be synchronized cannot be empty!");
		return;
	}

	bool result = false;
	string_ptr msg = nullptr;
	TBaseMessage_ptr bm = nullptr;
	TSystemErrorMessage_ptr sysErr = nullptr;

	logToFile("TStorageClient", "registerUser", "Try to register a new user " + aUser);
	if (!this->sendMsg((TBaseMessage_ptr&)move_TBaseMessage_ptr(new_TUserRegistrReqMessage_ptr(aUser, aPass, aRootPath)))){
		this->fCallbackObj->onRegistrationError("Temporary error! Try again.");
		return;
	}
	msg = this->readMsg();
	if (msg == nullptr){
		this->fCallbackObj->onRegistrationError("Temporary error! Try again.");
		return;
	}

	TUserRegistrReplyMessage_ptr regReply = nullptr;
	try{
		bm = new_TBaseMessage_ptr(msg);
		int kind = bm->getID();
		if (kind == SYSTEM_ERR_ID){
			sysErr = new_TSystemErrorMessage_ptr(bm);
			errorToFile("TStorageClient", "registerUser", sysErr->getDetail());
			this->fCallbackObj->onRegistrationError("Temporary error! Try again.");
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
			this->fCallbackObj->onRegistrationError("Temporary error! Try again.");
			result = false;
		}
	}
	catch (EMessageException& e){
		errorToFile("TStorageClient", "registerUser", e.getMessage());
		this->fCallbackObj->onRegistrationError("Temporary error! Try again.");

		bm.reset();
		sysErr.reset();
		regReply.reset();
		return;
	}

	bm.reset();

	if (result){
		logToFile("TStorageClient", "registerUser", "User " + aUser + " registered");
		this->fCallbackObj->onRegistrationSucces();
		this->issueRequest(gcnew UpdateRequest(unmarshalString(aUser), unmarshalString(aPass), unmarshalString(aRootPath)));
	}
	else{
		warningToFile("TStorageClient", "registerUser", "User " + aUser + " cannot be registered");
		this->fCallbackObj->onRegistrationError("Registration failed! Try again.");
	}
}

void TStorageClient::updateCurrentVersion(const string& aUser, const string& aPass, const string& aRootPath){
	if (aUser.empty()){
		this->fCallbackObj->onUpdateError("Username cannot be empty!");
		return;
	}
	if (aPass.empty()){
		this->fCallbackObj->onUpdateError("Password cannot be empty!");
		return;
	}
	if (aRootPath.empty()){
		this->fCallbackObj->onUpdateError("Directory to be synchronized cannot be empty!");
		return;
	}
	path p(aRootPath);
	if (!exists(p)){
		this->fCallbackObj->onUpdateError("Directory to be synchronized must exist!");
		return;
	}

	string_ptr msg = nullptr;
	TBaseMessage_ptr bm = nullptr;
	TSystemErrorMessage_ptr sysErr = nullptr;

	//verify if an update session could be started
	logToFile("TStorageClient", "updateCurrentVersion", "Verify if an update session could be started for user " + aUser);
	if (!this->sendMsg((TBaseMessage_ptr&)move_TBaseMessage_ptr(new_TUpdateStartReqMessage_ptr(aUser, aPass)))){
		this->fCallbackObj->onUpdateError("Temporary error; Update session cannot be started.");
		return;
	}
	msg = this->readMsg();
	if (msg == nullptr){
		this->fCallbackObj->onUpdateError("Temporary error; Update session cannot be started.");
		return;
	}

	TUpdateStartReplyMessage_ptr updReply = nullptr;
	bool result = false;
	try{
		bm = new_TBaseMessage_ptr(msg);
		int kind = bm->getID();
		if (kind == SYSTEM_ERR_ID){
			sysErr = new_TSystemErrorMessage_ptr(bm);
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
		this->fCallbackObj->onUpdateStart();
		token = updReply->getToken();
		updReply.reset();

		//init ping timer
		if (this->fPingTimer != nullptr)
			delete this->fPingTimer;
		this->fPingTimer = new deadline_timer(this->fMainIoService, boost::posix_time::seconds(PING_INTERVAL));
		this->fPingTimer->async_wait(bind(&TStorageClient::pingServer, this, token, boost::asio::placeholders::error));

		logToFile("TStorageClient", "updateCurrentVersion", "Start sending files");

		bool is_ok = true;
		List<UserFile^>^ flist = gcnew List<UserFile^>();
		try{
			if (is_regular_file(p)){
				is_ok = this->processFile(token, p, p, flist);
			}
			else if (is_directory(p)){
				is_ok = this->processDirectory(token, p, p, flist);
			}

			if (!is_ok){
				this->fCallbackObj->onUpdateError("Temporary error; Update session interrupted.");
				return;
			}

			//update is finished; send stop
			if (!this->sendMsg((TBaseMessage_ptr&)move_TBaseMessage_ptr(new_TUpdateStopReqMessage_ptr(token)))){
				this->fCallbackObj->onUpdateError("Temporary error; Update session interrupted.");
				return;
			}
			msg = this->readMsg();
			if (msg == nullptr){
				this->fCallbackObj->onUpdateError("Temporary error; Update session interrupted.");
				return;
			}

			TUpdateStopReplyMessage_ptr updstopReply = nullptr;
			try{
				is_ok = true;
				bm = new_TBaseMessage_ptr(msg);
				int kind = bm->getID();
				if (kind == SYSTEM_ERR_ID){
					sysErr = new_TSystemErrorMessage_ptr(bm);
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
						this->fCallbackObj->onUpdateSuccess(flist, v, unmarshalString(vdate));
					}
					else{
						errorToFile("TStorageClient", "updateCurrentVersion", "Update session aborted by the server!");
						this->fCallbackObj->onUpdateError("Temporary error; Update session interrupted.");
					}

					updstopReply.reset();
				}
				else{
					errorToFile("TStorageClient", "updateCurrentVersion", getMessageName(kind) + " is invalid");
					this->fCallbackObj->onUpdateError("Temporary error; Update session interrupted.");
					is_ok = false;
				}
			}
			catch (EMessageException& e){
				errorToFile("TStorageClient", "updateCurrentVersion", e.getMessage());
				this->fCallbackObj->onUpdateError("Temporary error; Update session interrupted.");
				bm.reset();
				sysErr.reset();
				updstopReply.reset();
			}
			bm.reset();
		}
		catch (const filesystem_error& e){
			errorToFile("TStorageClient", "updateCurrentVersion", e.what());
			this->fCallbackObj->onUpdateError("Temporary error; Update session interrupted.");
		}
	}
	else{
		errorToFile("TStorageClient", "updateCurrentVersion", "User " + aUser + " cannot start an update session.");
		this->fCallbackObj->onUpdateError("Temporary error; Update session cannot be started.");
	}

	////init session cleaner timer if necessary
	//if (this->fUpdateTimer == nullptr){
	//	this->fUpdateTimer = new deadline_timer(this->fMainIoService, boost::posix_time::seconds(10));
	//	this->fUpdateTimer->async_wait(bind(&TStorageClient::updateCurrentVersion, this, aUser, aPass, path(aRootPath), boost::asio::placeholders::error));
	//}
}

void TStorageClient::restoreVersion(const string& aUser, const string& aPass, const int aVersion, const string& aDestPath){
	path root(aDestPath);
	bool is_err = false;
	string_ptr msg = nullptr;
	TBaseMessage_ptr bm = nullptr;
	TSystemErrorMessage_ptr sysErr = nullptr;

	if (aUser.empty()){
		this->fCallbackObj->onRestoreError("Username cannot be empty!");
		return;
	}
	if (aPass.empty()){
		this->fCallbackObj->onRestoreError("Password cannot be empty!");
		return;
	}
	if (aDestPath.empty()){
		this->fCallbackObj->onRestoreError("Destination directory cannot be empty!");
		return;
	}
	if (aVersion <= 0){
		errorToFile("TStorageClient", "restoreVersion", "Required version cannot be lower than 0");
		this->fCallbackObj->onRestoreError("Required version cannot be lower than 0");
		return;
	}

	//verify if an update session could be started
	logToFile("TStorageClient", "restoreVersion", "Verify if a restore session could be started for user " + aUser);
	if (!this->sendMsg((TBaseMessage_ptr&)move_TBaseMessage_ptr(new_TRestoreVerReqMessage_ptr(aUser, aPass, aVersion)))){
		this->fCallbackObj->onRestoreError("Temporary error; Restore of version " + aVersion + " cannot be started.");
		return;
	}
	msg = this->readMsg();
	if (msg == nullptr){
		this->fCallbackObj->onRestoreError("Temporary error; Restore of version " + aVersion + " cannot be started.");
		return;
	}

	TRestoreVerReplyMessage_ptr restReply = nullptr;
	bool result = false;
	try{
		bm = new_TBaseMessage_ptr(msg);
		int kind = bm->getID();
		if (kind == SYSTEM_ERR_ID){
			sysErr = new_TSystemErrorMessage_ptr(bm);
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

		//init ping timer
		if (this->fPingTimer != nullptr)
			delete this->fPingTimer;
		this->fPingTimer = new deadline_timer(this->fMainIoService, boost::posix_time::seconds(PING_INTERVAL));
		this->fPingTimer->async_wait(bind(&TStorageClient::pingServer, this, token, boost::asio::placeholders::error));

		logToFile("TStorageClient", "restoreVersion", "Start receiving files");
		bool exit = false;
		while (!exit){
			msg = this->readMsg();
			if (msg == nullptr){
				is_err = true;
				break;
			}

			TRestoreFileMessage_ptr restFile = nullptr;
			TRestoreStopMessage_ptr restStop = nullptr;
			bool result = false;
			try{
				bm = new_TBaseMessage_ptr(msg);
				int kind = bm->getID();
				if (kind == SYSTEM_ERR_ID){
					sysErr = new_TSystemErrorMessage_ptr(bm);
					errorToFile("TStorageClient", "restoreVersion", sysErr->getDetail());
					exit = is_err = true;
				}
				else if (kind == RESTORE_FILE_ID){
					restFile = make_TRestoreFileMessage_ptr(bm);
					path f(restFile->getFilePath());
					logToFile("TStorageClient", "restoreVersion", "received file " + f.string());
					TFileAckMessage_ptr ack = nullptr;
					try{
						storeFile(root / f, restFile->getFileContent());
						if (!this->sendMsg((TBaseMessage_ptr&)move_TBaseMessage_ptr(new_TRestoreFileAckMessage_ptr(true, token, f.string()))))
							exit = is_err = true;
					}
					catch (EFilesystemException& e){
						errorToFile("TStorageClient", "restoreVersion", e.getMessage());
						if (!this->sendMsg((TBaseMessage_ptr&)move_TBaseMessage_ptr(new_TRestoreFileAckMessage_ptr(false, token, f.string()))))
							exit = is_err = true;
					}
				}
				else if (kind == RESTORE_STOP_ID){
					restStop = make_TRestoreStopMessage_ptr(bm);
					logToFile("TStorageClient", "restoreVersion", "Version " + to_string(restStop->getVersion()) + " [" + formatFileDate(restStop->getVersionDate()) + "] completely restored.");
					exit = true;
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
			this->fCallbackObj->onRestoreError("Temporary error; Restore of version " + aVersion + " interrupted!");
			removeDir(root);
		}
	}
	else{
		errorToFile("TStorageClient", "restoreVersion", "User " + aUser + " cannot start a restore session.");
		this->fCallbackObj->onRestoreError("Temporary error; Restore of version " + aVersion + " cannot start.");
	}
}

void TStorageClient::getLastVersion(const string& aUser, const string& aPass){
	string_ptr msg = nullptr;
	TBaseMessage_ptr bm = nullptr;
	TSystemErrorMessage_ptr sysErr = nullptr;

	int version = 0;
	if (!this->sendMsg((TBaseMessage_ptr&)move_TBaseMessage_ptr(new_TGetLastVerReqMessage_ptr(aUser, aPass))))
		return;
	msg = this->readMsg();

	TGetLastVerReplyMessage_ptr lastVer = nullptr;
	try{
		bm = new_TBaseMessage_ptr(msg);
		int kind = bm->getID();
		if (kind == SYSTEM_ERR_ID){
			sysErr = new_TSystemErrorMessage_ptr(bm);
			errorToFile("TStorageClient", "getLastVersion", sysErr->getDetail());
		}
		else if (kind == GET_LAST_VERSION_REPLY_ID){
			lastVer = make_TGetLastVerReplyMessage_ptr(bm);
			version = lastVer->getVersion();
		}
		else{
			errorToFile("TStorageClient", "getLastVersion", getMessageName(kind) + " is invalid");
		}
	}
	catch (EMessageException& e){
		errorToFile("TStorageClient", "getLastVersion", e.getMessage());
	}
	bm.reset();
	sysErr.reset();
	lastVer.reset();

	if (version == 0)
		warningToFile("TStorageClient", "getLastVersion", "User " + aUser + " has no version to restore");
}

//std::list<UserVersion> TStorageClient::getAllVersions(const string& aUser, const string& aPass){
//	std::list<UserVersion> result;
//
//	if (aUser.empty() || aPass.empty())
//		return result;
//
//	string_ptr msg = nullptr;
//	TBaseMessage_ptr bm = nullptr;
//	TSystemErrorMessage_ptr sysErr = nullptr;
//
//	if (!this->sendMsg((TBaseMessage_ptr&)move_TBaseMessage_ptr(new_TGetVersionsReqMessage_ptr(aUser, aPass))))
//		return result;
//	msg = this->readMsg();
//
//	TGetVersionsReplyMessage_ptr verReply = nullptr;
//	try{
//		bm = new_TBaseMessage_ptr(msg);
//		int kind = bm->getID();
//		if (kind == SYSTEM_ERR_ID){
//			sysErr = new_TSystemErrorMessage_ptr(bm);
//			errorToFile("TStorageClient", "getAllVersions", sysErr->getDetail());
//		}
//		else if (kind == GET_VERSIONS_REPLY_ID){
//			verReply = make_TGetVersionsReplyMessage_ptr(bm);
//			int tot = verReply->getTotVersions();
//			if (tot > 0){
//				int oldest = verReply->getOldestVersion();
//				int last = verReply->getLastVersion();
//				logToFile("TStorageClient", "getAllVersions", "Versions of user " + aUser + ":");
//				for (int i = oldest; i <= last; i++){
//					string vd = formatFileDate(verReply->getVersionDate(i));
//					logToFile("TStorageClient", "getAllVersions", "-\tVersion " + to_string(i) + " [" + vd + "]");
//					result.emplace_back(unmarshalString(vd), i);
//				}
//			}
//		}
//		else{
//			errorToFile("TStorageClient", "getAllVersions", getMessageName(kind) + " is invalid");
//		}
//	}
//	catch (EMessageException& e){
//		errorToFile("TStorageClient", "getAllVersions", e.getMessage());
//	}
//	bm.reset();
//	sysErr.reset();
//	verReply.reset();
//
//	return result;
//}

void TStorageClient::pingServer(const string& aToken, const boost::system::error_code& aErr){
	if (!aErr){
		if (this->fSessionOpen.load(boost::memory_order_acquire)){
			//send ping to server
			logToFile("TStorageClient", "pingServer", "Send ping to server fo token " + aToken);
			if (this->sendMsg((TBaseMessage_ptr&)move_TBaseMessage_ptr(new_TPingReqMessage_ptr(aToken)))){
				string_ptr msg = this->readMsg();
				if (msg == nullptr){
					errorToFile("TStorageClient", "pingServer", "Error receiving ping message from server.");
				}
			}
			else
				errorToFile("TStorageClient", "pingServer", "Unable to send ping message.");
		}
	}
	else
		errorToFile("TStorageClient", "pingServer", aErr.message());

	delete this->fPingTimer;
	if (this->fSessionOpen.load(boost::memory_order_acquire)){
		//re-init session cleaner timer
		this->fPingTimer = new deadline_timer(this->fMainIoService, boost::posix_time::seconds(PING_INTERVAL));
		this->fPingTimer->async_wait(bind(&TStorageClient::pingServer, this, aToken, boost::asio::placeholders::error));
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
			case UPDATE_REQ:{
				UpdateRequest^ ur = (UpdateRequest^)req;
				this->updateCurrentVersion(marshalString(ur->getUser()), marshalString(ur->getPass()), marshalString(ur->getPath()));
				break;
			}
			case GET_VERSIONS_REQ:{
				//GetVerRequest^ gvr = (GetVerRequest^)req;
				//this->getAllVersions(marshalString(gvr->getUser()), marshalString(gvr->getPass()));
				break;
			}
			case RESTORE_REQ:{
				RestoreRequest^ rr = (RestoreRequest^)req;
				this->restoreVersion(marshalString(rr->getUser()), marshalString(rr->getPass()), rr->getVersion(), marshalString(rr->getDestPath()));
				break;
			}
			default:{
				errorToFile("TStorageClient", "processRequest", "invalid request ignored");
			}
		}
	}
}

const bool TStorageClient::issueRequest(UserRequest^ aRequest){
	if (this->fQueue != nullptr){
		this->fQueue->pushRequest(aRequest);
		return true;
	}
	else
		return false;
}

