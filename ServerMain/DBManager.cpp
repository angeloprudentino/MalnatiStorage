/*
 * Author: Angelo Prudentino
 * Date: 10/07/2016
 * File: DBManager.cpp
 * Description: This class manages all the communications with the DB
 *
 */
#pragma once
#include "DBManager.h"
#include "Utility.h"

using namespace System::Configuration;
using namespace System::Data;

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
TDBManager::TDBManager(const string& aHost, const string& aDBName, IServerDBController* aCallback){
	//this->fCallbackObj = aCallback;

	//this->fConnection = gcnew SqlConnection();
	//string cs = "Server=" + aHost + "; Database=" + aDBName + "; Trusted_Connection=True;";
	//this->fConnection->ConnectionString = gcnew String(cs.c_str());
	//
	//doServerLog(this->fCallbackObj, "TDBManager", "constructor", "connecting to: " + aDBName + "@" + aHost);
	//try{
	//	this->fConnection->Open();
	//	doServerLog(this->fCallbackObj, "TDBManager", "constructor", "successfully connected to DB");
	//}
	//catch (InvalidOperationException^ e){
	//	doServerCriticalError(this->fCallbackObj, "TDBManager", "constructor", "Impossible to connect to DBdue to an InvalidOperationException: " + marshalString(e->Message));
	//}
	//catch (SqlException^ e){
	//	doServerCriticalError(this->fCallbackObj, "TDBManager", "constructor", "Impossible to connect to DB due to a SqlException: " + marshalString(e->Message));
	//}
	////catch (ConfigurationErrorsException^ e){
	////	doServerCriticalError(this->fCallbackObj, "TDBManager", "constructor", "Impossible to connect to DB due to a ConfigurationErrorsException: " + marshalString(e->Message));
	////}

	//if (this->fConnection->State == ConnectionState::Open){
	//	this->fHost = make_string_ptr(aHost);
	//	this->fDBName = make_string_ptr(aDBName);
	//}
}

TDBManager::~TDBManager(){
	//if (this->fConnection->State == ConnectionState::Open){
	//	this->fConnection->Close();
	//	delete this->fConnection;
	//	this->fConnection = nullptr;
	//}

	//if (this->fHost != nullptr){
	//	this->fHost.reset();
	//	this->fHost = nullptr;
	//}

	//if (this->fDBName != nullptr){
	//	this->fDBName.reset();
	//	this->fDBName = nullptr;
	//}

	//this->fCallbackObj = nullptr;
}

const bool TDBManager::insertNewUser(const string& aUser, const string& aPass){
	//SqlCommand^ cmd = gcnew SqlCommand("INSERT INTO Users (User, Password, Salt) VALUES (@id, @pass, @salt);");
	//cmd->CommandType = CommandType::Text;
	//cmd->Connection = this->fConnection;
	//cmd->Parameters->Add("@id", SqlDbType::NChar, 20)->Value = gcnew String(aUser.c_str());
	//cmd->Parameters->Add("@pass", SqlDbType::NChar, 20)->Value = gcnew String(aPass.c_str());
	//string_ptr salt = opensslB64RandomToken();
	//cmd->Parameters->Add("@salt", SqlDbType::NChar, 20)->Value = gcnew String(salt->c_str());
	//salt.reset();

	//try{
	//	int n = cmd->ExecuteNonQuery();
	//	if (n != 1){
	//		doServerError(this->fCallbackObj, "TDBManager", "insertNewUser", "insert failed: returned " + to_string(n));
	//		return false;
	//	}
	//}
	//catch (Exception^ e) {
	//	doServerError(this->fCallbackObj, "TDBManager", "insertNewUser", "Error during insert: " + marshalString(e->Message));
	//	return false;
	//}

	return true;
}

const bool TDBManager::InsertNewVersion(const string& aUser, TVersion_ptr& aVersion){
	//SqlCommand^ cmd = gcnew SqlCommand("INSERT INTO Users (User, Password, Salt) VALUES (@id, @pass, @salt);");
	//cmd->CommandType = CommandType::Text;
	//cmd->Connection = this->fConnection;
	//cmd->Parameters->Add("@id", SqlDbType::NChar, 20)->Value = gcnew String(aUser.c_str());
	//cmd->Parameters->Add("@pass", SqlDbType::NChar, 20)->Value = gcnew String(aPass.c_str());
	//string_ptr salt = opensslB64RandomToken();
	//cmd->Parameters->Add("@salt", SqlDbType::NChar, 20)->Value = gcnew String(salt->c_str());
	//salt.reset();

	//try{
	//	int n = cmd->ExecuteNonQuery();
	//	if (n != 1){
	//		doServerError(this->fCallbackObj, "TDBManager", "insertNewUser", "insert failed: returned " + to_string(n));
	//		return false;
	//	}
	//}
	//catch (Exception^ e) {
	//	doServerError(this->fCallbackObj, "TDBManager", "insertNewUser", "Error during insert: " + marshalString(e->Message));
	//	return false;
	//}

	return true;
}

const bool TDBManager::checkIfUserExists(const string& aUser){

	return true;
}

const bool TDBManager::verifyUserCredentials(const string& aUser, const string& aPass){
	
	return true;
}

TVersion_ptr TDBManager::getVersion(const string& aUser, int aVersion){

	return nullptr;
}

TVersion_ptr TDBManager::getLastVersion(const string& aUser){

	return nullptr;
}

TVersionList_ptr TDBManager::getAllVersions(const string& aUser){

	return nullptr;
}

#pragma endregion