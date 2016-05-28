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
#include <memory>

#include "Utility.h"

using namespace std;

#define END_MSG "END_MSG"

//type definitions
typedef std::vector<string_ptr> string_vector;
typedef std::unique_ptr<string_vector> string_vector_ptr;
#define new_string_vector_ptr() std::make_unique<string_vector>()
#define move_string_vector_ptr(ptr) std::move(ptr)

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
	string_ptr fEncodedMsg = nullptr;
	string_vector_ptr fItems = nullptr;

public:
	TBaseMessage();
	TBaseMessage(string_ptr& aMsg);
	~TBaseMessage();

	virtual string_ptr encodeMessage();
	virtual void decodeMessage();

	// getters
	const int getID(){ return this->fID; };
	const string getName(){	return getMessageName(this->fID); };
	string_vector_ptr getTokens(){ return move_string_vector_ptr(this->fItems); };
	string_ptr getMsg(){ return move_string_ptr(this->fEncodedMsg); };
};
typedef std::unique_ptr<TBaseMessage> TBaseMessage_ptr;


////////////////////////////////////////
//      TUserRegistrReqMessage        //
////////////////////////////////////////
public class TUserRegistrReqMessage : public TBaseMessage {
private:
	string_ptr fUser;
	string_ptr fPass;

public:
	TUserRegistrReqMessage(TBaseMessage_ptr& aBase);
	TUserRegistrReqMessage(const string aUser, const string aPass);
	string_ptr encodeMessage();
	void decodeMessage();

	//getters
	const string getUser(){ return *(this->fUser); }
	const string getPass(){ return *(this->fPass); }
};
typedef std::unique_ptr<TUserRegistrReqMessage> TUserRegistrReqMessage_ptr;


////////////////////////////////////////
//      TUserRegistrReplyMessage      //
////////////////////////////////////////
public class TUserRegistrReplyMessage : public TBaseMessage {
private:
	bool fResp;

public:
	TUserRegistrReplyMessage(TBaseMessage_ptr& aBase);
	TUserRegistrReplyMessage(const bool aResp);
	string_ptr encodeMessage();
	void decodeMessage();

	//getters
	const bool getResp(){ return this->fResp; }
};
typedef std::unique_ptr<TUserRegistrReplyMessage> TUserRegistrReplyMessage_ptr;


////////////////////////////////////////
//      TUpdateStartReqMessage        //
////////////////////////////////////////
public class TUpdateStartReqMessage : public TBaseMessage {
private:
	string_ptr fUser;
	string_ptr fPass;

public:
	TUpdateStartReqMessage(TBaseMessage_ptr& aBase);
	TUpdateStartReqMessage(const string aUser, const string aPass);
	string_ptr encodeMessage();
	void decodeMessage();

	//getters
	const string getUser(){ return *(this->fUser); }
	const string getPass(){ return *(this->fPass); }
};
typedef std::unique_ptr<TUpdateStartReqMessage> TUpdateStartReqMessage_ptr;


////////////////////////////////////////
//      TUpdateStartReplyMessage      //
////////////////////////////////////////
public class TUpdateStartReplyMessage : public TBaseMessage {
private:
	bool fResp;
	string_ptr fToken;

public:
	TUpdateStartReplyMessage(TBaseMessage_ptr& aBase);
	TUpdateStartReplyMessage(const bool aResp, const string aToken);
	string_ptr encodeMessage();
	void decodeMessage();

	//getters
	const bool getResp(){ return this->fResp; }
	const string getToken(){ return *(this->fToken); }
};
typedef std::unique_ptr<TUpdateStartReplyMessage> TUpdateStartReplyMessage_ptr;


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
	TAddNewFileMessage(TBaseMessage_ptr& aBase);
	TAddNewFileMessage(const string aToken, string aFilePath);
	string_ptr encodeMessage();
	void decodeMessage();

	//getters
	const string getToken(){ return *(this->fToken); }
	const string getFilePath(){ return *(this->fFilePath); }
	const time_t getFileDate(){ return this->fFileDate; }
	const string getChecksum(){ return *(this->fChecksum); }
	const string getFileContent(){ return *(this->fFileContent); }
};
typedef std::unique_ptr<TAddNewFileMessage> TAddNewFileMessage_ptr;


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
	TUpdateFileMessage(TBaseMessage_ptr& aBase);
	TUpdateFileMessage(const string aToken, string aFilePath);
	string_ptr encodeMessage();
	void decodeMessage();

	//getters
	const string getToken(){ return *(this->fToken); }
	const string getFilePath(){ return *(this->fFilePath); }
	const time_t getFileDate(){ return this->fFileDate; }
	const string getChecksum(){ return *(this->fChecksum); }
	const string getFileContent(){ return *(this->fFileContent); }
};
typedef std::unique_ptr<TUpdateFileMessage> TUpdateFileMessage_ptr;


