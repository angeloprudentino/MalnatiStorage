/*
 * Author: Angelo Prudentino
 * Date: 14/09/2016
 * File: StorageClientCore.cpp
 * Description: this file contains all classes implementing
 *              client-side core logic
 *
 */


#include "StorageClientCore.h"
#include "Utility.h"

StorageClientCore::StorageClientCore(StorageClientController^ aCallbackObj){
	this->fCore = new TStorageClient(aCallbackObj);
}

StorageClientCore::~StorageClientCore(){
	delete this->fCore;
}

const bool StorageClientCore::issueRequest(UserRequest aRequest){

	return true;
}