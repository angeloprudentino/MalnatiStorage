/*
 * Author: Angelo Prudentino
 * Date: 06/03/2016
 * File: Message.h
 * Description: this file contains all classes implementing
 *              messages exchanged between client and server
 *
 */
#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>

#include "Utility.h"
#include "Session.h"

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
#define GET_LAST_VERSION_REQ_ID 12
#define GET_LAST_VERSION_REPLY_ID 13
#define RESTORE_VER_REQ_ID 14
#define RESTORE_VER_REPLY_ID 15
#define RESTORE_FILE_ID 16
#define RESTORE_FILE_ACK_ID 17
#define RESTORE_STOP_ID 18
#define PING_REQ_ID 19
#define PING_REPLY_ID 20
#define VERIFY_CRED_REQ_ID 21
#define VERIFY_CRED_REPLY_ID 22
#define SYSTEM_ERR_ID 23

//Message utilities
const bool isValidMessage(const string aName);
const bool isValidMessageID(const int aID);
const string getMessageName(const int aIndex);

//////////////////////////////////////
//       EMessageException	        //
//////////////////////////////////////
class EMessageException : public EBaseException {
public:
	EMessageException(const string& aMsg) : EBaseException(aMsg){}
};


//////////////////////////////////////
//         TBaseMessage	            //
//////////////////////////////////////
class TBaseMessage{
private:
	void decodeMessageID();

protected:
	int fID = NO_ID;
	string_ptr fEncodedMsg = nullptr;
	string_vector_ptr fItems = nullptr;

public:
	TBaseMessage();
	TBaseMessage(string_ptr& aMsg);
	TBaseMessage(const TBaseMessage&) = delete;            // disable copying
	TBaseMessage& operator=(const TBaseMessage&) = delete; // disable assignment
	virtual ~TBaseMessage();

	virtual string_ptr encodeMessage();
	virtual void decodeMessage();

	// getters
	const int getID(){ return this->fID; };
	const string getName(){	return getMessageName(this->fID); };
	string_vector_ptr getTokens(){ return move_string_vector_ptr(this->fItems); };
	string_ptr getMsg(){ return move_string_ptr(this->fEncodedMsg); };
};
typedef std::unique_ptr<TBaseMessage> TBaseMessage_ptr;
#define new_TBaseMessage_ptr(str) std::make_unique<TBaseMessage>(str)
#define move_TBaseMessage_ptr(ptr) std::move(ptr)


////////////////////////////////////////
//      TUserRegistrReqMessage        //
////////////////////////////////////////
class TUserRegistrReqMessage : public TBaseMessage {
private:
	string_ptr fUser = nullptr;
	string_ptr fPass = nullptr;
	string_ptr fPath = nullptr;

public:
	TUserRegistrReqMessage(TBaseMessage_ptr& aBase);
	TUserRegistrReqMessage(const string& aUser, const string& aPass, const string& aPath);
	TUserRegistrReqMessage(const TUserRegistrReqMessage&) = delete;            // disable copying
	TUserRegistrReqMessage& operator=(const TUserRegistrReqMessage&) = delete; // disable assignment
	~TUserRegistrReqMessage();

	string_ptr encodeMessage();
	void decodeMessage();

	//getters
	const string getUser(){ return *(this->fUser); }
	const string getPass(){ return *(this->fPass); }
	const string getPath(){ return *(this->fPath); }
};
typedef std::unique_ptr<TUserRegistrReqMessage> TUserRegistrReqMessage_ptr;
#define new_TUserRegistrReqMessage_ptr(aUser, aPass, aPath) std::make_unique<TUserRegistrReqMessage>(aUser, aPass, aPath)
#define make_TUserRegistrReqMessage_ptr(ptr) std::make_unique<TUserRegistrReqMessage>(ptr)


