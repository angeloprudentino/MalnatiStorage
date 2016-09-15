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

void StorageClientCore::test(){
	//try{
	//	tcp::endpoint ep(ip::address::from_string("127.0.0.1"), DEFAULT_PORT); // TCP socket for connecting to server
	//	fSock.connect(ep);

	//	string aUser = "pippo";
	//	string aPass = "pippo";

	//	bool res = this->_verifyUser(aUser, aPass);
	//	if (!res)
	//		res = this->registerUser(aUser, aPass);

	//	if (res){
	//		this->updateCurrentVersion(aUser, aPass);
	//		Sleep(1000);
	//		this->restoreVersion(aUser, aPass, 1, "Version_" + to_string(1));
	//		Sleep(1000);
	//		printAllVersions(aUser, aPass);
	//	}

	//	fSock.shutdown(socket_base::shutdown_both);
	//	fSock.close();
	//}
	//catch (...){

	//}

}