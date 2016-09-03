/*
 * Author: Angelo Prudentino
 * Date: 10/07/2016
 * File: DBManagerInterface.h
 * Description: This file describes all the interactions with a generic DB
 *
 */

#pragma once
#include "Utility.h"
#include "Session.h"


//////////////////////////////////
//       EDBException	        //
//////////////////////////////////
public class EDBException : public EBaseException{
public:
	EDBException(const string aMsg) : EBaseException(aMsg){}
};


//////////////////////////////////
//     IDBManagerInterface      //
//////////////////////////////////
public class IDBManagerInterface{
public:
	virtual void insertNewUser(const string& aUser, const string& aPass) = 0; // throws EDBException
	virtual void InsertNewVersion(const string& aUser, TVersion_ptr& aVersion) = 0; // throws EDBException

	virtual const bool checkIfUserExists(const string& aUser) = 0; // throws EDBException
	virtual const bool verifyUserCredentials(const string& aUser, const string& aPass) = 0; // throws EDBException
	virtual TVersion_ptr getVersion(const string& aUser, int aVersion) = 0; // throws EDBException
	virtual TVersion_ptr getLastVersion(const string& aUser, bool aLoadFiles) = 0; // throws EDBException
	virtual TVersionList_ptr getAllVersions(const string& aUser) = 0; // throws EDBException
};