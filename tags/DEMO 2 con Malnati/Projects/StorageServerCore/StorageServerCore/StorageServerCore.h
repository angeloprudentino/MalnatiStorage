/*
 * Authors: Angelo Prudentino & Daniele Testa
 * Date: 27/09/2016
 * File: StorageServerCore.h
 * Description: This is the entry-point class of the server dll
 *
 */
#pragma once
#include "StorageServer.h"

using namespace System;
using namespace System::Threading;

public ref class StorageServerCore{
private:
    Thread^ fServerThread = nullptr;
	TStorageServer* fCore = nullptr;

	void start(Object^ aData);
	void stop();

public:
	StorageServerCore();
	~StorageServerCore();
};
