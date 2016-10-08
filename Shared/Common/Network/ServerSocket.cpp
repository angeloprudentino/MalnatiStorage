/*
 * Author: Angelo Prudentino
 * Date: 30/09/2015
 * File: ServerSocket.cpp
 * Description: this file contains all classes related to Server-side network protocol implementation
 *
 */

#include <boost\bind.hpp>

#include "ServerSocket.h"

#define DISCONNECTED 2


//////////////////////////////////////
//      TServerSockController	    //
//////////////////////////////////////
#pragma region "TServerSockController"
TServerSockController::TServerSockController(int aServerPort, io_service* aMainIoService, IServerBaseController* aCallback){
	this->fServerPort = aServerPort;

	this->fCallbackObj = aCallback;

	this->onServerLog("TServerSockController", "constructor", "creating incoming message queue object...");
	this->fInQueue = new TMessageQueue();
	this->onServerLog("TServerSockController", "constructor", "incoming message queue object created");

	this->onServerLog("TServerSockController", "constructor", "creating TServerSocket object...");
	this->fSock = new TServerSocket(aMainIoService, this);
	this->onServerLog("TServerSockController", "constructor", "TServerSocket object created");

	this->onServerLog("TServerSockController", "constructor", "creating sender thread object...");
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

	this->fCallbackObj = nullptr;
}

