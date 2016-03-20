/*
 * Author: Angelo Prudentino
 * Date: 06/03/2016
 * File: Message.h
 * Description: this file contains all classes implemented
 *              messages exchanged between client and server
 *
 */
#pragma once

#include <string>
#include <vector>

#include "ServerSocket.h"

using namespace std;

#define MSG_END "END_MSG\n\n"

//Message ids
#define NO_ID -1
#define USER_REG_REQ_ID 0
#define USER_REG_REPLY_ID 1
#define UPDATE_START_REQ_ID 2
#define UPDATE_START_REPLY_ID 3
#define ADD_NEW_FILE_ID 4
#define UPDATE_FILE_ID 5
#define REMOVE_FILE_ID 6
#define FILE_ACK_ID 7
#define UPDATE_STOP_REQ_ID 8
#define UPDATE_STOP_REPLY_ID 9
#define GET_VERSIONS_REQ_ID 10
#define GET_VERSIONS_REPLY_ID 11
#define RESTORE_VER_REQ_ID 12
#define RESTORE_VER_REPLY_ID 13
#define RESTORE_FILE_ID 14
#define RESTORE_FILE_ACK_ID 15
#define RESTORE_STOP_ID 16
#define PING_REQ_ID 17
#define PING_REPLY_ID 18

//Message names
const bool isValidMessage(const string aName);
const string getMessageName(const int aIndex);

//////////////////////////////////////
//       EMessageException	        //
//////////////////////////////////////
public class EMessageException : public std::exception{
private:
	string fMessage;
public:
	EMessageException(const string aMsg){ this->fMessage = aMsg; }
	const string getMessage(){ return this->fMessage; }
};


//////////////////////////////////////
//         TBaseMessage	            //
//////////////////////////////////////
public class TBaseMessage{
protected:
	int fID;
	vector<string> fItems;

public:
	TBaseMessage();
	TBaseMessage(const string aMsg);
	const string getName();

	virtual const string encodeMessage();
	virtual void decodeMessage(const string aMsg);
	virtual void processMessage(TServerSockController* aController);

	// getters
	const int getID(){ return this->fID; };
	const vector<string> getTokens(){ return this->fItems; };
};


////////////////////////////////////////
//      TUserRegistrReqMessage        //
////////////////////////////////////////
public class TUserRegistrReqMessage : public TBaseMessage {
private:
	string fUser;
	string fPass;

	void initFields();
public:
	TUserRegistrReqMessage(TBaseMessage& aBase);
	TUserRegistrReqMessage(const string aUser, const string aPass);
	const string encodeMessage();
	void decodeMessage(const string aMsg);
	void processMessage(TServerSockController* aController);

	//getters
	const string getUser(){ return this->fUser; }
	const string getPass(){ return this->fPass; }
};


////////////////////////////////////////
//      TUserRegistrReplyMessage      //
////////////////////////////////////////
public class TUserRegistrReplyMessage : public TBaseMessage {
private:
	bool fResp;

	void initFields();
public:
	TUserRegistrReplyMessage(TBaseMessage& aBase);
	TUserRegistrReplyMessage(const bool aResp);
	const string encodeMessage();
	void decodeMessage(const string aMsg);
	void processMessage(TServerSockController* aController);

	//getters
	const bool getResp(){ return this->fResp; }
};


////////////////////////////////////////
//      TUpdateStartReqMessage        //
////////////////////////////////////////
public class TUpdateStartReqMessage : public TBaseMessage {
private:
	string fUser;
	string fPass;

	void initFields();
public:
	TUpdateStartReqMessage(TBaseMessage& aBase);
	TUpdateStartReqMessage(const string aUser, const string aPass);
	const string encodeMessage();
	void decodeMessage(const string aMsg);
	void processMessage(TServerSockController* aController);

	//getters
	const string getUser(){ return this->fUser; }
	const string getPass(){ return this->fPass; }
};


////////////////////////////////////////
//      TUpdateStartReplyMessage      //
////////////////////////////////////////
public class TUpdateStartReplyMessage : public TBaseMessage {
private:
	bool fResp;
	string fToken;

	void initFields();
public:
	TUpdateStartReplyMessage(TBaseMessage& aBase);
	TUpdateStartReplyMessage(const bool aResp, const bool aToken);
	const string encodeMessage();
	void decodeMessage(const string aMsg);
	void processMessage(TServerSockController* aController);

	//getters
	const bool getResp(){ return this->fResp; }
	const string getToken(){ return this->fToken; }
};


////////////////////////////////////////
//        TAddNewFileMessage          //
////////////////////////////////////////
public class TAddNewFileMessage : public TBaseMessage {
private:
	string fToken;
	string fFilePath;
	string fChecksum;
	time_t fFileDate;
	//TODO: add file content

