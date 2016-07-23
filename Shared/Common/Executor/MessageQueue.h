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
#ifdef STORAGE_SERVER
public class TMessageContainer{
#else
class TMessageContainer{
#endif
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
#define new_TMessageContainer_ptr(aMsg, aConnection) std::make_unique<TMessageContainer>(aMsg, aConnection)
#define make_TMessageContainer_ptr(ptr) std::make_unique<TMessageContainer>(ptr)
#define move_TMessageContainer_ptr(ptr) std::move(ptr)



//////////////////////////////////////
//        TMessageQueue	            //
//////////////////////////////////////
#ifdef STORAGE_SERVER
public class TMessageQueue{
#else
class TMessageQueue{
#endif
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