/*
 * Authors: Angelo Prudentino & Daniele Testa
 * Date: 30/09/2015
 * File: NetworkController.h
 * Description: this file contains interfaces for socket controllers
 *
 */

#pragma once

#include <boost/asio.hpp>
#include "Utility.h"
#include "ServerController.h"

using namespace std;
using namespace boost;
using namespace boost::asio;
using namespace boost::asio::ip;

/////////////////////////////////
//         TConnection	       //
/////////////////////////////////
class TConnection{
private:
	tcp::socket fSocket;
	tcp::endpoint fPeer;
	boost::asio::streambuf fReadBuffer;
public:
	TConnection(io_service *aIoService) : fSocket(*aIoService), fPeer(), fReadBuffer() { }

	const bool isEqualTo(const TConnection& aConnection){ return ((this->fPeer.address() == aConnection.fPeer.address()) && (this->fPeer.port() == aConnection.fPeer.port())); }
	
	//live getters
	tcp::socket& getSocket() { return this->fSocket; }
	tcp::endpoint& getPeer() { return this->fPeer; }
	boost::asio::streambuf& getBuff() { return this->fReadBuffer; }
};
typedef std::shared_ptr<TConnection> TConnection_ptr;
typedef list<TConnection_ptr> TConnections;
#define new_TConnection_ptr(aIoService) std::make_shared<TConnection>(aIoService)


//////////////////////////////////////
//      IServerSockController	    //
//////////////////////////////////////
class IServerSockController : public IServerBaseController{
public:
	virtual void onServerSockAccept(TConnection_ptr& aConnection) = 0;
	virtual void onServerSockRead(TConnection_ptr& aConnection, string_ptr& aMsg) = 0;
	virtual void onServerSockWrite() = 0;
};