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
#ifdef STORAGE_SERVER
public class EMessageException : public EBaseException {
#else
class EMessageException : public EBaseException {
#endif
public:
	EMessageException(const string& aMsg) : EBaseException(aMsg){}
};


//////////////////////////////////////
//         TBaseMessage	            //
//////////////////////////////////////
#ifdef STORAGE_SERVER
public class TBaseMessage{
#else
class TBaseMessage{
#endif
private:
	void decodeMessageID();

protected:
	int fID = NO_ID;
	string_ptr fEncodedMsg = nullptr;
	string_vector_ptr fItems = nullptr;

public:
	TBaseMessage();
	TBaseMessage(string_ptr& aMsg);
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
#ifdef STORAGE_SERVER
public class TUserRegistrReqMessage : public TBaseMessage {
#else
class TUserRegistrReqMessage : public TBaseMessage {
#endif
private:
	string_ptr fUser = nullptr;
	string_ptr fPass = nullptr;

public:
	TUserRegistrReqMessage(TBaseMessage_ptr& aBase);
	TUserRegistrReqMessage(const string& aUser, const string& aPass);
	~TUserRegistrReqMessage();

	string_ptr encodeMessage();
	void decodeMessage();

	//getters
	const string getUser(){ return *(this->fUser); }
	const string getPass(){ return *(this->fPass); }
};
typedef std::unique_ptr<TUserRegistrReqMessage> TUserRegistrReqMessage_ptr;
#define make_TUserRegistrReqMessage_ptr(ptr) std::make_unique<TUserRegistrReqMessage>(ptr)


////////////////////////////////////////
//      TUserRegistrReplyMessage      //
////////////////////////////////////////
#ifdef STORAGE_SERVER
public class TUserRegistrReplyMessage : public TBaseMessage {
#else
class TUserRegistrReplyMessage : public TBaseMessage {
#endif
private:
	bool fResp = false;

public:
	TUserRegistrReplyMessage(TBaseMessage_ptr& aBase);
	TUserRegistrReplyMessage(const bool aResp);
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
#ifdef STORAGE_SERVER
public class TUpdateStartReqMessage : public TBaseMessage {
#else
class TUpdateStartReqMessage : public TBaseMessage {
#endif
private:
	string_ptr fUser = nullptr;
	string_ptr fPass = nullptr;

public:
	TUpdateStartReqMessage(TBaseMessage_ptr& aBase);
	TUpdateStartReqMessage(const string& aUser, const string& aPass);
	~TUpdateStartReqMessage();

	string_ptr encodeMessage();
	void decodeMessage();

	//getters
	const string getUser(){ return *(this->fUser); }
	const string getPass(){ return *(this->fPass); }
};
typedef std::unique_ptr<TUpdateStartReqMessage> TUpdateStartReqMessage_ptr;
#define make_TUpdateStartReqMessage_ptr(ptr) std::make_unique<TUpdateStartReqMessage>(ptr)


////////////////////////////////////////
//      TUpdateStartReplyMessage      //
////////////////////////////////////////
#ifdef STORAGE_SERVER
public class TUpdateStartReplyMessage : public TBaseMessage {
#else
class TUpdateStartReplyMessage : public TBaseMessage {
#endif
private:
	bool fResp = false;
	string_ptr fToken = nullptr;

public:
	TUpdateStartReplyMessage(TBaseMessage_ptr& aBase);
	TUpdateStartReplyMessage(const bool aResp, const string& aToken);
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
#ifdef STORAGE_SERVER
public class TAddNewFileMessage : public TBaseMessage {
#else
class TAddNewFileMessage : public TBaseMessage {
#endif
private:
	string_ptr fToken = nullptr;
	string_ptr fFilePath = nullptr;
	time_t fFileDate;
	string_ptr fChecksum = nullptr;
	string_ptr fFileContent = nullptr;

public:
	TAddNewFileMessage(TBaseMessage_ptr& aBase);
	TAddNewFileMessage(const string& aToken, const string& aFilePath);
	~TAddNewFileMessage();

	string_ptr encodeMessage();
	void decodeMessage();

