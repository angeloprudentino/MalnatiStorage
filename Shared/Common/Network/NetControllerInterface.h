/*
* Author: Angelo Prudentino
* Date: 30/09/2015
* File: NetControllerInterface.h
* Description: this file contains interfaces for socket controllers
*
*/

#pragma once
#include <boost/asio.hpp>

using namespace boost;
using namespace boost::asio;
using namespace boost::asio::ip;

public interface class ManagedServerSockControllerInterface{
public:
	void onServerSockCreate();
	void onServerSockError(std::string className, std::string funcName, std::string msg);
	void onServerSockLog(std::string className, std::string funcName, std::string msg);
};

public class ServerSockControllerInterface{
public:
	virtual void onServerSockCreate() = 0;
	virtual void onServerSockLog(std::string className, std::string funcName, std::string msg) = 0;
	virtual void onServerSockError(std::string className, std::string funcName, std::string msg) = 0;
	virtual void onAccept(tcp::socket* sock, tcp::endpoint* endp) = 0;
};
