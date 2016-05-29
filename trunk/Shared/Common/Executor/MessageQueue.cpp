/*
 * Author: Angelo Prudentino
 * Date: 17/04/2016
 * File: MessageQueue.cpp
 * Description: this file contains the class implementing a thread-safe message queue
 *
 */

#include "MessageQueue.h"


//////////////////////////////////////
//       TMessageContainer          //
//////////////////////////////////////
#pragma region "TMessageContainer"
TMessageContainer::TMessageContainer(TBaseMessage_ptr& aMsg, TConnectionHandle aConnection){
	this->fMsg = move_TMessageContainer_ptr(aMsg);
	this->fConnection = aConnection;
}

TMessageContainer::~TMessageContainer(){
	if (this->fMsg != nullptr){
		this->fMsg.reset();
		this->fMsg = nullptr;
	}
}

TBaseMessage_ptr TMessageContainer::getMessage(){
	if (this->fMsg != nullptr)
		return move_TMessageContainer_ptr(this->fMsg);
	else
		return nullptr;
}

const TConnectionHandle TMessageContainer::getConnection(){
	return this->fConnection; 
}

const bool TMessageContainer::isEmpty(){
	if (this->fMsg != nullptr)
		return this->fMsg->getID() == NO_ID;
	else
		return true;
}
#pragma endregion


//////////////////////////////////////
//        TMessageQueue	            //
//////////////////////////////////////
#pragma region "TMessageQueue"
bool TMessageQueue::isEmpty(){
	unique_lock<mutex> lock(this->fMutex);
	return this->fQueue.empty();
}

TMessageContainer_ptr TMessageQueue::popMessage(){
	unique_lock<mutex> lock(this->fMutex);

	while (this->fQueue.empty()){
		this->fCond.wait(lock);
	}
	TMessageContainer_ptr res = move_TMessageContainer_ptr(this->fQueue.front());
	this->fQueue.pop();

	return move_TMessageContainer_ptr(res);
}

void TMessageQueue::pushMessage(TMessageContainer_ptr& aMsg){
	try{
		unique_lock<mutex> lock(this->fMutex);

		this->fQueue.push(move_TMessageContainer_ptr(aMsg));
	}
	catch (...){
		//try-catch block to automatically release lock
	}

	this->fCond.notify_one();
}
#pragma endregion