	//getters
	const string getToken(){ return *(this->fToken); }
	const string getFilePath(){ return *(this->fFilePath); }
	const time_t getFileDate(){ return this->fFileDate; }
	string_ptr getChecksum(){ return move_string_ptr(this->fChecksum); }
	string_ptr getFileContent(){ return move_string_ptr(this->fFileContent); }
};
typedef std::unique_ptr<TAddNewFileMessage> TAddNewFileMessage_ptr;
#define make_TAddNewFileMessage_ptr(ptr) std::make_unique<TAddNewFileMessage>(ptr)


////////////////////////////////////////
//        TUpdateFileMessage          //
////////////////////////////////////////
#ifdef STORAGE_SERVER
public class TUpdateFileMessage : public TBaseMessage {
#else
class TUpdateFileMessage : public TBaseMessage {
#endif
private:
	string_ptr fToken = nullptr;
	string_ptr fFilePath = nullptr;
	time_t fFileDate;
	string_ptr fChecksum = nullptr;
	string_ptr fFileContent = nullptr;

public:
	TUpdateFileMessage(TBaseMessage_ptr& aBase);
	TUpdateFileMessage(const string& aToken, const string& aFilePath);
	~TUpdateFileMessage();

	string_ptr encodeMessage();
	void decodeMessage();

	//getters
	const string getToken(){ return *(this->fToken); }
	const string getFilePath(){ return *(this->fFilePath); }
	const time_t getFileDate(){ return this->fFileDate; }
	string_ptr getChecksum(){ return move_string_ptr(this->fChecksum); }
	string_ptr getFileContent(){ return move_string_ptr(this->fFileContent); }
};
typedef std::unique_ptr<TUpdateFileMessage> TUpdateFileMessage_ptr;
#define make_TUpdateFileMessage_ptr(ptr) std::make_unique<TUpdateFileMessage>(ptr)


////////////////////////////////////////
//        TRemoveFileMessage          //
////////////////////////////////////////
#ifdef STORAGE_SERVER
public class TRemoveFileMessage : public TBaseMessage {
#else
class TRemoveFileMessage : public TBaseMessage {
#endif
private:
	string_ptr fToken = nullptr;
	string_ptr fFilePath = nullptr;

public:
	TRemoveFileMessage(TBaseMessage_ptr& aBase);
	TRemoveFileMessage(const string& aToken, const string& aFilePath);
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
#ifdef STORAGE_SERVER
public class TFileAckMessage : public TBaseMessage {
#else
class TFileAckMessage : public TBaseMessage {
#endif
private:
	bool fResp = false;
	string_ptr fFilePath = nullptr;

public:
	TFileAckMessage(TBaseMessage_ptr& aBase);
	TFileAckMessage(const bool aResp, const string& aFilePath);
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
#ifdef STORAGE_SERVER
public class TUpdateStopReqMessage : public TBaseMessage {
#else
class TUpdateStopReqMessage : public TBaseMessage {
#endif
private:
	string_ptr fToken = nullptr;

public:
	TUpdateStopReqMessage(TBaseMessage_ptr& aBase);
	TUpdateStopReqMessage(const string& aToken);
	~TUpdateStopReqMessage();

	string_ptr encodeMessage();
	void decodeMessage();

	//getters
	const string getToken(){ return *(this->fToken); }
};
typedef std::unique_ptr<TUpdateStopReqMessage> TUpdateStopReqMessage_ptr;
#define make_TUpdateStopReqMessage_ptr(ptr) std::make_unique<TUpdateStopReqMessage>(ptr)


////////////////////////////////////////
//      TUpdateStopReplyMessage       //
////////////////////////////////////////
#ifdef STORAGE_SERVER
public class TUpdateStopReplyMessage : public TBaseMessage {
#else
class TUpdateStopReplyMessage : public TBaseMessage {
#endif
private:
	bool fResp = false;
	int fVersion = -1;
	time_t fTime;

public:
	TUpdateStopReplyMessage(TBaseMessage_ptr& aBase);
	TUpdateStopReplyMessage(const bool aResp, int aVersion, time_t aTime);
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
#ifdef STORAGE_SERVER
public class TGetVersionsReqMessage : public TBaseMessage {
#else
class TGetVersionsReqMessage : public TBaseMessage {
#endif
private:
	string_ptr fUser = nullptr;
	string_ptr fPass = nullptr;

public:
	TGetVersionsReqMessage(TBaseMessage_ptr& aBase);
	TGetVersionsReqMessage(const string& aUser, const string& aPass);
	~TGetVersionsReqMessage();

