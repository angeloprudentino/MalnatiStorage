/*
 * Authors: Angelo Prudentino & Daniele Testa
 * Date: 27/09/2016
 * File: StorageServerCore.cpp
 * Description: This is the entry-point class of the server dll
 *
 */

#include "StorageServerCore.h"
#include "Utility.h"

StorageServerCore::StorageServerCore(){
	try{
		this->fCore = new TStorageServer(DEFAULT_PORT, nullptr);
	}
	catch (EBaseException& e){
		throw gcnew Exception(unmarshalString(e.getMessage()));
	}
	this->fServerThread = gcnew Thread(gcnew ParameterizedThreadStart(this, &StorageServerCore::start));
	this->fServerThread->Start(nullptr);
}
StorageServerCore::~StorageServerCore(){
	this->stop();

	if (this->fCore != nullptr){
		delete this->fCore;
		this->fCore = nullptr;
	}

	if (!System::Object::ReferenceEquals(this->fServerThread, nullptr))
		this->fServerThread->Join();
}

void StorageServerCore::start(Object^ aData){
	try{
		this->fCore->startServer();
	}
	catch (EBaseException& e){
		throw gcnew Exception(unmarshalString(e.getMessage()));
	}
}

void StorageServerCore::stop(){
	try{
		if (this->fCore != nullptr)
			this->fCore->stopServer();
	}
	catch (EBaseException& e){
		throw gcnew Exception(unmarshalString(e.getMessage()));
	}
}