////////////////////////////////////////
//        TRemoveFileMessage          //
////////////////////////////////////////
public class TRemoveFileMessage : public TBaseMessage {
private:
	string_ptr fToken;
	string_ptr fFilePath;

public:
	TRemoveFileMessage(TBaseMessage_ptr& aBase);
	TRemoveFileMessage(const string aToken, string aFilePath);
	string_ptr encodeMessage();
	void decodeMessage();

	//getters
	const string getToken(){ return *(this->fToken); }
	const string getFilePath(){ return *(this->fFilePath); }
};
typedef std::unique_ptr<TRemoveFileMessage> TRemoveFileMessage_ptr;


///////////////////////////////////
//        TFileAckMessage        //
///////////////////////////////////
public class TFileAckMessage : public TBaseMessage {
private:
	bool fResp;
	string_ptr fFilePath;

public:
	TFileAckMessage(TBaseMessage_ptr& aBase);
	TFileAckMessage(const bool aResp, const string aFilePath);
	string_ptr encodeMessage();
	void decodeMessage();

	//getters
	const bool getResp(){ return this->fResp; }
	const string getFilePath(){ return *(this->fFilePath); }
};
typedef std::unique_ptr<TFileAckMessage> TFileAckMessage_ptr;


////////////////////////////////////////
//      TUpdateStopReqMessage         //
////////////////////////////////////////
public class TUpdateStopReqMessage : public TBaseMessage {
private:
	string_ptr fToken;

public:
	TUpdateStopReqMessage(TBaseMessage_ptr& aBase);
	TUpdateStopReqMessage(const string aToken);
	string_ptr encodeMessage();
	void decodeMessage();

	//getters
	const string getToken(){ return *(this->fToken); }
};
typedef std::unique_ptr<TUpdateStopReqMessage> TUpdateStopReqMessage_ptr;


////////////////////////////////////////
//      TUpdateStopReplyMessage       //
////////////////////////////////////////
public class TUpdateStopReplyMessage : public TBaseMessage {
private:
	bool fResp;
	unsigned int fVersion;
	time_t fTime;

public:
	TUpdateStopReplyMessage(TBaseMessage_ptr& aBase);
	TUpdateStopReplyMessage(const bool aResp, unsigned int aVersion, time_t aTime);
	string_ptr encodeMessage();
	void decodeMessage();

	//getters
	const bool getResp(){ return this->fResp; }
	const unsigned int getVersion(){ return this->fVersion; }
	const time_t getTime(){ return this->fTime; }
};
typedef std::unique_ptr<TUpdateStopReplyMessage> TUpdateStopReplyMessage_ptr;


////////////////////////////////////////
//      TGetVersionsReqMessage        //
////////////////////////////////////////
public class TGetVersionsReqMessage : public TBaseMessage {
private:
	string_ptr fUser;
	string_ptr fPass;

public:
	TGetVersionsReqMessage(TBaseMessage_ptr& aBase);
	TGetVersionsReqMessage(const string aUser, const string aPass);
	string_ptr encodeMessage();
	void decodeMessage();

	//getters
	const string getUser(){ return *(this->fUser); }
	const string getPass(){ return *(this->fPass); }
};
typedef std::unique_ptr<TGetVersionsReqMessage> TGetVersionsReqMessage_ptr;


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
	TGetVersionsReplyMessage(TBaseMessage_ptr& aBase);
	TGetVersionsReplyMessage(const unsigned int aTotVersions, const unsigned int aOldestVersion, const unsigned int aLastVersion, map<unsigned int, time_t> aVersions);
	string_ptr encodeMessage();
	void decodeMessage();

	//getters
	const unsigned int getTotVersions(){ return this->fTotVersions; }
	const unsigned int getOldestVersion(){ return this->fOldestVersion; }
	const unsigned int getLastVersion(){ return this->fLastVersion; }
	const time_t getVersion(const unsigned int aVersion){ return this->fVersions.at(aVersion); }
};
typedef std::unique_ptr<TGetVersionsReplyMessage> TGetVersionsReplyMessage_ptr;


