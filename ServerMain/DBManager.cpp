/*
 * Author: Angelo Prudentino
 * Date: 10/07/2016
 * File: DBManager.cpp
 * Description: This class manages all the communications with the DB
 *
 */
#pragma once
#include "DBManager.h"

using namespace System::Configuration;

string marshalString(String ^ aStr) {
	using namespace Runtime::InteropServices;
	const char* chars = (const char*)(Marshal::StringToHGlobalAnsi(aStr)).ToPointer();
	string res = chars;
	Marshal::FreeHGlobal(IntPtr((void*)chars));

	return res;
}


////////////////////////////////////
//          TDBManager	          //
////////////////////////////////////
#pragma region "TDBManager"
TDBManager::TDBManager(const string& aHost, const string& aDBName, IServerBaseController* aCallback){
	this->fCallbackObj = aCallback;

	this->fConnection = gcnew SqlConnection();
	string cs = "Server=" + aHost + "; Database=" + aDBName + "; Trusted_Connection=True;";
	this->fConnection->ConnectionString = gcnew String(cs.c_str());
	
	doServerLog(this->fCallbackObj, "TDBManager", "constructor", "connecting to: " + aDBName + "@" + aHost);
	try{
		this->fConnection->Open();
		this->fOpen = true;
		doServerLog(this->fCallbackObj, "TDBManager", "constructor", "successfully connected to DB");
	}
	catch (InvalidOperationException^ e){
		this->fOpen = false;
		doServerCriticalError(this->fCallbackObj, "TDBManager", "constructor", "Impossible to connect to DBdue to an InvalidOperationException: " + marshalString(e->Message));
	}
	catch (SqlException^ e){
		this->fOpen = false;
		doServerCriticalError(this->fCallbackObj, "TDBManager", "constructor", "Impossible to connect to DB due to a SqlException: " + marshalString(e->Message));
	}
	//catch (ConfigurationErrorsException^ e){
	//	this->fOpen = false;
	//	doServerCriticalError(this->fCallbackObj, "TDBManager", "constructor", "Impossible to connect to DB due to a ConfigurationErrorsException: " + marshalString(e->Message));
	//}

	if (this->fOpen){
		this->fHost = make_string_ptr(aHost);
		this->fDBName = make_string_ptr(aDBName);
	}
}

TDBManager::~TDBManager(){
	if (this->fConnection != nullptr){
		if (this->fOpen){
			this->fConnection->Close();
			this->fOpen = false;
		}
		delete this->fConnection;
		this->fConnection = nullptr;
	}

	if (this->fHost != nullptr){
		this->fHost.reset();
		this->fHost = nullptr;
	}

	if (this->fDBName != nullptr){
		this->fDBName.reset();
		this->fDBName = nullptr;
	}

	this->fCallbackObj = nullptr;
}

void TDBManager::insertNewUser(const string& aUser, const string& aPass, const string& aSalt){

}

void TDBManager::InsertNewVersion(TVersion_ptr& aVersion){

}

const bool TDBManager::checkIfUserExists(const string& aUser){

}

const bool TDBManager::verifyUserCredentials(const string& aUser, const string& aPass, const string& aSalt){

}

TVersion_ptr TDBManager::getLastVerison(const string& aUser){

}

#pragma endregion