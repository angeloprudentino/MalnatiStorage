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
#include <boost/thread/mutex.hpp>
#include <boost/filesystem.hpp>

#include "ServerSocket.h"
#include "Executor.h"
#include "Utility.h"
#include "Session.h"
#include "DBManager.h"

using namespace std;
using namespace boost::filesystem;


////////////////////////////////////
//        TStorageServer	      //
////////////////////////////////////
public class TStorageServer : public IServerSockController, public IServerExecutorController{
private:
	int fServerPort = -1;

	//this is the way to use managed obj inside unmanaged classes
	gcroot<IManagedServerController^> fCallbackObj = nullptr;
	
	TSessions* fSessions = nullptr;
	mutex fSessionsMutex;

	TServerSockController* fSockController = nullptr;
	IBaseExecutorController* fExeController = nullptr;
	TMessageExecutor* fExecutor = nullptr;
	TDBManager* fDBManager = nullptr;

	const bool newSession(const string& aUser, const string& aToken, const int aSessionType);
	TSession_ptr isThereASessionFor(const string& aUser);
	TSession_ptr isThereAnUpdateSessionFor(const string& aUser);
	TSession_ptr isThereARestoreSessionFor(const string& aUser);
	void removeSession(const string& aUser);

	const bool userExists(const string& aUser);
	const bool checkUserCredential(const string& aUser, const string& aPass);

public:
	TStorageServer(int AServerPort, IManagedServerController^ aCallbackObj);
	~TStorageServer();

	const bool startServer();
	//void stopServer();

	void onServerLog(string aClassName, string aFuncName, string aMsg) override;
	void onServerWarning(string aClassName, string aFuncName, string aMsg) override;
	void onServerError(string aClassName, string aFuncName, string aMsg) override;
	void onServerCriticalError(string aClassName, string aFuncName, string aMsg) override; 
	void onServerSockCreate() override;
	void onServerSockAccept(TConnectionHandle aConnection) override;
	void onServerSockRead(TConnectionHandle aConnection, string_ptr& aMsg) override;
	void onServerSockWrite() override;

	bool isInQueueEmpty() override;
	TMessageContainer_ptr getMessageToProcess() override;
	void enqueueMessageToSend(TMessageContainer_ptr& aMsg) override;
	void processRegistrationRequest(TConnectionHandle aConnection, TUserRegistrReqMessage_ptr& aMsg) override;
	void processUpdateStart(TConnectionHandle aConnection, TUpdateStartReqMessage_ptr& aMsg) override;
	void processAddNewFile(TConnectionHandle aConnection, TAddNewFileMessage_ptr& aMsg) override;
	void processUpdateFile(TConnectionHandle aConnection, TUpdateFileMessage_ptr& aMsg) override;
	void processRemoveFile(TConnectionHandle aConnection, TRemoveFileMessage_ptr& aMsg) override;
	void processUpdateStop(TConnectionHandle aConnection, TUpdateStopReqMessage_ptr& aMsg) override;
	void processGetVersions(TConnectionHandle aConnection, TGetVersionsReqMessage_ptr& aMsg) override;
	void processRestoreVersion(TConnectionHandle aConnection, TRestoreVerReqMessage_ptr& aMsg) override;
	void processRestoreFileAck(TConnectionHandle aConnection, TRestoreFileAckMessage_ptr& aMsg) override;
	void processPingRequest(TConnectionHandle aConnection, TPingReqMessage_ptr& aMsg) override;
};

