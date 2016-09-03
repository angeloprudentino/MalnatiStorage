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

//////////////////////////////////
//           TFile	            //
//////////////////////////////////
#pragma region "TFile"
TFile::TFile(const string& aUser, const int aVersion, const string& aClientRelativePath, const time_t& aLastMod){
	this->fVersion = aVersion;
	this->fLastMod = aLastMod;
	this->fServerPathPrefix = make_string_ptr(buildServerPathPrefix(aUser, aVersion));
	this->fClientRelativePath = make_string_ptr(aClientRelativePath);
}

TFile::TFile(const string& aServerPathPrefix, const string& aClientRelativePath, const time_t& aLastMod){
	this->fLastMod = aLastMod;
	this->fServerPathPrefix = make_string_ptr(aServerPathPrefix);
	this->fClientRelativePath = make_string_ptr(aClientRelativePath);
}

TFile::~TFile(){
	if (this->fServerPathPrefix != nullptr)
		this->fServerPathPrefix.reset();

	if (this->fClientRelativePath != nullptr)
		this->fClientRelativePath.reset();

	this->fServerPathPrefix = nullptr;
	this->fClientRelativePath = nullptr;
}

const bool TFile::isEqualTo(const TFile& aFile){
	return *(this->fClientRelativePath) == *(aFile.fClientRelativePath);
}
#pragma endregion


//////////////////////////////////
//          TVersion	        //
//////////////////////////////////
#pragma region "TVersion"
TVersion::TVersion(const int aId, const time_t aVersionDate){
	this->fId = aId;
	this->fVersionDate = aVersionDate;
	//this->fNext = this->fFileList.begin();
}

TVersion::~TVersion(){
	for (TFile_list::iterator it = this->fFileList.begin(); it != this->fFileList.end(); it++)
		it->reset();
	this->fFileList.clear();
}

void TVersion::addFile(TFile_ptr& aFile){
	int size = (int)this->fFileList.size();
	this->fFileList.push_back(move_TFile_ptr(aFile));
	if (size == 0)
		this->fNext = this->fFileList.begin();
}

const bool TVersion::updateFile(TFile_ptr& aFile){
	for (TFile_list::iterator it = this->fFileList.begin(); it != this->fFileList.end(); it++)
		if (*(it) != nullptr && aFile->isEqualTo(**(it))){
			it->reset();
			*it = move_TFile_ptr(aFile);
			return true;
		}

	return false;
}

void TVersion::removeFile(TFile_ptr& aFile){
	for (TFile_list::iterator it = this->fFileList.begin(); it != this->fFileList.end(); it++)
		if (*(it) != nullptr && aFile->isEqualTo(**(it))){
			it->reset();
			this->fFileList.erase(it);
			break;
		}

	aFile.reset();
}

void TVersion::terminateWithSuccess(){
	this->fId++;

	for (TFile_list::iterator it = this->fFileList.begin(); it != this->fFileList.end(); it++)
		if (*it != nullptr)
			(*it)->setVersion(this->fId);

	this->fVersionDate = time(NULL);
}

TFile_ptr TVersion::getNextFile(){
	if (this->fNext != this->fFileList.end()){		
		TFile_ptr f = copy_TFile_ptr((*this->fNext)->getServerPathPrefix(), (*this->fNext)->getClientRelativePath(), (*this->fNext)->getLastMod());
		f->setVersion((*this->fNext)->getVersion());
		return move_TFile_ptr(f);
	}
	else
		return nullptr;
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

const bool TSession::updateFile(TFile_ptr& aFile){
	return this->fVersion->updateFile(move_TFile_ptr(aFile));
}

void TSession::removeFile(TFile_ptr& aFile){
	this->fVersion->removeFile(move_TFile_ptr(aFile));
}

TVersion_ptr TSession::terminateWithSuccess(const bool aIsRestore){
	if (!aIsRestore)
		this->fVersion->terminateWithSuccess();

	return move_TVersion_ptr(this->fVersion);
}

const bool TSession::purge(){
	string u = getUserFromToken(this->fToken->c_str());
	int v = this->fVersion->getVersion();

	path p(buildServerPathPrefix(u, v));
	if (exists(p)){
		boost::system::error_code ec;
		remove_all(p, ec);
		if (ec)
			return false;
		else
			return true;
	}

	return true;
}
#pragma endregion