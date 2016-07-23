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

using namespace boost;


///////////////////////////////////
//    IBaseExecutorController    //
///////////////////////////////////
#ifdef STORAGE_SERVER
public class IBaseExecutorController : public IServerBaseController {
#else
class IBaseExecutorController : public IClientBaseController {
#endif
public:
	virtual bool isInQueueEmpty() = 0;
	virtual TMessageContainer_ptr getMessageToProcess() = 0;
	virtual void enqueueMessageToSend(TMessageContainer_ptr& aMsg) = 0;
};


#ifdef STORAGE_SERVER
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
	virtual void processRestoreVersion(TConnectionHandle aConnection, TRestoreVerReqMessage_ptr& aMsg) = 0;
	virtual void processRestoreFileAck(TConnectionHandle aConnection, TRestoreFileAckMessage_ptr& aMsg) = 0;
	virtual void processPingRequest(TConnectionHandle aConnection, TPingReqMessage_ptr& aMsg) = 0;
};
#else
//////////////////////////////////////
//    IClientExecutorController    //
//////////////////////////////////////
class IClientExecutorController : public IBaseExecutorController {
public:
};
#endif


//////////////////////////////////////
//        TMessageExecutor          //
//////////////////////////////////////
#ifdef STORAGE_SERVER
public class TMessageExecutor{
#else
class TMessageExecutor{
#endif
private:
	atomic<bool> fMustExit;
	thread_group fThreadPool;
#ifdef STORAGE_SERVER
	IServerExecutorController* fCallbackObj = nullptr;
#else
	IClientExecutorController* fCallbackObj = nullptr;
#endif

#ifdef STORAGE_SERVER
	void serverExecutor();
#else
	void clientExecutor();
#endif

public:
#ifdef STORAGE_SERVER
	TMessageExecutor(IServerExecutorController* aCallbackObj);
#else
	TMessageExecutor(IClientExecutorController* aCallbackObj);
#endif

	void stopExecutors();
	~TMessageExecutor();
};
