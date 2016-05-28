/*
 * Author: Angelo Prudentino
 * Date: 02/10/2015
 * File: StorageServer.h
 * Description: This is the main class of the server
 *
 */

#pragma once
#include <string>

#include "ServerSocket.h"
#include "Executor.h"
#include "Utility"

////////////////////////////////////
//        TStorageServer	      //
////////////////////////////////////
public class TStorageServer : public IServerSockController, public IServerExecutorController{
private:
	int fServerPort = -1;

	//this is the way to use managed obj inside unmanaged classes
	gcroot<IManagedServerSockController^> fCallbackObj = nullptr;
	
	TServerSockController* fSockController = nullptr;
	IBaseExecutorController* fExeController = nullptr;
	TMessageExecutor* fExecutor = nullptr;

public:
	TStorageServer(int AServerPort, IManagedServerSockController^ aCallbackObj);
	~TStorageServer();

	void startServer();
	void stopServer();

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
	void processUpdateStartRequest(TConnectionHandle aConnection, TUpdateStartReqMessage_ptr& aMsg) override;
	void processAddNewFile(TConnectionHandle aConnection, TAddNewFileMessage_ptr& aMsg) override;
	void processUpdateFile(TConnectionHandle aConnection, TUpdateFileMessage_ptr& aMsg) override;
	void processRemoveFile(TConnectionHandle aConnection, TRemoveFileMessage_ptr& aMsg) override;
	void processUpdateStopRequest(TConnectionHandle aConnection, TUpdateStopReqMessage_ptr& aMsg) override;
	void processGetVersions(TConnectionHandle aConnection, TGetVersionsReqMessage_ptr& aMsg) override;
	void processRestoreVersion(TConnectionHandle aConnection, TRestoreVerReqMessage_ptr& aMsg) override;
	void processRestoreFileAck(TConnectionHandle aConnection, TRestoreFileAckMessage_ptr& aMsg) override;
	void processPingRequest(TConnectionHandle aConnection, TPingReqMessage_ptr& aMsg) override;
};