	void initFields();
public:
	TAddNewFileMessage(TBaseMessage& aBase);
	TAddNewFileMessage(const bool aToken, string aFilePath);
	const string encodeMessage();
	void decodeMessage(const string aMsg);
	void processMessage(TServerSockController* aController);

	//getters
	const string getToken(){ return this->fToken; }
	const string getFilePath(){ return this->fFilePath; }
	const string getChecksum(){ return this->fChecksum; }
	const time_t getFileDate(){ return this->fFileDate; }
};


////////////////////////////////////////
//        TUpdateFileMessage          //
////////////////////////////////////////
public class TUpdateFileMessage : public TBaseMessage {
private:
	string fToken;
	string fFilePath;
	string fChecksum;
	time_t fFileDate;
	//TODO: add file content

	void initFields();
public:
	TUpdateFileMessage(TBaseMessage& aBase);
	TUpdateFileMessage(const bool aToken, string aFilePath);
	const string encodeMessage();
	void decodeMessage(const string aMsg);
	void processMessage(TServerSockController* aController);

	//getters
	const string getToken(){ return this->fToken; }
	const string getFilePath(){ return this->fFilePath; }
	const string getChecksum(){ return this->fChecksum; }
	const time_t getFileDate(){ return this->fFileDate; }
};


////////////////////////////////////////
//        TRemoveFileMessage          //
////////////////////////////////////////
public class TRemoveFileMessage : public TBaseMessage {
private:
	string fToken;
	string fFilePath;

	void initFields();
public:
	TRemoveFileMessage(TBaseMessage& aBase);
	TRemoveFileMessage(const string aToken, string aFilePath);
	const string encodeMessage();
	void decodeMessage(const string aMsg);
	void processMessage(TServerSockController* aController);

	//getters
	const string getToken(){ return this->fToken; }
	const string getFilePath(){ return this->fFilePath; }
};


///////////////////////////////////
//        TFileAckMessage        //
///////////////////////////////////
public class TFileAckMessage : public TBaseMessage {
private:
	bool fResp;
	string fFilePath;

	void initFields();
public:
	TFileAckMessage(TBaseMessage& aBase);
	TFileAckMessage(const bool aResp, const bool aFilePath);
	const string encodeMessage();
	void decodeMessage(const string aMsg);
	void processMessage(TServerSockController* aController);

	//getters
	const bool getResp(){ return this->fResp; }
	const string getFilePath(){ return this->fFilePath; }
};


////////////////////////////////////////
//      TUpdateStopReqMessage         //
////////////////////////////////////////
public class TUpdateStopReqMessage : public TBaseMessage {
private:
	string fToken;

	void initFields();
public:
	TUpdateStopReqMessage(TBaseMessage& aBase);
	TUpdateStopReqMessage(const string aToken);
	const string encodeMessage();
	void decodeMessage(const string aMsg);
	void processMessage(TServerSockController* aController);

	//getters
	const string getToken(){ return this->fToken; }
};


////////////////////////////////////////
//      TUpdateStopReplyMessage       //
////////////////////////////////////////
public class TUpdateStopReplyMessage : public TBaseMessage {
private:
	bool fResp;
	unsigned int fVersion;
	time_t fTime;

	void initFields();
public:
	TUpdateStopReplyMessage(TBaseMessage& aBase);
	TUpdateStopReplyMessage(const bool aResp, unsigned int aVersion, time_t aTime);
	const string encodeMessage();
	void decodeMessage(const string aMsg);
	void processMessage(TServerSockController* aController);

	//getters
	const bool getResp(){ return this->fResp; }
	const unsigned int getVersion(){ return this->fVersion; }
	const time_t getTime(){ return this->fTime; }
};


////////////////////////////////////////
//      TGetVersionsReqMessage        //
////////////////////////////////////////
public class TGetVersionsReqMessage : public TBaseMessage {
private:
	string fUser;
	string fPass;

	void initFields();
public:
	TGetVersionsReqMessage(TBaseMessage& aBase);
	TGetVersionsReqMessage(const string aUser, const string aPass);
	const string encodeMessage();
	void decodeMessage(const string aMsg);
	void processMessage(TServerSockController* aController);

	//getters
	const string getUser(){ return this->fUser; }
	const string getPass(){ return this->fPass; }
};


////////////////////////////////////////
//      TGetVersionsReplyMessage      //
////////////////////////////////////////
public class TGetVersionsReplyMessage : public TBaseMessage {
private:
	unsigned int fTotVersions;
	unsigned int fOldestVersion;
	unsigned int fLastVersion;
	map<unsigned int, time_t> fVersions;

	void initFields();
public:
	TGetVersionsReplyMessage(TBaseMessage& aBase);
	TGetVersionsReplyMessage(const unsigned int aTotVersions, const unsigned int aOldestVersion, const unsigned int aLastVersion, map<unsigned int, time_t> aVersions);
	const string encodeMessage();
	void decodeMessage(const string aMsg);
	void processMessage(TServerSockController* aController);