////////////////////////////////////////
//      TUserRegistrReplyMessage      //
////////////////////////////////////////
class TUserRegistrReplyMessage : public TBaseMessage {
private:
	bool fResp = false;

public:
	TUserRegistrReplyMessage(TBaseMessage_ptr& aBase);
	TUserRegistrReplyMessage(const bool aResp);
	TUserRegistrReplyMessage(const TUserRegistrReplyMessage&) = delete;            // disable copying
	TUserRegistrReplyMessage& operator=(const TUserRegistrReplyMessage&) = delete; // disable assignment
	~TUserRegistrReplyMessage(){};

	string_ptr encodeMessage();
	void decodeMessage();

	//getters
	const bool getResp(){ return this->fResp; }
};
typedef std::unique_ptr<TUserRegistrReplyMessage> TUserRegistrReplyMessage_ptr;
#define new_TUserRegistrReplyMessage_ptr(aResp) std::make_unique<TUserRegistrReplyMessage>(aResp)
#define make_TUserRegistrReplyMessage_ptr(ptr) std::make_unique<TUserRegistrReplyMessage>(ptr)


////////////////////////////////////////
//      TUpdateStartReqMessage        //
////////////////////////////////////////
class TUpdateStartReqMessage : public TBaseMessage {
private:
	string_ptr fUser = nullptr;
	string_ptr fPass = nullptr;

public:
	TUpdateStartReqMessage(TBaseMessage_ptr& aBase);
	TUpdateStartReqMessage(const string& aUser, const string& aPass);
	TUpdateStartReqMessage(const TUpdateStartReqMessage&) = delete;            // disable copying
	TUpdateStartReqMessage& operator=(const TUpdateStartReqMessage&) = delete; // disable assignment
	~TUpdateStartReqMessage();

	string_ptr encodeMessage();
	void decodeMessage();

	//getters
	const string getUser(){ return *(this->fUser); }
	const string getPass(){ return *(this->fPass); }
};
typedef std::unique_ptr<TUpdateStartReqMessage> TUpdateStartReqMessage_ptr;
#define new_TUpdateStartReqMessage_ptr(aUser, aPass) std::make_unique<TUpdateStartReqMessage>(aUser, aPass)
#define make_TUpdateStartReqMessage_ptr(ptr) std::make_unique<TUpdateStartReqMessage>(ptr)


////////////////////////////////////////
//      TUpdateStartReplyMessage      //
////////////////////////////////////////
class TUpdateStartReplyMessage : public TBaseMessage {
private:
	bool fResp = false;
	string_ptr fToken = nullptr;

public:
	TUpdateStartReplyMessage(TBaseMessage_ptr& aBase);
	TUpdateStartReplyMessage(const bool aResp, const string& aToken);
	TUpdateStartReplyMessage(const TUpdateStartReplyMessage&) = delete;            // disable copying
	TUpdateStartReplyMessage& operator=(const TUpdateStartReplyMessage&) = delete; // disable assignment
	~TUpdateStartReplyMessage();

	string_ptr encodeMessage();
	void decodeMessage();

	//getters
	const bool getResp(){ return this->fResp; }
	const string getToken(){ return *(this->fToken); }
};
typedef std::unique_ptr<TUpdateStartReplyMessage> TUpdateStartReplyMessage_ptr;
#define new_TUpdateStartReplyMessage_ptr(aResp, aToken) std::make_unique<TUpdateStartReplyMessage>(aResp, aToken)
#define make_TUpdateStartReplyMessage_ptr(ptr) std::make_unique<TUpdateStartReplyMessage>(ptr)


////////////////////////////////////////
//        TAddNewFileMessage          //
////////////////////////////////////////
class TAddNewFileMessage : public TBaseMessage {
private:
	string_ptr fToken = nullptr;
	string_ptr fFilePath = nullptr;
	time_t fFileDate;
	string_ptr fChecksum = nullptr;
	string_ptr fFileContent = nullptr;

public:
	TAddNewFileMessage(TBaseMessage_ptr& aBase);
	TAddNewFileMessage(const string& aToken, const string& aFilePath);
	TAddNewFileMessage(const TAddNewFileMessage&) = delete;            // disable copying
	TAddNewFileMessage& operator=(const TAddNewFileMessage&) = delete; // disable assignment
	~TAddNewFileMessage();

