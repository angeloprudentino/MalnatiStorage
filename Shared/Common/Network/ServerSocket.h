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
// Socket controller class
public class TServerSockController : public IServerSockController, public IBaseExecutorController{
private:
	int fServerPort = -1;
	
	TServerSocket* fSock = nullptr;
	TMessageQueue* fInQueue = nullptr;
	IServerBaseController* fCallbackObj = nullptr;

	const bool checkMessageToSend(const string& aClassName, const string& aFuncName, TBaseMessage_ptr& aBMsg);

public:
	TServerSockController(int AServerPort, io_service* aMainIoService, IServerBaseController* aCallback);
	TServerSockController(const TServerSockController&) = delete;            // disable copying
	TServerSockController& operator=(const TServerSockController&) = delete; // disable assignment
	virtual ~TServerSockController();

	const bool startSocket();
	//void stopSocketIn();
	
	void onServerLog(const string& aClassName, const string& aFuncName, const string& aMsg) override;
	void onServerWarning(const string& aClassName, const string& aFuncName, const string& aMsg) override;
	void onServerError(const string& aClassName, const string& aFuncName, const string& aMsg) override;
	void onServerCriticalError(const string& aClassName, const string& aFuncName, const string& aMsg) override;
	void onServerSockAccept(TConnectionHandle aConnection) override;
	void onServerSockRead(TConnectionHandle aConnection, string_ptr& aMsg) override;
	void onServerSockWrite() override;

	bool isMessageQueueEmpty() override;
	TMessageContainer_ptr getMessageToProcess() override;
	void sendMessage(TMessageContainer_ptr& aMsg) override;
};


//////////////////////////////////////
//         TServerSocket	        //
//////////////////////////////////////
//Boost-asio server socket class 
public class TServerSocket{
private:
	int fAcceptPort = -1;
	//thread* fMainIO = nullptr;
	io_service* fMainIoService;
	tcp::acceptor fServerAcceptor;
	TConnectionList fConnections;
	mutex fOutQueueMutex;
	list<string_ptr>* fOutQueue = nullptr;

	//Callback object
	IServerSockController* fCallbackObj = nullptr;

	void doAsyncRead(TConnectionHandle aConnction);
	void handleAccept(TConnectionHandle aConnection, const boost::system::error_code& aErr);
	void handleRead(TConnectionHandle aConnection, const boost::system::error_code& aErr, std::size_t aBytes);
	void handleWrite(TConnectionHandle aConnection, list<string_ptr>::iterator aBuff, const boost::system::error_code& aErr);
public:
	TServerSocket(io_service* aMainIoService, IServerSockController* aCallbackObj);
	TServerSocket(const TServerSockController&) = delete;            // disable copying
	TServerSocket& operator=(const TServerSockController&) = delete; // disable assignment
	~TServerSocket();

	bool setAcceptState(int aAcceptPort);
	void doAccept();
	void doSend(TConnectionHandle aConnection, TBaseMessage_ptr& aMsg);
	//void stopIncoming();
};