	//getters
	const unsigned int getTotVersions(){ return this->fTotVersions; }
	const unsigned int getOldestVersion(){ return this->fOldestVersion; }
	const unsigned int getLastVersion(){ return this->fLastVersion; }
	const time_t getVersion(const unsigned int aVersion){ return this->fVersions.at(aVersion); }
};


////////////////////////////////////////
//       TRestoreVerReqMessage        //
////////////////////////////////////////
public class TRestoreVerReqMessage : public TBaseMessage {
private:
	string fUser;
	string fPass;
	unsigned int fVersion;

	void initFields();
public:
	TRestoreVerReqMessage(TBaseMessage& aBase);
	TRestoreVerReqMessage(const string aUser, const string aPass, const unsigned int aVersion);
	const string encodeMessage();
	void decodeMessage(const string aMsg);
	void processMessage(TServerSockController* aController);

	//getters
	const string getUser(){ return this->fUser; }
	const string getPass(){ return this->fPass; }
	const unsigned int getVersion(){ return this->fVersion; }
};


////////////////////////////////////////
//      TRestoreVerReplyMessage       //
////////////////////////////////////////
public class TRestoreVerReplyMessage : public TBaseMessage {
private:
	bool fResp;
	string fToken;

	void initFields();
public:
	TRestoreVerReplyMessage(TBaseMessage& aBase);
	TRestoreVerReplyMessage(const bool aResp, const bool aToken);
	const string encodeMessage();
	void decodeMessage(const string aMsg);
	void processMessage(TServerSockController* aController);

	//getters
	const bool getResp(){ return this->fResp; }
	const string getToken(){ return this->fToken; }
};


////////////////////////////////////////
//        TRestoreFileMessage         //
////////////////////////////////////////
public class TRestoreFileMessage : public TBaseMessage {
private:
	string fFilePath;
	string fChecksum;
	time_t fFileDate;
	//TODO: add file content

	void initFields();
public:
	TRestoreFileMessage(TBaseMessage& aBase);
	TRestoreFileMessage(string aFilePath);
	const string encodeMessage();
	void decodeMessage(const string aMsg);
	void processMessage(TServerSockController* aController);

	//getters
	const string getFilePath(){ return this->fFilePath; }
	const string getChecksum(){ return this->fChecksum; }
	const time_t getFileDate(){ return this->fFileDate; }
};


//////////////////////////////////////////
//        TRestoreFileAckMessage        //
//////////////////////////////////////////
public class TRestoreFileAckMessage : public TBaseMessage {
private:
	string fToken;
	bool fResp;
	string fFilePath;

	void initFields();
public:
	TRestoreFileAckMessage(TBaseMessage& aBase);
	TRestoreFileAckMessage(const string aToken, const bool aResp, const bool aFilePath);
	const string encodeMessage();
	void decodeMessage(const string aMsg);
	void processMessage(TServerSockController* aController);

	//getters
	const string getToken(){ return this->fToken; }
	const bool getResp(){ return this->fResp; }
	const string getFilePath(){ return this->fFilePath; }
};


////////////////////////////////////
//      TRestoreStopMessage       //
////////////////////////////////////
public class TRestoreStopMessage : public TBaseMessage {
private:
	unsigned int fVersion;
	time_t fTime;

	void initFields();
public:
	TRestoreStopMessage(TBaseMessage& aBase);
	TRestoreStopMessage(unsigned int aVersion, time_t aTime);
	const string encodeMessage();
	void decodeMessage(const string aMsg);
	void processMessage(TServerSockController* aController);

	//getters
	const unsigned int getVersion(){ return this->fVersion; }
	const time_t getTime(){ return this->fTime; }
};


//////////////////////////////////////
//        TPingReqMessage           //
//////////////////////////////////////
public class TPingReqMessage : public TBaseMessage {
private:
	time_t fTime;

	void initFields();
public:
	TPingReqMessage(TBaseMessage& aBase);
	TPingReqMessage();
	const string encodeMessage();
	void decodeMessage(const string aMsg);
	void processMessage(TServerSockController* aController);

	//getters
	const time_t getTime(){ return this->fTime; }
};


//////////////////////////////////////
//        TPingReplyMessage         //
//////////////////////////////////////
public class TPingReplyMessage : public TBaseMessage {
private:
	time_t fTime;

	void initFields();
public:
	TPingReplyMessage(TBaseMessage& aBase);
	TPingReplyMessage();
	const string encodeMessage();
	void decodeMessage(const string aMsg);
	void processMessage(TServerSockController* aController);

	//getters
	const time_t getTime(){ return this->fTime; }
};