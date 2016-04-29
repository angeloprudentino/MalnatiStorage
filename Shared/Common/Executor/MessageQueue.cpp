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
TMessageContainer::TMessageContainer(TBaseMessage aMsg, TConnectionHandle aConnection){
	this->fMsg = aMsg;
	this->fConnection = aConnection;
}
#pragma endregion


//////////////////////////////////////
//        TMessageQueue	            //
//////////////////////////////////////
#pragma region "TMessageQueue"
const TMessageContainer TMessageQueue::popMessage(){
	unique_lock<mutex> lock(this->fMutex);

	while (this->fQueue.empty()){
		this->fCond.wait(lock);
	}
	TMessageContainer res = this->fQueue.front();
	this->fQueue.pop();

	return res;
}

void TMessageQueue::pushMessage(const TMessageContainer aMsg){
	try{
		unique_lock<mutex> lock(this->fMutex);

		this->fQueue.push(aMsg);
	}
	catch (...){
		//try-catch block to automatically release lock
	}

	this->fCond.notify_one();
}
#pragma endregion
