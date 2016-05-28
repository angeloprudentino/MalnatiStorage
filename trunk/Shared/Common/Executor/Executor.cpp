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

#ifdef STORAGE_SERVER
void TMessageExecutor::serverExecutor(){
	while (!this->fMustExit.load(boost::memory_order_acquire) && this->fCallbackObj != nullptr && !this->fCallbackObj->isInQueueEmpty()){
		TMessageContainer_ptr msg;
		if (this->fCallbackObj != nullptr)
			msg = this->fCallbackObj->getMessageToProcess();

		if (!msg->isEmpty()){
			TBaseMessage_ptr bm = msg->getMessage();
			int kind = bm->getID();

			switch (kind){
			case USER_REG_REQ_ID:
				doServerLog(this->fCallbackObj, "TMessageExecutor", "serverExecutor", "calling processRegistrationRequest method of the controller");
				try{
					TUserRegistrReqMessage_ptr mptr = std::make_unique<TUserRegistrReqMessage>(bm);
					this->fCallbackObj->processRegistrationRequest(msg->getConnection(), mptr);
				}
				catch (EMessageException& e){
					doServerError(this->fCallbackObj, "TMessageExecutor", "serverExecutor", "error creating TUserRegistrReqMessage: " + e.getMessage());
				}
				break;

			case UPDATE_START_REQ_ID:
				doServerLog(this->fCallbackObj, "TMessageExecutor", "serverExecutor", "calling processUpdateStartRequest method of the controller");
				try{
					TUpdateStartReqMessage_ptr mptr = std::make_unique<TUpdateStartReqMessage>(bm);
					this->fCallbackObj->processUpdateStartRequest(msg->getConnection(), mptr);
				}
				catch (EMessageException& e){
					doServerError(this->fCallbackObj, "TMessageExecutor", "serverExecutor", "error creating TUpdateStartReqMessage: " + e.getMessage());
				}
				break;

			case ADD_NEW_FILE_ID:
				doServerLog(this->fCallbackObj, "TMessageExecutor", "serverExecutor", "calling processAddNewFile method of the controller");
				try{
					TAddNewFileMessage_ptr mptr = std::make_unique<TAddNewFileMessage>(bm);
					this->fCallbackObj->processAddNewFile(msg->getConnection(), mptr);
				}
				catch (EMessageException& e){
					doServerError(this->fCallbackObj, "TMessageExecutor", "serverExecutor", "error creating TAddNewFileMessage: " + e.getMessage());
				}
				break;

			case UPDATE_FILE_ID:
				doServerLog(this->fCallbackObj, "TMessageExecutor", "serverExecutor", "calling processUpdateFile method of the controller");
				try{
					TUpdateFileMessage_ptr mptr = std::make_unique<TUpdateFileMessage>(bm);
					this->fCallbackObj->processUpdateFile(msg->getConnection(), mptr);
				}
				catch (EMessageException& e){
					doServerError(this->fCallbackObj, "TMessageExecutor", "serverExecutor", "error creating TUpdateFileMessage: " + e.getMessage());
				}
				break;

			case REMOVE_FILE_ID:
				doServerLog(this->fCallbackObj, "TMessageExecutor", "serverExecutor", "calling processRemoveFile method of the controller");
				try{
					TRemoveFileMessage_ptr mptr = std::make_unique<TRemoveFileMessage>(bm);
					this->fCallbackObj->processRemoveFile(msg->getConnection(), mptr);
				}
				catch (EMessageException& e){
					doServerError(this->fCallbackObj, "TMessageExecutor", "serverExecutor", "error creating TRemoveFileMessage: " + e.getMessage());
				}
				break;

			case UPDATE_STOP_REQ_ID:
				doServerLog(this->fCallbackObj, "TMessageExecutor", "serverExecutor", "calling processUpdateStopRequest method of the controller");
				try{
					TUpdateStopReqMessage_ptr mptr = std::make_unique<TUpdateStopReqMessage>(bm);
					this->fCallbackObj->processUpdateStopRequest(msg->getConnection(), mptr);
				}
				catch (EMessageException& e){
					doServerError(this->fCallbackObj, "TMessageExecutor", "serverExecutor", "error creating TUpdateStopReqMessage: " + e.getMessage());
				}
				break;

			case GET_VERSIONS_REQ_ID:
				doServerLog(this->fCallbackObj, "TMessageExecutor", "serverExecutor", "calling processGetVersions method of the controller");
				try{
					TGetVersionsReqMessage_ptr mptr = std::make_unique<TGetVersionsReqMessage>(bm);
					this->fCallbackObj->processGetVersions(msg->getConnection(), mptr);
				}
				catch (EMessageException& e){
					doServerError(this->fCallbackObj, "TMessageExecutor", "serverExecutor", "error creating TGetVersionsReqMessage: " + e.getMessage());
				}
				break;

			case RESTORE_VER_REQ_ID:
				doServerLog(this->fCallbackObj, "TMessageExecutor", "serverExecutor", "calling processRestoreVersion method of the controller");
				try{
					TRestoreVerReqMessage_ptr mptr = std::make_unique<TRestoreVerReqMessage>(bm);
					this->fCallbackObj->processRestoreVersion(msg->getConnection(), mptr);
				}
				catch (EMessageException& e){
					doServerError(this->fCallbackObj, "TMessageExecutor", "serverExecutor", "error creating TRestoreVerReqMessage: " + e.getMessage());
				}
				break;

			case RESTORE_FILE_ACK_ID:
				doServerLog(this->fCallbackObj, "TMessageExecutor", "serverExecutor", "calling processRestoreFileAck method of the controller");
				try{
					TRestoreFileAckMessage_ptr mptr = std::make_unique<TRestoreFileAckMessage>(bm);
					this->fCallbackObj->processRestoreFileAck(msg->getConnection(), mptr);
				}
				catch (EMessageException& e){
					doServerError(this->fCallbackObj, "TMessageExecutor", "serverExecutor", "error creating TRestoreFileAckMessage: " + e.getMessage());
				}
				break;

			case PING_REQ_ID:
				doServerLog(this->fCallbackObj, "TMessageExecutor", "serverExecutor", "calling processPingRequest method of the controller");
				try{
					TPingReqMessage_ptr mptr = std::make_unique<TPingReqMessage>(bm);
					this->fCallbackObj->processPingRequest(msg->getConnection(), mptr);
				}
				catch (EMessageException& e){
					doServerError(this->fCallbackObj, "TMessageExecutor", "serverExecutor", "error creating TPingReqMessage: " + e.getMessage());
				}
				break;
			
			default:
				//should never get here
				break;
			}
		}
	}

	//safe thread exit
	return;
}
#else
void TMessageExecutor::clientExecutor(){
	while (!this->fMustExit.load(boost::memory_order_acquire)){

	}

	//safe thread exit
}
#endif

#pragma endregion