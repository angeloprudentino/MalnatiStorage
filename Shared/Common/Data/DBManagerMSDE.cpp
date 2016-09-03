/*
 * Author: Angelo Prudentino
 * Date: 10/07/2016
 * File: DBManagerMSDE.cpp
 * Description: This class manages all the interactions with an MSDE DB
 *
 */
#pragma once
#include "DBManagerMSDE.h"
#include "Utility.h"
#include <ctime>

using namespace System::Configuration;


string marshalString(String ^ aStr) {
	using namespace Runtime::InteropServices;
	const char* chars = (const char*)(Marshal::StringToHGlobalAnsi(aStr)).ToPointer();
	string res = chars;
	Marshal::FreeHGlobal(IntPtr((void*)chars));

	return res;
}


////////////////////////////////////
//       TDBManagerMSDE	          //
////////////////////////////////////
#pragma region "TDBManagerMSDE"
const int TDBManagerMSDE::getUserID(const string& aUser, SqlTransaction^ aTransaction){
	int uID = -1;

	if (!System::Object::ReferenceEquals(this->fConnection, nullptr) && (this->fConnection->State == ConnectionState::Open)){
		if (System::Object::ReferenceEquals(this->fSelectUserIdCmd, nullptr)){
			this->fSelectUserIdCmd = gcnew SqlCommand("SELECT UserID FROM Users WHERE Username = @username;");
			this->fSelectUserIdCmd->CommandType = CommandType::Text;
			this->fSelectUserIdCmd->Connection = this->fConnection;
		}
		this->fSelectUserIdCmd->Transaction = aTransaction;
		this->fSelectUserIdCmd->Parameters->Add("@username", SqlDbType::NVarChar, 50)->Value = gcnew String(aUser.c_str());

		SqlDataReader^ reader;
		try{
			reader = this->fSelectUserIdCmd->ExecuteReader();

			if (reader->HasRows){
				reader->Read();
				uID = reader->GetInt32(0);
			}
			else
				uID = -1;

			this->fSelectUserIdCmd->Parameters->Clear();
			reader->Close();
		}
		catch (Exception^ e) {
			reader->Close();
			this->fSelectUserIdCmd->Parameters->Clear();
			uID = -1;
			throw EDBException("Error during SelectUserIdCmd: " + marshalString(e->Message));
		}
	}

	return uID;
}

TDBManagerMSDE::TDBManagerMSDE(const string& aHost, const string& aDBName){
	this->fConnection = gcnew SqlConnection();
	string cs = "Server=" + aHost + "; Database=" + aDBName + "; Trusted_Connection=True;";
	this->fConnection->ConnectionString = gcnew String(cs.c_str());
	
	try{
		this->fConnection->Open();
	}
	catch (InvalidOperationException^ e){
		throw EDBException("Impossible to connect to DBdue to an InvalidOperationException: " + marshalString(e->Message));
	}
	catch (SqlException^ e){
		throw EDBException("Impossible to connect to DB due to a SqlException: " + marshalString(e->Message));
	}
	//catch (ConfigurationErrorsException^ e){
	//	throw EDBException("Impossible to connect to DB due to a ConfigurationErrorsException: " + marshalString(e->Message));
	//}

	if (!System::Object::ReferenceEquals(this->fConnection, nullptr) && (this->fConnection->State == ConnectionState::Open)){
		this->fHost = make_string_ptr(aHost);
		this->fDBName = make_string_ptr(aDBName);
	}
}

