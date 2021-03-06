#pragma once

#include <string>
//#include "sqlite3.h"
#include <boost\asio.hpp>
#include <boost\thread\thread.hpp>
#include <boost\bind.hpp>
#include "Utility.h"
#include "Message.h"
#include <sqlite3.h>
#include "SQlite_db.h"
#include <string.h>
using namespace std;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Storage;
using namespace Windows::Storage::Pickers;
using namespace Windows::Storage::AccessCache;
using namespace Windows::Storage::Search;
//using namespace boost;
//using namespace boost::asio;
//using namespace boost::asio::ip;


class ClientMain {
private:
	static ClientMain* instance;
	boost::asio::io_service fMainIoService;
	boost::asio::ip::tcp::socket fSock;
	SQlite_db db;
	//void handle_write(std::string msg_buffer, boost::system::error_code const & err);
protected:
	ClientMain();
	~ClientMain();

public:
	static ClientMain* getInstance();
	
	bool PasswordCheck(string user, string pass);
	void sendMsg();
	char* InitializeDB(StorageFolder^ sf);
	char* CheckFile(Platform::String^ name, Platform::String^ path, double time, double version);
	char* ShowTableInFile();
};