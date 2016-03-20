/*
 * Author: Angelo Prudentino
 * Date: 30/09/2015
 * File: ServerSocket.h
 * Description: this file contains all classes related to Server-side network protocol implementation
 *
 */

#pragma once

#include <boost/asio.hpp>
#include <string>
#include <vcclr.h>
#include "NetworkController.h"

using namespace boost;
using namespace boost::asio;
using namespace boost::asio::ip;

class TServerSocket;

//////////////////////////////////////
//      TServerSockController	    //
//////////////////////////////////////

// Socket controller class
public class TServerSockController : public IServerSockController{
private:
	int fServerPort = -1;

	TServerSocket* fSock;
	IServerSockController* fCallbackObj = nullptr;
public:
	TServerSockController(int AServerPort, IServerSockController* aCallback);
	virtual ~TServerSockController();

	void StartSocket();

	void onServerSockCreate() override;
	void onServerSockLog(string aClassName, string aFuncName, string aMsg) override;
	void onServerSockError(string aClassName, string aFuncName, string aMsg) override;
	void onServerSockCriticalError(string aClassName, string aFuncName, string aMsg) override;
	void onServerSockAccept(TConnectionHandle aConnection) override;
	void onServerSockRead(TConnectionHandle aConnection, string aMsg) override;
};


//////////////////////////////////////
//         TServerSocket	        //
//////////////////////////////////////

//Boost-asio server socket class 
public class TServerSocket{
private:
	int fAcceptPort = -1;
	io_service fMainIoService;
	tcp::acceptor fServerAcceptor;
	std::list<TConnection> fConnections;

	//Callback object
	IServerSockController* fCallbackObj = nullptr;

	void doAsyncRead(TConnectionHandle aConnction);
	void handleAccept(TConnectionHandle aConnection, boost::system::error_code const& aErr);
	void handleRead(TConnectionHandle aConnection, const boost::system::error_code& aErr, std::size_t aBytes);

public:
	TServerSocket(IServerSockController* aCallbackObj);
	virtual ~TServerSocket();

	bool setAcceptState(int aAcceptPort);
	void doAccept();
};

