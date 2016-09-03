/*
 * Author: Angelo Prudentino
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

// Struct representing a single connection
typedef struct Connection{
	tcp::socket fSocket;
	tcp::endpoint fPeer;
	boost::asio::streambuf fReadBuffer;
	Connection(io_service *aIoService) : fSocket(*aIoService), fPeer(), fReadBuffer() { }
	Connection(io_service *aIoService, size_t aMaxBuffSize) : fSocket(*aIoService), fPeer(), fReadBuffer(aMaxBuffSize) { }
} TConnection;
typedef list<TConnection> TConnectionList;
typedef TConnectionList::iterator TConnectionHandle;


//////////////////////////////////////
//      IServerSockController	    //
//////////////////////////////////////
// Standard server socket controller interface
public class IServerSockController : public IServerBaseController{
public:
	virtual void onServerSockAccept(TConnectionHandle aConnection) = 0;
	virtual void onServerSockRead(TConnectionHandle aConnection, string_ptr& aMsg) = 0;
	virtual void onServerSockWrite() = 0;
};