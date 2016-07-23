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


#ifdef STORAGE_SERVER
//////////////////////////////////
//       EDBException	        //
//////////////////////////////////
public class EDBException : public EBaseException{
public:
	EDBException(const string aMsg) : EBaseException(aMsg){}
};


/////////////////////////////////////
//		IServerDBController		   //
/////////////////////////////////////
public class IServerDBController : public IServerBaseController {

};


////////////////////////////////////
//          TDBManager	          //
////////////////////////////////////
public class TDBManager {
private:
	gcroot<SqlConnection^> fConnection = nullptr;
	string_ptr fHost = nullptr;
	string_ptr fDBName = nullptr;

	IServerDBController* fCallbackObj = nullptr;

public:
	TDBManager(const string& aHost, const string& aDBName, IServerDBController* aCallback);
	~TDBManager();
	
	void insertNewUser(const string& aUser, const string& aPass); // throws EDBException
	void InsertNewVersion(const string& aUser, TVersion_ptr& aVersion); // throws EDBException

	const bool checkIfUserExists(const string& aUser);
	const bool verifyUserCredentials(const string& aUser, const string& aPass);
	TVersion_ptr getVersion(const string& aUser, int aVersion);
	TVersion_ptr getLastVersion(const string& aUser);
	TVersionList_ptr getAllVersions(const string& aUser);
};
#endif;

