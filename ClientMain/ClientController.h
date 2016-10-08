/*
 * Author: Angelo Prudentino
 * Date: 14/09/2016
 * File: ClientController.h
 * Description: 
 *
 */

#pragma once
#include <string>

using namespace System;
using namespace System::Collections::Generic;

//////////////////////////////////
//         UserVersion	        //
//////////////////////////////////
public ref class UserVersion{
private:
	int fVersionID = -1;
	String^ fVersionDate = nullptr;

public:
	UserVersion(String^ aVersionDate, const int aVersionID);

	//getters
	const int getVersionID(){ return this->fVersionID; }
	String^ getVersionDate(){ return this->fVersionDate; }
};


///////////////////////////////
//          UserFile	     //
///////////////////////////////
public ref class UserFile{
private :
	String^ fFileName;
	String^ fFilePath;

public:
	UserFile(String^ aFilePath);

	//getters
	String^ getFileName() { return this->fFileName; }
	String^ getFilePath() { return this->fFilePath; }
};


//////////////////////////////////////
//		StorageClientController		//
//////////////////////////////////////
public interface class StorageClientController{
public:
	void onLoginSuccess(String^ aPath);
	void onLoginError(String^ aMsg);
	void onRegistrationSucces();
	void onRegistrationError(String^ aMsg);
	void onUpdateStart(String^ aToken);
	void onUpdateSuccess(List<UserFile^>^ aFileList, const int aVersion, String^ aVersionDate);
	void onUpdateError(String^ aMsg);
	void onRestoreStart(String^ aToken);
	void onRestoreSuccess(const int aVersion, String^ aVersionDate);
	void onRestoreError(String^ aMsg);
	void onGetVersionsSuccess(List<UserVersion^>^ aVersionsList);
	void onGetVersionsError(String^ aMsg);
};