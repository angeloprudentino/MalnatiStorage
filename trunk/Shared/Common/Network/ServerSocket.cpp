/*
 * Author: Angelo Prudentino
 * Date: 30/09/2015
 * File: ServerSocket.cpp
 * Description: this file contains all classes related to Server-side network protocol implementation
 *
 */

#include <boost\bind.hpp>

#include "ServerSocket.h"

//////////////////////////////////////
//      TServerSockController	    //
//////////////////////////////////////
#pragma region "TServerSockController"
TServerSockController::TServerSockController(int aServerPort, IServerSockController* aCallback){
	this->fServerPort = aServerPort;

	this->fCallbackObj = aCallback;

	this->onServerLog("TServerSockController", "constructor", "creating incoming message queue object...");
	this->fInQueue = new TMessageQueue();
	this->onServerLog("TServerSockController", "constructor", "incoming message queue object created");

	this->onServerLog("TServerSockController", "constructor", "creating outgoing message queue object...");
	this->fOutQueue = new TMessageQueue();
	this->onServerLog("TServerSockController", "constructor", "outgoing message queue object created");

	this->onServerLog("TServerSockController", "constructor", "creating TServerSocket object...");
	this->fSock = new TServerSocket(this);
	this->onServerLog("TServerSockController", "constructor", "TServerSocket object created");

	this->onServerLog("TServerSockController", "constructor", "creating sender thread object...");

	//TODO: verificare se un thread basta
	this->fSender = new thread(bind(&TServerSockController::sendBaseMessage, this));
	this->onServerLog("TServerSockController", "constructor", "sender thread object created");
}

TServerSockController::~TServerSockController(){
	if (this->fSock != nullptr){
		this->onServerLog("TServerSockController", "destructor", "deleting TServerSocket object...");
		delete this->fSock;
		this->fSock = nullptr;
		this->onServerLog("TServerSockController", "destructor", "TServerSocket object deleted");
	}

	if (this->fInQueue != nullptr){
		this->onServerLog("TServerSockController", "destructor", "deleting incoming message queue object...");
		delete this->fInQueue;
		this->fInQueue = nullptr;
		this->onServerLog("TServerSockController", "destructor", "incoming message queue object deleted");
	}

	if (this->fOutQueue != nullptr){
		this->onServerLog("TServerSockController", "destructor", "deleting outgoing message queue object...");
		delete this->fOutQueue;
		this->fOutQueue = nullptr;
		this->onServerLog("TServerSockController", "destructor", "outgoing message queue object deleted");
	}

	if (this->fSender != nullptr){
		this->onServerLog("TServerSockController", "destructor", "deleting fSender thread object...");
		this->fSender->join();
		delete this->fSender;
		this->fSender = nullptr;
		this->onServerLog("TServerSockController", "destructor", "fSender thread object deleted");
	}

	this->fCallbackObj = nullptr;
}

void TServerSockController::sendBaseMessage(){
	if (this->fSock != nullptr){
		if (this->fOutQueue != nullptr){
			TMessageContainer_ptr msg = this->fOutQueue->popMessage();
			if (msg != nullptr && !msg->isEmpty()){
				TConnectionHandle conn = msg->getConnection();
				TBaseMessage_ptr bmsg = msg->getMessage();
				tcp::endpoint peer = conn->fPeer;
				this->onServerLog("TServerSockController", "sendBaseMessage", "preparing to send a " + bmsg->getName() + " message to " + peer.address().to_string() + ":" + std::to_string(peer.port()));
				this->fSock->doSend(conn, move_TMessageContainer_ptr(bmsg));
			}

			this->sendBaseMessage();
		}
	}
}

const bool TServerSockController::startSocket(){
	this->onServerLog("TServerSockController", "startSocket", "setting the internal socket into accepting state...");
	if (!this->fSock->setAcceptState(this->fServerPort)){
		//server socket not ready; exit
		return false;
	}

	this->onServerLog("TServerSockController", "startSocket", "internal server socket is ready to accept");
	//if (this->fCallbackObj != nullptr)
	//	this->fCallbackObj->onServerSockCreate();

	// socket server is created and ready to accept connections	
	this->fSock->doAccept();
	return true;
}

//void TServerSockController::stopSocketIn(){
//	this->onServerLog("TServerSockController", "stopSocketIn", "closing incoming socket side...");
//	this->fSock->stopIncoming();
//	this->onServerLog("TServerSockController", "stopSocketIn", "incoming socket closed");
//
//}


#pragma region "IServerBaseController implementation"
void TServerSockController::onServerLog(string aClassName, string aFuncName, string aMsg){
	if (this->fCallbackObj != nullptr)
		this->fCallbackObj->onServerLog(aClassName, aFuncName, aMsg);
}

