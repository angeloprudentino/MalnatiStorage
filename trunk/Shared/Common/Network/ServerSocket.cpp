/*
 * Author: Angelo Prudentino
 * Date: 30/09/2015
 * File: ServerSocket.cpp
 * Description: this file contains all classes related to Server-side network protocol implementation
 *
 */

#include <boost\bind.hpp>
#include "ServerSocket.h"
#include "Message.h"

//////////////////////////////////////
//      TServerSockController	    //
//////////////////////////////////////
TServerSockController::TServerSockController(int aServerPort, IServerSockController* aCallback){
	this->fServerPort = aServerPort;

	this->fCallbackObj = aCallback;
	this->onServerSockLog("TServerSockController", "constructor", "creating TServerSocket object...");
	this->fSock = new TServerSocket(this);
	this->onServerSockLog("TServerSockController", "constructor", "TServerSocket object created");
}

TServerSockController::~TServerSockController(){
	if (this->fSock != nullptr){
		this->onServerSockLog("TServerSockController", "destructor", "deleting TServerSocket object...");
		delete this->fSock;
		this->onServerSockLog("TServerSockController", "destructor", "TServerSocket object deleted");
	}
	this->fSock = nullptr;
	this->fCallbackObj = nullptr;
}

void TServerSockController::StartSocket(){
	this->onServerSockLog("TServerSockController", "StartSocket", "setting the internal socket into accepting state...");
	if (!this->fSock->setAcceptState(this->fServerPort)){
		//server socket not ready; exit
		this->fCallbackObj->onServerSockError("TServerSockController", "setAcceptState", "unable to open a passive socket");
		return;
	}
	this->onServerSockLog("TServerSockController", "StartSocket", "internal server socket is ready to accept");
	this->fCallbackObj->onServerSockCreate();

	// socket server is created and ready to accept connections	
	this->fSock->doAccept();
}

void TServerSockController::onServerSockCreate(){
	if (this->fCallbackObj != nullptr)
		this->fCallbackObj->onServerSockCreate();
}

void TServerSockController::onServerSockLog(string aClassName, string aFuncName, string aMsg){
	if (this->fCallbackObj != nullptr)
		this->fCallbackObj->onServerSockLog(aClassName, aFuncName, aMsg);
}

void TServerSockController::onServerSockError(string aClassName, string aFuncName, string aMsg){
	if (this->fCallbackObj != nullptr)
		this->fCallbackObj->onServerSockError(aClassName, aFuncName, aMsg);
}

void TServerSockController::onServerSockCriticalError(string aClassName, string aFuncName, string aMsg){
	if (this->fCallbackObj != nullptr)
		this->fCallbackObj->onServerSockCriticalError(aClassName, aFuncName, aMsg);
}

void TServerSockController::onServerSockAccept(TConnectionHandle aConnection){

}

void TServerSockController::onServerSockRead(TConnectionHandle aConnection, string aMsg){
	tcp::endpoint peer = aConnection->fPeer;
	TBaseMessage bmsg;
	bmsg.decodeMessage(aMsg);
	int type = bmsg.getID();

	switch (type){
	case USER_REG_REQ_ID:
		break;
	case UPDATE_START_REQ_ID:
		break;
	case ADD_NEW_FILE_ID:
		break;
	case UPDATE_FILE_ID:
		break;
	case REMOVE_FILE_ID:
		break;
	case UPDATE_STOP_REQ_ID:
		break;
	case GET_VERSIONS_REQ_ID:
		break;
	case RESTORE_VER_REQ_ID:
		break;
	case RESTORE_FILE_ACK_ID:
		break;
	case PING_REQ_ID:
		this->fCallbackObj->onServerSockLog("TServerSockController", "onServerSockRead", "received a ping message from" + peer.address().to_string() + ":" + std::to_string(peer.port()));
		break;
	default:
		//ignore message
		break;
	}
}

//////////////////////////////////////
//         TServerSocket	        //
//////////////////////////////////////
TServerSocket::TServerSocket(IServerSockController* aCallbackObj): fMainIoService(), fServerAcceptor(fMainIoService){
	this->fCallbackObj = aCallbackObj;
}

TServerSocket::~TServerSocket(){
	this->fMainIoService.stop();
	this->fCallbackObj = nullptr;
}

