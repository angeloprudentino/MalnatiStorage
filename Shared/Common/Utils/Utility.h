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
#include <boost/filesystem.hpp>

using namespace std;
using namespace System;
using namespace boost::filesystem;

#define EMPTY ""
#define TRUE_STR "true"
#define FALSE_STR "false"
#define DEFAULT_HOST "127.0.0.1"
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


//////////////////////////////////////
//        EBaseException	        //
//////////////////////////////////////
public class EBaseException : public std::exception {
private:
	string fMessage;
public:
	EBaseException(const string& aMsg){ this->fMessage = aMsg; }
	virtual const string getMessage(){ return this->fMessage; }
};

//////////////////////////////////////
//       EOpensslException	        //
//////////////////////////////////////
class EOpensslException : public EBaseException{
public:
	EOpensslException(const string& aMsg) : EBaseException(aMsg){}
};

//////////////////////////////////////
//      EFilesystemException	    //
//////////////////////////////////////
class EFilesystemException : public EBaseException{
public:
	EFilesystemException(const string& aMsg) : EBaseException(aMsg){}
};


// Get current date/time; format is [YYYY-MM-DD HH:mm:ss]
const string currentDateTime();

// Convert time_t to string and vice versa
const string timeToString(const time_t& t);
const time_t stringToTime(const string& s);
const string formatFileDate(const time_t& t);

// Convert strings from std to System and viceversa
const string marshalString(String^ aStr);
String^ unmarshalString(const string& aStr);

// Generate a unique token to identify a session
string_ptr getUniqueToken(const string& aUser); //throws EOpensslException
const string getUserFromToken(const string& aToken); //throws EOpensslException

// openssl crypto system init
void initCrypto();

// Evaluate content and make its checksum
string_ptr opensslB64Checksum(const string& aString, const bool aStrongAlg); //throws EOpensslException
string_ptr opensslB64PathChecksum(const string& aString); //throws EOpensslException
#define opensslB64FileChecksum(aString) opensslB64Checksum(aString, false)
string_ptr opensslB64RandomToken(); //throws EOpensslException

//Log to file
void logToFile(const string& aClassName, const string& aFuncName, const string& aMsg);
void warningToFile(const string& aClassName, const string& aFuncName, const string& aMsg);
void errorToFile(const string& aClassName, const string& aFuncName, const string& aMsg);
void criticalErrorToFile(const string& aClassName, const string& aFuncName, const string& aMsg);

//Filesystem utilities
void storeFile(const path& aPath, string_ptr& aFileContent); //throws EFilesystemException
string_ptr readFile(const path& aPath); //throws EFilesystemException
void removeDir(const path& aPath); //throws EFilesystemException
string buildServerPathPrefix(const string& aUser, const int aVersion); //throws EFilesystemException