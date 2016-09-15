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


//////////////////////////////////
//       TStorageClient	        //
//////////////////////////////////
#pragma region "TStorageClient"
const bool TStorageClient::sendMsg(TBaseMessage_ptr& aMsg){
	if (aMsg == nullptr){
		errorToFile("TStorageClient", "sendMsg", "msg to send is null!");
		return false;
	}
	string_ptr msg = aMsg->encodeMessage();
	msg->append("\n");
	boost::asio::write(this->fSock, boost::asio::buffer(*msg));
	msg.reset();
	aMsg.reset();

	return true;
}

string_ptr TStorageClient::readMsg(){
	boost::asio::streambuf* buf = new boost::asio::streambuf();
	boost::asio::read_until(this->fSock, *buf, END_MSG + string("\n"));
	istream*is = new istream(buf);
	string_ptr msg = new_string_ptr();
	getline(*is, *msg);
	delete buf;
	delete is;

	return move_string_ptr(msg);
}

void TStorageClient::processDirectory(const string& aToken, const path& aDirPath){
	for (directory_entry& x : directory_iterator(aDirPath)){
		path f = x.path().relative_path();
		if (is_regular_file(f)){
			this->processFile(aToken, f);
		}
		else if (is_directory(f))
			this->processDirectory(aToken, f);
	}
}

void TStorageClient::processFile(const string& aToken, const path& aFilePath){
	string_ptr msg = nullptr;
	TBaseMessage_ptr bm = nullptr;
	TSystemErrorMessage_ptr sysErr = nullptr;
	TFileAckMessage_ptr ack = nullptr;
	bool ackResp = false;

	logToFile("TStorageClient", "processFile", "Sending file: " + aFilePath.string());
	try{
		if (!this->sendMsg((TBaseMessage_ptr&)move_TBaseMessage_ptr(new_TAddNewFileMessage_ptr(aToken, aFilePath.string()))))
			return;
		msg = this->readMsg();

		bm = new_TBaseMessage_ptr(msg);
		int kind = bm->getID();
		if (kind == SYSTEM_ERR_ID){
			sysErr = new_TSystemErrorMessage_ptr(bm);
			errorToFile("TStorageClient", "processFile", sysErr->getDetail());
			sysErr.reset();
			ackResp = false;
		}
		else if (kind == FILE_ACK_ID){
			ack = make_TFileAckMessage_ptr(bm);
			ackResp = ack->getResp();
			string file = ack->getFilePath();
			ack.reset();
			if (!ackResp)
				logToFile("TStorageClient", "processFile", "Received bad ack for file: " + file);
		}
		else{
			errorToFile("TStorageClient", "processFile", getMessageName(kind) + " is invalid!");
			ackResp = false;
		}

		bm.reset();
	}
	catch (EMessageException& e){
		errorToFile("TStorageClient", "processFile", e.getMessage());

		bm.reset();
		sysErr.reset();
		ack.reset();
	}
}

void TStorageClient::connect(const string& aHost, int aPort){
	tcp::endpoint ep(ip::address::from_string(aHost), aPort); 
	this->fSock.connect(ep);
}

void TStorageClient::disconnect(){
	this->fSock.shutdown(socket_base::shutdown_both);
	this->fSock.close();
}

TStorageClient::TStorageClient(StorageClientController^ aCallbackObj) : fMainIoService(), fSock(fMainIoService){
	this->fCallbackObj = aCallbackObj;
}

const bool TStorageClient::verifyUser(const string& aUser, const string& aPass){
	if (aUser.empty() || aPass.empty())
		return false;

	string_ptr msg = nullptr;
	TBaseMessage_ptr bm = nullptr;
	TSystemErrorMessage_ptr sysErr = nullptr;

	//verify if user exists
	logToFile("TStorageClient", "verifyUser", "Verify if user " + aUser + " is valid");
	if (!this->sendMsg((TBaseMessage_ptr&)move_TBaseMessage_ptr(new_TVerifyCredReqMessage_ptr(aUser, aPass))))
		return false;
	msg = this->readMsg();

	TVerifyCredReplyMessage_ptr verReply = nullptr;
	try{
		bm = new_TBaseMessage_ptr(msg);
		verReply = new_TVerifyCredReplyMessage_ptr(bm);
		bm.reset();
	}
	catch (EMessageException& e){
		errorToFile("TStorageClient", "verifyUser", e.getMessage());

		bm.reset();
		verReply.reset();
		return false;
	}

	bool resp = verReply->getResp();
	verReply.reset();

	return resp;
}