	string_ptr encodeMessage();
	void decodeMessage();
	bool verifyChecksum();

	//getters
	const string getToken(){ return *(this->fToken); }
	const string getFilePath(){ return *(this->fFilePath); }
	const time_t getFileDate(){ return this->fFileDate; }
	string_ptr getFileContent(){ return move_string_ptr(this->fFileContent); }
};
typedef std::unique_ptr<TAddNewFileMessage> TAddNewFileMessage_ptr;
#define new_TAddNewFileMessage_ptr(aToken, aFilePath) std::make_unique<TAddNewFileMessage>(aToken, aFilePath)
#define make_TAddNewFileMessage_ptr(ptr) std::make_unique<TAddNewFileMessage>(ptr)


////////////////////////////////////////
//        TUpdateFileMessage          //
////////////////////////////////////////
class TUpdateFileMessage : public TBaseMessage {
private:
	string_ptr fToken = nullptr;
	string_ptr fFilePath = nullptr;
	time_t fFileDate;
	string_ptr fChecksum = nullptr;
	string_ptr fFileContent = nullptr;

public:
	TUpdateFileMessage(TBaseMessage_ptr& aBase);
	TUpdateFileMessage(const string& aToken, const string& aFilePath);
	TUpdateFileMessage(const TUpdateFileMessage&) = delete;            // disable copying
	TUpdateFileMessage& operator=(const TUpdateFileMessage&) = delete; // disable assignment
	~TUpdateFileMessage();

	string_ptr encodeMessage();
	void decodeMessage();
	bool verifyChecksum();

	//getters
	const string getToken(){ return *(this->fToken); }
	const string getFilePath(){ return *(this->fFilePath); }
	const time_t getFileDate(){ return this->fFileDate; }
	string_ptr getFileContent(){ return move_string_ptr(this->fFileContent); }
};
typedef std::unique_ptr<TUpdateFileMessage> TUpdateFileMessage_ptr;
#define make_TUpdateFileMessage_ptr(ptr) std::make_unique<TUpdateFileMessage>(ptr)


////////////////////////////////////////
//        TRemoveFileMessage          //
////////////////////////////////////////
class TRemoveFileMessage : public TBaseMessage {
private:
	string_ptr fToken = nullptr;
	string_ptr fFilePath = nullptr;

public:
	TRemoveFileMessage(TBaseMessage_ptr& aBase);
	TRemoveFileMessage(const string& aToken, const string& aFilePath);
	TRemoveFileMessage(const TRemoveFileMessage&) = delete;            // disable copying
	TRemoveFileMessage& operator=(const TRemoveFileMessage&) = delete; // disable assignment
	~TRemoveFileMessage();

	string_ptr encodeMessage();
	void decodeMessage();

	//getters
	const string getToken(){ return *(this->fToken); }
	const string getFilePath(){ return *(this->fFilePath); }
};
typedef std::unique_ptr<TRemoveFileMessage> TRemoveFileMessage_ptr;
#define make_TRemoveFileMessage_ptr(ptr) std::make_unique<TRemoveFileMessage>(ptr)


///////////////////////////////////
//        TFileAckMessage        //
///////////////////////////////////
class TFileAckMessage : public TBaseMessage {
private:
	bool fResp = false;
	string_ptr fFilePath = nullptr;

public:
	TFileAckMessage(TBaseMessage_ptr& aBase);
	TFileAckMessage(const bool aResp, const string& aFilePath);
	TFileAckMessage(const TFileAckMessage&) = delete;            // disable copying
	TFileAckMessage& operator=(const TFileAckMessage&) = delete; // disable assignment
	~TFileAckMessage();

	string_ptr encodeMessage();
	void decodeMessage();

	//getters
	const bool getResp(){ return this->fResp; }
	const string getFilePath(){ return *(this->fFilePath); }
};
typedef std::unique_ptr<TFileAckMessage> TFileAckMessage_ptr;
#define new_TFileAckMessage_ptr(aResp, aFilePath) std::make_unique<TFileAckMessage>(aResp, aFilePath)
#define make_TFileAckMessage_ptr(ptr) std::make_unique<TFileAckMessage>(ptr)


