/*
 * Author: Angelo Prudentino
 * Date: 02/10/2015
 * File: StorageServer.cpp
 * Description: This is the main class of the server
 *
 */

#include "StorageServer.h"
#include "Utility.h"


StorageServer::StorageServer(int AServerPort, ManagedServerSockControllerInterface^ callback){
	this->serverPort = AServerPort;
	this->callbackObj = callback;
	onServerSockLog("StorageServer", "constructor", "creating ServerSockController object...");
	this->sockController = new ServerSockController(this->serverPort, this);
	onServerSockLog("StorageServer", "constructor", "ServerSockController object created");
}

StorageServer::~StorageServer(){
	if (sockController != nullptr){
		onServerSockLog("StorageServer", "destructor", "deleting ServerSockController object...");
		delete sockController;
		onServerSockLog("StorageServer", "destructor", "ServerSockController object deleted");
	}
	sockController = nullptr;
	this->callbackObj = nullptr;
}

void StorageServer::StartServer(){
	onServerSockLog("StorageServer", "StartServer", "starting the internal server socket...");
	this->sockController->StartSocket();
}

void StorageServer::onServerSockCreate(){
	onServerSockLog("StorageServer", "onServerSockCreate", "internal server socket correctly started");
	if (!System::Object::ReferenceEquals(callbackObj,nullptr))
		callbackObj->onServerSockCreate();
}

void StorageServer::onServerSockLog(std::string className, std::string funcName, std::string msg){
	if (!System::Object::ReferenceEquals(callbackObj, nullptr))
		callbackObj->onServerSockLog(className, funcName, msg);
}

void StorageServer::onServerSockError(std::string className, std::string funcName, std::string msg){
	if (!System::Object::ReferenceEquals(callbackObj, nullptr))
		callbackObj->onServerSockError(className, funcName, msg);
}

void StorageServer::onAccept(tcp::socket* sock, tcp::endpoint* endp){

}