TDBManagerMSDE::~TDBManagerMSDE(){
	delete this->fInsertUserCmd;
	delete this->fInsertVersionCmd;
	delete this->fInsertFilesCmd;
	delete this->fSelectUserIdCmd;
	delete this->fSelectUserSaltCmd;
	delete this->fVerifyCredentialCmd;
	delete this->fSelectVersionCmd;
	delete this->fSelectLastVersionCmd;
	delete this->fSelectLastVersionFilesCmd;
	delete this->fSelectAllVersionsCmd;

	if (!System::Object::ReferenceEquals(this->fConnection, nullptr) && (this->fConnection->State == ConnectionState::Open)){
		this->fConnection->Close();
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
}

void TDBManagerMSDE::insertNewUser(const string& aUser, const string& aPass){
	if (!System::Object::ReferenceEquals(this->fConnection, nullptr) && (this->fConnection->State == ConnectionState::Open)){

		if (System::Object::ReferenceEquals(this->fInsertUserCmd, nullptr)){
			this->fInsertUserCmd = gcnew SqlCommand("INSERT INTO Users (Username, Password, Salt) VALUES (@username, @password, @salt);");
			this->fInsertUserCmd->CommandType = CommandType::Text;
			this->fInsertUserCmd->Connection = this->fConnection;
		}
		this->fInsertUserCmd->Parameters->Add("@username", SqlDbType::NVarChar, 50)->Value = gcnew String(aUser.c_str());
		string_ptr salt = nullptr;
		string_ptr saltedPass = nullptr;
		try{
			salt = opensslB64RandomToken();
			saltedPass = make_string_ptr(salt->c_str());
			saltedPass->append(aPass.c_str());
			saltedPass->append(salt->c_str());
			saltedPass = opensslB64Checksum(saltedPass->c_str(), true);
		}
		catch (EOpensslException e){
			salt.reset();
			saltedPass.reset();
			throw EDBException("insertNewUser failed due to an EOpensslException: " + e.getMessage());
		}
		this->fInsertUserCmd->Parameters->Add("@password", SqlDbType::NVarChar)->Value = gcnew String(saltedPass->c_str());
		this->fInsertUserCmd->Parameters->Add("@salt", SqlDbType::NVarChar)->Value = gcnew String(salt->c_str());
		salt.reset();
		saltedPass.reset();

		try{
			int n = this->fInsertUserCmd->ExecuteNonQuery();
			this->fInsertUserCmd->Parameters->Clear();
			if (n != 1)
				throw EDBException("InsertUserCmd failed: returned " + to_string(n));
		}
		catch (Exception^ e) {
			this->fInsertUserCmd->Parameters->Clear();
			throw EDBException("Error during InsertUserCmd: " + marshalString(e->Message));
		}
	}
	else{
		throw EDBException("No connection to DB!");
	}
}

void TDBManagerMSDE::InsertNewVersion(const string& aUser, TVersion_ptr& aVersion){
	if (!System::Object::ReferenceEquals(this->fConnection, nullptr) && (this->fConnection->State == ConnectionState::Open)){
		int uID = -1;
		SqlTransaction^ transaction;

		//begin a new transaction
		transaction = this->fConnection->BeginTransaction();

		//Extract user ID
		try{
			uID = this->getUserID(aUser, transaction);
			if (uID == -1){
				transaction->Rollback();
				throw EDBException("UserID is not valid!");
			}
		}
		catch (EDBException e){
			transaction->Rollback();
			throw EDBException("InsertNewVersion failed due to an EDBException in getUserID(): " + e.getMessage());

		}

		//Insert new version for user
		if (System::Object::ReferenceEquals(this->fInsertVersionCmd, nullptr)){
			this->fInsertVersionCmd = gcnew SqlCommand("INSERT INTO Versions (VerID, UserID, VerDate) VALUES (@verID, @userID, @verDate);");
			this->fInsertVersionCmd->CommandType = CommandType::Text;
			this->fInsertVersionCmd->Connection = this->fConnection;
		}
		this->fInsertVersionCmd->Transaction = transaction;
		this->fInsertVersionCmd->Parameters->Add("@verID", SqlDbType::Int)->Value = aVersion->getVersion();
		this->fInsertVersionCmd->Parameters->Add("@userID", SqlDbType::Int)->Value = uID;
		this->fInsertVersionCmd->Parameters->Add("@verDate", SqlDbType::BigInt)->Value = aVersion->getDate();

		try{
			int n = this->fInsertVersionCmd->ExecuteNonQuery();
			this->fInsertVersionCmd->Parameters->Clear();
			if (n != 1){
				transaction->Rollback();
				throw EDBException("InsertVersionCmd failed: returned " + to_string(n));
			}
		}
		catch (Exception^ e) {
			this->fInsertVersionCmd->Parameters->Clear();
			transaction->Rollback();
			throw EDBException("Error during InsertVersionCmd: " + marshalString(e->Message));
		}

		//Insert files for this version
		TFile_ptr f = aVersion->getNextFile();
		while (f != nullptr){
			if (System::Object::ReferenceEquals(this->fInsertFilesCmd, nullptr)){
				this->fInsertFilesCmd = gcnew SqlCommand("INSERT INTO Files (VerID, UserID, ServerPath, ClientRelativePath, LastModDate) VALUES (@verID, @userID, @serverPath, @clientRelativePath, @lastModDate);");
				this->fInsertFilesCmd->CommandType = CommandType::Text;
				this->fInsertFilesCmd->Connection = this->fConnection;
			}
			this->fInsertFilesCmd->Transaction = transaction;
			this->fInsertFilesCmd->Parameters->Add("@verID", SqlDbType::Int)->Value = aVersion->getVersion();
			this->fInsertFilesCmd->Parameters->Add("@userID", SqlDbType::Int)->Value = uID;
			this->fInsertFilesCmd->Parameters->Add("@serverPath", SqlDbType::NVarChar)->Value = gcnew String(f->getServerPathPrefix().c_str());
			this->fInsertFilesCmd->Parameters->Add("@clientRelativePath", SqlDbType::NVarChar)->Value = gcnew String(f->getClientRelativePath().c_str());
			this->fInsertFilesCmd->Parameters->Add("@lastModDate", SqlDbType::BigInt)->Value = f->getLastMod();

			try{
				int n = this->fInsertFilesCmd->ExecuteNonQuery();
				this->fInsertFilesCmd->Parameters->Clear();
				if (n != 1){
					transaction->Rollback();
					throw EDBException("InsertFilesCmd failed: returned " + to_string(n));
				}
			}
			catch (Exception^ e) {
				this->fInsertFilesCmd->Parameters->Clear();
				transaction->Rollback();
				throw EDBException("Error during InsertFilesCmd: " + marshalString(e->Message));
			}

			aVersion->updateNext();
			f = aVersion->getNextFile();
		}
		transaction->Commit();
	}
	else{
		throw EDBException("No connection to DB!");
	}
}

const bool TDBManagerMSDE::checkIfUserExists(const string& aUser){
	SqlTransaction^ transaction = this->fConnection->BeginTransaction();
	int uID = -1;

	try{
		uID = this->getUserID(aUser, transaction);
		transaction->Commit();
	}
	catch (EDBException e){
		transaction->Rollback();
		throw EDBException("checkIfUserExists failed due to an EDBException in getUserID(): " + e.getMessage());
	}

	if (uID == -1)
		return false;
	else
		return true;
}

const bool TDBManagerMSDE::verifyUserCredentials(const string& aUser, const string& aPass){
	if (!System::Object::ReferenceEquals(this->fConnection, nullptr) && (this->fConnection->State == ConnectionState::Open)){
		string salt = "";
		SqlDataReader^ reader;
		SqlTransaction^ transaction = this->fConnection->BeginTransaction();

		//Select user salt
		if (System::Object::ReferenceEquals(this->fSelectUserSaltCmd, nullptr)){
			this->fSelectUserSaltCmd = gcnew SqlCommand("SELECT Salt FROM Users WHERE Username = @username;");
			this->fSelectUserSaltCmd->CommandType = CommandType::Text;
			this->fSelectUserSaltCmd->Connection = this->fConnection;
		}
		this->fSelectUserSaltCmd->Transaction = transaction;
		this->fSelectUserSaltCmd->Parameters->Add("@username", SqlDbType::NVarChar, 50)->Value = gcnew String(aUser.c_str());
		try{
			reader = fSelectUserSaltCmd->ExecuteReader();
			if (reader->HasRows){
				reader->Read();
				salt = marshalString(reader->GetString(0));
			}
			else{
				reader->Close();
				transaction->Rollback();
				throw EDBException("User " + aUser + " not found!");
			}

			this->fSelectUserSaltCmd->Parameters->Clear();
			reader->Close();
		}
		catch (Exception^ e) {
			this->fSelectUserSaltCmd->Parameters->Clear();
			throw EDBException("Error during SelectUserSaltCmd: " + marshalString(e->Message));
		}

		//verify user credentials
		if (System::Object::ReferenceEquals(this->fVerifyCredentialCmd, nullptr)){
			this->fVerifyCredentialCmd = gcnew SqlCommand("SELECT COUNT(*) FROM Users WHERE Username = @username AND Password = @password;");
			this->fVerifyCredentialCmd->CommandType = CommandType::Text;
			this->fVerifyCredentialCmd->Connection = this->fConnection;
		}
		this->fVerifyCredentialCmd->Transaction = transaction;
		string_ptr saltedPass = make_string_ptr(salt.c_str());
		saltedPass->append(aPass.c_str());
		saltedPass->append(salt);
		try{
			saltedPass = opensslB64Checksum(saltedPass->c_str(), true);
		}
		catch (EOpensslException e){
			saltedPass.reset();
			transaction->Rollback();
			throw EDBException("verifyUserCredentials failed due to an EOpensslException: " + e.getMessage());
		}
		this->fVerifyCredentialCmd->Parameters->Add("@username", SqlDbType::NVarChar, 50)->Value = gcnew String(aUser.c_str());
		this->fVerifyCredentialCmd->Parameters->Add("@password", SqlDbType::NVarChar)->Value = gcnew String(saltedPass->c_str());
		saltedPass.reset();

		int count = 0;
		try{
			reader = fVerifyCredentialCmd->ExecuteReader();
			this->fVerifyCredentialCmd->Parameters->Clear();

			if (reader->HasRows){
				reader->Read();
				count = reader->GetInt32(0);
			}
			else{
				reader->Close(); 
				transaction->Rollback();
				throw EDBException("Passed credentials are not valid!");
			}

			reader->Close();
		}
		catch (Exception^ e) {
			this->fVerifyCredentialCmd->Parameters->Clear();
			reader->Close();
			throw EDBException("Error during VerifyCredentialCmd: " + marshalString(e->Message));
		}

		transaction->Commit();
		if (count == 1)
			return true;
		else
			return false;
	}
	else{
		throw EDBException("No connection to DB!");
	}
}

TVersion_ptr TDBManagerMSDE::getVersion(const string& aUser, int aVersion){
	if (!System::Object::ReferenceEquals(this->fConnection, nullptr) && (this->fConnection->State == ConnectionState::Open)){
		TVersion_ptr version = nullptr;

		//Select user version
		if (System::Object::ReferenceEquals(this->fSelectVersionCmd, nullptr)){
			this->fSelectVersionCmd = gcnew SqlCommand("SELECT VerDate, ServerPath, ClientRelativePath, LastModDate FROM Users, Versions, Files WHERE Users.UserID = Versions.UserID AND Users.UserID = Files.UserID AND Versions.VerID = Files.VerID AND Versions.VerID = @verID AND Username = @username;");
			this->fSelectVersionCmd->CommandType = CommandType::Text;
			this->fSelectVersionCmd->Connection = this->fConnection;
		}
		this->fSelectVersionCmd->Parameters->Add("@verID", SqlDbType::Int)->Value = aVersion;
		this->fSelectVersionCmd->Parameters->Add("@username", SqlDbType::NVarChar, 50)->Value = gcnew String(aUser.c_str());
		SqlDataReader^ reader;
		try{
			reader = fSelectVersionCmd->ExecuteReader();

			if (reader->HasRows){
				while (reader->Read()){
					if (version == nullptr){
						//build version object
						version = new_TVersion_ptr(aVersion, reader->GetInt64(0));
					}
					string serverPath = marshalString(reader->GetString(1));
					string clientPath = marshalString(reader->GetString(2));
					long long lastMod = reader->GetInt64(3);
					TFile_ptr file = copy_TFile_ptr(serverPath, clientPath, lastMod);
					file->setVersion(aVersion);
					version->addFile(move_TFile_ptr(file));
				}
			}

			this->fSelectVersionCmd->Parameters->Clear();
			reader->Close();

			return version;
		}
		catch (Exception^ e) {
			this->fSelectVersionCmd->Parameters->Clear();
			reader->Close();
			throw EDBException("Error during SelectVersionCmd: " + marshalString(e->Message));
		}
	}
	else{
		throw EDBException("No connection to DB!");
	}
}

TVersion_ptr TDBManagerMSDE::getLastVersion(const string& aUser, bool aLoadFiles){
	if (!System::Object::ReferenceEquals(this->fConnection, nullptr) && (this->fConnection->State == ConnectionState::Open)){
		TVersion_ptr version = nullptr;

		SqlCommand^ cmd = nullptr;
		string f;
		if (aLoadFiles){
			if (System::Object::ReferenceEquals(this->fSelectLastVersionFilesCmd, nullptr)){
				this->fSelectLastVersionFilesCmd = gcnew SqlCommand("SELECT Versions.VerID, VerDate, ServerPath, ClientRelativePath, LastModDate FROM Users, Versions, Files WHERE Users.UserID = Versions.UserID AND Users.UserID = Files.UserID AND Versions.VerID = Files.VerID AND Username = @username AND Versions.VerID = (SELECT MAX(VerID) FROM Versions, Users WHERE Users.UserID = Versions.UserID AND Username = @username);");
				this->fSelectLastVersionFilesCmd->CommandType = CommandType::Text;
				this->fSelectLastVersionFilesCmd->Connection = this->fConnection;
			}
			cmd = this->fSelectLastVersionFilesCmd;
			f = "SelectLastVersionFiles";
		}
		else{
			if (System::Object::ReferenceEquals(this->fSelectLastVersionCmd, nullptr)){
				this->fSelectLastVersionCmd = gcnew SqlCommand("SELECT VerID, VerDate FROM Users, Versions WHERE Users.UserID = Versions.UserID AND Username = @username AND Versions.VerID = (SELECT MAX(VerID) FROM Versions, Users WHERE Users.UserID = Versions.UserID AND Username = @username);");
				this->fSelectLastVersionCmd->CommandType = CommandType::Text;
				this->fSelectLastVersionCmd->Connection = this->fConnection;
			}
			cmd = this->fSelectLastVersionCmd;
			f = "SelectLastVersion";
		}
		
		//Select user version
		cmd->Parameters->Add("@username", SqlDbType::NVarChar, 50)->Value = gcnew String(aUser.c_str());
		SqlDataReader^ reader;
		try{
			reader = cmd->ExecuteReader();

			if (reader->HasRows){
				int v = -0;
				while (reader->Read()){
					if (version == nullptr){
						//build version object
						v = reader->GetInt32(0);
						version = new_TVersion_ptr(v, reader->GetInt64(1));
					}
					if (aLoadFiles){
						string serverPath = marshalString(reader->GetString(2));
						string clientPath = marshalString(reader->GetString(3));
						long long lastMod = reader->GetInt64(4);
						TFile_ptr file = copy_TFile_ptr(serverPath, clientPath, lastMod);
						file->setVersion(v);
						version->addFile(move_TFile_ptr(file));
					}
				}
			}

			cmd->Parameters->Clear();
			reader->Close();

			return version;
		}
		catch (Exception^ e) {
			cmd->Parameters->Clear();
			reader->Close();
			throw EDBException("Error during " + f + ": " + marshalString(e->Message));
		}
	}
	else{
		throw EDBException("No connection to DB!");
	}
}

TVersionList_ptr TDBManagerMSDE::getAllVersions(const string& aUser){
	if (!System::Object::ReferenceEquals(this->fConnection, nullptr) && (this->fConnection->State == ConnectionState::Open)){
		TVersionList_ptr versionList = nullptr;

		//Select user version
		if (System::Object::ReferenceEquals(this->fSelectAllVersionsCmd, nullptr)){
			this->fSelectAllVersionsCmd = gcnew SqlCommand("SELECT VerID, VerDate FROM Users, Versions WHERE Users.UserID = Versions.UserID AND Username = @username;");
			this->fSelectAllVersionsCmd->CommandType = CommandType::Text;
			this->fSelectAllVersionsCmd->Connection = this->fConnection;
		}
		this->fSelectAllVersionsCmd->Parameters->Add("@username", SqlDbType::NVarChar, 50)->Value = gcnew String(aUser.c_str());
		SqlDataReader^ reader;
		try{
			reader = fSelectAllVersionsCmd->ExecuteReader();

			if (reader->HasRows){
				//build version list object
				versionList = new_TVersionList_ptr();
				while (reader->Read()){
					int v = reader->GetInt32(0);
					long long vDate = reader->GetInt64(1);
					TVersion_ptr vPtr = new_TVersion_ptr(v, vDate);
					versionList->push_back(move_TVersion_ptr(vPtr));
				}
			}

			this->fSelectAllVersionsCmd->Parameters->Clear();
			reader->Close();

			return versionList;
		}
		catch (Exception^ e) {
			this->fSelectAllVersionsCmd->Parameters->Clear();
			reader->Close();
			throw EDBException("Error during SelectAllVersionsCmd: " + marshalString(e->Message));
		}
	}
	else{
		throw EDBException("No connection to DB!");
	}
}
#pragma endregion