const bool TServerSockController::checkMessageToSend(const string& aClassName, const string& aFuncName, TBaseMessage_ptr& aBMsg){
	int msgType = aBMsg->getID();

	bool valid = ((msgType == USER_REG_REPLY_ID) || (msgType == UPDATE_START_REPLY_ID) || (msgType == VERIFY_CRED_REPLY_ID) || (msgType == SYSTEM_ERR_ID)
		|| (msgType == FILE_ACK_ID) || (msgType == UPDATE_STOP_REPLY_ID) || (msgType == GET_VERSIONS_REPLY_ID) || (msgType == GET_LAST_VERSION_REPLY_ID)
		|| (msgType == RESTORE_VER_REPLY_ID) || (msgType == RESTORE_FILE_ID) || (msgType == RESTORE_STOP_ID) || (msgType == PING_REPLY_ID));

	if (valid){
		this->onServerLog(aClassName, aFuncName, "=> => => => => => => => => => => => => ");
		this->onServerLog(aClassName, aFuncName, "=> => => => => => => => => => => => => ");
		this->onServerLog(aClassName, aFuncName, "=>  ");

		switch (msgType){
		case USER_REG_REPLY_ID:{
			this->onServerLog(aClassName, aFuncName, "=>  UserRegistrReplyMessage");
			this->onServerLog(aClassName, aFuncName, "=>  ");
			bool resp = ((TUserRegistrReplyMessage_ptr&)aBMsg)->getResp();
			if (resp)
				this->onServerLog(aClassName, aFuncName, "=>  result: ok");
			else
				this->onServerLog(aClassName, aFuncName, "=>  result: error");
			break;
		}
		case UPDATE_START_REPLY_ID:{
			this->onServerLog(aClassName, aFuncName, "=>  UpdateStartReplyMessage");
			this->onServerLog(aClassName, aFuncName, "=>  ");

			bool resp = ((TUpdateStartReplyMessage_ptr&)aBMsg)->getResp();
			if (resp)
				this->onServerLog(aClassName, aFuncName, "=>  result: ok");
			else
				this->onServerLog(aClassName, aFuncName, "=>  result: error");
			this->onServerLog(aClassName, aFuncName, "=>  token: " + ((TUpdateStartReplyMessage_ptr&)aBMsg)->getToken());
			break;
		}
		case FILE_ACK_ID:{
			this->onServerLog(aClassName, aFuncName, "=>  FileAckMessage");
			this->onServerLog(aClassName, aFuncName, "=>  ");
			bool resp = ((TFileAckMessage_ptr&)aBMsg)->getResp();
			if (resp)
				this->onServerLog(aClassName, aFuncName, "=>  result: ok");
			else
				this->onServerLog(aClassName, aFuncName, "=>  result: error");
			this->onServerLog(aClassName, aFuncName, "=>  file path: " + ((TFileAckMessage_ptr&)aBMsg)->getFilePath());
			break;
		}
		case UPDATE_STOP_REPLY_ID:{
			this->onServerLog(aClassName, aFuncName, "=>  UpdateStopReplyMessage");
			this->onServerLog(aClassName, aFuncName, "=>  ");
			bool resp = ((TUpdateStopReplyMessage_ptr&)aBMsg)->getResp();
			if (resp)
				this->onServerLog(aClassName, aFuncName, "=>  result: ok");
			else
				this->onServerLog(aClassName, aFuncName, "=>  result: error");
			this->onServerLog(aClassName, aFuncName, "=>  version: " + to_string(((TUpdateStopReplyMessage_ptr&)aBMsg)->getVersion()));
			this->onServerLog(aClassName, aFuncName, "=>  version date: " + formatFileDate(((TUpdateStopReplyMessage_ptr&)aBMsg)->getTime()));
			break;
		}
		case GET_VERSIONS_REPLY_ID:{
			this->onServerLog(aClassName, aFuncName, "=>  GetVersionsReplyMessage");
			this->onServerLog(aClassName, aFuncName, "=>  tot versions: " + to_string(((TGetVersionsReplyMessage_ptr&)aBMsg)->getTotVersions()));
			this->onServerLog(aClassName, aFuncName, "=>  oldest version: " + to_string(((TGetVersionsReplyMessage_ptr&)aBMsg)->getOldestVersion()));
			this->onServerLog(aClassName, aFuncName, "=>  last version: " + to_string(((TGetVersionsReplyMessage_ptr&)aBMsg)->getLastVersion()));
			break;
		}
		case GET_LAST_VERSION_REPLY_ID:{
			this->onServerLog(aClassName, aFuncName, "=>  GetLastVersionReplyMessage");
			this->onServerLog(aClassName, aFuncName, "=>  version: " + to_string(((TGetLastVerReplyMessage_ptr&)aBMsg)->getVersion()));
			this->onServerLog(aClassName, aFuncName, "=>  version date: " + formatFileDate(((TGetLastVerReplyMessage_ptr&)aBMsg)->getVersionDate()));
			break;
		}
		case RESTORE_VER_REPLY_ID:{
			this->onServerLog(aClassName, aFuncName, "=>  RestoreVerReplyMessage");
			this->onServerLog(aClassName, aFuncName, "=>  ");
			bool resp = ((TRestoreVerReplyMessage_ptr&)aBMsg)->getResp();
			if (resp)
				this->onServerLog(aClassName, aFuncName, "=>  result: ok");
			else
				this->onServerLog(aClassName, aFuncName, "=>  result: error");
			this->onServerLog(aClassName, aFuncName, "=>  token: " + ((TRestoreVerReplyMessage_ptr&)aBMsg)->getToken());
			break;
		}
		case RESTORE_FILE_ID:{
			this->onServerLog(aClassName, aFuncName, "=>  RestoreFileMessage");
			this->onServerLog(aClassName, aFuncName, "=>  ");
			this->onServerLog(aClassName, aFuncName, "=>  file path: " + ((TRestoreFileMessage_ptr&)aBMsg)->getFilePath());
			this->onServerLog(aClassName, aFuncName, "=>  file date: " + formatFileDate(((TRestoreFileMessage_ptr&)aBMsg)->getFileDate()));
			break;
		}
		case RESTORE_STOP_ID:{
			this->onServerLog(aClassName, aFuncName, "=>  RestoreSotpMessage");
			this->onServerLog(aClassName, aFuncName, "=>  ");
			this->onServerLog(aClassName, aFuncName, "=>  version: " + to_string(((TRestoreStopMessage_ptr&)aBMsg)->getVersion()));
			this->onServerLog(aClassName, aFuncName, "=>  version date: " + formatFileDate(((TRestoreStopMessage_ptr&)aBMsg)->getVersionDate()));
			break;
		}
		case PING_REPLY_ID:{
			this->onServerLog(aClassName, aFuncName, "=>  PingReplyMessage");
			this->onServerLog(aClassName, aFuncName, "=>  ");
			this->onServerLog(aClassName, aFuncName, "=>  time: " + formatFileDate(((TPingReplyMessage_ptr&)aBMsg)->getTime()));
			this->onServerLog(aClassName, aFuncName, "=>  token: " + ((TPingReplyMessage_ptr&)aBMsg)->getToken());
			break;
		}
		case VERIFY_CRED_REPLY_ID:{
			this->onServerLog(aClassName, aFuncName, "=>  VerifyCredentialsReplyMessage");
			this->onServerLog(aClassName, aFuncName, "=>  ");
			bool resp = ((TVerifyCredReplyMessage_ptr&)aBMsg)->getResp();
			if (resp){
				this->onServerLog(aClassName, aFuncName, "=>  result: ok");
				this->onServerLog(aClassName, aFuncName, "=>  root path: " + ((TVerifyCredReplyMessage_ptr&)aBMsg)->getPath());
			}
			else
				this->onServerLog(aClassName, aFuncName, "=>  result: unauthorized");
			break;
		}
		case SYSTEM_ERR_ID:{
			this->onServerLog(aClassName, aFuncName, "=>  SystemErrorMessage");
			this->onServerLog(aClassName, aFuncName, "=>  ");
			this->onServerLog(aClassName, aFuncName, "=>  detail: " + ((TSystemErrorMessage_ptr&)aBMsg)->getDetail());
			break;
		}
		default:
			break;
		}

		this->onServerLog(aClassName, aFuncName, "=>  ");
		this->onServerLog(aClassName, aFuncName, "=> => => => => => => => => => => => => ");
		this->onServerLog(aClassName, aFuncName, "=> => => => => => => => => => => => => ");
		return true;
	}
	else{
		this->onServerWarning(aClassName, aFuncName, getMessageName(msgType) + " is not a valid message to be sent!");
		return false;
	}
}

