/*
 * Authors: Angelo Prudentino & Daniele Testa
 * Date: 14/09/2016
 * File: ClientController.cpp
 * Description: Objects to interface C++ core dll and WPF gui 
 *
 */

#include "ClientController.h"
#include "Utility.h"
#include <boost/filesystem.hpp>

using namespace boost::filesystem;


///////////////////////////////
//          UserFile         //
///////////////////////////////
UserFile::UserFile(String^ aFilePath){
	std::string fp = marshalString(aFilePath);
	path p(fp);

	this->fFilePath = unmarshalString(p.string());
	this->fFileName = unmarshalString(p.filename().string());
}


//////////////////////////////////
//         UserVersion	        //
//////////////////////////////////
UserVersion::UserVersion(String^ aVersionDate, const int aVersionID, List<UserFile^>^ aFileList){
	this->fVersionID = aVersionID;
	this->fVersionDate = aVersionDate;
	this->fFileList = aFileList;
}