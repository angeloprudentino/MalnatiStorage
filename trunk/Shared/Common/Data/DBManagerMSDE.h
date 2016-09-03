/*
 * Author: Angelo Prudentino
 * Date: 10/07/2016
 * File: DBManagerMSDE.h
 * Description: This class manages all the interactions with an MSDE DB
 *
 */

#pragma once
#include <string>
#include <memory>
#include <vcclr.h>
#include "Utility.h"
#include "Session.h"
#include "DBManagerInterface.h"

#define DEFAULT_DB_HOST "localhost"
#define DEFAULT_DB_NAME "StorageDB"

using namespace std;
using namespace System;
using namespace System::Data;
using namespace System::Data::SqlClient;


////////////////////////////////////
//       TDBManagerMSDE	          //
////////////////////////////////////
public class TDBManagerMSDE : public IDBManagerInterface {
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
	TDBManagerMSDE(const string& aHost, const string& aDBName); // throws EDBException
	TDBManagerMSDE(const TDBManagerMSDE&) = delete;            // disable copying
	TDBManagerMSDE& operator=(const TDBManagerMSDE&) = delete; // disable assignment
	~TDBManagerMSDE();
	
	void insertNewUser(const string& aUser, const string& aPass) override; // throws EDBException
	void InsertNewVersion(const string& aUser, TVersion_ptr& aVersion) override; // throws EDBException

	const bool checkIfUserExists(const string& aUser) override; // throws EDBException
	const bool verifyUserCredentials(const string& aUser, const string& aPass) override; // throws EDBException
	TVersion_ptr getVersion(const string& aUser, int aVersion) override; // throws EDBException
	TVersion_ptr getLastVersion(const string& aUser, bool aLoadFiles) override; // throws EDBException
	TVersionList_ptr getAllVersions(const string& aUser) override; // throws EDBException
};