const bool TServerSockController::startSocket(){
	this->onServerLog("TServerSockController", "startSocket", "setting the internal socket into accepting state...");
	if (!this->fSock->setAcceptState(this->fServerPort)){
		//server socket not ready; exit
		return false;
	}

	this->onServerLog("TServerSockController", "startSocket", "internal server socket is ready to accept");
	this->fSock->doAccept();
	return true;
}

void TServerSockController::stopSocket(){
	this->onServerLog("TServerSockController", "stopSocket", "closing incoming connections...");
	this->fInQueue->clear();
	this->fSock->stopIncoming();
	this->onServerLog("TServerSockController", "stopSocket", "incoming connections closed");
}


#pragma region "IServerBaseController implementation"
void TServerSockController::onServerReady(const bool aReadyState){
	if (this->fCallbackObj != nullptr)
		this->fCallbackObj->onServerReady(aReadyState);
}

void TServerSockController::onServerLog(const string& aClassName, const string& aFuncName, const string& aMsg){
	if (this->fCallbackObj != nullptr)
		this->fCallbackObj->onServerLog(aClassName, aFuncName, aMsg);
}

void TServerSockController::onServerWarning(const string& aClassName, const string& aFuncName, const string& aMsg){
	if (this->fCallbackObj != nullptr)
		this->fCallbackObj->onServerWarning(aClassName, aFuncName, aMsg);
}

void TServerSockController::onServerError(const string& aClassName, const string& aFuncName, const string& aMsg){
	if (this->fCallbackObj != nullptr)
		this->fCallbackObj->onServerError(aClassName, aFuncName, aMsg);
}

void TServerSockController::onServerCriticalError(const string& aClassName, const string& aFuncName, const string& aMsg){
	if (this->fCallbackObj != nullptr)
		this->fCallbackObj->onServerCriticalError(aClassName, aFuncName, aMsg);
}
#pragma endregion

#pragma region "IServerSockController implementation"
void TServerSockController::onServerSockAccept(TConnection_ptr& aConnection){

}