bool TServerSocket::setAcceptState(int aAcceptPort){
	this->fAcceptPort = aAcceptPort;
	boost::system::error_code ec;
	tcp::endpoint AcceptEndPoint(tcp::v4(), this->fAcceptPort);

	if (!this->fServerAcceptor.is_open()){
		this->fServerAcceptor.open(tcp::v4(), ec);
		if (ec){
			this->fCallbackObj->onServerSockLog("TServerSocket", "setAcceptState", "unable to open a passive socket");
			return false;
		}
	}

	//setting socket options
	socket_base::linger opt_linger(true, 30);
	this->fServerAcceptor.set_option(opt_linger);
	socket_base::keep_alive opt_keep(true);
	this->fServerAcceptor.set_option(opt_keep);
	tcp::acceptor::reuse_address opt_reuse(true);
	this->fServerAcceptor.set_option(opt_reuse);

	this->fServerAcceptor.bind(AcceptEndPoint, ec);
	if (ec){
		string msg("unable to bind port ");
		msg.append(std::to_string(aAcceptPort));
		this->fCallbackObj->onServerSockError("TServerSocket", "setAcceptState", msg);
		return false;
	}

	this->fServerAcceptor.listen(socket_base::max_connections, ec);
	if (ec){
		this->fCallbackObj->onServerSockError("TServerSocket", "setAcceptState", "unable to put the socket in listening state");
		return false;
	}

	return true;
}

void TServerSocket::doAccept(){
		//connection points to the newly inserted element
		TConnectionHandle connection = fConnections.emplace(fConnections.begin(), this->fMainIoService);
		auto handler = bind(&TServerSocket::handleAccept, this, connection, boost::asio::placeholders::error);
		this->fServerAcceptor.async_accept(connection->fSocket, connection->fPeer, handler);
		this->fMainIoService.run();
}

void TServerSocket::handleAccept(TConnectionHandle aConnection, boost::system::error_code const& aErr){
	tcp::endpoint peer = aConnection->fPeer;

	if (!aErr){
		this->fCallbackObj->onServerSockLog("TServerSocket", "doAcceptLambda", "connection enstablished with " + peer.address().to_string() + ":" + std::to_string(peer.port()));
		this->fCallbackObj->onServerSockAccept(aConnection);

		//auto buff = std::make_shared<std::string>("Hello World!\r\n\r\n");
		//auto handler = boost::bind(&TServerSocket::handle_write, this, aConnection, buff, boost::asio::placeholders::error);
		//boost::asio::async_write(aConnection->fSocket, boost::asio::buffer(*buff), handler);
		this->doAsyncRead(aConnection);
	}
	else{
		//remove this connection
		fConnections.erase(aConnection);
		this->fCallbackObj->onServerSockError("TServerSocket", "doAcceptLambda", "Error \"" + aErr.message() + "\" trying to enstablish connection with " + peer.address().to_string() + ":" + std::to_string(peer.port()));
	}

	this->doAccept();
}

void TServerSocket::doAsyncRead(TConnectionHandle aConnection) {
	auto handler = bind(&TServerSocket::handleRead, this, aConnection, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred);
	async_read_until(aConnection->fSocket, aConnection->fReadBuffer, MSG_END, handler);
}

void TServerSocket::handleRead(TConnectionHandle aConnection, const boost::system::error_code& aErr, std::size_t aBytes){
	tcp::endpoint peer = aConnection->fPeer;

	if (aBytes > 0) {
		istream is(&aConnection->fReadBuffer);
		string line;
		getline(is, line);
		this->fCallbackObj->onServerSockLog("TServerSocket", "doAsyncReadLambda", "Message Received from " + peer.address().to_string() + ":" + std::to_string(peer.port()) + "-> " + line);
		this->fCallbackObj->onServerSockRead(aConnection, line);
	}

	if (!aErr) {
		this->doAsyncRead(aConnection);
	}
	else {
		this->fConnections.erase(aConnection);
		this->fCallbackObj->onServerSockError("TServerSocket", "doAsyncReadLambda", "We had an error: " + aErr.message() + "from " + peer.address().to_string() + ":" + std::to_string(peer.port()));
	}

}