/*
 * Author: Angelo Prudentino
 * Date: 02/10/2015
 * File: StorageServer.h
 * Description: This is the main class of the server
 *
 */

#pragma once
#include <string>
#include <memory>
#include <boost/asio/deadline_timer.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/filesystem.hpp>

#include "ServerSocket.h"
#include "Executor.h"
#include "Utility.h"
#include "Session.h"
#include "DBManagerInterface.h"
#include "ServerController.h"

using namespace std;
using namespace boost::asio;
using namespace boost::filesystem;


////////////////////////////////////
//        TStorageServer	      //
////////////////////////////////////
class TStorageServer : public IServerExecutorController{
private:
	int fServerPort = -1;

	//this is the way to use managed obj inside unmanaged classes
	gcroot<IManagedServerController^> fCallbackObj = nullptr;
	
	io_service fMainIoService;
	deadline_timer* fSessionsCleaner = nullptr;
	TSessions* fSessions = nullptr;
	mutex fSessionsMutex;
	mutex fLogMutex;

	TServerSockController* fSockController = nullptr;
	IBaseExecutorController* fExeController = nullptr;
	TMessageExecutor* fExecutor = nullptr;
	IDBManagerInterface* fDBManager = nullptr;

	const bool newSession(const string& aUser, const string& aToken, const int aSessionType);
	TSession_ptr isThereASessionFor(const string& aUser);
	TSession_ptr isThereAnUpdateSessionFor(const string& aUser);
	TSession_ptr isThereARestoreSessionFor(const string& aUser);
	void removeSession(const string& aUser);
	void checkAndCleanSessions(const boost::system::error_code& aErr);

	const bool userExists(const string& aUser);
	string_ptr checkUserCredential(const string& aUser, const string& aPass);

public:
	TStorageServer(int AServerPort, IManagedServerController^ aCallbackObj); //throws EBaseException
	TStorageServer(const TStorageServer&) = delete;            // disable copying
	TStorageServer& operator=(const TStorageServer&) = delete; // disable assignment
	~TStorageServer();

	void startServer();
	void stopServer();

	void onServerReady(const bool aReadyState) override;
	void onServerLog(const string& aClassName, const string& aFuncName, const string& aMsg) override;
	void onServerWarning(const string& aClassName, const string& aFuncName, const string& aMsg) override;
	void onServerError(const string& aClassName, const string& aFuncName, const string& aMsg) override;
	void onServerCriticalError(const string& aClassName, const string& aFuncName, const string& aMsg) override;

	bool isMessageQueueEmpty() override;
	TMessageContainer_ptr getMessageToProcess() override;
	void sendMessage(TMessageContainer_ptr& aMsg, const bool aCloseAfterSend) override;
	void processRegistrationRequest(TConnection_ptr& aConnection, TUserRegistrReqMessage_ptr& aMsg) override;
	void processUpdateStart(TConnection_ptr& aConnection, TUpdateStartReqMessage_ptr& aMsg) override;
	void processAddNewFile(TConnection_ptr& aConnection, TAddNewFileMessage_ptr& aMsg) override;
	void processUpdateFile(TConnection_ptr& aConnection, TUpdateFileMessage_ptr& aMsg) override;
	void processRemoveFile(TConnection_ptr& aConnection, TRemoveFileMessage_ptr& aMsg) override;
	void processUpdateStop(TConnection_ptr& aConnection, TUpdateStopReqMessage_ptr& aMsg) override;
	void processGetVersions(TConnection_ptr& aConnection, TGetVersionsReqMessage_ptr& aMsg) override;
	void processGetLastVersion(TConnection_ptr& aConnection, TGetLastVerReqMessage_ptr& aMsg) override;
	void processRestoreVersion(TConnection_ptr& aConnection, TRestoreVerReqMessage_ptr& aMsg) override;
	void processRestoreFileAck(TConnection_ptr& aConnection, TRestoreFileAckMessage_ptr& aMsg) override;
	void processPingRequest(TConnection_ptr& aConnection, TPingReqMessage_ptr& aMsg) override;
	void processVerifyCred(TConnection_ptr& aConnection, TVerifyCredReqMessage_ptr& aMsg) override;
};

