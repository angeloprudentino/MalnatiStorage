/*
 * Authors: Angelo Prudentino & Daniele Testa
 * Date: 16/09/2016
 * File: ClientRequests.h
 * Description: Objects to interface C++ core dll and WPF gui 
 *
 */

#pragma once
#include <vcclr.h>
#include <string>
#include <queue>
#include <memory>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/atomic.hpp>
#include "ClientController.h"

using namespace System;
using namespace System::Collections::Generic;

#define NO_ID -1
#define REGISTR_REQ 0
#define LOGIN_REQ 1
#define LOGOUT_REQ 2
#define UPDATE_REQ 3
#define GET_VERSIONS_REQ 4
#define RESTORE_REQ 5
#define PING_REQ 6


//////////////////////////////////
//         UserRequest	        //
//////////////////////////////////
public ref class UserRequest{
protected:
	int fID;

public:
	UserRequest(){ this->fID = NO_ID; }

	//getter
	const int getID(){ return this->fID; }
};


//////////////////////////////////
//        RegistrRequest	    //
//////////////////////////////////
public ref class RegistrRequest : public UserRequest{
private:
	String^ fUser;
	String^ fPass;
	String^ fPath;

public:
	RegistrRequest(String^ aUser, String^ aPass, String^ aPath);

	//getters
	String^ getUser() { return this->fUser; }
	String^ getPass() { return this->fPass; }
	String^ getPath() { return this->fPath; }
};


//////////////////////////////////
//         LoginRequest	        //
//////////////////////////////////
public ref class LoginRequest : public UserRequest{
private:
	String^ fUser;
	String^ fPass;

public:
	LoginRequest(String^ aUser, String^ aPass);

	//getters
	String^ getUser() { return this->fUser; }
	String^ getPass() { return this->fPass; }
};


//////////////////////////////////
//        LogoutRequest	        //
//////////////////////////////////
public ref class LogoutRequest : public UserRequest{
public:
	LogoutRequest() { this->fID = LOGOUT_REQ; }
};


//////////////////////////////////
//         UpdateRequest	    //
//////////////////////////////////
public ref class UpdateRequest : public UserRequest{
private:
	String^ fUser;
	String^ fPass;
	String^ fPath;

public:
	UpdateRequest(String^ aUser, String^ aPass, String^ aPath);

	//getters
	String^ getUser() { return this->fUser; }
	String^ getPass() { return this->fPass; }
	String^ getPath() { return this->fPath; }
};


//////////////////////////////////
//         GetVerRequest	    //
//////////////////////////////////
public ref class GetVerRequest : public UserRequest{
private:
	String^ fUser;
	String^ fPass;

public:
	GetVerRequest(String^ aUser, String^ aPass);

	//getters
	String^ getUser() { return this->fUser; }
	String^ getPass() { return this->fPass; }
};


//////////////////////////////////
//        RestoreRequest	    //
//////////////////////////////////
public ref class RestoreRequest : public UserRequest{
private:
	String^ fUser;
	String^ fPass;
	int fVersion;
	String^ fFile;
	String^ fDestPath;

public:
	RestoreRequest(String^ aUser, String^ aPass, const int aVersion, String^ aFile, String^ aDestPath);

	//getters
	String^ getUser() { return this->fUser; }
	String^ getPass() { return this->fPass; }
	const int getVersion() { return this->fVersion; }
	String^ getFile() { return this->fFile; }
	String^ getDestPath() { return this->fDestPath; }
};


//////////////////////////////////
//         PingRequest	        //
//////////////////////////////////
public ref class PingRequest : public UserRequest{
private:
	String^ fToken;

public:
	PingRequest(String^ aToken);

	//getters
	String^ getToken() { return this->fToken; }
};


/////////////////////////////////////
//         RequestsQueue           //
/////////////////////////////////////
class RequestsQueue{
private:
	std::queue<gcroot<UserRequest^>> fQueue;
	boost::mutex fMutex;
	boost::condition_variable fCond;
	boost::atomic<bool> fMustExit;

public:
	RequestsQueue();
	~RequestsQueue();
	RequestsQueue(const RequestsQueue&) = delete;            // disable copying
	RequestsQueue& operator=(const RequestsQueue&) = delete; // disable assignment

	bool isEmpty();
	UserRequest^ popRequest();
	void pushRequest(UserRequest^ aReq);
};