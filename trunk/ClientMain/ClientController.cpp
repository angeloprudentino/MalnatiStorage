/*
 * Author: Angelo Prudentino
 * Date: 14/09/2016
 * File: ClientController.cpp
 * Description: this file contains all classes implementing
 *              client-side core logic
 *
 */

#include "ClientController.h


//////////////////////////////////
//          Version	            //
//////////////////////////////////
Version::Version(const String^ aVersionDate, const int aVersionID){ 
	this->fVersionID = aVersionID; 
	this->fVersionDate = (String^)aVersionDate; 
}


///////////////////////////////
//          File		     //
///////////////////////////////
File::File(const String^ aFilePath){

}
