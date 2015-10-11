/*
 * Author: Angelo Prudentino
 * Date: 02/10/2015
 * File: StorageServer.h
 * Description: This is the main class of the server
 *
 */

#pragma once

#include <string>

#include "ServerSocket.h"

public class StorageServer : public ServerSockControllerInterface{
private:
	int serverPort = -1;

	//this is the way to use managed obj inside unmanaged classes
	gcroot<ManagedServerSockControllerInterface^> callbackObj = nullptr;
	
	ServerSockController* sockController = nullptr;

public:
	StorageServer(int AServerPort, ManagedServerSockControllerInterface^ callback);
	~StorageServer();

	void StartServer();

	void onServerSockCreate() override;
	void onServerSockLog(std::string className, std::string funcName, std::string msg) override;
	void onServerSockError(std::string className, std::string funcName, std::string msg) override;
	void onAccept(tcp::socket* sock, tcp::endpoint* endp) override;
};