////////////////////////////////////////
//      TUpdateStopReqMessage         //
////////////////////////////////////////
class TUpdateStopReqMessage : public TBaseMessage {
private:
	string_ptr fToken = nullptr;

public:
	TUpdateStopReqMessage(TBaseMessage_ptr& aBase);
	TUpdateStopReqMessage(const string& aToken);
	TUpdateStopReqMessage(const TUpdateStopReqMessage&) = delete;            // disable copying
	TUpdateStopReqMessage& operator=(const TUpdateStopReqMessage&) = delete; // disable assignment
	~TUpdateStopReqMessage();

	string_ptr encodeMessage();
	void decodeMessage();

	//getters
	const string getToken(){ return *(this->fToken); }
};
typedef std::unique_ptr<TUpdateStopReqMessage> TUpdateStopReqMessage_ptr;
#define new_TUpdateStopReqMessage_ptr(aToken) std::make_unique<TUpdateStopReqMessage>(aToken)
#define make_TUpdateStopReqMessage_ptr(ptr) std::make_unique<TUpdateStopReqMessage>(ptr)


////////////////////////////////////////
//      TUpdateStopReplyMessage       //
////////////////////////////////////////
class TUpdateStopReplyMessage : public TBaseMessage {
private:
	bool fResp = false;
	int fVersion = -1;
	time_t fTime;

public:
	TUpdateStopReplyMessage(TBaseMessage_ptr& aBase);
	TUpdateStopReplyMessage(const bool aResp, const int aVersion, const time_t aTime);
	TUpdateStopReplyMessage(const TUpdateStopReplyMessage&) = delete;            // disable copying
	TUpdateStopReplyMessage& operator=(const TUpdateStopReplyMessage&) = delete; // disable assignment
	~TUpdateStopReplyMessage() {};

	string_ptr encodeMessage();
	void decodeMessage();

	//getters
	const bool getResp(){ return this->fResp; }
	const int getVersion(){ return this->fVersion; }
	const time_t getTime(){ return this->fTime; }
};
typedef std::unique_ptr<TUpdateStopReplyMessage> TUpdateStopReplyMessage_ptr;
#define new_TUpdateStopReplyMessage_ptr(aResp, aVersion, aTime) std::make_unique<TUpdateStopReplyMessage>(aResp, aVersion, aTime)
#define make_TUpdateStopReplyMessage_ptr(ptr) std::make_unique<TUpdateStopReplyMessage>(ptr)


////////////////////////////////////////
//      TGetVersionsReqMessage        //
////////////////////////////////////////
class TGetVersionsReqMessage : public TBaseMessage {
private:
	string_ptr fUser = nullptr;
	string_ptr fPass = nullptr;

public:
	TGetVersionsReqMessage(TBaseMessage_ptr& aBase);
	TGetVersionsReqMessage(const string& aUser, const string& aPass);
	TGetVersionsReqMessage(const TGetVersionsReqMessage&) = delete;            // disable copying
	TGetVersionsReqMessage& operator=(const TGetVersionsReqMessage&) = delete; // disable assignment
	~TGetVersionsReqMessage();

	string_ptr encodeMessage();
	void decodeMessage();

	//getters
	const string getUser(){ return *(this->fUser); }
	const string getPass(){ return *(this->fPass); }
};
typedef std::unique_ptr<TGetVersionsReqMessage> TGetVersionsReqMessage_ptr;
#define new_TGetVersionsReqMessage_ptr(aUser, aPass) std::make_unique<TGetVersionsReqMessage>(aUser, aPass)
#define make_TGetVersionsReqMessage_ptr(ptr) std::make_unique<TGetVersionsReqMessage>(ptr)