void TServerSockController::onServerWarning(string aClassName, string aFuncName, string aMsg){
	if (this->fCallbackObj != nullptr)
		this->fCallbackObj->onServerWarning(aClassName, aFuncName, aMsg);
}

void TServerSockController::onServerError(string aClassName, string aFuncName, string aMsg){
	if (this->fCallbackObj != nullptr)
		this->fCallbackObj->onServerError(aClassName, aFuncName, aMsg);
}

void TServerSockController::onServerCriticalError(string aClassName, string aFuncName, string aMsg){
	if (this->fCallbackObj != nullptr)
		this->fCallbackObj->onServerCriticalError(aClassName, aFuncName, aMsg);
}
#pragma endregion

#pragma region "IServerSockController implementation"
void TServerSockController::onServerSockCreate(){
	if (this->fCallbackObj != nullptr)
		this->fCallbackObj->onServerSockCreate();
}

void TServerSockController::onServerSockAccept(TConnectionHandle aConnection){

}

void TServerSockController::onServerSockRead(TConnectionHandle aConnection, string_ptr& aMsg){
	tcp::endpoint peer = aConnection->fPeer;
	TBaseMessage_ptr bmsg = new_TBaseMessage_ptr(aMsg);
	int msgType = bmsg->getID();

	if (!isValidMessageID(msgType)){
		this->onServerWarning("TServerSockController", "onServerSockRead", "received an unknown message from " + peer.address().to_string() + ":" + std::to_string(peer.port()));
		bmsg.reset();
		return;
	}

	this->onServerLog("TServerSockController", "onServerSockRead", "received a " + getMessageName(msgType) + " message from " + peer.address().to_string() + ":" + std::to_string(peer.port()));

	//check if the message is valid to be received server-side
	bool valid = ((msgType == USER_REG_REQ_ID) || (msgType == UPDATE_START_REQ_ID) || (msgType == ADD_NEW_FILE_ID)
		|| (msgType == UPDATE_FILE_ID) || (msgType == REMOVE_FILE_ID) || (msgType == UPDATE_STOP_REQ_ID)
		|| (msgType == GET_VERSIONS_REQ_ID) || (msgType == RESTORE_VER_REQ_ID) || (msgType == RESTORE_FILE_ACK_ID) || (msgType == PING_REQ_ID));

	if (!valid){
		//ignore message
		this->onServerWarning("TServerSockController", "onServerSockRead", getMessageName(msgType) + " message should not be received by the server; ignoring it");
		bmsg.reset();
		return;
	}

	//enqueue the message to be processed
	TMessageContainer_ptr mcp = new_TMessageContainer_ptr(move_TBaseMessage_ptr(bmsg), aConnection);
	this->fInQueue->pushMessage(move_TMessageContainer_ptr(mcp));
}

void TServerSockController::onServerSockWrite(){
	//try to send another message, if any
	//this->sendBaseMessage();
}
#pragma endregion

#pragma region "IBaseExecutorController implementation"
bool TServerSockController::isInQueueEmpty(){
	return this->fInQueue->isEmpty();
}

TMessageContainer_ptr TServerSockController::getMessageToProcess(){
	return move_TMessageContainer_ptr(this->fInQueue->popMessage());
}

void TServerSockController::enqueueMessageToSend(TMessageContainer_ptr& aMsg){
	if (!aMsg->isEmpty())
		this->fOutQueue->pushMessage(move_TMessageContainer_ptr(aMsg));
}
#pragma endregion
#pragma endregion


//////////////////////////////////////
//         TServerSocket	        //
//////////////////////////////////////
#pragma region "TServerSocket"
TServerSocket::TServerSocket(IServerSockController* aCallbackObj): fMainIoService(), fServerAcceptor(fMainIoService){
	this->fCallbackObj = aCallbackObj;
}

TServerSocket::~TServerSocket(){
	doServerLog(this->fCallbackObj, "TServerSocket", "destructor", "clearing open connections list...");
	this->fConnections.clear();
	doServerLog(this->fCallbackObj, "TServerSocket", "destructor", "open connections cleared");

	doServerLog(this->fCallbackObj, "TServerSocket", "destructor", "stopping IO service...");
	this->fMainIoService.stop();
	this->fMainIO->join();
	delete this->fMainIO;
	this->fMainIO = nullptr;
	doServerLog(this->fCallbackObj, "TServerSocket", "destructor", "IO service stopped");

	this->fCallbackObj = nullptr;
}