	string_ptr encodeMessage();
	void decodeMessage();

	//getters
	const string getUser(){ return *(this->fUser); }
	const string getPass(){ return *(this->fPass); }
};
typedef std::unique_ptr<TGetVersionsReqMessage> TGetVersionsReqMessage_ptr;
#define make_TGetVersionsReqMessage_ptr(ptr) std::make_unique<TGetVersionsReqMessage>(ptr)


////////////////////////////////////////
//      TGetVersionsReplyMessage      //
////////////////////////////////////////
#ifdef STORAGE_SERVER
public class TGetVersionsReplyMessage : public TBaseMessage {
#else
class TGetVersionsReplyMessage : public TBaseMessage {
#endif
private:
	unsigned int fTotVersions = -1;
	unsigned int fOldestVersion = -1;
	unsigned int fLastVersion = -1;
	TVersionList_ptr fVersions = nullptr;

public:
	TGetVersionsReplyMessage(TBaseMessage_ptr& aBase);
	TGetVersionsReplyMessage(const unsigned int aTotVersions, const unsigned int aOldestVersion, const unsigned int aLastVersion, TVersionList_ptr& aVersions);
	~TGetVersionsReplyMessage();

	string_ptr encodeMessage();
	void decodeMessage();

	//getters
	const unsigned int getTotVersions(){ return this->fTotVersions; }
	const unsigned int getOldestVersion(){ return this->fOldestVersion; }
	const unsigned int getLastVersion(){ return this->fLastVersion; }
	const time_t getVersion(const unsigned int aVersion){ return this->fVersions->at(aVersion)->getDate(); }
};
typedef std::unique_ptr<TGetVersionsReplyMessage> TGetVersionsReplyMessage_ptr;
#define new_TGetVersionsReplyMessage_ptr(aTotVersions, aOldestVersion, aLastVersion, aVersions) std::make_unique<TGetVersionsReplyMessage>(aTotVersions, aOldestVersion, aLastVersion, aVersions)
#define make_TGetVersionsReplyMessage_ptr(ptr) std::make_unique<TGetVersionsReplyMessage>(ptr)


////////////////////////////////////////
//       TRestoreVerReqMessage        //
////////////////////////////////////////
#ifdef STORAGE_SERVER
public class TRestoreVerReqMessage : public TBaseMessage {
#else
class TRestoreVerReqMessage : public TBaseMessage {
#endif
private:
	string_ptr fUser = nullptr;
	string_ptr fPass = nullptr;
	unsigned int fVersion = -1;

public:
	TRestoreVerReqMessage(TBaseMessage_ptr& aBase);
	TRestoreVerReqMessage(const string& aUser, const string& aPass, const unsigned int aVersion);
	~TRestoreVerReqMessage();

	string_ptr encodeMessage();
	void decodeMessage();

	//getters
	const string getUser(){ return *(this->fUser); }
	const string getPass(){ return *(this->fPass); }
	const unsigned int getVersion(){ return this->fVersion; }
};
typedef std::unique_ptr<TRestoreVerReqMessage> TRestoreVerReqMessage_ptr;
#define make_TRestoreVerReqMessage_ptr(ptr) std::make_unique<TRestoreVerReqMessage>(ptr)


////////////////////////////////////////
//      TRestoreVerReplyMessage       //
////////////////////////////////////////
#ifdef STORAGE_SERVER
public class TRestoreVerReplyMessage : public TBaseMessage {
#else
class TRestoreVerReplyMessage : public TBaseMessage {
#endif
private:
	bool fResp = false;
	string_ptr fToken = nullptr;

public:
	TRestoreVerReplyMessage(TBaseMessage_ptr& aBase);
	TRestoreVerReplyMessage(const bool aResp, const string& aToken);
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
#ifdef STORAGE_SERVER
public class TRestoreFileMessage : public TBaseMessage {
#else
class TRestoreFileMessage : public TBaseMessage {
#endif
private:
	string_ptr fFilePath = nullptr;
	time_t fFileDate;
	string_ptr fChecksum = nullptr;
	string_ptr fFileContent = nullptr;

public:
	TRestoreFileMessage(TBaseMessage_ptr& aBase);
	TRestoreFileMessage(const string& aFilePath);
	~TRestoreFileMessage();