////////////////////////////////////////
//      TGetVersionsReplyMessage      //
////////////////////////////////////////
class TGetVersionsReplyMessage : public TBaseMessage {
private:
	unsigned int fTotVersions = 0;
	unsigned int fOldestVersion = 0;
	unsigned int fLastVersion = 0;
	TVersionList_ptr fVersions = nullptr;

public:
	TGetVersionsReplyMessage(TBaseMessage_ptr& aBase);
	TGetVersionsReplyMessage(const unsigned int aTotVersions, const unsigned int aOldestVersion, const unsigned int aLastVersion, TVersionList_ptr& aVersions);
	TGetVersionsReplyMessage(const TGetVersionsReplyMessage&) = delete;            // disable copying
	TGetVersionsReplyMessage& operator=(const TGetVersionsReplyMessage&) = delete; // disable assignment
	~TGetVersionsReplyMessage();

	string_ptr encodeMessage();
	void decodeMessage();

	//getters
	const unsigned int getTotVersions(){ return this->fTotVersions; }
	const unsigned int getOldestVersion(){ return this->fOldestVersion; }
	const unsigned int getLastVersion(){ return this->fLastVersion; }
	const time_t getVersionDate(const unsigned int aVersion){ return this->fVersions->at(aVersion-1)->getDate(); }
};
typedef std::unique_ptr<TGetVersionsReplyMessage> TGetVersionsReplyMessage_ptr;
#define new_TGetVersionsReplyMessage_ptr(aTotVersions, aOldestVersion, aLastVersion, aVersions) std::make_unique<TGetVersionsReplyMessage>(aTotVersions, aOldestVersion, aLastVersion, aVersions)
#define make_TGetVersionsReplyMessage_ptr(ptr) std::make_unique<TGetVersionsReplyMessage>(ptr)


////////////////////////////////////////
//       TGetLastVerReqMessage        //
////////////////////////////////////////
class TGetLastVerReqMessage : public TBaseMessage {
private:
	string_ptr fUser = nullptr;
	string_ptr fPass = nullptr;

public:
	TGetLastVerReqMessage(TBaseMessage_ptr& aBase);
	TGetLastVerReqMessage(const string& aUser, const string& aPass);
	TGetLastVerReqMessage(const TGetLastVerReqMessage&) = delete;            // disable copying
	TGetLastVerReqMessage& operator=(const TGetLastVerReqMessage&) = delete; // disable assignment
	~TGetLastVerReqMessage();

	string_ptr encodeMessage();
	void decodeMessage();

	//getters
	const string getUser(){ return *(this->fUser); }
	const string getPass(){ return *(this->fPass); }
};
typedef std::unique_ptr<TGetLastVerReqMessage> TGetLastVerReqMessage_ptr;
#define new_TGetLastVerReqMessage_ptr(aUser, aPass) std::make_unique<TGetLastVerReqMessage>(aUser, aPass)
#define make_TGetLastVerReqMessage_ptr(ptr) std::make_unique<TGetLastVerReqMessage>(ptr)


////////////////////////////////////////
//      TGetLastVerReplyMessage       //
////////////////////////////////////////
class TGetLastVerReplyMessage : public TBaseMessage {
private:
	int fVersion = -1;
	time_t fVersionDate;

public:
	TGetLastVerReplyMessage(TBaseMessage_ptr& aBase);
	TGetLastVerReplyMessage(const int aVersion, const time_t aVersionDate);
	TGetLastVerReplyMessage(const TGetLastVerReplyMessage&) = delete;            // disable copying
	TGetLastVerReplyMessage& operator=(const TGetLastVerReplyMessage&) = delete; // disable assignment
	~TGetLastVerReplyMessage() {};

	string_ptr encodeMessage();
	void decodeMessage();

	//getters
	const int getVersion(){ return this->fVersion; }
	const time_t getVersionDate(){ return this->fVersionDate; }
};
typedef std::unique_ptr<TGetLastVerReplyMessage> TGetLastVerReplyMessage_ptr;
#define new_TGetLastVerReplyMessage_ptr(aVersion, aVersionDate) std::make_unique<TGetLastVerReplyMessage>(aVersion, aVersionDate)
#define make_TGetLastVerReplyMessage_ptr(ptr) std::make_unique<TGetLastVerReplyMessage>(ptr)


