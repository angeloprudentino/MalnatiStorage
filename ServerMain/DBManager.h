/*
 * Author: Angelo Prudentino
 * Date: 10/07/2016
 * File: DBManager.h
 * Description: This class manages all the communications with the DB
 *
 */

#pragma once
#include <string>
#include <memory>
#include <vcclr.h>
#include "Utility.h"
#include "Session.h"

#define DEFAULT_DB_HOST "localhost"
#define DEFAULT_DB_NAME "StorageDB"

using namespace std;
using namespace System;
using namespace System::Data;
using namespace System::Data::SqlClient;


//////////////////////////////////
//       EDBException	        //
//////////////////////////////////
public class EDBException : public EBaseException{
public:
	EDBException(const string aMsg) : EBaseException(aMsg){}
};


////////////////////////////////////
//          TDBManager	          //
////////////////////////////////////
public class TDBManager {
private:
	gcroot<SqlConnection^> fConnection = nullptr;
	string_ptr fHost = nullptr;
	string_ptr fDBName = nullptr;

	gcroot<SqlCommand^> fInsertUserCmd = nullptr;
	gcroot<SqlCommand^> fInsertVersionCmd = nullptr;
	gcroot<SqlCommand^> fInsertFilesCmd = nullptr;
	gcroot<SqlCommand^> fSelectUserIdCmd = nullptr;
	gcroot<SqlCommand^> fSelectUserSaltCmd = nullptr;
	gcroot<SqlCommand^> fVerifyCredentialCmd = nullptr;
	gcroot<SqlCommand^> fSelectVersionCmd = nullptr;
	gcroot<SqlCommand^> fSelectLastVersionCmd = nullptr;
	gcroot<SqlCommand^> fSelectLastVersionFilesCmd = nullptr;
	gcroot<SqlCommand^> fSelectAllVersionsCmd = nullptr;

	const int getUserID(const string& aUser, SqlTransaction^ aTransaction);

public:
	TDBManager(const string& aHost, const string& aDBName); // throws EDBException
	~TDBManager();
	
	void insertNewUser(const string& aUser, const string& aPass); // throws EDBException
	void InsertNewVersion(const string& aUser, TVersion_ptr& aVersion); // throws EDBException

	const bool checkIfUserExists(const string& aUser); // throws EDBException
	const bool verifyUserCredentials(const string& aUser, const string& aPass); // throws EDBException
	TVersion_ptr getVersion(const string& aUser, int aVersion); // throws EDBException
	TVersion_ptr getLastVersion(const string& aUser, bool aLoadFiles); // throws EDBException
	TVersionList_ptr getAllVersions(const string& aUser); // throws EDBException
};
