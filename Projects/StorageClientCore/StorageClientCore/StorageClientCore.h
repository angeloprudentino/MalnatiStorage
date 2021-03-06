/*
 * Authors: Angelo Prudentino & Daniele Testa
 * Date: 14/09/2016
 * File: StorageClientCore.h
 * Description: this file contains all classes implementing
 *              client-side core logic
 *
 */

#pragma once
#include "StorageClient.h"
#include "ClientController.h"


//////////////////////////////////
//      StorageClientCore	    //
//////////////////////////////////
public ref class StorageClientCore{
private:
	TStorageClient* fCore = nullptr;

public:
	StorageClientCore(StorageClientController^ aCallbackObj);
	~StorageClientCore();

	const bool issueRequest(UserRequest^ aRequest);
};

