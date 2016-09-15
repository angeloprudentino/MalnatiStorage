/*
 * Author: Angelo Prudentino
 * Date: 14/09/2016
 * File: ClientController.h
 * Description: this file contains all classes implementing
 *              client-side core logic
 *
 */

#pragma once
#include <vcclr.h>
#include <string>

using namespace System;


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
	void onUpdateSuccess();
	String^ onUpdateError();
	void onRestoreSuccess();
	String^ onRestoreError();
	void onGetVersionsSuccess();
	String^ onGetVersionsError();
};


//////////////////////////////////
//          Version	            //
//////////////////////////////////
public ref class Version{
private:
	int fVersionID = -1;
	String^ fVersionDate = nullptr;

public:
	Version(const String^ aVersionDate, const int aVersionID){ this->fVersionID = aVersionID; this->fVersionDate = (String^)aVersionDate; }

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
};