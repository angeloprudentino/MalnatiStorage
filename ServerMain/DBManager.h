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


////////////////////////////////////
//          TDBManager	          //
////////////////////////////////////
public class TDBManager {
private:
	bool fOpen = false;

	gcroot<SqlConnection^> fConnection = nullptr;
	string_ptr fHost = nullptr;
	string_ptr fDBName = nullptr;

	IServerBaseController* fCallbackObj = nullptr;

public:
	TDBManager(const string& aHost, const string& aDBName, IServerBaseController* aCallback);
	~TDBManager();
	
	void insertNewUser(const string& aUser, const string& aPass, const string& aSalt);
	void InsertNewVersion(TVersion_ptr& aVersion);

	const bool checkIfUserExists(const string& aUser);
	const bool verifyUserCredentials(const string& aUser, const string& aPass, const string& aSalt);
	TVersion_ptr getLastVerison(const string& aUser);

	//getters
	const bool isOpen() { return this->fOpen; }
};


