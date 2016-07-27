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

#define NO_VERSION -1


//////////////////////////////////
//           TFile	            //
//////////////////////////////////
public class TFile{
private:
	string_ptr fServerPathPrefix = nullptr;
	string_ptr fClientRelativePath = nullptr;
	int fVersion = -1;
	time_t fFileDate;
	string_ptr fChecksum = nullptr;
	bool fProcessed = false;

public:
	TFile(const string& aServerPathPrefix, const string& aClientRelativePath, const time_t aFileDate, string_ptr& aChecksum, const bool aProcessed);
	~TFile();

	const bool isEqualTo(const TFile& aFile);

	//getters
	const string getServerPathPrefix() { return *(this->fServerPathPrefix); }
	const string getClientRelativePath() { return *(this->fClientRelativePath); }
	const time_t getFileDate() { return this->fFileDate; }
	const string getFileChecksum() { return *(this->fChecksum); }

	const int getVersion() { return this->fVersion; }

	//setters
	void setVersion(const int aVersion){ this->fVersion = aVersion; }
	void setProcessed() { this->fProcessed = true; }
};
typedef unique_ptr<TFile> TFile_ptr;
typedef list<TFile_ptr> TFile_list;
typedef TFile_list::iterator TFileHandle;
#define new_TFile_ptr(aServerPathPrefix, aClientRelativePath, aFileDate, aChecksum, aProcessed) std::make_unique<TFile>(aServerPathPrefix, aClientRelativePath, aFileDate, aChecksum, aProcessed)
#define move_TFile_ptr(ptr) std::move(ptr)


//////////////////////////////////
//          TVersion	        //
//////////////////////////////////
public class TVersion{
private:
	int fId = NO_ID;
	time_t fVersionDate;
	TFile_list fFileList;
	TFileHandle fNext;  //used only to manage RESTORE_SESSION cases

public:
	TVersion(const int aId, const time_t aVersionDate);
	~TVersion();
	
	void addFile(TFile_ptr& aFile);
	void updateFile(TFile_ptr& aFile);
	void removeFile(TFile_ptr& aFile);

	void updateNext() { (*this->fNext)->setProcessed(); this->fNext++; }

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
	TVersion_ptr terminateWithSucces();

	//getters
	const int getKind() { return this->fKind; }
	const string getToken() { return *(this->fToken); }
	const int getVersion() { this->fVersion->getVersion(); }
	TFile_ptr getNextFileToSend(){ return move_TFile_ptr(this->fVersion->getNextFile()); }
	TFile_ptr updateNextFileToSend(){ this->fVersion->updateNext(); return move_TFile_ptr(this->fVersion->getNextFile()); }

	//setters
	void setLastPing(const time_t aPing) { this->fLastPing = aPing; }
	void setVersion(TVersion_ptr& aVersion) { this->fVersion = move_TVersion_ptr(aVersion); }
};
typedef std::shared_ptr<TSession> TSession_ptr;
typedef map<string, TSession_ptr> TSessions;
#define new_TSession_ptr(aKind, aToken) std::make_shared<TSession>(aKind, aToken)
