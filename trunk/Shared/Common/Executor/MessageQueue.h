/*
 * Author: Angelo Prudentino
 * Date: 17/04/2016
 * File: MessageQueue.h
 * Description: this file contains the class implementing a thread-safe message queue
 *
 */

#pragma once

#include <queue>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>

#include "Message.h"
#include "NetworkController.h"

//////////////////////////////////////
//       TMessageContainer          //
//////////////////////////////////////
public class TMessageContainer{
private:
	TBaseMessage fMsg;
	TConnectionHandle fConnection;

public:
	TMessageContainer() : fMsg(), fConnection(){};
	TMessageContainer(TBaseMessage aMsg, TConnectionHandle aConnection);
	//~TMessageContainer();
	
	//getters
	const TBaseMessage getMessage(){ return this->fMsg; }
	const TConnectionHandle getConnection(){ return this->fConnection; }
	const bool isEmpty() { return this->fMsg.getID() == NO_ID; }
};



//////////////////////////////////////
//        TMessageQueue	            //
//////////////////////////////////////
public class TMessageQueue{
private:
	queue<TMessageContainer> fQueue;
	mutex fMutex;
	condition_variable fCond;

public:
	TMessageQueue() = default;
	TMessageQueue(const TMessageQueue&) = delete;            // disable copying
	TMessageQueue& operator=(const TMessageQueue&) = delete; // disable assignment

	const TMessageContainer popMessage();
	void pushMessage(const TMessageContainer aMsg);
};