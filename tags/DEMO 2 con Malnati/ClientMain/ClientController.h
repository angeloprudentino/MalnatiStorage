/*
 * Authors: Angelo Prudentino & Daniele Testa
 * Date: 14/09/2016
 * File: ClientController.h
 * Description: Objects to interface C++ core dll and WPF gui 
 *
 */

#pragma once
#include <string>

using namespace System;
using namespace System::Collections::Generic;

///////////////////////////////
//          UserFile	     //
///////////////////////////////
public ref class UserFile{
private :
	int fVersion = 0;
	String^ fFileName;
	String^ fFilePath;

public:
	UserFile(const int aVersion, String^ aFilePath);

	//getters
	const int getFileVersion() { return this->fVersion; }
	String^ getFileName() { return this->fFileName; }
	String^ getFilePath() { return this->fFilePath; }
};


//////////////////////////////////
//         UserVersion	        //
//////////////////////////////////
public ref class UserVersion{
private:
	int fVersionID = -1;
	String^ fVersionDate = nullptr;
	List<UserFile^>^ fFileList = nullptr;

public:
	UserVersion(String^ aVersionDate, const int aVersionID, List<UserFile^>^ aFileList);

	//getters
	const int getVersionID() { return this->fVersionID; }
	String^ getVersionDate() { return this->fVersionDate; }
	List<UserFile^>^ getFileList() { return this->fFileList; }
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
	void onRestoreSuccess(List<UserFile^>^ aFileList, const int aVersion, String^ aVersionDate);
	void onRestoreError(String^ aMsg);
	void onGetVersionsSuccess(List<UserVersion^>^ aVersionsList);
	void onGetVersionsError(String^ aMsg);
};