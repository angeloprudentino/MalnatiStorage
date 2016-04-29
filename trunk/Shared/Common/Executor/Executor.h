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
public class IBaseExecutorController : public IServerBaseController {
public:
	virtual TMessageContainer getMessageToProcess() = 0;
	virtual void enqueueMessageToSend(TMessageContainer aMsg) = 0;
};

#ifdef STORAGE_SERVER
//////////////////////////////////////
//    IServerExecutorController    //
//////////////////////////////////////
public class IServerExecutorController : public IBaseExecutorController {
public:
	virtual void processRegistrationRequest(TConnectionHandle aConnection, TUserRegistrReqMessage aMsg) = 0;
	virtual void processUpdateStartRequest(TConnectionHandle aConnection, TUpdateStartReqMessage aMsg) = 0;
	virtual void processAddNewFile(TConnectionHandle aConnection, TAddNewFileMessage aMsg) = 0;
	virtual void processUpdateFile(TConnectionHandle aConnection, TUpdateFileMessage aMsg) = 0;
	virtual void processRemoveFile(TConnectionHandle aConnection, TRemoveFileMessage aMsg) = 0;
	virtual void processUpdateStopRequest(TConnectionHandle aConnection, TUpdateStopReqMessage aMsg) = 0;
	virtual void processGetVersions(TConnectionHandle aConnection, TGetVersionsReqMessage aMsg) = 0;
	virtual void processRestoreVersion(TConnectionHandle aConnection, TRestoreVerReqMessage aMsg) = 0;
	virtual void processRestoreFileAck(TConnectionHandle aConnection, TRestoreFileAckMessage aMsg) = 0;
	virtual void processPingRequest(TConnectionHandle aConnection, TPingReqMessage aMsg) = 0;
};
#else
//////////////////////////////////////
//    IClientExecutorController    //
//////////////////////////////////////
public class IClientExecutorController : public IBaseExecutorController {
public:
};
#endif


//////////////////////////////////////
//        TMessageExecutor          //
//////////////////////////////////////
public class TMessageExecutor{
private:
	atomic<bool> fMustExit;
	thread_group fThreadPool;
#ifdef STORAGE_SERVER
	IServerExecutorController* fCallbackObj = nullptr;
#else
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
#endif

	~TMessageExecutor();
};
