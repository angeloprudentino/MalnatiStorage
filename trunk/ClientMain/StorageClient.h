/*
 * Author: Angelo Prudentino
 * Date: 14/09/2016
 * File: StorageClient.h
 * Description: this file contains all classes implementing
 *              client-side core logic
 *
 */

#pragma once
#include <vcclr.h>
#include "Utility.h"
#include "Message.h"
#include "Session.h"
#include "ClientController.h"
#include "ClientRequests.h"
#include <boost/asio.hpp>
#include <boost/atomic.hpp>
#include <boost/thread/thread.hpp>

using namespace std;
using namespace boost;
using namespace boost::asio;
using namespace boost::asio::ip;


//////////////////////////////////
//       TStorageClient	        //
//////////////////////////////////
class TStorageClient{
private:
	io_service fMainIoService;
	tcp::socket* fSock = nullptr;
	boost::atomic<bool> fMustExit;
	boost::atomic<bool> fSessionOpen;
	gcroot<StorageClientController^> fCallbackObj = nullptr;
	RequestsQueue* fQueue = nullptr;
	thread* fExecutor = nullptr;
	deadline_timer* fUpdateTimer = nullptr;
	deadline_timer* fPingTimer = nullptr;

	void connect(const string& aHost, int aPort);
	void disconnect();
	const bool sendMsg(TBaseMessage_ptr& aMsg);
	string_ptr readMsg();
	const bool processDirectory(const string& aToken, const path& aRootPath, const path& aDirPath, List<UserFile^>^ aFileList);
	const bool processFile(const string& aToken, const path& aRootPath, const path& aFilePath, List<UserFile^>^ aFileList);
	void getLastVersion(const string& aUser, const string& aPass);
	void verifyUser(const string& aUser, const string& aPass);
	void registerUser(const string& aUser, const string& aPass, const string& aRootPath);
	void updateCurrentVersion(const string& aUser, const string& aPass, const string& aRootPath);
	void restoreVersion(const string& aUser, const string& aPass, const int aVersion, const string& aDestPath);
	void getAllVersions(const string& aUser, const string& aPass);
	void pingServer(const string& aToken, const boost::system::error_code& aErr);
	
	void processRequest();

public:
	TStorageClient(StorageClientController^ aCallbackObj);
	~TStorageClient();
	TStorageClient(const TStorageClient&) = delete;            // disable copying
	TStorageClient& operator=(const TStorageClient&) = delete; // disable assignment

	const bool issueRequest(UserRequest^ aRequest);
};