bool TServerSocket::setAcceptState(int aAcceptPort){
	this->fAcceptPort = aAcceptPort;
	boost::system::error_code ec;
	tcp::endpoint AcceptEndPoint(tcp::v4(), this->fAcceptPort);

	if (!this->fServerAcceptor.is_open()){
		this->fServerAcceptor.open(tcp::v4(), ec);
		if (ec){
			doServerCriticalError(this->fCallbackObj, "TServerSocket", "setAcceptState", "unable to open a passive socket");
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
		doServerCriticalError(this->fCallbackObj, "TServerSocket", "setAcceptState", "unable to bind port " + to_string(aAcceptPort));
		return false;
	}

	this->fServerAcceptor.listen(socket_base::max_connections, ec);
	if (ec){
		doServerCriticalError(this->fCallbackObj, "TServerSocket", "setAcceptState", "unable to put the socket in listening state");
		return false;
	}

	return true;
}

void TServerSocket::doAccept(){
		//connection points to the newly inserted element
		TConnectionHandle connection = fConnections.emplace(fConnections.begin(), this->fMainIoService);
		auto handler = bind(&TServerSocket::handleAccept, this, connection, boost::asio::placeholders::error);
		this->fServerAcceptor.async_accept(connection->fSocket, connection->fPeer, handler);
		this->fMainIoService.reset();

		//TODO: controllare se un thread solo basta
		fMainIO = new thread([this](){ this->fMainIoService.run(); });
}

void TServerSocket::handleAccept(TConnectionHandle aConnection, const boost::system::error_code& aErr){
	tcp::endpoint peer = aConnection->fPeer;

	if (!aErr){
		doServerLog(this->fCallbackObj, "TServerSocket", "handleAccept", "connection enstablished with " + peer.address().to_string() + ":" + std::to_string(peer.port()));
		if (this->fCallbackObj != nullptr)
			this->fCallbackObj->onServerSockAccept(aConnection);

		this->doAsyncRead(aConnection);
	}
	else{
		//remove this connection
		this->fConnections.erase(aConnection);
		doServerError(this->fCallbackObj, "TServerSocket", "handleAccept", "Error \"" + aErr.message() + "\" trying to enstablish connection with " + peer.address().to_string() + ":" + std::to_string(peer.port()));
	}

	this->doAccept();
}

void TServerSocket::doAsyncRead(TConnectionHandle aConnection) {
	auto handler = bind(&TServerSocket::handleRead, this, aConnection, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred);
	async_read_until(aConnection->fSocket, aConnection->fReadBuffer, END_MSG, handler);
}

void TServerSocket::handleRead(TConnectionHandle aConnection, const boost::system::error_code& aErr, std::size_t aBytes){
	tcp::endpoint peer = aConnection->fPeer;

	if (!aErr) {
		if (aBytes > 0) {
			istream is(&aConnection->fReadBuffer);
			string_ptr line = new_string_ptr();
			getline(is, *line);
			doServerLog(this->fCallbackObj, "TServerSocket", "handleRead", "Message Received from " + peer.address().to_string() + ":" + std::to_string(peer.port()));
			this->fCallbackObj->onServerSockRead(aConnection, move_string_ptr(line));
			line.reset();
		}

		this->doAsyncRead(aConnection);
	}
	else {
		this->fConnections.erase(aConnection);
		doServerError(this->fCallbackObj, "TServerSocket", "handleRead", "Error \"" + aErr.message() + "\" from " + peer.address().to_string() + ":" + std::to_string(peer.port()));
	}
}

void TServerSocket::doSend(TConnectionHandle aConnection, TBaseMessage_ptr& aMsg){
	string_ptr buff = nullptr;
	try{
		buff = aMsg->encodeMessage();
		aMsg.reset();
		auto handler = boost::bind(&TServerSocket::handle_write, this, aConnection, boost::asio::placeholders::error);
		boost::asio::async_write(aConnection->fSocket, boost::asio::buffer(*buff), handler);
		buff.reset();
	}
	catch (EMessageException e){
		doServerError(this->fCallbackObj, "TServerSocket", "doSend", e.getMessage());
		buff.reset();
	}
}

void TServerSocket::handle_write(TConnectionHandle aConnection, const boost::system::error_code& aErr) {
	tcp::endpoint peer = aConnection->fPeer;

	if (!aErr) {
		doServerLog(this->fCallbackObj, "TServerSocket", "handle_write", "Finished sending message to " + peer.address().to_string() + ":" + std::to_string(peer.port()));
		//if (aConnection->fSocket.is_open()) {
		//	// Write completed successfully and connection is open
		//}
	}
	else {
		//remove this connection
		this->fConnections.erase(aConnection);
		doServerError(this->fCallbackObj, "TServerSocket", "handle_write", "Error \"" + aErr.message() + "\" while sending a message to " + peer.address().to_string() + ":" + std::to_string(peer.port()));
	}

	this->fCallbackObj->onServerSockWrite();
}

//void TServerSocket::stopIncoming(){
//	doServerLog(this->fCallbackObj, "TServerSocket", "stopIncoming", "closing acceptor...");
//	//this->fServerAcceptor.close(); -> porta il MainIO in deadlock
//	doServerLog(this->fCallbackObj, "TServerSocket", "stopIncoming", "acceptor closed");
//}
#pragma endregion