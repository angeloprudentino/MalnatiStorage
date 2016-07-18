#include "pch.h"
#include "ClientMain.h"

ClientMain* ClientMain::instance = NULL;

ClientMain::ClientMain(): fMainIoService(), fSock(fMainIoService){

}

ClientMain* ClientMain::getInstance(){
		if (instance == NULL)
			instance = new ClientMain();

		
		return instance;
}

bool ClientMain::PasswordCheck(string user, string pass){
	
	return true;
}