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

public class TStorageServer : public IServerSockController{
private:
	int fServerPort = -1;

	//this is the way to use managed obj inside unmanaged classes
	gcroot<IManagedServerSockController^> fCallbackObj = nullptr;
	
	TServerSockController* fSockController = nullptr;

public:
	TStorageServer(int AServerPort, IManagedServerSockController^ aCallback);
	~TStorageServer();

	void StartServer();

	void onServerSockCreate() override;
	void onServerSockLog(string aClassName, string aFuncName, string aMsg) override;
	void onServerSockError(string aClassName, string aFuncName, string aMsg) override;
	void onServerSockCriticalError(string aClassName, string aFuncName, string aMsg) override; 
	void onServerSockAccept(TConnectionHandle aConnection) override;
	void onServerSockRead(TConnectionHandle aConnection, string aMsg) override;
};

