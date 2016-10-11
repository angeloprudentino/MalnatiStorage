/*
 * Authors: Angelo Prudentino & Daniele Testa
 * Date: 16/09/2016
 * File: ClientRequests.cpp
 * Description: Objects to interface C++ core dll and WPF gui 
 *
 */

#include "ClientRequests.h"

using namespace boost;

//////////////////////////////////
//        RegistrRequest	    //
//////////////////////////////////
RegistrRequest::RegistrRequest(String^ aUser, String^ aPass, String^ aPath){
	this->fID = REGISTR_REQ;
	this->fUser = aUser;
	this->fPass = aPass;
	this->fPath = aPath;
}


//////////////////////////////////
//         LoginRequest	        //
//////////////////////////////////
LoginRequest::LoginRequest(String^ aUser, String^ aPass){
	this->fID = LOGIN_REQ;
	this->fUser = aUser;
	this->fPass = aPass;
}


//////////////////////////////////
//         UpdateRequest	    //
//////////////////////////////////
UpdateRequest::UpdateRequest(String^ aUser, String^ aPass, String^ aPath){
	this->fID = UPDATE_REQ;
	this->fUser = aUser;
	this->fPass = aPass;
	this->fPath = aPath;
}


//////////////////////////////////
//         GetVerRequest	    //
//////////////////////////////////
GetVerRequest::GetVerRequest(String^ aUser, String^ aPass){
	this->fID = GET_VERSIONS_REQ;
	this->fUser = aUser;
	this->fPass = aPass;
}


//////////////////////////////////
//        RestoreRequest	    //
//////////////////////////////////
RestoreRequest::RestoreRequest(String^ aUser, String^ aPass, const int aVersion, String^ aDestPath){
	this->fID = RESTORE_REQ;
	this->fUser = aUser;
	this->fPass = aPass;
	this->fVersion = aVersion;
	this->fDestPath = aDestPath;
}


//////////////////////////////////
//         PingRequest	        //
//////////////////////////////////
PingRequest::PingRequest(String^ aToken){
	this->fID = PING_REQ;
	this->fToken = aToken;
}


/////////////////////////////////////
//         RequestsQueue           //
/////////////////////////////////////
RequestsQueue::RequestsQueue() {
	this->fMustExit.store(false, boost::memory_order_release);
};

RequestsQueue::~RequestsQueue(){
	this->fMustExit.store(true, boost::memory_order_release);
	this->fCond.notify_all();

	//unique_lock<mutex> lock(this->fMutex);

	//int size = (int)this->fQueue.size();
	//for (int i = 0; i < size; i++){
	//	TMessageContainer_ptr temp = move_TMessageContainer_ptr(this->fQueue.front());
	//	this->fQueue.pop();
	//	temp.reset();
	//}
}

bool RequestsQueue::isEmpty(){
	unique_lock<mutex> lock(this->fMutex);
	return this->fQueue.empty();
}

UserRequest^ RequestsQueue::popRequest(){
	unique_lock<mutex> lock(this->fMutex);

	while (this->fQueue.empty() && !this->fMustExit.load(boost::memory_order_acquire)){
		this->fCond.wait(lock);
	}

	//return null object if should exit
	if (this->fMustExit.load(boost::memory_order_acquire)){
		//empty queue before leaving
		int size = (int)this->fQueue.size();
		for (int i = 0; i < size; i++){
			//UserRequest^ temp = this->fQueue.front();
			this->fQueue.pop();
			//delete temp;
		}

		return gcnew UserRequest();
	}

	UserRequest^ res = this->fQueue.front();
	this->fQueue.pop();

	return res;
}

void RequestsQueue::pushRequest(UserRequest^ aReq){
	//do not push if should exit
	if (this->fMustExit.load(boost::memory_order_acquire))
		return;

	try{
		unique_lock<mutex> lock(this->fMutex);

		this->fQueue.push(aReq);
	}
	catch (...){
		//try-catch block to automatically release lock
	}

	this->fCond.notify_one();
}