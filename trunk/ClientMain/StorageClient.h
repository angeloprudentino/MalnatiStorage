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
	gcroot<StorageClientController^> fCallbackObj = nullptr;
	RequestsQueue* fQueue = nullptr;
	thread* fExecutor = nullptr;

	void connect(const string& aHost, int aPort);
	void disconnect();
	const bool sendMsg(TBaseMessage_ptr& aMsg);
	string_ptr readMsg();
	void processDirectory(const string& aToken, const path& aDirPath);
	void processFile(const string& aToken, const path& aFilePath);
	const int getLastVersion(const string& aUser, const string& aPass);
	string_ptr verifyUser(const string& aUser, const string& aPass);
	const bool registerUser(const string& aUser, const string& aPass, const string& aRootPath);
	void updateCurrentVersion(const string& aUser, const string& aPass);
	void restoreVersion(const string& aUser, const string& aPass, const int aVersion, const string& aDestPath);
	List<UserVersion^>^ getAllVersions(const string& aUser, const string& aPass);

	void processRequest();

public:
	TStorageClient(StorageClientController^ aCallbackObj);
	~TStorageClient();
	TStorageClient(const TStorageClient&) = delete;            // disable copying
	TStorageClient& operator=(const TStorageClient&) = delete; // disable assignment

	const bool issueRequest(UserRequest^ aRequest);
};