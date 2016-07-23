#include "pch.h"
#include "ClientMain.h"
#include "SQlite_db.h"
#include "Utility.h"
#include "Message.h"
#include <string>
#include <iostream>
#include <boost\bind.hpp>

using namespace boost;
using namespace boost::asio;
using namespace boost::asio::ip;


ClientMain* ClientMain::instance = NULL;

ClientMain::ClientMain(): fMainIoService(), fSock(fMainIoService){
	//db.OpenSQlite_db();
	//const char *sqlCreateTable = "CREATE TABLE IF NOT EXISTS Files (id STRING PRIMARY KEY,path STRING,time STRING,version DOUBLE);";
	//int result = db.CreateTable(sqlCreateTable); //decommentare
	//if (result != 0){
	//	cout << "tabella già esistente" << endl;
	//}

}

ClientMain::~ClientMain(){
//	db.CloseSQlite_db();
}

ClientMain* ClientMain::getInstance(){
		if (instance == NULL)
			instance = new ClientMain();

		
		return instance;
}
//funzioni per il Db lato client


bool ClientMain::PasswordCheck(string user, string pass){
	
	return true;
}

void ClientMain::sendMsg(){
	//TAddNewFileMessage* m = new TAddNewFileMessage("tokendiprova", "prova.jpg");
	//string_ptr msg = m->encodeMessage();

	//tcp::endpoint ep(ip::address::from_string("127.0.0.1"), DEFAULT_PORT); // TCP socket for connecting to server
	//this->fSock.connect(ep);
	//boost::asio::write(this->fSock, boost::asio::buffer(*msg));
	//delete m;
	//msg.reset();

	////wait for reply
	//boost::asio::streambuf buf;
	//boost::asio::read_until(this->fSock, buf, END_MSG);
	//istream is(&buf);
	//string_ptr line = new_string_ptr();
	//getline(is, *line);
	//cout << *line;
	////TBaseMessage bm(line);
	////bm.decodeMessage();
	//line.reset();


	////TBaseMessage* m1 = new TBaseMessage(std::make_shared<std::string>("pippo$pluto$END_MSG"));
	////msg = m1->getMsg();
	////boost::asio::write(this->fSock, boost::asio::buffer(*msg));
	////delete m1;

	////TAddNewFileMessage* m2 = new TAddNewFileMessage("token", "prova.jpg");
	////msg = m2->encodeMessage();
	////boost::asio::write(this->fSock, boost::asio::buffer(*msg));
	////delete m2;

	////TUpdateFileMessage* m3 = new TUpdateFileMessage("token", "prova.jpg");
	////msg = m3->encodeMessage();
	////boost::asio::write(this->fSock, boost::asio::buffer(*msg));
	////delete m3;

	////TRemoveFileMessage* m4 = new TRemoveFileMessage("token", "prova.jpg");
	////msg = m4->encodeMessage();
	////boost::asio::write(this->fSock, boost::asio::buffer(*msg));
	////delete m4;

	//this->fSock.shutdown(socket_base::shutdown_both);
	//this->fSock.close();
}