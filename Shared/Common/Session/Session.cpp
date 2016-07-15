/*
* Author: Angelo Prudentino
* Date: 29/05/2016
* File: Session.cpp
* Description: this file contains all classes related to user versions
*              stored on the server
*
*/

#include <time.h>
#include "Session.h"

using namespace std;

//////////////////////////////////
//           TFile	            //
//////////////////////////////////
#pragma region "TFile"
TFile::TFile(const string& aServerPathPrefix, const string& aClientRelativePath, const time_t aFileDate, string_ptr& aChecksum, const bool aProcessed){
	this->fServerPathPrefix = make_string_ptr(aServerPathPrefix);
	this->fClientRelativePath = make_string_ptr(aClientRelativePath);
	this->fFileDate = aFileDate;
	this->fChecksum = move_string_ptr(aChecksum);
	this->fProcessed = aProcessed;
}

TFile::~TFile(){
	if (this->fServerPathPrefix != nullptr)
		this->fServerPathPrefix.reset();

	if (this->fClientRelativePath != nullptr)
		this->fClientRelativePath.reset();

	if (this->fChecksum != nullptr)
		this->fChecksum.reset();

	this->fServerPathPrefix = nullptr;
	this->fClientRelativePath = nullptr;
	this->fChecksum = nullptr;
}

const bool TFile::isEqualTo(const TFile& aFile){
	return (*(this->fServerPathPrefix) == *(aFile.fServerPathPrefix) &&
		    *(this->fClientRelativePath) == *(aFile.fClientRelativePath));
}
#pragma endregion


//////////////////////////////////
//          TVersion	        //
//////////////////////////////////
#pragma region "TVersion"
TVersion::TVersion(const int aId, const time_t aVersionDate){
	this->fId = aId;
	this->fVersionDate = aVersionDate;
	this->fFileList = new_TFile_list_ptr();
	this->fNext = this->fFileList->begin();
}

TVersion::~TVersion(){
	if (this->fFileList != nullptr){
		for (TFile_list::iterator it = this->fFileList->begin(); it != this->fFileList->end(); it++)
			it->reset();
		this->fFileList->clear();
		this->fFileList.reset();
		this->fFileList = nullptr;
	}
}

void TVersion::addFile(TFile_ptr& aFile){
	this->fFileList->push_back(move_TFile_ptr(aFile));
}

void TVersion::updateFile(TFile_ptr& aFile){
	for (TFile_list::iterator it = this->fFileList->begin(); it != this->fFileList->end(); it++)
		if (aFile->isEqualTo(**(it)))
			*it = move_TFile_ptr(aFile);
}

void TVersion::removeFile(TFile_ptr& aFile){
	for (TFile_list::iterator it = this->fFileList->begin(); it != this->fFileList->end(); it++)
		if (aFile->isEqualTo(**(it))){
			it->reset();
			this->fFileList->erase(it);
		}
}
#pragma endregion


//////////////////////////////////
//          TSession	        //
//////////////////////////////////
#pragma region "TSession" 
TSession::TSession(const int aKind, const string& aToken){
	this->fKind = aKind;
	this->fToken = make_string_ptr(aToken);
	this->fLastPing = time(nullptr);
	this->fVersion = new_TVersion_ptr(NO_VERSION, this->fLastPing);
}

TSession::~TSession(){
	if (this->fToken != nullptr)
		this->fToken.reset();

	if (this->fVersion != nullptr)
		this->fVersion.reset();

	this->fToken = nullptr;
	this->fVersion = nullptr;
}

void TSession::addFile(TFile_ptr& aFile){
	this->fVersion->addFile(move_TFile_ptr(aFile));
}

void TSession::updateFile(TFile_ptr& aFile){
	this->fVersion->updateFile(move_TFile_ptr(aFile));
}

void TSession::removeFile(TFile_ptr& aFile){
	this->fVersion->removeFile(move_TFile_ptr(aFile));
}
#pragma endregion