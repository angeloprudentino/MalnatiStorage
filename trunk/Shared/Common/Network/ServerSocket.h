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
#include <string>
#include <vcclr.h>
#include "NetworkController.h"
#include "Message.h"
#include "MessageQueue.h"
#include "Executor.h"

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
	TMessageQueue* fOutQueue = nullptr;
	IServerSockController* fCallbackObj = nullptr;

public:
	TServerSockController(int AServerPort, IServerSockController* aCallback);
	virtual ~TServerSockController();

	void startSocket();
	void sendBaseMessage(TConnectionHandle aConnection, TBaseMessage aMsg);
	
	void onServerLog(string aClassName, string aFuncName, string aMsg) override;
	void onServerWarning(string aClassName, string aFuncName, string aMsg) override;
	void onServerError(string aClassName, string aFuncName, string aMsg) override;
	void onServerCriticalError(string aClassName, string aFuncName, string aMsg) override;
	void onServerSockCreate() override;
	void onServerSockAccept(TConnectionHandle aConnection) override;
	void onServerSockRead(TConnectionHandle aConnection, string_ptr aMsg) override;
	void onServerSockWrite() override;

	TMessageContainer getMessageToProcess() override;
	void enqueueMessageToSend(TMessageContainer aMsg) override;
};


//////////////////////////////////////
//         TServerSocket	        //
//////////////////////////////////////

//Boost-asio server socket class 
public class TServerSocket{
private:
	int fAcceptPort = -1;
	thread* fMainIO = nullptr;
	io_service fMainIoService;
	tcp::acceptor fServerAcceptor;
	std::list<TConnection> fConnections;

	//Callback object
	IServerSockController* fCallbackObj = nullptr;

	void doAsyncRead(TConnectionHandle aConnction);
	void handleAccept(TConnectionHandle aConnection, const boost::system::error_code& aErr);
	void handleRead(TConnectionHandle aConnection, const boost::system::error_code& aErr, std::size_t aBytes);
	void handle_write(TConnectionHandle aConnection, string_ptr aMsgBuffer, const boost::system::error_code& aErr);
public:
	TServerSocket(IServerSockController* aCallbackObj);
	virtual ~TServerSocket();

	bool setAcceptState(int aAcceptPort);
	void doAccept();
	void doSend(TConnectionHandle aConnection, TBaseMessage aMsg);
};