////////////////////////////////////////
//       TRestoreVerReqMessage        //
////////////////////////////////////////
class TRestoreVerReqMessage : public TBaseMessage {
private:
	string_ptr fUser = nullptr;
	string_ptr fPass = nullptr;
	unsigned int fVersion = -1;

public:
	TRestoreVerReqMessage(TBaseMessage_ptr& aBase);
	TRestoreVerReqMessage(const string& aUser, const string& aPass, const unsigned int aVersion);
	TRestoreVerReqMessage(const TRestoreVerReqMessage&) = delete;            // disable copying
	TRestoreVerReqMessage& operator=(const TRestoreVerReqMessage&) = delete; // disable assignment
	~TRestoreVerReqMessage();

	string_ptr encodeMessage();
	void decodeMessage();

	//getters
	const string getUser(){ return *(this->fUser); }
	const string getPass(){ return *(this->fPass); }
	const unsigned int getVersion(){ return this->fVersion; }
};
typedef std::unique_ptr<TRestoreVerReqMessage> TRestoreVerReqMessage_ptr;
#define new_TRestoreVerReqMessage_ptr(aUser, aPass, aVersion) std::make_unique<TRestoreVerReqMessage>(aUser, aPass, aVersion)
#define make_TRestoreVerReqMessage_ptr(ptr) std::make_unique<TRestoreVerReqMessage>(ptr)


////////////////////////////////////////
//      TRestoreVerReplyMessage       //
////////////////////////////////////////
class TRestoreVerReplyMessage : public TBaseMessage {
private:
	bool fResp = false;
	string_ptr fToken = nullptr;

public:
	TRestoreVerReplyMessage(TBaseMessage_ptr& aBase);
	TRestoreVerReplyMessage(const bool aResp, const string& aToken);
	TRestoreVerReplyMessage(const TRestoreVerReplyMessage&) = delete;            // disable copying
	TRestoreVerReplyMessage& operator=(const TRestoreVerReplyMessage&) = delete; // disable assignment
	~TRestoreVerReplyMessage();

	string_ptr encodeMessage();
	void decodeMessage();

	//getters
	const bool getResp(){ return this->fResp; }
	const string getToken(){ return *(this->fToken); }
};
typedef std::unique_ptr<TRestoreVerReplyMessage> TRestoreVerReplyMessage_ptr;
#define new_TRestoreVerReplyMessage_ptr(aResp, aToken) std::make_unique<TRestoreVerReplyMessage>(aResp, aToken)
#define make_TRestoreVerReplyMessage_ptr(ptr) std::make_unique<TRestoreVerReplyMessage>(ptr)


////////////////////////////////////////
//        TRestoreFileMessage         //
////////////////////////////////////////
class TRestoreFileMessage : public TBaseMessage {
private:
	string_ptr fFilePath = nullptr;
	time_t fFileDate;
	string_ptr fChecksum = nullptr;
	string_ptr fFileContent = nullptr;

public:
	TRestoreFileMessage(TBaseMessage_ptr& aBase);
	TRestoreFileMessage(const string& aBasePath, const string& aFilePath, const time_t aFileDate);
	TRestoreFileMessage(const TRestoreFileMessage&) = delete;            // disable copying
	TRestoreFileMessage& operator=(const TRestoreFileMessage&) = delete; // disable assignment
	~TRestoreFileMessage();

	string_ptr encodeMessage();
	void decodeMessage();
	bool verifyChecksum();

	//getters
	const string getFilePath(){ return *(this->fFilePath); }
	const time_t getFileDate(){ return this->fFileDate; }
	string_ptr getFileContent(){ return move_string_ptr(this->fFileContent); }
};
typedef std::unique_ptr<TRestoreFileMessage> TRestoreFileMessage_ptr;
#define new_TRestoreFileMessage_ptr(aBasePath, aFilePath, aFileDate) std::make_unique<TRestoreFileMessage>(aBasePath, aFilePath, aFileDate)
#define make_TRestoreFileMessage_ptr(ptr) std::make_unique<TRestoreFileMessage>(ptr)


