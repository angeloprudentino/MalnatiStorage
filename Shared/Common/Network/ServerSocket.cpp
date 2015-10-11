/*
 * Author: Angelo Prudentino
 * Date: 30/09/2015
 * File: ServerSocket.cpp
 * Description: this file contains all classes related to Server-side network protocol implementation
 *
 */

#include "ServerSocket.h"

//////////////////////////////////////
//      ServerSockController	    //
//////////////////////////////////////
ServerSockController::ServerSockController(int AServerPort, ServerSockControllerInterface* callback){
	this->serverPort = AServerPort;

	this->callbackObj = callback;
	onServerSockLog("ServerSockController", "constructor", "creating ServerSocket object...");
	this->sock = new ServerSocket(MainIoService);
	onServerSockLog("ServerSockController", "constructor", "ServerSocket object created");
	this->sock->setCallbackObj(this);
}

ServerSockController::~ServerSockController(){
	this->callbackObj = nullptr;
	if (this->sock != nullptr)
		delete this->sock;
	this->sock = nullptr;
	//this->MainIoService.stop();
}

void ServerSockController::StartSocket(){
	onServerSockLog("ServerSockController", "StartSocket", "setting the internal socket into accepting state...");
	if (!this->sock->setAcceptState(this->serverPort)){
		//server socket not ready; exit
		this->callbackObj->onServerSockError("ServerSockController", "setAcceptState", "unable to open a passive socket");
		return;
	}
	onServerSockLog("ServerSockController", "StartSocket", "internal server socket is ready to accept");
	this->callbackObj->onServerSockCreate();

	// socket server is created and ready to accept connections	
	this->sock->doAccept();
	//this->MainIoService.run();
}
void ServerSockController::onServerSockCreate(){
	if (this->callbackObj != nullptr)
		this->callbackObj->onServerSockCreate();
}

void ServerSockController::onServerSockLog(std::string className, std::string funcName, std::string msg){
	if (this->callbackObj != nullptr)
		this->callbackObj->onServerSockLog(className, funcName, msg);
}

void ServerSockController::onServerSockError(std::string className, std::string funcName, std::string msg){
	if (this->callbackObj != nullptr)
		this->callbackObj->onServerSockError(className, funcName, msg);
}

void ServerSockController::onAccept(tcp::socket* sock, tcp::endpoint* endp){

}

//////////////////////////////////////
//         ServerSocket	            //
//////////////////////////////////////
ServerSocket::ServerSocket(io_service& ios){
	this->peerEndPoint = new tcp::endpoint();
	this->ServerAcceptor = new tcp::acceptor(ios);
	this->InternalSocket = new tcp::socket(ios);
}

ServerSocket::~ServerSocket(){
	this->callbackObj = nullptr;
	delete this->peerEndPoint;
	delete this->ServerAcceptor;
	delete this->InternalSocket;
}

bool ServerSocket::setAcceptState(int AcceptPort){
	this->acceptPort = AcceptPort;
	boost::system::error_code ec;
	tcp::endpoint Acceptendpoint(tcp::v4(), this->acceptPort);

	if (!this->ServerAcceptor->is_open()){
		this->ServerAcceptor->open(tcp::v4(), ec);
		if (ec){
			this->callbackObj->onServerSockLog("ServerSocket", "setAcceptState", "unable to open a passive socket");
			return false;
		}
	}

	//setting socket options
	socket_base::linger opt_linger(true, 30);
	this->ServerAcceptor->set_option(opt_linger);
	socket_base::keep_alive opt_keep(true);
	this->ServerAcceptor->set_option(opt_keep);

	this->ServerAcceptor->bind(Acceptendpoint, ec);
	if (ec){
		std::string msg("unable to unable to bind port ");
		msg.append(std::to_string(AcceptPort));
		this->callbackObj->onServerSockLog("ServerSocket", "setAcceptState", msg);
		return false;
	}

	this->ServerAcceptor->listen(socket_base::max_connections, ec);
	if (ec){
		this->callbackObj->onServerSockLog("ServerSocket", "setAcceptState", "unable to put the socket in listening state");
		return false;
	}

	return true;
}

void ServerSocket::doAccept(){
	this->ServerAcceptor->async_accept(*(this->InternalSocket), *peerEndPoint,
		[this](boost::system::error_code ec)
	{
		if (!ec){
			this->callbackObj->onServerSockLog("ServerSocket", "doAccept", "connection enstablished with " + peerEndPoint->address().to_string() + ":" + std::to_string(peerEndPoint->port()));
			this->callbackObj->onAccept(std::move(this->InternalSocket), std::move(peerEndPoint));
		}
		else{
			this->callbackObj->onServerSockLog("ServerSocket", "doAccept", "Error trying to enstablish connection with " + peerEndPoint->address().to_string() + ":" + std::to_string(peerEndPoint->port()));
		}

		this->doAccept();
	});

}