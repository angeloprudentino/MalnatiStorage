#include "pch.h"
#include "ClientMain.h"
#include "SQlite_db.h"


ClientMain* ClientMain::instance = NULL;

ClientMain::ClientMain(): fMainIoService(), fSock(fMainIoService){
	db.OpenSQlite_db();
	const char *sqlCreateTable = "CREATE TABLE IF NOT EXISTS Files (id STRING PRIMARY KEY,path STRING,time STRING,version DOUBLE);";
	int result = db.CreateTable(sqlCreateTable); //decommentare
	if (result != 0){
		cout << "tabella già esistente" << endl;
	}

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


bool ClientMain::PasswordCheck(string user, string pass){
	
	return true;
}