//////////////////////////////////////////
//        TRestoreFileAckMessage        //
//////////////////////////////////////////
class TRestoreFileAckMessage : public TBaseMessage {
private:
	string_ptr fToken = nullptr;
	bool fResp = false;
	string_ptr fFilePath = nullptr;

public:
	TRestoreFileAckMessage(TBaseMessage_ptr& aBase);
	TRestoreFileAckMessage(const bool aResp, const string& aToken, const string& aFilePath);
	TRestoreFileAckMessage(const TRestoreFileAckMessage&) = delete;            // disable copying
	TRestoreFileAckMessage& operator=(const TRestoreFileAckMessage&) = delete; // disable assignment
	~TRestoreFileAckMessage();

	string_ptr encodeMessage();
	void decodeMessage();

	//getters
	const string getToken(){ return *(this->fToken); }
	const bool getResp(){ return this->fResp; }
	const string getFilePath(){ return *(this->fFilePath); }
};
typedef std::unique_ptr<TRestoreFileAckMessage> TRestoreFileAckMessage_ptr;
#define new_TRestoreFileAckMessage_ptr(aResp, aToken, aFilePath) std::make_unique<TRestoreFileAckMessage>(aResp, aToken, aFilePath)
#define make_TRestoreFileAckMessage_ptr(ptr) std::make_unique<TRestoreFileAckMessage>(ptr)


////////////////////////////////////
//      TRestoreStopMessage       //
////////////////////////////////////
class TRestoreStopMessage : public TBaseMessage {
private:
	int fVersion = -1;
	time_t fVersionDate;

public:
	TRestoreStopMessage(TBaseMessage_ptr& aBase);
	TRestoreStopMessage(const int aVersion, const time_t aVersionDate);
	TRestoreStopMessage(const TRestoreStopMessage&) = delete;            // disable copying
	TRestoreStopMessage& operator=(const TRestoreStopMessage&) = delete; // disable assignment
	~TRestoreStopMessage() {};

	string_ptr encodeMessage();
	void decodeMessage();

	//getters
	const int getVersion(){ return this->fVersion; }
	const time_t getVersionDate(){ return this->fVersionDate; }
};
typedef std::unique_ptr<TRestoreStopMessage> TRestoreStopMessage_ptr;
#define new_TRestoreStopMessage_ptr(aVersion, aVersionDate) std::make_unique<TRestoreStopMessage>(aVersion, aVersionDate)
#define make_TRestoreStopMessage_ptr(ptr) std::make_unique<TRestoreStopMessage>(ptr)


//////////////////////////////////////
//        TPingReqMessage           //
//////////////////////////////////////
class TPingReqMessage : public TBaseMessage {
private:
	time_t fTime;
	string_ptr fToken = nullptr;

public:
	TPingReqMessage(TBaseMessage_ptr& aBase);
	TPingReqMessage(const string& aToken);
	TPingReqMessage(const TPingReqMessage&) = delete;            // disable copying
	TPingReqMessage& operator=(const TPingReqMessage&) = delete; // disable assignment
	~TPingReqMessage() {};

	string_ptr encodeMessage();
	void decodeMessage();

	//getters
	const time_t getTime(){ return this->fTime; }
	const string getToken(){ return *(this->fToken); }
};
typedef std::unique_ptr<TPingReqMessage> TPingReqMessage_ptr;
#define make_TPingReqMessage_ptr(ptr) std::make_unique<TPingReqMessage>(ptr)


//////////////////////////////////////
//        TPingReplyMessage         //
//////////////////////////////////////
class TPingReplyMessage : public TBaseMessage {
private:
	time_t fTime;
	string_ptr fToken = nullptr;

public:
	TPingReplyMessage(TBaseMessage_ptr& aBase);
	TPingReplyMessage(const string& aToken);
	TPingReplyMessage(const TPingReplyMessage&) = delete;            // disable copying
	TPingReplyMessage& operator=(const TPingReplyMessage&) = delete; // disable assignment
	~TPingReplyMessage() {};