void TServerSockController::onServerSockRead(TConnection_ptr& aConnection, string_ptr& aMsg){
	tcp::endpoint peer = aConnection->getPeer();
	//string s = aMsg->c_str();
	TBaseMessage_ptr bmsg = new_TBaseMessage_ptr(aMsg);
	int msgType = bmsg->getID();
	

	if (!isValidMessageID(msgType)){
		this->onServerWarning("TServerSockController", "onServerSockRead", "received an unknown message from " + peer.address().to_string() + ":" + std::to_string(peer.port()));//+ "content:" + s->c_str()
		bmsg.reset();
		return;
	}

	this->onServerLog("TServerSockController", "onServerSockRead", "received a " + getMessageName(msgType) + " message from " + peer.address().to_string() + ":" + std::to_string(peer.port())/* + " -> " + s*/);

	//check if the message is valid to be received server-side
	bool valid = ((msgType == USER_REG_REQ_ID) || (msgType == UPDATE_START_REQ_ID) || (msgType == ADD_NEW_FILE_ID) || (msgType == VERIFY_CRED_REQ_ID)
		|| (msgType == UPDATE_FILE_ID) || (msgType == REMOVE_FILE_ID) || (msgType == UPDATE_STOP_REQ_ID) || (msgType == GET_VERSIONS_REQ_ID)
		|| (msgType == GET_LAST_VERSION_REQ_ID) || (msgType == RESTORE_VER_REQ_ID) || (msgType == RESTORE_FILE_ACK_ID) || (msgType == PING_REQ_ID));

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

}
#pragma endregion

#pragma region "IBaseExecutorController implementation"
bool TServerSockController::isMessageQueueEmpty(){
	return this->fInQueue->isEmpty();
}

TMessageContainer_ptr TServerSockController::getMessageToProcess(){
	return move_TMessageContainer_ptr(this->fInQueue->popMessage());
}

void TServerSockController::sendMessage(TMessageContainer_ptr& aMsg, const bool aCloseAfterSend){
	if (this->fSock != nullptr){
		if (aMsg != nullptr && !aMsg->isEmpty()){
			TConnection_ptr conn = aMsg->getConnection();
			if (conn->getSocket().is_open()){
				TBaseMessage_ptr bmsg = aMsg->getMessage();
				tcp::endpoint peer = conn->getPeer();
				this->onServerLog("TServerSockController", "sendMessage", "preparing to send a " + bmsg->getName() + " message to " + peer.address().to_string() + ":" + std::to_string(peer.port()));
				if (this->checkMessageToSend("TServerSockController", "sendMessage", bmsg))
					this->fSock->doSend(conn, move_TBaseMessage_ptr(bmsg), aCloseAfterSend);
				else
					bmsg.reset();
			}
			
			aMsg.reset();
		}
	}
}
#pragma endregion
#pragma endregion


//////////////////////////////////////
//         TServerSocket	        //
//////////////////////////////////////
#pragma region "TServerSocket"
TServerSocket::TServerSocket(io_service* aMainIoService, IServerSockController* aCallbackObj) : fServerAcceptor(*aMainIoService){
	this->fMainIoService = aMainIoService;
	this->fCallbackObj = aCallbackObj;
	this->fMustExit.store(false, boost::memory_order_release);
}

