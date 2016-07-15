/*
* Author: Angelo Prudentino
* Date: 29/05/2016
* File: Session.h
* Description: this file contains all classes related to user versions 
*              stored on the server
*
*/
#pragma once
#include <string>
#include <map>
#include <list>
#include <memory>
#include "Utility.h"

using namespace std;

#define NO_ID -1
//possible user sessions
#define NO_SESSION -1
#define UPDATE_SESSION 0
#define RESTORE_SESSION 1

#define NO_VERSION -1


//////////////////////////////////
//           TFile	            //
//////////////////////////////////
public class TFile{
private:
	string_ptr fServerPathPrefix = nullptr;
	string_ptr fClientRelativePath = nullptr;
	time_t fFileDate;
	string_ptr fChecksum = nullptr;
	bool fProcessed = false;

public:
	TFile(const string& aServerPathPrefix, const string& aClientRelativePath, const time_t aFileDate, string_ptr& aChecksum, const bool aProcessed);
	~TFile();

	const bool isEqualTo(const TFile& aFile);

	//getters
};
typedef unique_ptr<TFile> TFile_ptr;
typedef list<TFile_ptr> TFile_list;
typedef unique_ptr<TFile_list> TFile_list_ptr;
typedef TFile_list::iterator TFileHandle;
#define new_TFile_ptr(aServerPathPrefix, aClientRelativePath, aFileDate, aChecksum, aProcessed) std::make_unique<TFile>(aServerPathPrefix, aClientRelativePath, aFileDate, aChecksum, aProcessed)
#define move_TFile_ptr(ptr) std::move(ptr)
#define new_TFile_list_ptr() std::make_unique<TFile_list>()
#define move_TFile_list_ptr(ptr) std::move(ptr)


//////////////////////////////////
//          TVersion	        //
//////////////////////////////////
public class TVersion{
private:
	int fId = NO_ID;
	time_t fVersionDate;
	TFile_list_ptr fFileList = nullptr;
	TFileHandle fNext;  //used only to manage RESTORE_SESSION cases

public:
	TVersion(const int aId, const time_t aVersionDate);
	~TVersion();
	
	void addFile(TFile_ptr& aFile);
	void updateFile(TFile_ptr& aFile);
	void removeFile(TFile_ptr& aFile);

	//getters
	const int getVersion() { return this->fId; }
	const TFile getNextFile() { return *(*(this->fNext)); }
};
typedef unique_ptr<TVersion> TVersion_ptr;
#define new_TVersion_ptr(aId, aVersionDate) std::make_unique<TVersion>()
#define move_TVersion_ptr(ptr) std::move(ptr)


//////////////////////////////////
//          TSession	        //
//////////////////////////////////
public class TSession{
private:
	int fKind = NO_SESSION;
	string_ptr fToken = nullptr;
	time_t fLastPing;
	TVersion_ptr fVersion = nullptr;

public:
	TSession(const int aKind, const string& aToken);
	~TSession();

	void addFile(TFile_ptr& aFile);
	void updateFile(TFile_ptr& aFile);
	void removeFile(TFile_ptr& aFile);

	//getters
	const int getKind() { return this->fKind; }
	const string getToken() { return *(this->fToken); }

	//setters
	void setLastPing(const time_t aPing) { this->fLastPing = aPing; }
	void setVersion(TVersion_ptr& aVersion) { this->fVersion = move_TVersion_ptr(aVersion); }
};
typedef unique_ptr<TSession> TSession_ptr;
typedef map<string, TSession_ptr> TSessions;
#define new_TSession_ptr(aKind, aToken) std::make_unique<TSession>(aKind, aToken)
#define move_TSession_ptr(ptr) std::move(ptr)

