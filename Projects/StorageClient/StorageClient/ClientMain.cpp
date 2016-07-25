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

}

char* ClientMain::ShowTableInFile(){
	char *sqlSelect = "SELECT * FROM Files;";
	char* w = db.DisplayTable(sqlSelect);
	return w;
}

ClientMain::~ClientMain(){
	db.CloseSQlite_db();
}

ClientMain* ClientMain::getInstance(){
		if (instance == NULL)
			instance = new ClientMain();
	
		return instance;
}


//funzioni per il Db lato client

char* ClientMain::InitializeDB(){
	db.OpenSQlite_db();
	const char *sqlCreateTable = "CREATE TABLE IF NOT EXISTS Files (id STRING PRIMARY KEY,path STRING PRIMARY KEY,time DOUBLE,version DOUBLE);";
	char* result = db.CreateTable(sqlCreateTable); //decommentare
	//if (result != 0){
	//	cout << "tabella già esistente" << endl;
	//}
	return result;
}

char* ClientMain::CheckFile(Platform::String^ name, Platform::String^ path, double time, double version){
	//cout << "sono dentro il check" << endl;
	//Platform::String^ fooRT = "aoeu";
	//conversione in char*
	std::wstring fooW(name->Begin());
	std::string fooA(fooW.begin(), fooW.end());
	const char* name_char = fooA.c_str();

	std::wstring fooW2(path->Begin());
	std::string fooA2(fooW2.begin(), fooW2.end());
	const char* path_char = fooA2.c_str();

	//provo a fare una insert
	string query_p("INSERT INTO Files VALUES('");
	query_p += name_char;
	query_p += "','";
	query_p += path_char;
	query_p += "','";
	query_p += time;
	query_p += "','";
	query_p += version;
	query_p += "');";
	//copia di string in un char* , da inserire nel DB
	char* query = new char[query_p.length() + 1];
	strcpy_s(query, query_p.length() + 1, query_p.c_str());
	//cout << "insert query" << query << endl;
	
	char* res = db.ExecuteSQl(query);
	// res = db.ExecuteSQl();
	
	return res;
}


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