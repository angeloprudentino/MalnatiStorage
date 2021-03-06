/*
 * Authors: Angelo Prudentino & Daniele Testa
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
#include <boost/atomic.hpp>

#include "Message.h"
#include "NetworkController.h"

//////////////////////////////////////
//       TMessageContainer          //
//////////////////////////////////////
class TMessageContainer{
private:
	TBaseMessage_ptr fMsg = nullptr;
	TConnection_ptr fConnection = nullptr;

public:
	TMessageContainer() : fConnection(){};
	TMessageContainer(TBaseMessage_ptr& aMsg, TConnection_ptr& aConnection);
	TMessageContainer(const TMessageContainer&) = delete;            // disable copying
	TMessageContainer& operator=(const TMessageContainer&) = delete; // disable assignment
	~TMessageContainer();
	
	//getters
	TBaseMessage_ptr getMessage();
	TConnection_ptr getConnection();
	const bool isEmpty();
};
typedef std::unique_ptr<TMessageContainer> TMessageContainer_ptr;
#define new_TMessageContainer_ptr(aMsg, aConnection) std::make_unique<TMessageContainer>(aMsg, aConnection)
#define make_TMessageContainer_ptr(ptr) std::make_unique<TMessageContainer>(ptr)
#define move_TMessageContainer_ptr(ptr) std::move(ptr)



//////////////////////////////////////
//        TMessageQueue	            //
//////////////////////////////////////
class TMessageQueue{
private:
	queue<TMessageContainer_ptr> fQueue;
	mutex fMutex;
	condition_variable fCond;
	atomic<bool> fMustExit;

public:
	TMessageQueue();
	~TMessageQueue();
	TMessageQueue(const TMessageQueue&) = delete;            // disable copying
	TMessageQueue& operator=(const TMessageQueue&) = delete; // disable assignment

	void clear();

	bool isEmpty();
	TMessageContainer_ptr popMessage();
	void pushMessage(TMessageContainer_ptr& aMsg);
};