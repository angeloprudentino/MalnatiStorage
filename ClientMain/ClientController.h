/*
 * Author: Angelo Prudentino
 * Date: 14/09/2016
 * File: ClientController.h
 * Description: this file contains all classes implementing
 *              client-side core logic
 *
 */

#pragma once
#include <string>

using namespace System;
//using namespace System::Collections::Generic;

//////////////////////////////////
//         UserVersion	        //
//////////////////////////////////
public ref class UserVersion{
private:
	int fVersionID = -1;
	String^ fVersionDate = nullptr;

public:
	UserVersion(const String^ aVersionDate, const int aVersionID);

	//getters
	const int getVersionID(){ return this->fVersionID; }
	const String^ getVersionDate(){ return this->fVersionDate; }
};


///////////////////////////////
//          File		     //
///////////////////////////////
public ref class File{
private :
	String^ fFileName;
	String^ fFilePath;

public:
	File(const String^ aFilePath);

	//getters
	const String^ getFileName() { return this->fFileName; }
	const String^ getFilePath() { return this->fFilePath; }
};


//////////////////////////////////////
//		StorageClientController		//
//////////////////////////////////////
public interface class StorageClientController{
public:
	void onLoginSuccess();
	String^ onLoginError();
	void onRegistrationSucces();
	String^ onRegistrationError();
	void onUpdateStart();
	///List<File> onUpdateSuccess();
	String^ onUpdateError();
	void onRestoreSuccess();
	String^ onRestoreError();
	//std::list<UserVersion> onGetVersionsSuccess();
	String^ onGetVersionsError();
};


#define NO_ID -1
#define REGISTR_REQ 0
#define LOGIN_REQ 1
#define UPDATE_REQ 2
#define GET_VERSIONS_REQ 3
#define RESTORE_REQ 4

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
	RegistrRequest(const String^ aUser, const String^ aPass, const String^ aPath);

	//getters
	const String^ getUser() { return this->fUser; }
	const String^ getPass() { return this->fPass; }
	const String^ getPath() { return this->fPath; }
};


//////////////////////////////////
//         LoginRequest	        //
//////////////////////////////////
public ref class LoginRequest : public UserRequest{
private:
	String^ fUser;
	String^ fPass;

public:
	LoginRequest(const String^ aUser, const String^ aPass);

	//getters
	const String^ getUser() { return this->fUser; }
	const String^ getPass() { return this->fPass; }
};


//////////////////////////////////
//         UpdateRequest	    //
//////////////////////////////////
public ref class UpdateRequest : public UserRequest{
private:
	String^ fUser;
	String^ fPass;

public:
	UpdateRequest(const String^ aUser, const String^ aPass);

	//getters
	const String^ getUser() { return this->fUser; }
	const String^ getPass() { return this->fPass; }
};


//////////////////////////////////
//         GetVerRequest	    //
//////////////////////////////////
public ref class GetVerRequest : public UserRequest{
private:
	String^ fUser;
	String^ fPass;

public:
	GetVerRequest(const String^ aUser, const String^ aPass);

	//getters
	const String^ getUser() { return this->fUser; }
	const String^ getPass() { return this->fPass; }
};


//////////////////////////////////
//        RestoreRequest	    //
//////////////////////////////////
public ref class RestoreRequest : public UserRequest{
private:
	String^ fUser;
	String^ fPass;
	int fVersion;

public:
	RestoreRequest(const String^ aUser, const String^ aPass, const int aVersion);

	//getters
	const String^ getUser() { return this->fUser; }
	const String^ getPass() { return this->fPass; }
	const int getVersion() { return this->fVersion; }
};
