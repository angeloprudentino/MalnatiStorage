/*
 * Author: Angelo Prudentino
 * Date: 30/09/2015
 * File: ServerSocket.h
 * Description: this file contains all classes related to Server-side network protocol implementation
 *
 */

#pragma once
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <boost\bind.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/atomic.hpp>
#include <string>
#include <vcclr.h>
#include "NetworkController.h"
#include "Message.h"
#include "MessageQueue.h"
#include "Executor.h"
#include "ServerController.h"

using namespace boost;
using namespace boost::asio;
using namespace boost::asio::ip;

class TServerSocket;

//////////////////////////////////////
//      TServerSockController	    //
//////////////////////////////////////
class TServerSockController : public IServerSockController, public IBaseExecutorController{
private:
	int fServerPort = -1;
	
	TServerSocket* fSock = nullptr;
	TMessageQueue* fInQueue = nullptr;
	IServerBaseController* fCallbackObj = nullptr;

	const bool checkMessageToSend(const string& aClassName, const string& aFuncName, TBaseMessage_ptr& aBMsg);

	void onServerReady(const bool aReadyState) override;
	void onServerLog(const string& aClassName, const string& aFuncName, const string& aMsg) override;
	void onServerWarning(const string& aClassName, const string& aFuncName, const string& aMsg) override;
	void onServerError(const string& aClassName, const string& aFuncName, const string& aMsg) override;
	void onServerCriticalError(const string& aClassName, const string& aFuncName, const string& aMsg) override;
	void onServerSockAccept(TConnection_ptr& aConnection) override;
	void onServerSockRead(TConnection_ptr& aConnection, string_ptr& aMsg) override;
	void onServerSockWrite() override;

	bool isMessageQueueEmpty() override;
	TMessageContainer_ptr getMessageToProcess() override;
	void sendMessage(TMessageContainer_ptr& aMsg, const bool aCloseAfterSend) override;

public:
	TServerSockController(int AServerPort, io_service* aMainIoService, IServerBaseController* aCallback);
	TServerSockController(const TServerSockController&) = delete;            // disable copying
	TServerSockController& operator=(const TServerSockController&) = delete; // disable assignment
	virtual ~TServerSockController();

	const bool startSocket();
	void stopSocket();
};


//////////////////////////////////////
//         TServerSocket	        //
//////////////////////////////////////
class TServerSocket{
private:
	int fAcceptPort = -1;
	//thread* fMainIO = nullptr;
	io_service* fMainIoService = nullptr;
	tcp::acceptor fServerAcceptor;
	mutex fConnectionsMutex;
	TConnections* fConnections = nullptr;
	atomic<bool> fMustExit;
	mutex fOutQueueMutex;
	list<string_ptr>* fOutQueue = nullptr;

	//Callback object
	IServerSockController* fCallbackObj = nullptr;

	void checkAndRemove(TConnection_ptr& aConnection);
	void doAsyncRead(TConnection_ptr& aConnction);
	void handleAccept(TConnection_ptr& aConnection, const boost::system::error_code& aErr);
	void handleRead(TConnection_ptr& aConnection, const boost::system::error_code& aErr, std::size_t aBytes);
	void handleWrite(TConnection_ptr& aConnection, list<string_ptr>::iterator aBuff, const bool aCloseAfterSend, const boost::system::error_code& aErr);
public:
	TServerSocket(io_service* aMainIoService, IServerSockController* aCallbackObj);
	TServerSocket(const TServerSockController&) = delete;            // disable copying
	TServerSocket& operator=(const TServerSockController&) = delete; // disable assignment
	~TServerSocket();

	bool setAcceptState(int aAcceptPort);
	void doAccept();
	void doSend(TConnection_ptr& aConnection, TBaseMessage_ptr& aMsg, const bool aCloseAfterSend);
	void stopIncoming();
};

