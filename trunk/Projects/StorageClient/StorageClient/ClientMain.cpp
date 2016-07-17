#include "pch.h"
#include "ClientMain.h"

ClientMain* ClientMain::instance = NULL;

ClientMain::ClientMain(){

}

ClientMain* ClientMain::getInstance(){
		if (instance == NULL)
			instance = new ClientMain();

		
		return instance;
}

bool ClientMain::PasswordCheck(string user, string pass){
	//fare la conversione a tipi non gestiti e poi la query al server
	return true;
}