/*
 * Author: Angelo Prudentino
 * Date: 17/04/2016
 * File: Executor.h
 * Description: this file contains the classes implementing message consumers
 *              both for client and server side
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
public class IBaseExecutorController : public IServerBaseController {
public:
	virtual bool isInQueueEmpty() = 0;
	virtual TMessageContainer_ptr getMessageToProcess() = 0;
	virtual void enqueueMessageToSend(TMessageContainer_ptr& aMsg) = 0;
};


//////////////////////////////////////
//    IServerExecutorController    //
//////////////////////////////////////
public class IServerExecutorController : public IBaseExecutorController {
public:
	virtual void processRegistrationRequest(TConnectionHandle aConnection, TUserRegistrReqMessage_ptr& aMsg) = 0;
	virtual void processUpdateStart(TConnectionHandle aConnection, TUpdateStartReqMessage_ptr& aMsg) = 0;
	virtual void processAddNewFile(TConnectionHandle aConnection, TAddNewFileMessage_ptr& aMsg) = 0;
	virtual void processUpdateFile(TConnectionHandle aConnection, TUpdateFileMessage_ptr& aMsg) = 0;
	virtual void processRemoveFile(TConnectionHandle aConnection, TRemoveFileMessage_ptr& aMsg) = 0;
	virtual void processUpdateStop(TConnectionHandle aConnection, TUpdateStopReqMessage_ptr& aMsg) = 0;
	virtual void processGetVersions(TConnectionHandle aConnection, TGetVersionsReqMessage_ptr& aMsg) = 0;
	virtual void processGetLastVersion(TConnectionHandle aConnection, TGetLastVerReqMessage_ptr& aMsg) = 0;
	virtual void processRestoreVersion(TConnectionHandle aConnection, TRestoreVerReqMessage_ptr& aMsg) = 0;
	virtual void processRestoreFileAck(TConnectionHandle aConnection, TRestoreFileAckMessage_ptr& aMsg) = 0;
	virtual void processPingRequest(TConnectionHandle aConnection, TPingReqMessage_ptr& aMsg) = 0;
};


//////////////////////////////////////
//        TMessageExecutor          //
//////////////////////////////////////
public class TMessageExecutor{
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