	string_ptr encodeMessage();
	void decodeMessage();

	//getters
	const string getFilePath(){ return *(this->fFilePath); }
	const string getChecksum(){ return *(this->fChecksum); }
	const string getFileContent(){ return *(this->fFileContent); }
	const time_t getFileDate(){ return this->fFileDate; }
};
typedef std::unique_ptr<TRestoreFileMessage> TRestoreFileMessage_ptr;
#define new_TRestoreFileMessage_ptr(aFilePath) std::make_unique<TRestoreFileMessage>(aFilePath)
#define make_TRestoreFileMessage_ptr(ptr) std::make_unique<TRestoreFileMessage>(ptr)


//////////////////////////////////////////
//        TRestoreFileAckMessage        //
//////////////////////////////////////////
#ifdef STORAGE_SERVER
public class TRestoreFileAckMessage : public TBaseMessage {
#else
class TRestoreFileAckMessage : public TBaseMessage {
#endif
private:
	string_ptr fToken = nullptr;
	bool fResp = false;
	string_ptr fFilePath = nullptr;

public:
	TRestoreFileAckMessage(TBaseMessage_ptr& aBase);
	TRestoreFileAckMessage(const bool aResp, const string& aToken, const string& aFilePath);
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
#ifdef STORAGE_SERVER
public class TRestoreStopMessage : public TBaseMessage {
#else
class TRestoreStopMessage : public TBaseMessage {
#endif
private:
	unsigned int fVersion = -1;
	time_t fTime;

public:
	TRestoreStopMessage(TBaseMessage_ptr& aBase);
	TRestoreStopMessage(unsigned int aVersion, time_t aTime);
	~TRestoreStopMessage() {};

	string_ptr encodeMessage();
	void decodeMessage();

	//getters
	const unsigned int getVersion(){ return this->fVersion; }
	const time_t getTime(){ return this->fTime; }
};
typedef std::unique_ptr<TRestoreStopMessage> TRestoreStopMessage_ptr;
#define make_TRestoreStopMessage_ptr(ptr) std::make_unique<TRestoreStopMessage>(ptr)


//////////////////////////////////////
//        TPingReqMessage           //
//////////////////////////////////////
#ifdef STORAGE_SERVER
public class TPingReqMessage : public TBaseMessage {
#else
class TPingReqMessage : public TBaseMessage {
#endif
private:
	time_t fTime;

public:
	TPingReqMessage(TBaseMessage_ptr& aBase);
	TPingReqMessage();
	~TPingReqMessage() {};

	string_ptr encodeMessage();
	void decodeMessage();

	//getters
	const time_t getTime(){ return this->fTime; }
};
typedef std::unique_ptr<TPingReqMessage> TPingReqMessage_ptr;
#define make_TPingReqMessage_ptr(ptr) std::make_unique<TPingReqMessage>(ptr)


//////////////////////////////////////
//        TPingReplyMessage         //
//////////////////////////////////////
#ifdef STORAGE_SERVER
public class TPingReplyMessage : public TBaseMessage {
#else
class TPingReplyMessage : public TBaseMessage {
#endif
private:
	time_t fTime;

public:
	TPingReplyMessage(TBaseMessage_ptr& aBase);
	TPingReplyMessage();
	~TPingReplyMessage() {};

	string_ptr encodeMessage();
	void decodeMessage();

	//getters
	const time_t getTime(){ return this->fTime; }
};
typedef std::unique_ptr<TPingReplyMessage> TPingReplyMessage_ptr;
#define new_TPingReplyMessage_ptr() std::make_unique<TPingReplyMessage>()
#define make_TPingReplyMessage_ptr(ptr) std::make_unique<TPingReplyMessage>(ptr)
