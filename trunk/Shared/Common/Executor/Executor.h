/*
 * Author: Angelo Prudentino
 * Date: 17/04/2016
 * File: Executor.h
 * Description: this file contains the class implementing message consumer
 *
 */

#pragma once
#include <boost/thread/thread.hpp>
#include <boost/atomic.hpp>

#include "Utility.h"
#include "MessageQueue.h"
#include "ServerController.h"

using namespace boost;


///////////////////////////////////
//    IBaseExecutorController    //
///////////////////////////////////
class IBaseExecutorController : public IServerBaseController {
public:
	virtual bool isMessageQueueEmpty() = 0;
	virtual TMessageContainer_ptr getMessageToProcess() = 0;
	virtual void sendMessage(TMessageContainer_ptr& aMsg, const bool aCloseAfterSend) = 0;
};


//////////////////////////////////////
//    IServerExecutorController    //
//////////////////////////////////////
class IServerExecutorController : public IBaseExecutorController {
public:
	virtual void processRegistrationRequest(TConnection_ptr& aConnection, TUserRegistrReqMessage_ptr& aMsg) = 0;
	virtual void processUpdateStart(TConnection_ptr& aConnection, TUpdateStartReqMessage_ptr& aMsg) = 0;
	virtual void processAddNewFile(TConnection_ptr& aConnection, TAddNewFileMessage_ptr& aMsg) = 0;
	virtual void processUpdateFile(TConnection_ptr& aConnection, TUpdateFileMessage_ptr& aMsg) = 0;
	virtual void processRemoveFile(TConnection_ptr& aConnection, TRemoveFileMessage_ptr& aMsg) = 0;
	virtual void processUpdateStop(TConnection_ptr& aConnection, TUpdateStopReqMessage_ptr& aMsg) = 0;
	virtual void processGetVersions(TConnection_ptr& aConnection, TGetVersionsReqMessage_ptr& aMsg) = 0;
	virtual void processGetLastVersion(TConnection_ptr& aConnection, TGetLastVerReqMessage_ptr& aMsg) = 0;
	virtual void processRestoreVersion(TConnection_ptr& aConnection, TRestoreVerReqMessage_ptr& aMsg) = 0;
	virtual void processRestoreFileAck(TConnection_ptr& aConnection, TRestoreFileAckMessage_ptr& aMsg) = 0;
	virtual void processPingRequest(TConnection_ptr& aConnection, TPingReqMessage_ptr& aMsg) = 0;
	virtual void processVerifyCred(TConnection_ptr& aConnection, TVerifyCredReqMessage_ptr& aMsg) = 0;
};


//////////////////////////////////////
//        TMessageExecutor          //
//////////////////////////////////////
class TMessageExecutor{
private:
	atomic<bool> fMustExit;
	thread_group fThreadPool;
	IServerExecutorController* fCallbackObj = nullptr;

	void serverExecutor();

public:
	TMessageExecutor(IServerExecutorController* aCallbackObj);
	TMessageExecutor(const TMessageExecutor&) = delete;            // disable copying
	TMessageExecutor& operator=(const TMessageExecutor&) = delete; // disable assignment
	~TMessageExecutor();

	void stopExecutors();
};