const bool TStorageClient::registerUser(const string& aUser, const string& aPass, const string& aRootPath){
	if (aUser.empty() || aPass.empty())
		return false;

	bool result = false;
	string_ptr msg = nullptr;
	TBaseMessage_ptr bm = nullptr;
	TSystemErrorMessage_ptr sysErr = nullptr;

	logToFile("TStorageClient", "registerUser", "Try to register a new user " + aUser);
	if (!this->sendMsg((TBaseMessage_ptr&)move_TBaseMessage_ptr(new_TUserRegistrReqMessage_ptr(aUser, aPass))))
		return false;
	msg = this->readMsg();

	TUserRegistrReplyMessage_ptr regReply = nullptr;
	try{
		bm = new_TBaseMessage_ptr(msg);
		int kind = bm->getID();
		if (kind == SYSTEM_ERR_ID){
			sysErr = new_TSystemErrorMessage_ptr(bm);
			errorToFile("TStorageClient", "registerUser", sysErr->getDetail());
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
			result = false;
		}
	}
	catch (EMessageException& e){
		errorToFile("TStorageClient", "registerUser", e.getMessage());

		bm.reset();
		sysErr.reset();
		regReply.reset();
		return false;
	}

	bm.reset();

	if (result)
		logToFile("TStorageClient", "registerUser", "User " + aUser + " registered");
	else
		warningToFile("TStorageClient", "registerUser", "User " + aUser + " cannot be registered");


	if (result){
		//register current user in db with user, pass, dir
	}

	return result;
}

void TStorageClient::updateCurrentVersion(const string& aUser, const string& aPass){
	string_ptr msg = nullptr;
	TBaseMessage_ptr bm = nullptr;
	TSystemErrorMessage_ptr sysErr = nullptr;

	//verify if an update session could be started
	logToFile("TStorageClient", "updateCurrentVersion", "Verify if an update session could be started for user " + aUser);
	if (!this->sendMsg((TBaseMessage_ptr&)move_TBaseMessage_ptr(new_TUpdateStartReqMessage_ptr(aUser, aPass))))
		return;
	msg = this->readMsg();

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
		token = updReply->getToken();
		updReply.reset();

		logToFile("TStorageClient", "updateCurrentVersion", "Start sending files");

		path p("testDir");
		try{
			if (exists(p)){
				if (is_regular_file(p)){
					this->processFile(token, p);
				}
				else if (is_directory(p)){
					this->processDirectory(token, p);
				}

				//update is finished; send stop
				if (!this->sendMsg((TBaseMessage_ptr&)move_TBaseMessage_ptr(new_TUpdateStopReqMessage_ptr(token))))
					return;
				msg = this->readMsg();

				TUpdateStopReplyMessage_ptr updstopReply = nullptr;
				try{
					bm = new_TBaseMessage_ptr(msg);
					int kind = bm->getID();
					if (kind == SYSTEM_ERR_ID){
						sysErr = new_TSystemErrorMessage_ptr(bm);
						errorToFile("TStorageClient", "updateCurrentVersion", sysErr->getDetail());
						sysErr.reset();
					}
					else if (kind == UPDATE_STOP_REPLY_ID){
						updstopReply = make_TUpdateStopReplyMessage_ptr(bm);
						if (updstopReply->getResp()){
							logToFile("TStorageClient", "updateCurrentVersion", "Version " + to_string(updstopReply->getVersion()) + " at " + formatFileDate(updstopReply->getTime()));
						}
						else
							errorToFile("TStorageClient", "updateCurrentVersion", "Update session aborted by the server!");

						updstopReply.reset();
					}
					else{
						errorToFile("TStorageClient", "updateCurrentVersion", getMessageName(kind) + " is invalid");
					}
				}
				catch (EMessageException& e){
					errorToFile("TStorageClient", "updateCurrentVersion", e.getMessage());
					bm.reset();
					sysErr.reset();
					updstopReply.reset();
				}
				bm.reset();
			}
			else
				errorToFile("TStorageClient", "updateCurrentVersion", p.string() + " does not exist");
		}
		catch (const filesystem_error& e){
			errorToFile("TStorageClient", "updateCurrentVersion", e.what());
		}
	}
	else
		errorToFile("TStorageClient", "updateCurrentVersion", "User " + aUser + " cannot start an update session.");
}

