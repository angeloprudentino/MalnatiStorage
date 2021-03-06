#include "pch.h"

/*
 * Author: Angelo Prudentino
 * Date: 30/09/2015
 * File: Utility.h
 * Description: utility functions for both client and server
 *
 */
#pragma once

#include <string>
#include <memory>

using namespace std;

#define EMPTY ""
#define TRUE_STR "true"
#define FALSE_STR "false"
#define DEFAULT_PORT 4700

//typedef std::shared_ptr<string> string_ptr;
//#define make_string_ptr(ptr) std::make_shared<string>(ptr)
//#define move_string_ptr(ptr) (ptr)

typedef std::unique_ptr<string> string_ptr;
#define make_string_ptr(ptr) std::make_unique<string>(ptr)
#define new_string_ptr() std::make_unique<string>()
#define move_string_ptr(ptr) std::move(ptr)

typedef struct{
	int size;
	char* data;
}B64result;

#ifdef STORAGE_SERVER

//////////////////////////////////////
//      IServerBaseController	    //
//////////////////////////////////////
public class IServerBaseController{
public:
	virtual void onServerLog(string aClassName, string aFuncName, string aMsg) = 0;
	virtual void onServerWarning(string aClassName, string aFuncName, string aMsg) = 0;
	virtual void onServerError(string aClassName, string aFuncName, string aMsg) = 0;
	virtual void onServerCriticalError(string aClassName, string aFuncName, string aMsg) = 0;
};
#define doServerLog(ptr, aClassName, aFuncName, aMsg) if(ptr!=nullptr){ptr->onServerLog(aClassName, aFuncName, aMsg);}
#define doServerWarning(ptr, aClassName, aFuncName, aMsg) if(ptr!=nullptr){ptr->onServerWarning(aClassName, aFuncName, aMsg);}
#define doServerError(ptr, aClassName, aFuncName, aMsg) if(ptr!=nullptr){ptr->onServerError(aClassName, aFuncName, aMsg);}
#define doServerCriticalError(ptr, aClassName, aFuncName, aMsg) if(ptr!=nullptr){ptr->onServerCriticalError(aClassName, aFuncName, aMsg);}

#endif


//////////////////////////////////////
//        EBaseException	        //
//////////////////////////////////////
class EBaseException : public std::exception {
private:
	string fMessage;
public:
	EBaseException(const string aMsg){ this->fMessage = aMsg; }
	virtual const string getMessage(){ return this->fMessage; }
};

//////////////////////////////////////
//       EOpensslException	        //
//////////////////////////////////////
class EOpensslException : public EBaseException{
public:
	EOpensslException(const string aMsg) : EBaseException(aMsg){}
};


// Get current date/time; format is [YYYY-MM-DD HH:mm:ss]
const string currentDateTime();

// Convert time_t to string and vice versa
const string timeToString(const time_t& t);
const time_t stringToTime(const string& s);
const string formatFileDate(const time_t& t);

// Generate a unique token to identify a session
string_ptr getUniqueToken(const string& aUser); //throws EOpensslException
const string getUserFromToken(const string& aToken); //throws EOpensslException

// openssl crypto system init
void initCrypto();

// Base64 encode/decode functions
string_ptr opensslB64Encode(char* aContent, int aLen); //throws EOpensslException
B64result opensslB64Decode(const string& aString); //throws EOpensslException
string_ptr opensslB64EncodeFile(const string& aFileName); //throws EOpensslException

// Evaluate a file and make its checksum
//string_ptr opensslChecksum(char* aContent, int aLen); //throws EOpensslException
//string_ptr opensslFileChecksum(const string aFileName); //throws EOpensslException
string_ptr opensslB64Checksum(const string& aString); //throws EOpensslException
string_ptr opensslB64RandomToken();
