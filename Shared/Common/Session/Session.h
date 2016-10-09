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
#include <vector>
#include <memory>
#include "Utility.h"

using namespace std;

#define NO_ID -1
//possible user sessions
#define NO_SESSION -1
#define UPDATE_SESSION 0
#define RESTORE_SESSION 1

#define NO_VERSION 0

#define SESSION_TIMEOUT 180

//////////////////////////////////
//           TFile	            //
//////////////////////////////////
class TFile{
private:
	string_ptr fServerPathPrefix = nullptr;
	string_ptr fClientRelativePath = nullptr;
	time_t fLastMod;
	int fVersion = -1;

public:
	TFile(const string& aUser, const int aVersion, const string& aClientRelativePath, const time_t& aLastMod);
	TFile(const string& aServerPathPrefix, const string& aClientRelativePath, const time_t& aLastMod);
	TFile(const TFile&) = delete;            // disable copying
	TFile& operator=(const TFile&) = delete; // disable assignment
	~TFile();

	const bool isEqualTo(const TFile& aFile);

	//getters
	const string getServerPathPrefix() { return *(this->fServerPathPrefix); }
	const string getClientRelativePath() { return *(this->fClientRelativePath); }
	const int getVersion() { return this->fVersion; }
	const time_t getLastMod(){ return this->fLastMod; }

	//setters
	const void setVersion(const int aVersion){ this->fVersion = aVersion; }
};
typedef unique_ptr<TFile> TFile_ptr;
typedef list<TFile_ptr> TFile_list;
typedef TFile_list::iterator TFileHandle;
#define new_TFile_ptr(aUser, aVersion, aClientRelativePath, aLastMod) std::make_unique<TFile>(aUser, aVersion, aClientRelativePath, aLastMod)
#define copy_TFile_ptr(aServerPathPrefix, aClientRelativePath, aLastMod) std::make_unique<TFile>(aServerPathPrefix, aClientRelativePath, aLastMod)
#define move_TFile_ptr(ptr) std::move(ptr)


//////////////////////////////////
//          TVersion	        //
//////////////////////////////////
class TVersion{
private:
	int fId = NO_ID;
	time_t fVersionDate;
	bool fSomethingRemoved = false;
	TFile_list fFileList;
	TFileHandle fNext;  //used only to manage RESTORE_SESSION cases

public:
	TVersion(const int aId, const time_t aVersionDate);
	TVersion(const TVersion&) = delete;            // disable copying
	TVersion& operator=(const TVersion&) = delete; // disable assignment
	~TVersion();
	
	void addFile(TFile_ptr& aFile);
	const bool updateFile(TFile_ptr& aFile);
	void removeFile(TFile_ptr& aFile);

	void updateNext() { this->fNext++; }
	void terminateWithSuccess();
	const bool isValid();
	
	//getters
	const int getVersion() { return this->fId; }
	const time_t getDate() { return this->fVersionDate; }
	TFile_ptr getNextFile();
};
typedef unique_ptr<TVersion> TVersion_ptr;
typedef vector<TVersion_ptr> TVersionList;
typedef unique_ptr<TVersionList> TVersionList_ptr;

#define new_TVersion_ptr(aId, aVersionDate) std::make_unique<TVersion>(aId, aVersionDate)
#define move_TVersion_ptr(ptr) std::move(ptr)
#define new_TVersionList_ptr() std::make_unique<TVersionList>()
#define move_TVersionList_ptr(ptr) std::move(ptr)


//////////////////////////////////
//          TSession	        //
//////////////////////////////////
class TSession{
private:
	int fKind = NO_SESSION;
	string_ptr fToken = nullptr;
	time_t fLastPing;
	TVersion_ptr fVersion = nullptr;

public:
	TSession(const int aKind, const string& aToken);
	TSession(const TSession&) = delete;            // disable copying
	TSession& operator=(const TSession&) = delete; // disable assignment
	~TSession();

	void addFile(TFile_ptr& aFile);
	const bool updateFile(TFile_ptr& aFile);
	void removeFile(TFile_ptr& aFile);

	//getters
	const int getKind() { return this->fKind; }
	const string getToken() { return *(this->fToken); }
	const time_t getLastPing() { return this->fLastPing; }
	const int getVersion() { return this->fVersion->getVersion(); }
	TFile_ptr getNextFileToSend(){ return move_TFile_ptr(this->fVersion->getNextFile()); }
	TVersion_ptr terminateSession() { return move_TVersion_ptr(this->fVersion); }

	//setters
	void setLastPing(const time_t aPing) { this->fLastPing = aPing; }
	void setVersion(TVersion_ptr& aVersion) { this->fVersion = move_TVersion_ptr(aVersion); }
	TFile_ptr updateNextFileToSend(){ this->fVersion->updateNext(); return move_TFile_ptr(this->fVersion->getNextFile()); }
};
typedef std::shared_ptr<TSession> TSession_ptr;
typedef map<string, TSession_ptr> TSessions;
#define new_TSession_ptr(aKind, aToken) std::make_shared<TSession>(aKind, aToken)
