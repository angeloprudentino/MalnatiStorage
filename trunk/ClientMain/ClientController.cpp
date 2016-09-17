/*
 * Author: Angelo Prudentino
 * Date: 14/09/2016
 * File: ClientController.cpp
 * Description: 
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
//          UserFile         //
///////////////////////////////
UserFile::UserFile(const String^ aFilePath){
	std::string fp = marshalString((String^)aFilePath);
	path p(fp);

	this->fFilePath = unmarshalString(p.string());
	this->fFileName = unmarshalString(p.filename().string());
}