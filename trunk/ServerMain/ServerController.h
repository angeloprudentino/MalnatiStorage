/*
 * Author: Angelo Prudentino
 * Date: 06/08/2016
 * File: ServerController.h
 * Description: interface definition for server controllers
 *
 */
#pragma once
#include <string>

using namespace std;

//////////////////////////////////////
//		IManagedServerController	//
//////////////////////////////////////
// Server controller interface used in managed code
public interface class IManagedServerController{
public:
	void onServerLog(const string& aClassName, const string& aFuncName, const string& aMsg);
	void onServerWarning(const string& aClassName, const string& aFuncName, const string& aMsg);
	void onServerError(const string& aClassName, const string& aFuncName, const string& aMsg);
	void onServerCriticalError(const string& aClassName, const string& aFuncName, const string& aMsg);
};


//////////////////////////////////////
//      IServerBaseController	    //
//////////////////////////////////////
public class IServerBaseController{
public:
	virtual void onServerLog(const string& aClassName, const string& aFuncName, const string& aMsg) = 0;
	virtual void onServerWarning(const string& aClassName, const string& aFuncName, const string& aMsg) = 0;
	virtual void onServerError(const string& aClassName, const string& aFuncName, const string& aMsg) = 0;
	virtual void onServerCriticalError(const string& aClassName, const string& aFuncName, const string& aMsg) = 0;
};
#define doServerLog(ptr, aClassName, aFuncName, aMsg) if(ptr!=nullptr) ptr->onServerLog(aClassName, aFuncName, aMsg);
#define doServerWarning(ptr, aClassName, aFuncName, aMsg) if(ptr!=nullptr) ptr->onServerWarning(aClassName, aFuncName, aMsg);
#define doServerError(ptr, aClassName, aFuncName, aMsg) if(ptr!=nullptr) ptr->onServerError(aClassName, aFuncName, aMsg);
#define doServerCriticalError(ptr, aClassName, aFuncName, aMsg) if(ptr!=nullptr) ptr->onServerCriticalError(aClassName, aFuncName, aMsg);
