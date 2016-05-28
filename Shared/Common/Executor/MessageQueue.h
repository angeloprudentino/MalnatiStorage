/*
 * Author: Angelo Prudentino
 * Date: 17/04/2016
 * File: MessageQueue.h
 * Description: this file contains the class implementing a thread-safe message queue
 *
 */

#pragma once

#include <queue>
#include <memory>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>

#include "Message.h"
#include "NetworkController.h"

//////////////////////////////////////
//       TMessageContainer          //
//////////////////////////////////////
public class TMessageContainer{
private:
	TBaseMessage_ptr fMsg = nullptr;
	TConnectionHandle fConnection;

public:
	TMessageContainer() : fConnection(){};
	TMessageContainer(TBaseMessage_ptr& aMsg, TConnectionHandle aConnection);
	~TMessageContainer();
	
	//getters
	TBaseMessage_ptr getMessage();
	const TConnectionHandle getConnection();
	const bool isEmpty();
};
typedef std::unique_ptr<TMessageContainer> TMessageContainer_ptr;



//////////////////////////////////////
//        TMessageQueue	            //
//////////////////////////////////////
public class TMessageQueue{
private:
	queue<TMessageContainer_ptr> fQueue;
	mutex fMutex;
	condition_variable fCond;

public:
	TMessageQueue() = default;
	~TMessageQueue(){ this->fCond.notify_all(); };
	TMessageQueue(const TMessageQueue&) = delete;            // disable copying
	TMessageQueue& operator=(const TMessageQueue&) = delete; // disable assignment

	bool isEmpty();
	TMessageContainer_ptr popMessage();
	void pushMessage(TMessageContainer_ptr& aMsg);
};