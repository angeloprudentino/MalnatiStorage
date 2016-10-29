/*
 * Authors: Angelo Prudentino & Daniele Testa
 * Date: 14/09/2016
 * File: StorageClient.h
 * Description: this file contains all classes implementing
 *              client-side core logic
 *
 */

#pragma once
#include <vcclr.h>
#include "Utility.h"
#include <boost/asio.hpp>
#include <boost/atomic.hpp>
#include <boost/thread/thread.hpp>
#include "Message.h"
#include "Session.h"
#include "ClientController.h"
#include "ClientRequests.h"
#include "SqliteClientDB.h"

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
	TSqliteDB* fSqliteDB = nullptr;

	// Wrapper methods for UI callbacks invocators
	void onLoginSuccess(String^ aPath);
	void onLoginError(String^ aMsg);
	void onRegistrationSucces();
	void onRegistrationError(String^ aMsg);
	void onUpdateStart(String^ aToken);
	void onUpdateSuccess(List<UserFile^>^ aFileList, const int aVersion, String^ aVersionDate);
	void onUpdateError(String^ aMsg);
	void onRestoreStart(String^ aToken, const bool aStoreOnLocalDB);
	void onRestoreSuccess(const int aVersion, String^ aVersionDate, const bool aStoreOnLocalDB, List<UserFile^>^ aFileList);
	void onRestoreError(String^ aMsg, const bool aStoreOnLocalDB);
	void onGetVersionsSuccess(List<UserVersion^>^ aVersionsList);
	void onGetVersionsError(String^ aMsg);

	void connect(const string& aHost, int aPort);
	void disconnect();
	const bool sendMsg(TBaseMessage_ptr& aMsg);
	string_ptr readMsg();
	TBaseMessage_ptr filterPingMsgs(const string& aCaller);
	const bool processDirectory(const int aVersion, const string& aToken, const path& aRootPath, const path& aDirPath, List<UserFile^>^ aFileList, TUserFileList_ptr& aSqliteFileList);
	const bool processFile(const int aVersion, const string& aToken, const path& aRootPath, const path& aFilePath, List<UserFile^>^ aFileList, TUserFileList_ptr& aSqliteFileList);
	const bool removeFiles(const string& aToken, const path& aRootPath, TUserFileList_ptr& aSqliteFileList);
	void verifyUser(const string& aUser, const string& aPass);
	void registerUser(const string& aUser, const string& aPass, const string& aRootPath);
	int getLastVersion(const string& aUser, const string& aPass);
	void updateCurrentVersion(const string& aUser, const string& aPass, const string& aRootPath);
	const bool restoreVersion(const string& aUser, const string& aPass, const int aVersion, const string& aFile, const string& aDestPath, const bool aStoreOnLocalDB);
	void getAllVersions(const string& aUser, const string& aPass);
	const bool pingServer(const string& aToken);
	
	void processRequest();

public:
	TStorageClient(StorageClientController^ aCallbackObj);
	~TStorageClient();
	TStorageClient(const TStorageClient&) = delete;            // disable copying
	TStorageClient& operator=(const TStorageClient&) = delete; // disable assignment

	const bool issueRequest(UserRequest^ aRequest);
};