TServerSocket::~TServerSocket(){
	if (this->fConnections != nullptr){
		doServerLog(this->fCallbackObj, "TServerSocket", "destructor", "clearing open connections list...");
		unique_lock<mutex> lock(this->fConnectionsMutex);
		this->fConnections->clear();
		delete this->fConnections;
		this->fConnections = nullptr;
		doServerLog(this->fCallbackObj, "TServerSocket", "destructor", "open connections cleared");
	}

	if (this->fOutQueue != nullptr){
		doServerLog(this->fCallbackObj, "TServerSocket", "destructor", "deleting outgoing message queue object...");
		unique_lock<mutex> lock(this->fOutQueueMutex);
		//for (list<string_ptr>::iterator it = this->fOutQueue->begin(); it != this->fOutQueue->end(); it++)
		//	it->reset();
		this->fOutQueue->clear();
		delete this->fOutQueue;
		this->fOutQueue = nullptr;
		doServerLog(this->fCallbackObj, "TServerSocket", "destructor", "outgoing message queue object deleted");
	}

	//doServerLog(this->fCallbackObj, "TServerSocket", "destructor", "stopping IO service...");
	//this->fMainIoService.stop();
	//this->fMainIO->join();
	//delete this->fMainIO;
	//this->fMainIO = nullptr;
	//doServerLog(this->fCallbackObj, "TServerSocket", "destructor", "IO service stopped");

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
	if (this->fConnections == nullptr)
		this->fConnections = new TConnections();

	TConnection_ptr connection = new_TConnection_ptr(this->fMainIoService);
	try{
		unique_lock<mutex> lock(this->fConnectionsMutex);
		this->fConnections->push_back(connection);
	}
	catch (...){
		//just to have a syncronized block
	}

	//TConnectionHandle connection = this->fConnections.emplace(fConnections.begin(), this->fMainIoService);
	auto handler = bind(&TServerSocket::handleAccept, this, connection, boost::asio::placeholders::error);
	this->fServerAcceptor.async_accept(connection->getSocket(), connection->getPeer(), handler);
}

void TServerSocket::handleAccept(TConnection_ptr& aConnection, const boost::system::error_code& aErr){
	if (this->fMustExit.load(boost::memory_order_acquire))
		return;

	tcp::endpoint peer = aConnection->getPeer();
	if (!aErr){
		doServerLog(this->fCallbackObj, "TServerSocket", "handleAccept", "connection enstablished with " + peer.address().to_string() + ":" + std::to_string(peer.port()));
		if (this->fCallbackObj != nullptr)
			this->fCallbackObj->onServerSockAccept(aConnection);

		this->doAsyncRead(aConnection);
	}
	else{
		doServerError(this->fCallbackObj, "TServerSocket", "handleAccept", "Error \"" + aErr.message() + "\" trying to enstablish connection with " + peer.address().to_string() + ":" + std::to_string(peer.port()));
		//remove this connection
		this->checkAndRemove(aConnection);
	}

	this->doAccept();
}

void TServerSocket::doAsyncRead(TConnection_ptr& aConnection) {
	if (this->fMustExit.load(boost::memory_order_acquire))
		return;

	auto handler = bind(&TServerSocket::handleRead, this, aConnection, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred);
	async_read_until(aConnection->getSocket(), aConnection->getBuff(), END_MSG + string("\n"), handler);
}

void TServerSocket::handleRead(TConnection_ptr& aConnection, const boost::system::error_code& aErr, std::size_t aBytes){
	if (this->fMustExit.load(boost::memory_order_acquire))
		return;
	
	tcp::endpoint peer = aConnection->getPeer();
	if (!aErr) {
		if (aBytes > 0) {
			istream is(&aConnection->getBuff());
			string_ptr line = new_string_ptr();
			getline(is, *line);
			doServerLog(this->fCallbackObj, "TServerSocket", "handleRead", "Message Received from " + peer.address().to_string() + ":" + std::to_string(peer.port()));
			if (this->fCallbackObj != nullptr)
				this->fCallbackObj->onServerSockRead(aConnection, move_string_ptr(line));
			else
				line.reset();
		}

		this->doAsyncRead(aConnection);
	}
	else {
		if (aErr.value() == DISCONNECTED)
			doServerLog(this->fCallbackObj, "TServerSocket", "handleRead", "Received disconnection from " + peer.address().to_string() + ":" + std::to_string(peer.port()))
		else
			doServerError(this->fCallbackObj, "TServerSocket", "handleRead", "Error \"" + aErr.message() + "\" from " + peer.address().to_string() + ":" + std::to_string(peer.port()))
			
		if (aConnection->getSocket().is_open()) {
			doServerLog(this->fCallbackObj, "TServerSocket", "handleRead", "Closing socket with " + peer.address().to_string() + ":" + std::to_string(peer.port()));
			aConnection->getSocket().shutdown(socket_base::shutdown_both);
			aConnection->getSocket().close();
		}
		//remove this connection
		this->checkAndRemove(aConnection);
	}
}

