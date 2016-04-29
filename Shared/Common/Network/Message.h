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
#include <map>

#include "Utility.h"

using namespace std;

#define END_MSG "END_MSG"

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

//Message utilities
const bool isValidMessage(const string aName);
const bool isValidMessageID(const int aID);
const string getMessageName(const int aIndex);

//////////////////////////////////////
//       EMessageException	        //
//////////////////////////////////////
public class EMessageException : public EBaseException {
public:
	EMessageException(const string aMsg) : EBaseException(aMsg){}
};


//////////////////////////////////////
//         TBaseMessage	            //
//////////////////////////////////////
public class TBaseMessage{
private:
	void decodeMessageID();

protected:
	int fID;
	string_ptr fEncodedMsg = NULL;
	vector<string_ptr> fItems;

public:
	TBaseMessage();
	TBaseMessage(const string_ptr aMsg);

	virtual const string_ptr encodeMessage();
	virtual void decodeMessage();

	// getters
	const int getID(){ return this->fID; };
	const vector<string_ptr> getTokens(){ return this->fItems; };
	const string_ptr getMsg(){ return this->fEncodedMsg; };
	const string getName(){	return getMessageName(this->fID); };
};


////////////////////////////////////////
//      TUserRegistrReqMessage        //
////////////////////////////////////////
public class TUserRegistrReqMessage : public TBaseMessage {
private:
	string_ptr fUser;
	string_ptr fPass;

public:
	TUserRegistrReqMessage(TBaseMessage& aBase);
	TUserRegistrReqMessage(const string aUser, const string aPass);
	const string_ptr encodeMessage();
	void decodeMessage();

	//getters
	const string getUser(){ return *(this->fUser); }
	const string getPass(){ return *(this->fPass); }
};


////////////////////////////////////////
//      TUserRegistrReplyMessage      //
////////////////////////////////////////
public class TUserRegistrReplyMessage : public TBaseMessage {
private:
	bool fResp;

public:
	TUserRegistrReplyMessage(TBaseMessage& aBase);
	TUserRegistrReplyMessage(const bool aResp);
	const string_ptr encodeMessage();
	void decodeMessage();

	//getters
	const bool getResp(){ return this->fResp; }
};


////////////////////////////////////////
//      TUpdateStartReqMessage        //
////////////////////////////////////////
public class TUpdateStartReqMessage : public TBaseMessage {
private:
	string_ptr fUser;
	string_ptr fPass;

public:
	TUpdateStartReqMessage(TBaseMessage& aBase);
	TUpdateStartReqMessage(const string aUser, const string aPass);
	const string_ptr encodeMessage();
	void decodeMessage();

	//getters
	const string getUser(){ return *(this->fUser); }
	const string getPass(){ return *(this->fPass); }
};


////////////////////////////////////////
//      TUpdateStartReplyMessage      //
////////////////////////////////////////
public class TUpdateStartReplyMessage : public TBaseMessage {
private:
	bool fResp;
	string_ptr fToken;

public:
	TUpdateStartReplyMessage(TBaseMessage& aBase);
	TUpdateStartReplyMessage(const bool aResp, const string aToken);
	const string_ptr encodeMessage();
	void decodeMessage();

	//getters
	const bool getResp(){ return this->fResp; }
	const string getToken(){ return *(this->fToken); }
};


////////////////////////////////////////
//        TAddNewFileMessage          //
////////////////////////////////////////
public class TAddNewFileMessage : public TBaseMessage {
private:
	string_ptr fToken;
	string_ptr fFilePath;
	time_t fFileDate;
	string_ptr fChecksum;
	string_ptr fFileContent;

public:
	TAddNewFileMessage(TBaseMessage& aBase);
	TAddNewFileMessage(const string aToken, string aFilePath);
	const string_ptr encodeMessage();
	void decodeMessage();

	//getters
	const string getToken(){ return *(this->fToken); }
	const string getFilePath(){ return *(this->fFilePath); }
	const time_t getFileDate(){ return this->fFileDate; }
	const string getChecksum(){ return *(this->fChecksum); }
	const string getFileContent(){ return *(this->fFileContent); }
};


////////////////////////////////////////
//        TUpdateFileMessage          //
////////////////////////////////////////
public class TUpdateFileMessage : public TBaseMessage {
private:
	string_ptr fToken;
	string_ptr fFilePath;
	time_t fFileDate;
	string_ptr fChecksum;
	string_ptr fFileContent;

public:
	TUpdateFileMessage(TBaseMessage& aBase);
	TUpdateFileMessage(const string aToken, string aFilePath);
	const string_ptr encodeMessage();
	void decodeMessage();

	//getters
	const string getToken(){ return *(this->fToken); }
	const string getFilePath(){ return *(this->fFilePath); }
	const time_t getFileDate(){ return this->fFileDate; }
	const string getChecksum(){ return *(this->fChecksum); }
	const string getFileContent(){ return *(this->fFileContent); }
};


////////////////////////////////////////
//        TRemoveFileMessage          //
////////////////////////////////////////
public class TRemoveFileMessage : public TBaseMessage {
private:
	string_ptr fToken;
	string_ptr fFilePath;

public:
	TRemoveFileMessage(TBaseMessage& aBase);
	TRemoveFileMessage(const string aToken, string aFilePath);
	const string_ptr encodeMessage();
	void decodeMessage();

	//getters
	const string getToken(){ return *(this->fToken); }
	const string getFilePath(){ return *(this->fFilePath); }
};


