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

#define EXECUTOR_NUM 5


//////////////////////////////////////
//        TMessageExecutor          //
//////////////////////////////////////
#pragma region "TMessageExecutor"
TMessageExecutor::TMessageExecutor(IServerExecutorController* aCallbackObj){
	this->fCallbackObj = aCallbackObj;
	//create all threads
	for (int i = 0; i < EXECUTOR_NUM; i++){
		doServerLog(this->fCallbackObj, "TMessageExecutor", "constructor", "creating executor thread " + to_string(i+1));
		auto main = bind(&TMessageExecutor::serverExecutor, this);
		fThreadPool.create_thread(main);
	}

	this->fMustExit.store(false, boost::memory_order_release);
}

void TMessageExecutor::stopExecutors(){
	//all thread must exit when message queue is empty
	this->fMustExit.store(true, boost::memory_order_release);
}

TMessageExecutor::~TMessageExecutor(){
	//stop and free all threads
	doServerLog(this->fCallbackObj, "TMessageExecutor", "destructor", "waiting for executor threads to terminate");
	this->fThreadPool.join_all();

	this->fMustExit.store(false, boost::memory_order_release);
	this->fCallbackObj = nullptr;
}

void TMessageExecutor::serverExecutor(){
    //exit even if other messages are queued to be processed
	while (!this->fMustExit.load(boost::memory_order_acquire)){
		TMessageContainer_ptr msg = nullptr;
		if (this->fCallbackObj != nullptr)
			msg = this->fCallbackObj->getMessageToProcess();

		if (msg != nullptr && !msg->isEmpty()){
			TBaseMessage_ptr bm = msg->getMessage();
			int kind = bm->getID();

			switch (kind){
			case USER_REG_REQ_ID:
				doServerLog(this->fCallbackObj, "TMessageExecutor", "serverExecutor", "calling processRegistrationRequest method of the controller");
				try{
					TUserRegistrReqMessage_ptr mptr = make_TUserRegistrReqMessage_ptr(bm);
					this->fCallbackObj->processRegistrationRequest(msg->getConnection(), mptr);
					mptr.reset();
				}
				catch (EMessageException& e){
					doServerError(this->fCallbackObj, "TMessageExecutor", "serverExecutor", "error creating TUserRegistrReqMessage: " + e.getMessage());
				}
				break;

			case UPDATE_START_REQ_ID:
				doServerLog(this->fCallbackObj, "TMessageExecutor", "serverExecutor", "calling processUpdateStartRequest method of the controller");
				try{
					TUpdateStartReqMessage_ptr mptr = make_TUpdateStartReqMessage_ptr(bm);
					this->fCallbackObj->processUpdateStart(msg->getConnection(), mptr);
					mptr.reset();
				}
				catch (EMessageException& e){
					doServerError(this->fCallbackObj, "TMessageExecutor", "serverExecutor", "error creating TUpdateStartReqMessage: " + e.getMessage());
				}
				break;

			case ADD_NEW_FILE_ID:
				doServerLog(this->fCallbackObj, "TMessageExecutor", "serverExecutor", "calling processAddNewFile method of the controller");
				try{
					TAddNewFileMessage_ptr mptr = make_TAddNewFileMessage_ptr(bm);
					this->fCallbackObj->processAddNewFile(msg->getConnection(), mptr);
					mptr.reset();
				}
				catch (EMessageException& e){
					doServerError(this->fCallbackObj, "TMessageExecutor", "serverExecutor", "error creating TAddNewFileMessage: " + e.getMessage());
				}
				break;

			case UPDATE_FILE_ID:
				doServerLog(this->fCallbackObj, "TMessageExecutor", "serverExecutor", "calling processUpdateFile method of the controller");
				try{
					TUpdateFileMessage_ptr mptr = make_TUpdateFileMessage_ptr(bm);
					this->fCallbackObj->processUpdateFile(msg->getConnection(), mptr);
					mptr.reset();
				}
				catch (EMessageException& e){
					doServerError(this->fCallbackObj, "TMessageExecutor", "serverExecutor", "error creating TUpdateFileMessage: " + e.getMessage());
				}
				break;

			case REMOVE_FILE_ID:
				doServerLog(this->fCallbackObj, "TMessageExecutor", "serverExecutor", "calling processRemoveFile method of the controller");
				try{
					TRemoveFileMessage_ptr mptr = make_TRemoveFileMessage_ptr(bm);
					this->fCallbackObj->processRemoveFile(msg->getConnection(), mptr);
					mptr.reset();
				}
				catch (EMessageException& e){
					doServerError(this->fCallbackObj, "TMessageExecutor", "serverExecutor", "error creating TRemoveFileMessage: " + e.getMessage());
				}
				break;

			case UPDATE_STOP_REQ_ID:
				doServerLog(this->fCallbackObj, "TMessageExecutor", "serverExecutor", "calling processUpdateStopRequest method of the controller");
				try{
					TUpdateStopReqMessage_ptr mptr = make_TUpdateStopReqMessage_ptr(bm);
					this->fCallbackObj->processUpdateStop(msg->getConnection(), mptr);
					mptr.reset();
				}
				catch (EMessageException& e){
					doServerError(this->fCallbackObj, "TMessageExecutor", "serverExecutor", "error creating TUpdateStopReqMessage: " + e.getMessage());
				}
				break;

			case GET_VERSIONS_REQ_ID:
				doServerLog(this->fCallbackObj, "TMessageExecutor", "serverExecutor", "calling processGetVersions method of the controller");
				try{
					TGetVersionsReqMessage_ptr mptr = make_TGetVersionsReqMessage_ptr(bm);
					this->fCallbackObj->processGetVersions(msg->getConnection(), mptr);
					mptr.reset();
				}
				catch (EMessageException& e){
					doServerError(this->fCallbackObj, "TMessageExecutor", "serverExecutor", "error creating TGetVersionsReqMessage: " + e.getMessage());
				}
				break;

			case RESTORE_VER_REQ_ID:
				doServerLog(this->fCallbackObj, "TMessageExecutor", "serverExecutor", "calling processRestoreVersion method of the controller");
				try{
					TRestoreVerReqMessage_ptr mptr = make_TRestoreVerReqMessage_ptr(bm);
					this->fCallbackObj->processRestoreVersion(msg->getConnection(), mptr);
					mptr.reset();
				}
				catch (EMessageException& e){
					doServerError(this->fCallbackObj, "TMessageExecutor", "serverExecutor", "error creating TRestoreVerReqMessage: " + e.getMessage());
				}
				break;

			case RESTORE_FILE_ACK_ID:
				doServerLog(this->fCallbackObj, "TMessageExecutor", "serverExecutor", "calling processRestoreFileAck method of the controller");
				try{
					TRestoreFileAckMessage_ptr mptr = make_TRestoreFileAckMessage_ptr(bm);
					this->fCallbackObj->processRestoreFileAck(msg->getConnection(), mptr);
					mptr.reset();
				}
				catch (EMessageException& e){
					doServerError(this->fCallbackObj, "TMessageExecutor", "serverExecutor", "error creating TRestoreFileAckMessage: " + e.getMessage());
				}
				break;

			case PING_REQ_ID:
				doServerLog(this->fCallbackObj, "TMessageExecutor", "serverExecutor", "calling processPingRequest method of the controller");
				try{
					TPingReqMessage_ptr mptr = make_TPingReqMessage_ptr(bm);
					this->fCallbackObj->processPingRequest(msg->getConnection(), mptr);
					mptr.reset();
				}
				catch (EMessageException& e){
					doServerError(this->fCallbackObj, "TMessageExecutor", "serverExecutor", "error creating TPingReqMessage: " + e.getMessage());
				}
				break;
			
			default:
				//should never get here
				break;
			}
			bm.reset();
		}
		msg.reset();
	}

	//safe thread exit
	return;
}
#pragma endregion