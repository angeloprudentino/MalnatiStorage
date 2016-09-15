/*
 * Author: Angelo Prudentino
 * Date: 14/09/2016
 * File: ClientController.cpp
 * Description: this file contains all classes implementing
 *              client-side core logic
 *
 */

#include "ClientController.h"
#include "Utility.h"
#include <boost/filesystem.hpp>

using namespace boost::filesystem;


//////////////////////////////////
//         UserVersion	        //
//////////////////////////////////
UserVersion::UserVersion(const String^ aVersionDate, const int aVersionID){
	this->fVersionID = aVersionID; 
	this->fVersionDate = (String^)aVersionDate; 
}

///////////////////////////////
//          File		     //
///////////////////////////////
File::File(const String^ aFilePath){
	std::string fp = marshalString((String^)aFilePath);
	path p(fp);

	this->fFilePath = unmarshalString(p.string());
	this->fFileName = unmarshalString(p.filename().string());
}


//////////////////////////////////
//        RegistrRequest	    //
//////////////////////////////////
RegistrRequest::RegistrRequest(const String^ aUser, const String^ aPass, const String^ aPath){
	this->fID = REGISTR_REQ;
	this->fUser = (String^)aUser;
	this->fPass = (String^)aPass;
	this->fPath = (String^)aPath;
}


//////////////////////////////////
//         LoginRequest	        //
//////////////////////////////////
LoginRequest::LoginRequest(const String^ aUser, const String^ aPass){
	this->fID = LOGIN_REQ;
	this->fUser = (String^)aUser;
	this->fPass = (String^)aPass;
}


//////////////////////////////////
//         UpdateRequest	    //
//////////////////////////////////
UpdateRequest::UpdateRequest(const String^ aUser, const String^ aPass){
	this->fID = UPDATE_REQ;
	this->fUser = (String^)aUser;
	this->fPass = (String^)aPass;
}


//////////////////////////////////
//         GetVerRequest	    //
//////////////////////////////////
GetVerRequest::GetVerRequest(const String^ aUser, const String^ aPass){
	this->fID = GET_VERSIONS_REQ;
	this->fUser = (String^)aUser;
	this->fPass = (String^)aPass;
}


//////////////////////////////////
//        RestoreRequest	    //
//////////////////////////////////
RestoreRequest::RestoreRequest(const String^ aUser, const String^ aPass, const int aVersion){
	this->fID = RESTORE_REQ;
	this->fUser = (String^)aUser;
	this->fPass = (String^)aPass;
	this->fVersion = aVersion;
}
