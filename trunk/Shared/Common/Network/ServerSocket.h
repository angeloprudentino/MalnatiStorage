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
#include "NetControllerInterface.h"

using namespace boost;
using namespace boost::asio;
using namespace boost::asio::ip;

class ServerSocket;

public class ServerSockController : public ServerSockControllerInterface{
private:
	int serverPort = -1;

	ServerSocket* sock;
	ServerSockControllerInterface* callbackObj = nullptr;
	io_service MainIoService;
public:
	ServerSockController(int AServerPort, ServerSockControllerInterface* callback);
	virtual ~ServerSockController();

	void StartSocket();

	void onServerSockCreate() override;
	void onServerSockLog(std::string className, std::string funcName, std::string msg) override;
	void onServerSockError(std::string className, std::string funcName, std::string msg) override;
	void onAccept(tcp::socket* sock, tcp::endpoint* endp) override;
};

public class ServerSocket{
private:
	int acceptPort = -1;
	tcp::endpoint* peerEndPoint;

	io_service* MainIoServicePtr;
	tcp::acceptor* ServerAcceptor;
	tcp::socket* InternalSocket;
	
	ServerSockControllerInterface* callbackObj = nullptr;
public:
	ServerSocket(io_service& ios);
	virtual ~ServerSocket();

	bool setAcceptState(int AcceptPort);
	void doAccept();

	void setCallbackObj(ServerSockControllerInterface* obj){
		this->callbackObj = obj;
	}
};

