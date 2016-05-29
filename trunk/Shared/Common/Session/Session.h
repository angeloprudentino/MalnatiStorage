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
#include <list>
#include <memory>
#include "Utility.h"

using namespace std;

#define NO_ID -1
//possible user sessions
#define NO_SESSION -1
#define UPDATE_SESSION 0
#define RESTORE_SESSION 1


//////////////////////////////////
//           TFile	            //
//////////////////////////////////
public class TFile{
	string_ptr fServerPathPrefix = nullptr;
	string_ptr fClientRelativePath = nullptr;
	time_t fFileDate;
	string_ptr fChecksum = nullptr;
	bool processed = false;
};
typedef list<TFile> TFile_list;
typedef unique_ptr<TFile_list> TFile_list_ptr;
typedef TFile_list::iterator TFileHandle;
#define new_TFile_list_ptr() std::make_unique<TFile_list_ptr>()
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
};


//////////////////////////////////
//          TSession	        //
//////////////////////////////////
public class TSession{
private:
	string_ptr fToken = nullptr;
	int fKind = NO_SESSION;
	time_t fLastPing;
	TVersion fVersion;
};