void TServerSocket::doSend(TConnection_ptr& aConnection, TBaseMessage_ptr& aMsg, const bool aCloseAfterSend){
	if (this->fMustExit.load(boost::memory_order_acquire)){
		aMsg.reset();
		return;
	}

	if (!aConnection->getSocket().is_open()){
		doServerWarning(this->fCallbackObj, "TServerSocket", "doSend", "Cannot send message because the socket is already closed.");
		aMsg.reset();
		return;
	}

	if (this->fOutQueue == nullptr)
		this->fOutQueue = new list<string_ptr>();

	string_ptr s = nullptr;
	try{
		s = aMsg->encodeMessage();
		s->append("\n");
		aMsg.reset();
	}
	catch (EMessageException& e){
		doServerError(this->fCallbackObj, "TServerSocket", "doSend", e.getMessage());
		aMsg.reset();
		return;
	}

	list<string_ptr>::iterator buff;
	try{
		unique_lock<mutex> lock(this->fOutQueueMutex);
		this->fOutQueue->push_front(move_string_ptr(s));
		buff = this->fOutQueue->begin();
	}
	catch (...){
		//just to have a syncronized block
	}

	auto handler = boost::bind(&TServerSocket::handleWrite, this, aConnection, buff, aCloseAfterSend, boost::asio::placeholders::error);
	boost::asio::async_write(aConnection->getSocket(), boost::asio::buffer(**buff), handler);
}

void TServerSocket::handleWrite(TConnection_ptr& aConnection, list<string_ptr>::iterator aBuff, const bool aCloseAfterSend, const boost::system::error_code& aErr) {
	try{
		unique_lock<mutex> lock(this->fOutQueueMutex);
		if (aBuff != this->fOutQueue->end()){
			aBuff->reset();
			aBuff = this->fOutQueue->erase(aBuff);
		}
	}
	catch (...){
		//just to have a syncronized block
	}

	if (this->fMustExit.load(boost::memory_order_acquire))
		return;

	tcp::endpoint peer = aConnection->getPeer();
	if (!aErr) {
		doServerLog(this->fCallbackObj, "TServerSocket", "handleWrite", "Finished sending message to " + peer.address().to_string() + ":" + std::to_string(peer.port()));
		//if (aConnection->fSocket.is_open()) {
		//	// Write completed successfully and connection is open
		//	doServerLog(this->fCallbackObj, "TServerSocket", "handleWrite", "Closing socket with " + peer.address().to_string() + ":" + std::to_string(peer.port()));
		//	aConnection->fSocket.close();
		//}
	}
	else {
		doServerError(this->fCallbackObj, "TServerSocket", "handleWrite", "Error \"" + aErr.message() + "\" while sending a message to " + peer.address().to_string() + ":" + std::to_string(peer.port()));
	}

	if (aErr || aCloseAfterSend){
		if (aConnection->getSocket().is_open()) {
			doServerLog(this->fCallbackObj, "TServerSocket", "handleWrite", "Closing socket with " + peer.address().to_string() + ":" + std::to_string(peer.port()));
			aConnection->getSocket().shutdown(socket_base::shutdown_both);
			aConnection->getSocket().close();
		}
		//remove this connection
		this->checkAndRemove(aConnection);
	}

	if (this->fCallbackObj != nullptr)
		this->fCallbackObj->onServerSockWrite();
}

void TServerSocket::stopIncoming(){
	doServerLog(this->fCallbackObj, "TServerSocket", "stopIncoming", "closing acceptor...");
	this->fMustExit.store(true, boost::memory_order_release);
	this->fServerAcceptor.close();
	doServerLog(this->fCallbackObj, "TServerSocket", "stopIncoming", "acceptor closed");
}

void TServerSocket::checkAndRemove(TConnection_ptr& aConnection){
	unique_lock<mutex> lock(this->fConnectionsMutex);

	for (TConnections::iterator it = this->fConnections->begin(); it != this->fConnections->end(); it++){
		if (*it != nullptr && aConnection->isEqualTo(**it)){
			it->reset(); 
			this->fConnections->erase(it);
			break;
		}
	}
	aConnection.reset();
}
#pragma endregion