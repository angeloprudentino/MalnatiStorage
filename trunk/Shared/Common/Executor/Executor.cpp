/*
 * Author: Angelo Prudentino
 * Date: 17/04/2016
 * File: Executor.cpp
 * Description: this file contains the classes implementing message consumers
 *              both for client and server side
 *
 */

#include <boost\bind.hpp>
#include "Executor.h"
#include "MessageQueue.h"

#ifdef STORAGE_SERVER
  #define EXECUTOR_NUM 5
#else
  #define EXECUTOR_NUM 1
#endif

//////////////////////////////////////
//        TMessageExecutor          //
//////////////////////////////////////
#pragma region "TMessageExecutor"
#ifdef STORAGE_SERVER
TMessageExecutor::TMessageExecutor(IServerExecutorController* aCallbackObj){
#else
#endif
	this->fCallbackObj = aCallbackObj;
	//create all threads
	for (int i = 0; i < EXECUTOR_NUM; i++){
		doServerLog(this->fCallbackObj, "TMessageExecutor", "constructor", "creating executor thread " + to_string(i+1));
#ifdef STORAGE_SERVER
		auto main = bind(&TMessageExecutor::serverExecutor, this);
#else
		auto main = bind(&TMessageExecutor::clientExecutor, this);
#endif
		fThreadPool.create_thread(main);
	}

	this->fMustExit.store(false, boost::memory_order_release);
}

TMessageExecutor::~TMessageExecutor(){
	this->fMustExit.store(false, boost::memory_order_release);

	//stop and free all threads
	doServerLog(this->fCallbackObj, "TMessageExecutor", "destructor", "waiting for executor threads to terminate");
	this->fThreadPool.join_all();

	this->fCallbackObj = nullptr;
}

#ifdef STORAGE_SERVER
void TMessageExecutor::serverExecutor(){
	while (!this->fMustExit.load(boost::memory_order_acquire)){
		TMessageContainer msg;
		if (this->fCallbackObj != nullptr)
			msg = this->fCallbackObj->getMessageToProcess();

		if (!msg.isEmpty()){
			TBaseMessage bm = msg.getMessage();
			int kind = bm.getID();

			switch (kind){
			case USER_REG_REQ_ID:
				doServerLog(this->fCallbackObj, "TMessageExecutor", "serverExecutor", "calling processRegistrationRequest method of the controller");
				this->fCallbackObj->processRegistrationRequest(msg.getConnection(), TUserRegistrReqMessage(bm));
				break;
			case UPDATE_START_REQ_ID:
				doServerLog(this->fCallbackObj, "TMessageExecutor", "serverExecutor", "calling processUpdateStartRequest method of the controller");
				this->fCallbackObj->processUpdateStartRequest(msg.getConnection(), TUpdateStartReqMessage(bm));
				break;
			case ADD_NEW_FILE_ID:
				doServerLog(this->fCallbackObj, "TMessageExecutor", "serverExecutor", "calling processAddNewFile method of the controller");
				this->fCallbackObj->processAddNewFile(msg.getConnection(), TAddNewFileMessage(bm));
				break;
			case UPDATE_FILE_ID:
				doServerLog(this->fCallbackObj, "TMessageExecutor", "serverExecutor", "calling processUpdateFile method of the controller");
				this->fCallbackObj->processUpdateFile(msg.getConnection(), TUpdateFileMessage(bm));
				break;
			case REMOVE_FILE_ID:
				doServerLog(this->fCallbackObj, "TMessageExecutor", "serverExecutor", "calling processRemoveFile method of the controller");
				this->fCallbackObj->processRemoveFile(msg.getConnection(), TRemoveFileMessage(bm));
				break;
			case UPDATE_STOP_REQ_ID:
				doServerLog(this->fCallbackObj, "TMessageExecutor", "serverExecutor", "calling processUpdateStopRequest method of the controller");
				this->fCallbackObj->processUpdateStopRequest(msg.getConnection(), TUpdateStopReqMessage(bm));
				break;
			case GET_VERSIONS_REQ_ID:
				doServerLog(this->fCallbackObj, "TMessageExecutor", "serverExecutor", "calling processGetVersions method of the controller");
				this->fCallbackObj->processGetVersions(msg.getConnection(), TGetVersionsReqMessage(bm));
				break;
			case RESTORE_VER_REQ_ID:
				doServerLog(this->fCallbackObj, "TMessageExecutor", "serverExecutor", "calling processRestoreVersion method of the controller");
				this->fCallbackObj->processRestoreVersion(msg.getConnection(), TRestoreVerReqMessage(bm));
				break;
			case RESTORE_FILE_ACK_ID:
				doServerLog(this->fCallbackObj, "TMessageExecutor", "serverExecutor", "calling processRestoreFileAck method of the controller");
				this->fCallbackObj->processRestoreFileAck(msg.getConnection(), TRestoreFileAckMessage(bm));
				break;
			case PING_REQ_ID:
				doServerLog(this->fCallbackObj, "TMessageExecutor", "serverExecutor", "calling processPingRequest method of the controller");
				this->fCallbackObj->processPingRequest(msg.getConnection(), TPingReqMessage(bm));
				break;
			default:
				//should never get here
				break;
			}
		}
	}

	//safe thread exit
}
#else
void TMessageExecutor::clientExecutor(){
	while (!this->fMustExit.load(boost::memory_order_acquire)){

	}

	//safe thread exit
}
#endif

#pragma endregion