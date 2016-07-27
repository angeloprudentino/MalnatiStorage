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
		return move_TBaseMessage_ptr(this->fMsg);
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
TMessageQueue::TMessageQueue() {
	this->fMustExit.store(false, boost::memory_order_release); 
};

TMessageQueue::~TMessageQueue(){
	this->fMustExit.store(true, boost::memory_order_release);
	this->fCond.notify_all();

	//unique_lock<mutex> lock(this->fMutex);

	//int size = (int)this->fQueue.size();
	//for (int i = 0; i < size; i++){
	//	TMessageContainer_ptr temp = move_TMessageContainer_ptr(this->fQueue.front());
	//	this->fQueue.pop();
	//	temp.reset();
	//}
}

bool TMessageQueue::isEmpty(){
	unique_lock<mutex> lock(this->fMutex);
	return this->fQueue.empty();
}

TMessageContainer_ptr TMessageQueue::popMessage(){
	unique_lock<mutex> lock(this->fMutex);

	while (this->fQueue.empty() && !this->fMustExit.load(boost::memory_order_acquire)){
		this->fCond.wait(lock);
	}

	//return null object if should exit
	if (this->fMustExit.load(boost::memory_order_acquire)){
		//empty queue before leaving
		int size = (int)this->fQueue.size();
		for (int i = 0; i < size; i++){
			TMessageContainer_ptr temp = move_TMessageContainer_ptr(this->fQueue.front());
			this->fQueue.pop();
			temp.reset();
		}

		return nullptr;
	}

	TMessageContainer_ptr res = move_TMessageContainer_ptr(this->fQueue.front());
	this->fQueue.pop();

	return move_TMessageContainer_ptr(res);
}

void TMessageQueue::pushMessage(TMessageContainer_ptr& aMsg){
	//do not push if should exit
	if (this->fMustExit.load(boost::memory_order_acquire))
		return;

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