///////////////////////////////////
//        TFileAckMessage        //
///////////////////////////////////
public class TFileAckMessage : public TBaseMessage {
private:
	bool fResp;
	string_ptr fFilePath;

public:
	TFileAckMessage(TBaseMessage& aBase);
	TFileAckMessage(const bool aResp, const string aFilePath);
	const string_ptr encodeMessage();
	void decodeMessage();

	//getters
	const bool getResp(){ return this->fResp; }
	const string getFilePath(){ return *(this->fFilePath); }
};


////////////////////////////////////////
//      TUpdateStopReqMessage         //
////////////////////////////////////////
public class TUpdateStopReqMessage : public TBaseMessage {
private:
	string_ptr fToken;

public:
	TUpdateStopReqMessage(TBaseMessage& aBase);
	TUpdateStopReqMessage(const string aToken);
	const string_ptr encodeMessage();
	void decodeMessage();

	//getters
	const string getToken(){ return *(this->fToken); }
};


////////////////////////////////////////
//      TUpdateStopReplyMessage       //
////////////////////////////////////////
public class TUpdateStopReplyMessage : public TBaseMessage {
private:
	bool fResp;
	unsigned int fVersion;
	time_t fTime;

public:
	TUpdateStopReplyMessage(TBaseMessage& aBase);
	TUpdateStopReplyMessage(const bool aResp, unsigned int aVersion, time_t aTime);
	const string_ptr encodeMessage();
	void decodeMessage();

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
	string_ptr fUser;
	string_ptr fPass;

public:
	TGetVersionsReqMessage(TBaseMessage& aBase);
	TGetVersionsReqMessage(const string aUser, const string aPass);
	const string_ptr encodeMessage();
	void decodeMessage();

	//getters
	const string getUser(){ return *(this->fUser); }
	const string getPass(){ return *(this->fPass); }
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

public:
	TGetVersionsReplyMessage(TBaseMessage& aBase);
	TGetVersionsReplyMessage(const unsigned int aTotVersions, const unsigned int aOldestVersion, const unsigned int aLastVersion, map<unsigned int, time_t> aVersions);
	const string_ptr encodeMessage();
	void decodeMessage();

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
	string_ptr fUser;
	string_ptr fPass;
	unsigned int fVersion;

public:
	TRestoreVerReqMessage(TBaseMessage& aBase);
	TRestoreVerReqMessage(const string aUser, const string aPass, const unsigned int aVersion);
	const string_ptr encodeMessage();
	void decodeMessage();

	//getters
	const string getUser(){ return *(this->fUser); }
	const string getPass(){ return *(this->fPass); }
	const unsigned int getVersion(){ return this->fVersion; }
};


////////////////////////////////////////
//      TRestoreVerReplyMessage       //
////////////////////////////////////////
public class TRestoreVerReplyMessage : public TBaseMessage {
private:
	bool fResp;
	string_ptr fToken;

public:
	TRestoreVerReplyMessage(TBaseMessage& aBase);
	TRestoreVerReplyMessage(const bool aResp, const string aToken);
	const string_ptr encodeMessage();
	void decodeMessage();

	//getters
	const bool getResp(){ return this->fResp; }
	const string getToken(){ return *(this->fToken); }
};


////////////////////////////////////////
//        TRestoreFileMessage         //
////////////////////////////////////////
public class TRestoreFileMessage : public TBaseMessage {
private:
	string_ptr fFilePath;
	time_t fFileDate;
	string_ptr fChecksum;
	string_ptr fFileContent;

public:
	TRestoreFileMessage(TBaseMessage& aBase);
	TRestoreFileMessage(string aFilePath);
	const string_ptr encodeMessage();
	void decodeMessage();

	//getters
	const string getFilePath(){ return *(this->fFilePath); }
	const string getChecksum(){ return *(this->fChecksum); }
	const string getFileContent(){ return *(this->fFileContent); }
	const time_t getFileDate(){ return this->fFileDate; }
};


//////////////////////////////////////////
//        TRestoreFileAckMessage        //
//////////////////////////////////////////
public class TRestoreFileAckMessage : public TBaseMessage {
private:
	string_ptr fToken;
	bool fResp;
	string_ptr fFilePath;

public:
	TRestoreFileAckMessage(TBaseMessage& aBase);
	TRestoreFileAckMessage(const string aToken, const bool aResp, const string aFilePath);
	const string_ptr encodeMessage();
	void decodeMessage();

	//getters
	const string getToken(){ return *(this->fToken); }
	const bool getResp(){ return this->fResp; }
	const string getFilePath(){ return *(this->fFilePath); }
};


////////////////////////////////////
//      TRestoreStopMessage       //
////////////////////////////////////
public class TRestoreStopMessage : public TBaseMessage {
private:
	unsigned int fVersion;
	time_t fTime;

public:
	TRestoreStopMessage(TBaseMessage& aBase);
	TRestoreStopMessage(unsigned int aVersion, time_t aTime);
	const string_ptr encodeMessage();
	void decodeMessage();

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

public:
	TPingReqMessage(TBaseMessage& aBase);
	TPingReqMessage();
	const string_ptr encodeMessage();
	void decodeMessage();

	//getters
	const time_t getTime(){ return this->fTime; }
};


//////////////////////////////////////
//        TPingReplyMessage         //
//////////////////////////////////////
public class TPingReplyMessage : public TBaseMessage {
private:
	time_t fTime;

public:
	TPingReplyMessage(TBaseMessage& aBase);
	TPingReplyMessage();
	const string_ptr encodeMessage();
	void decodeMessage();

	//getters
	const time_t getTime(){ return this->fTime; }
};