////////////////////////////////////////
//       TRestoreVerReqMessage        //
////////////////////////////////////////
public class TRestoreVerReqMessage : public TBaseMessage {
private:
	string_ptr fUser;
	string_ptr fPass;
	unsigned int fVersion;

public:
	TRestoreVerReqMessage(TBaseMessage_ptr& aBase);
	TRestoreVerReqMessage(const string aUser, const string aPass, const unsigned int aVersion);
	string_ptr encodeMessage();
	void decodeMessage();

	//getters
	const string getUser(){ return *(this->fUser); }
	const string getPass(){ return *(this->fPass); }
	const unsigned int getVersion(){ return this->fVersion; }
};
typedef std::unique_ptr<TRestoreVerReqMessage> TRestoreVerReqMessage_ptr;


////////////////////////////////////////
//      TRestoreVerReplyMessage       //
////////////////////////////////////////
public class TRestoreVerReplyMessage : public TBaseMessage {
private:
	bool fResp;
	string_ptr fToken;

public:
	TRestoreVerReplyMessage(TBaseMessage_ptr& aBase);
	TRestoreVerReplyMessage(const bool aResp, const string aToken);
	string_ptr encodeMessage();
	void decodeMessage();

	//getters
	const bool getResp(){ return this->fResp; }
	const string getToken(){ return *(this->fToken); }
};
typedef std::unique_ptr<TRestoreVerReplyMessage> TRestoreVerReplyMessage_ptr;


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
	TRestoreFileMessage(TBaseMessage_ptr& aBase);
	TRestoreFileMessage(string aFilePath);
	string_ptr encodeMessage();
	void decodeMessage();

	//getters
	const string getFilePath(){ return *(this->fFilePath); }
	const string getChecksum(){ return *(this->fChecksum); }
	const string getFileContent(){ return *(this->fFileContent); }
	const time_t getFileDate(){ return this->fFileDate; }
};
typedef std::unique_ptr<TRestoreFileMessage> TRestoreFileMessage_ptr;


//////////////////////////////////////////
//        TRestoreFileAckMessage        //
//////////////////////////////////////////
public class TRestoreFileAckMessage : public TBaseMessage {
private:
	string_ptr fToken;
	bool fResp;
	string_ptr fFilePath;

public:
	TRestoreFileAckMessage(TBaseMessage_ptr& aBase);
	TRestoreFileAckMessage(const string aToken, const bool aResp, const string aFilePath);
	string_ptr encodeMessage();
	void decodeMessage();

	//getters
	const string getToken(){ return *(this->fToken); }
	const bool getResp(){ return this->fResp; }
	const string getFilePath(){ return *(this->fFilePath); }
};
typedef std::unique_ptr<TRestoreFileAckMessage> TRestoreFileAckMessage_ptr;


////////////////////////////////////
//      TRestoreStopMessage       //
////////////////////////////////////
public class TRestoreStopMessage : public TBaseMessage {
private:
	unsigned int fVersion;
	time_t fTime;

public:
	TRestoreStopMessage(TBaseMessage_ptr& aBase);
	TRestoreStopMessage(unsigned int aVersion, time_t aTime);
	string_ptr encodeMessage();
	void decodeMessage();

	//getters
	const unsigned int getVersion(){ return this->fVersion; }
	const time_t getTime(){ return this->fTime; }
};
typedef std::unique_ptr<TRestoreStopMessage> TRestoreStopMessage_ptr;


//////////////////////////////////////
//        TPingReqMessage           //
//////////////////////////////////////
public class TPingReqMessage : public TBaseMessage {
private:
	time_t fTime;

public:
	TPingReqMessage(TBaseMessage_ptr& aBase);
	TPingReqMessage();
	string_ptr encodeMessage();
	void decodeMessage();

	//getters
	const time_t getTime(){ return this->fTime; }
};
typedef std::unique_ptr<TPingReqMessage> TPingReqMessage_ptr;


//////////////////////////////////////
//        TPingReplyMessage         //
//////////////////////////////////////
public class TPingReplyMessage : public TBaseMessage {
private:
	time_t fTime;

public:
	TPingReplyMessage(TBaseMessage_ptr& aBase);
	TPingReplyMessage();
	string_ptr encodeMessage();
	void decodeMessage();

	//getters
	const time_t getTime(){ return this->fTime; }
};
typedef std::unique_ptr<TPingReplyMessage> TPingReplyMessage_ptr;