void TStorageClient::restoreVersion(const string& aUser, const string& aPass, const int aVersion, const string& aDestPath){
	path root(aDestPath);
	string_ptr msg = nullptr;
	TBaseMessage_ptr bm = nullptr;
	TSystemErrorMessage_ptr sysErr = nullptr;

	if (aVersion <= 0){
		errorToFile("TStorageClient", "restoreVersion", "required version cannot be lower than 0");
		return;
	}

	//verify if an update session could be started
	logToFile("TStorageClient", "restoreVersion", "Verify if a restore session could be started for user " + aUser);
	if (!this->sendMsg((TBaseMessage_ptr&)move_TBaseMessage_ptr(new_TRestoreVerReqMessage_ptr(aUser, aPass, aVersion))))
		return;
	msg = this->readMsg();

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

		logToFile("TStorageClient", "restoreVersion", "Start receiving files");
		bool exit = false;
		while (!exit){
			msg = this->readMsg();

			TRestoreFileMessage_ptr restFile = nullptr;
			TRestoreStopMessage_ptr restStop = nullptr;
			bool result = false;
			try{
				bm = new_TBaseMessage_ptr(msg);
				int kind = bm->getID();
				if (kind == SYSTEM_ERR_ID){
					sysErr = new_TSystemErrorMessage_ptr(bm);
					errorToFile("TStorageClient", "restoreVersion", sysErr->getDetail());
					exit = true;
				}
				else if (kind == RESTORE_FILE_ID){
					restFile = make_TRestoreFileMessage_ptr(bm);
					path f(restFile->getFilePath());
					logToFile("TStorageClient", "restoreVersion", "received file " + f.string());
					TFileAckMessage_ptr ack = nullptr;
					try{
						storeFile(root / f, restFile->getFileContent());
						if (!this->sendMsg((TBaseMessage_ptr&)move_TBaseMessage_ptr(new_TRestoreFileAckMessage_ptr(true, token, f.string()))))
							exit = true;
					}
					catch (EFilesystemException& e){
						errorToFile("TStorageClient", "restoreVersion", e.getMessage());
						if (!this->sendMsg((TBaseMessage_ptr&)move_TBaseMessage_ptr(new_TRestoreFileAckMessage_ptr(false, token, f.string()))))
							exit = true;
					}
				}
				else if (kind == RESTORE_STOP_ID){
					restStop = make_TRestoreStopMessage_ptr(bm);
					logToFile("TStorageClient", "restoreVersion", "Version " + to_string(restStop->getVersion()) + " [" + formatFileDate(restStop->getVersionDate()) + "] completely restored.");
					exit = true;
				}
				else{
					errorToFile("TStorageClient", "restoreVersion", getMessageName(kind) + " is invalid");
					exit = true;
				}
			}
			catch (EMessageException& e){
				errorToFile("TStorageClient", "restoreVersion", e.getMessage());
				exit = true;
			}

			bm.reset();
			restFile.reset();
			restStop.reset();
		}
	}
	else
		errorToFile("TStorageClient", "restoreVersion", "User " + aUser + " cannot start a restore session.");
}

const int TStorageClient::getLastVersion(const string& aUser, const string& aPass){
	string_ptr msg = nullptr;
	TBaseMessage_ptr bm = nullptr;
	TSystemErrorMessage_ptr sysErr = nullptr;

	int version = 0;
	if (!this->sendMsg((TBaseMessage_ptr&)move_TBaseMessage_ptr(new_TGetLastVerReqMessage_ptr(aUser, aPass))))
		return version;
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

	return version;
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

const bool TStorageClient::issueRequest(UserRequest aRequest){
	if (aRequest.getID() == LOGIN_REQ)
		this->fCallbackObj->onLoginSuccess();

	return true;
}

void TStorageClient::processRequest(UserRequest aRequest){

}