	string_ptr encodeMessage();
	void decodeMessage();

	//getters
	const time_t getTime(){ return this->fTime; }
	const string getToken(){ return *(this->fToken); }
};
typedef std::unique_ptr<TPingReplyMessage> TPingReplyMessage_ptr;
#define new_TPingReplyMessage_ptr(aToken) std::make_unique<TPingReplyMessage>(aToken)
#define make_TPingReplyMessage_ptr(ptr) std::make_unique<TPingReplyMessage>(ptr)


////////////////////////////////////////
//       TVerifyCredReqMessage        //
////////////////////////////////////////
class TVerifyCredReqMessage : public TBaseMessage {
private:
	string_ptr fUser = nullptr;
	string_ptr fPass = nullptr;

public:
	TVerifyCredReqMessage(TBaseMessage_ptr& aBase);
	TVerifyCredReqMessage(const string& aUser, const string& aPass);
	TVerifyCredReqMessage(const TVerifyCredReqMessage&) = delete;            // disable copying
	TVerifyCredReqMessage& operator=(const TVerifyCredReqMessage&) = delete; // disable assignment
	~TVerifyCredReqMessage();

	string_ptr encodeMessage();
	void decodeMessage();

	//getters
	const string getUser(){ return *(this->fUser); }
	const string getPass(){ return *(this->fPass); }
};
typedef std::unique_ptr<TVerifyCredReqMessage> TVerifyCredReqMessage_ptr;
#define new_TVerifyCredReqMessage_ptr(aUser, aPass) std::make_unique<TVerifyCredReqMessage>(aUser, aPass)
#define make_TVerifyCredReqMessage_ptr(ptr) std::make_unique<TVerifyCredReqMessage>(ptr)


///////////////////////////////////////
//      TVerifyCredReplyMessage      //
///////////////////////////////////////
class TVerifyCredReplyMessage : public TBaseMessage {
private:
	bool fResp = false;
	string_ptr fPath = nullptr;

public:
	TVerifyCredReplyMessage(TBaseMessage_ptr& aBase);
	TVerifyCredReplyMessage(const bool aResp, const string& aPath);
	TVerifyCredReplyMessage(const TVerifyCredReplyMessage&) = delete;            // disable copying
	TVerifyCredReplyMessage& operator=(const TVerifyCredReplyMessage&) = delete; // disable assignment
	~TVerifyCredReplyMessage() {};

	string_ptr encodeMessage();
	void decodeMessage();

	//getters
	const bool getResp() { return this->fResp; }
	const string getPath() { return this->fPath->c_str(); }
};
typedef std::unique_ptr<TVerifyCredReplyMessage> TVerifyCredReplyMessage_ptr;
#define new_TVerifyCredReplyMessage_ptr(aResp, aPath) std::make_unique<TVerifyCredReplyMessage>(aResp, aPath)
#define make_TVerifyCredReplyMessage_ptr(ptr) std::make_unique<TVerifyCredReplyMessage>(ptr)


//////////////////////////////////////
//       TSystemErrorMessage        //
//////////////////////////////////////
class TSystemErrorMessage : public TBaseMessage {
private:
	string_ptr fDetail = nullptr;

public:
	TSystemErrorMessage(TBaseMessage_ptr& aBase);
	TSystemErrorMessage(const string& aDetail);
	TSystemErrorMessage(const TSystemErrorMessage&) = delete;            // disable copying
	TSystemErrorMessage& operator=(const TSystemErrorMessage&) = delete; // disable assignment
	~TSystemErrorMessage() {};

	string_ptr encodeMessage();
	void decodeMessage();

	//getters
	const string getDetail(){ return *(this->fDetail); }
};
typedef std::unique_ptr<TSystemErrorMessage> TSystemErrorMessage_ptr;
#define new_TSystemErrorMessage_ptr(aToken) std::make_unique<TSystemErrorMessage>(aToken)
#define make_TSystemErrorMessage_ptr(ptr) std::make_unique<TSystemErrorMessage>(ptr)