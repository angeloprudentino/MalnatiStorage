/*
 * Author: Angelo Prudentino
 * Date: 06/03/2016
 * File: Message.cpp
 * Description: this file contains all classes implemented
 *              messages exchanged between client and server
 *
 */

#include <boost/algorithm/string/replace.hpp>
#include <sstream>
#include <vector>
#include <time.h>

#include "Message.h"

using namespace std;

#define MSG_SEP '$'
#define MSG_SEP_ESC "&#36"
#define MSG_INVALID "invalid"

//Message tokens
#define USER_REG_REQ_TOK_NUM 3
#define USER_REG_REPLY_TOK_NUM 2
#define UPDATE_START_REQ_TOK_NUM 3
#define UPDATE_START_REPLY_TOK_NUM 3
#define ADD_NEW_FILE_TOK_NUM 6
#define UPDATE_FILE_TOK_NUM 6
#define REMOVE_FILE_TOK_NUM 3
#define FILE_ACK_TOK_NUM 3
#define UPDATE_STOP_REQ_TOK_NUM 2
#define UPDATE_STOP_REPLY_TOK_NUM 4
#define GET_VERSIONS_REQ_TOK_NUM 3
#define GET_VERSIONS_REPLY_MIN_TOK_NUM 4
#define RESTORE_VER_REQ_TOK_NUM 4
#define RESTORE_VER_REPLY_TOK_NUM 3
#define RESTORE_FILE_TOK_NUM 5
#define RESTORE_FILE_ACK_TOK_NUM 4
#define RESTORE_STOP_TOK_NUM 3
#define PING_TOK_NUM 2

//Message names
#define MSG_NUM 19
std::string messageNames[] = {
	/*  0*/ "USER_REG_REQ",
	/*  1*/ "USER_REG_REPLY",
	/*  2*/ "UPDATE_START_REQ",
	/*  3*/ "UPDATE_START_REPLY",
	/*  4*/ "ADD_NEW_FILE",
	/*  5*/ "UPDATE_FILE",
	/*  6*/ "REMOVE_FILE",
	/*  7*/ "FILE_ACK",
	/*  8*/ "UPDATE_STOP_REQ",
	/*  9*/ "UPDATE_STOP_REPLY",
	/* 10*/ "GET_VERSIONS_REQ",
	/* 11*/ "GET_VERSIONS_REPLY",
	/* 12*/ "RESTORE_VER_REQ",
	/* 13*/ "RESTORE_VER_REPLY",
	/* 14*/ "RESTORE_FILE",
	/* 15*/ "RESTORE_FILE_ACK",
	/* 16*/ "RESTORE_STOP",
	/* 17*/ "PING_REQ",
	/* 18*/ "PING_REPLY"
};

#pragma region "Message Utility"
const bool isValidMessage(const string aName){
	for (int i = 0; i < MSG_NUM; i++){
		if (aName == messageNames[i])
			return true;
	}
	return false;
}

const bool isValidMessageID(const int aID){
	if (aID >= 0 && aID < MSG_NUM)
		return true;
	else
		return false;
}

const string getMessageName(const int aIndex){
	if (aIndex >= 0 && aIndex < MSG_NUM)
		return messageNames[aIndex];
	else
		return MSG_INVALID;
}

// Utility functions to escape/unescape messages
void escape(string_ptr aMsg){
	if (aMsg != NULL){
		string from = to_string(MSG_SEP);
		string to = MSG_SEP_ESC;
		boost::replace_all(*aMsg, from, to);
	}
}

void unescape(string_ptr aMsg){
	if (aMsg != NULL){
		string from = MSG_SEP_ESC;
		string to = to_string(MSG_SEP);
		boost::replace_all(*aMsg, from, to);
	}
}
#pragma endregion


//////////////////////////////////////
//         TBaseMessage	            //
//////////////////////////////////////
#pragma region "TBaseMessage"
TBaseMessage::TBaseMessage(){
	this->fID = NO_ID;
	this->fEncodedMsg = NULL;
}

TBaseMessage::TBaseMessage(const string_ptr aMsg){
	this->fID = NO_ID;
	this->fEncodedMsg = aMsg;
	this->decodeMessageID();
}

void TBaseMessage::decodeMessageID(){
	if (this->fEncodedMsg == NULL)
		throw new EMessageException("fEncodedMsg field is NULL");

	// Turn the string into a stream.
	stringstream ss(*(this->fEncodedMsg));
	string tok;

	getline(ss, tok, MSG_SEP);

	for (int i = 0; i < MSG_NUM; i++){
		if (tok == getMessageName(i)){
			this->fID = i;
			break;
		}
	}
}

const string_ptr TBaseMessage::encodeMessage(){
	int len = (int)this->fItems.size();
	string_ptr msg(new string());
	int i = 0;
	for (i = 0; i < len; i++){
		if (this->fItems[i] == NULL)
			throw new EMessageException("item " + to_string(i) + " is NULL");

		escape(this->fItems[i]);
		*msg += *(this->fItems[i]) + MSG_SEP;
	}

	*msg +=  END_MSG;
	return msg;
}

void TBaseMessage::decodeMessage(){
	if (this->fEncodedMsg == NULL)
		throw new EMessageException("fEncodedMsg field is NULL");

	// Turn the string into a stream.
	stringstream ss(*(this->fEncodedMsg));
	string tok;

	while (getline(ss, tok, MSG_SEP)) {
		string_ptr tok_ptr = make_shared<string>(tok);
		unescape(tok_ptr);
		this->fItems.push_back(tok_ptr);
	}

	int len = (int)this->fItems.size();
	if (*(this->fItems[len - 1]) != END_MSG)
		throw new EMessageException("The given " + *(this->fItems[0]) + " message is not properly terminated");
}
#pragma endregion


////////////////////////////////////////
//      TUserRegistrReqMessage        //
////////////////////////////////////////
#pragma region "TUserRegistrReqMessage"
TUserRegistrReqMessage::TUserRegistrReqMessage(TBaseMessage& aBase){
	this->fID = aBase.getID();
	this->fItems = aBase.getTokens();
	this->fEncodedMsg = aBase.getMsg();

	this->decodeMessage();
}

TUserRegistrReqMessage::TUserRegistrReqMessage(const string aUser, const string aPass){
	this->fID = USER_REG_REQ_ID;
	this->fUser = make_shared<string>(aUser);
	this->fPass = make_shared<string>(aPass);
}

const string_ptr TUserRegistrReqMessage::encodeMessage(){
	this->fItems.push_back(make_shared<string>(getMessageName(this->fID)));
	this->fItems.push_back(this->fUser);
	this->fItems.push_back(this->fPass);

	return TBaseMessage::encodeMessage();
}

void TUserRegistrReqMessage::decodeMessage(){
	/*
	* item[0] -> msg name
	* item[1] -> username
	* item[2] -> password
	*/
	TBaseMessage::decodeMessage();

	int size = (int)this->fItems.size();
	if (size != USER_REG_REQ_TOK_NUM)
		throw new EMessageException("USER_REG_REQ message contains wrong number of tokens(" + to_string(size) + " instead of " + to_string(USER_REG_REQ_TOK_NUM) + ")");

	for (int i = 0; i < size; i++)
		if (this->fItems[i] == NULL)
			throw new EMessageException("item " + to_string(i) + " is NULL");

	if (this->fID != USER_REG_REQ_ID)
		throw new EMessageException("The given message is not a USER_REG_REQ message");

	//username
	if (*(this->fItems[1]) == EMPTY)
		throw new EMessageException("The user name field cannot be empty");
	this->fUser = this->fItems[1];

	//password
	if (*(this->fItems[2]) == EMPTY)
		throw new EMessageException("The password field cannot be empty");
	this->fPass = this->fItems[2];
}
#pragma endregion


////////////////////////////////////////
//      TUserRegistrReplyMessage      //
////////////////////////////////////////
#pragma region "TUserRegistrReplyMessage"
TUserRegistrReplyMessage::TUserRegistrReplyMessage(TBaseMessage& aBase){
	this->fID = aBase.getID();
	this->fItems = aBase.getTokens();
	this->fEncodedMsg = aBase.getMsg();

	this->decodeMessage();
}

TUserRegistrReplyMessage::TUserRegistrReplyMessage(const bool aResp){
	this->fID = USER_REG_REPLY_ID;
	this->fResp = aResp;
}

const string_ptr TUserRegistrReplyMessage::encodeMessage(){
	string resp = "";
	if (this->fResp)
		resp = TRUE_STR;
	else
		resp = FALSE_STR;

	this->fItems.push_back(make_shared<string>(getMessageName(this->fID)));
	this->fItems.push_back(make_shared<string>(resp));

	return TBaseMessage::encodeMessage();
}

void TUserRegistrReplyMessage::decodeMessage(){
	/*
	* item[0] -> msg name
	* item[1] -> response
	*/
	TBaseMessage::decodeMessage();

	int size = (int)this->fItems.size();
	if (size != USER_REG_REPLY_TOK_NUM)
		throw new EMessageException("USER_REG_REPLY message contains wrong number of tokens(" + to_string(size) + " instead of " + to_string(USER_REG_REPLY_TOK_NUM) + ")");

	for (int i = 0; i < size; i++)
		if (this->fItems[i] == NULL)
			throw new EMessageException("item " + to_string(i) + " is NULL");

	if (this->fID != USER_REG_REPLY_ID)
		throw new EMessageException("The given message is not a USER_REG_REPLY message");

	//response
	if (*(this->fItems[1]) == EMPTY)
		throw new EMessageException("The response field cannot be empty");
	if (*(this->fItems[1]) != TRUE_STR && *(this->fItems[1]) != FALSE_STR)
		throw new EMessageException("The response field could be only true or false");

	if (*(this->fItems[1]) == TRUE_STR)
		this->fResp = true;
	else
		this->fResp = false;
}
#pragma endregion


////////////////////////////////////////
//      TUpdateStartReqMessage        //
////////////////////////////////////////
#pragma region "TUpdateStartReqMessage"
TUpdateStartReqMessage::TUpdateStartReqMessage(TBaseMessage& aBase){
	this->fID = aBase.getID();
	this->fItems = aBase.getTokens();
	this->fEncodedMsg = aBase.getMsg();

	this->decodeMessage();
}

TUpdateStartReqMessage::TUpdateStartReqMessage(const string aUser, const string aPass){
	this->fID = UPDATE_START_REQ_ID;
	this->fUser = make_shared<string>(aUser);
	this->fPass = make_shared<string>(aPass);
}

const string_ptr TUpdateStartReqMessage::encodeMessage(){
	this->fItems.push_back(make_shared<string>(getMessageName(this->fID)));
	this->fItems.push_back(this->fUser);
	this->fItems.push_back(this->fPass);

	return TBaseMessage::encodeMessage();
}

void TUpdateStartReqMessage::decodeMessage(){
	/*
	* item[0] -> msg name
	* item[1] -> username
	* item[2] -> password
	*/
	TBaseMessage::decodeMessage();

	int size = (int)this->fItems.size();
	if (size != UPDATE_START_REQ_TOK_NUM)
		throw new EMessageException("UPDATE_START_REQ message contains wrong number of tokens(" + to_string(size) + " instead of " + to_string(UPDATE_START_REQ_TOK_NUM) + ")");

	for (int i = 0; i < size; i++)
		if (this->fItems[i] == NULL)
			throw new EMessageException("item " + to_string(i) + " is NULL");

	if (this->fID != UPDATE_START_REQ_ID)
		throw new EMessageException("The given message is not a UPDATE_START_REQ message");

	//username
	if (*(this->fItems[1]) == EMPTY)
		throw new EMessageException("The user name field cannot be empty");
	this->fUser = this->fItems[1];

	//password
	if (*(this->fItems[2]) == EMPTY)
		throw new EMessageException("The password field cannot be empty");
	this->fPass = this->fItems[2];
}
#pragma endregion


////////////////////////////////////////
//      TUpdateStartReplyMessage      //
////////////////////////////////////////
#pragma region "TUpdateStartReplyMessage"
TUpdateStartReplyMessage::TUpdateStartReplyMessage(TBaseMessage& aBase){
	this->fID = aBase.getID();
	this->fItems = aBase.getTokens();
	this->fEncodedMsg = aBase.getMsg();

	this->decodeMessage();
}

TUpdateStartReplyMessage::TUpdateStartReplyMessage(const bool aResp, const string aToken){
	this->fID = UPDATE_START_REPLY_ID;
	this->fResp = aResp;
	this->fToken = make_shared<string>(aToken);
}

const string_ptr TUpdateStartReplyMessage::encodeMessage(){
	string resp = "";
	if (this->fResp)
		resp = TRUE_STR;
	else
		resp = FALSE_STR;

	this->fItems.push_back(make_shared<string>(getMessageName(this->fID)));
	this->fItems.push_back(make_shared<string>(resp));
	this->fItems.push_back(this->fToken);

	return TBaseMessage::encodeMessage();
}

void TUpdateStartReplyMessage::decodeMessage(){
	/*
	* item[0] -> msg name
	* item[1] -> response
	* item[2] -> user token
	*/
	TBaseMessage::decodeMessage();

	int size = (int)this->fItems.size();
	if (size != UPDATE_START_REPLY_TOK_NUM)
		throw new EMessageException("UPDATE_START_REPLY message contains wrong number of tokens(" + to_string(size) + " instead of " + to_string(UPDATE_START_REPLY_TOK_NUM) + ")");

	for (int i = 0; i < size; i++)
		if (this->fItems[i] == NULL)
			throw new EMessageException("item " + to_string(i) + " is NULL");

	if (this->fID != UPDATE_START_REPLY_ID)
		throw new EMessageException("The given message is not a UPDATE_START_REPLY message");

	//response
	if (*(this->fItems[1]) == EMPTY)
		throw new EMessageException("The response field cannot be empty");
	if (*(this->fItems[1]) != TRUE_STR && *(this->fItems[1]) != FALSE_STR)
		throw new EMessageException("The response field could be only true or false");

	if (*(this->fItems[1]) == TRUE_STR)
		this->fResp = true;
	else
		this->fResp = false;

	//user token
	if (*(this->fItems[2]) == EMPTY)
		throw new EMessageException("The token field cannot be empty");
	this->fToken = this->fItems[2]; //TODO: implement -> checkToken(this->fItems[2]);
}
#pragma endregion


////////////////////////////////////////
//        TAddNewFileMessage          //
////////////////////////////////////////
#pragma region "TAddNewFileMessage"
TAddNewFileMessage::TAddNewFileMessage(TBaseMessage& aBase){
	this->fID = aBase.getID();
	this->fItems = aBase.getTokens();
	this->fEncodedMsg = aBase.getMsg();

	this->decodeMessage();
}

TAddNewFileMessage::TAddNewFileMessage(const string aToken, string aFilePath){
	this->fID = ADD_NEW_FILE_ID;
	this->fToken = make_shared<string>(aToken);
	this->fFilePath = make_shared<string>(aFilePath);
	this->fFileContent = NULL;
	this->fChecksum = NULL;
	this->fFileDate = time(NULL);
}

const string_ptr TAddNewFileMessage::encodeMessage(){
	this->fItems.push_back(make_shared<string>(getMessageName(this->fID)));
	this->fItems.push_back(this->fToken);
	this->fItems.push_back(this->fFilePath);

	//read file, encode it and calculate checksum
	this->fFileContent = opensslB64EncodeFile(*(this->fFilePath));
	this->fChecksum = opensslB64Checksum(this->fFileContent);

	this->fItems.push_back(this->fChecksum);
	this->fItems.push_back(make_shared<string>(timeToString(this->fFileDate)));
	this->fItems.push_back(this->fFileContent);

	return TBaseMessage::encodeMessage();
}

void TAddNewFileMessage::decodeMessage(){
	/*
	* item[0] -> msg name
	* item[1] -> token
	* item[2] -> file path
	* item[3] -> file checksum
	* item[4] -> file date
	* item[5] -> file content
	*/
	TBaseMessage::decodeMessage();

	int size = (int)this->fItems.size();
	if (size != ADD_NEW_FILE_TOK_NUM)
		throw new EMessageException("ADD_NEW_FILE message contains wrong number of tokens(" + to_string(size) + " instead of " + to_string(ADD_NEW_FILE_TOK_NUM) + ")");

	for (int i = 0; i < size; i++)
		if (this->fItems[i] == NULL)
			throw new EMessageException("item " + to_string(i) + " is NULL");

	//message id
	if (this->fID != ADD_NEW_FILE_ID)
		throw new EMessageException("The given message is not a ADD_NEW_FILE message");

	//user token
	if (*(this->fItems[1]) == EMPTY)
		throw new EMessageException("The token field cannot be empty");
	this->fToken = this->fItems[1]; //TODO: implement -> checkToken(this->fItems[2]);

	//file path
	if (*(this->fItems[2]) == EMPTY)
		throw new EMessageException("The token field cannot be empty");
	if (*(this->fItems[2]) == EMPTY)
		throw new EMessageException("The file path field cannot be empty");
	//TODO: check se il path è valido
	//if (!isValidPath(this->fFilePath))
	//	throw new EMessageException("The file path field must contain a valid path");
	this->fFilePath = this->fItems[2];

	//file checksum
	if (*(this->fItems[3]) == EMPTY)
		throw new EMessageException("The file checksum field cannot be empty");
	this->fChecksum = this->fItems[3];

	//file date
	if (*(this->fItems[4]) == EMPTY)
		throw new EMessageException("The file date field cannot be empty");
	this->fFileDate = stringToTime(*(this->fItems[4]));

	//file content
	if (*(this->fItems[5]) == EMPTY)
		throw new EMessageException("The file content field cannot be empty");
	this->fFileContent = this->fItems[5];

	//verify file correctness
	string_ptr myChecksum = opensslB64Checksum(this->fFileContent);
	if (*(myChecksum) != *(this->fChecksum))
		throw new EMessageException("The received file could be corrupted; checksums do not match");
}
#pragma endregion


////////////////////////////////////////
//        TUpdateFileMessage          //
////////////////////////////////////////
#pragma region "TUpdateFileMessage"
TUpdateFileMessage::TUpdateFileMessage(TBaseMessage& aBase){
	this->fID = aBase.getID();
	this->fItems = aBase.getTokens();
	this->fEncodedMsg = aBase.getMsg();

	this->decodeMessage();
}

TUpdateFileMessage::TUpdateFileMessage(const string aToken, string aFilePath){
	this->fID = UPDATE_FILE_ID;
	this->fToken = make_shared<string>(aToken);
	this->fFilePath = make_shared<string>(aFilePath);
	this->fFileContent = NULL;
	this->fChecksum = NULL;
	this->fFileDate = time(NULL);
}

const string_ptr TUpdateFileMessage::encodeMessage(){
	this->fItems.push_back(make_shared<string>(getMessageName(this->fID)));
	this->fItems.push_back(this->fToken);
	this->fItems.push_back(this->fFilePath);

	//read file, encode it and calculate checksum
	this->fFileContent = opensslB64EncodeFile(*(this->fFilePath));
	this->fChecksum = opensslB64Checksum(this->fFileContent);

	this->fItems.push_back(this->fChecksum);
	this->fItems.push_back(make_shared<string>(timeToString(this->fFileDate)));
	this->fItems.push_back(this->fFileContent);

	return TBaseMessage::encodeMessage();
}

void TUpdateFileMessage::decodeMessage(){
	/*
	* item[0] -> msg name
	* item[1] -> token
	* item[2] -> file path
	* item[3] -> file checksum
	* item[4] -> file date
	* item[5] -> file content
	*/
	TBaseMessage::decodeMessage();

	int size = (int)this->fItems.size();
	if (size != UPDATE_FILE_TOK_NUM)
		throw new EMessageException("UPDATE_FILE message contains wrong number of tokens(" + to_string(size) + " instead of " + to_string(UPDATE_FILE_TOK_NUM) + ")");

	for (int i = 0; i < size; i++)
		if (this->fItems[i] == NULL)
			throw new EMessageException("item " + to_string(i) + " is NULL");

	//message id
	if (this->fID != UPDATE_FILE_ID)
		throw new EMessageException("The given message is not a UPDATE_FILE message");

	//user token
	if (*(this->fItems[1]) == EMPTY)
		throw new EMessageException("The token field cannot be empty");
	this->fToken = this->fItems[1]; //TODO: implement -> checkToken(this->fItems[2]);

	//file path
	if (*(this->fItems[2]) == EMPTY)
		throw new EMessageException("The token field cannot be empty");
	if (*(this->fItems[2]) == EMPTY)
		throw new EMessageException("The file path field cannot be empty");
	//TODO: check se il path è valido
	//if (!isValidPath(this->fFilePath))
	//	throw new EMessageException("The file path field must contain a valid path");
	this->fFilePath = this->fItems[2];

	//file checksum
	if (*(this->fItems[3]) == EMPTY)
		throw new EMessageException("The file checksum field cannot be empty");
	this->fChecksum = this->fItems[3];

	//file date
	if (*(this->fItems[4]) == EMPTY)
		throw new EMessageException("The file date field cannot be empty");
	this->fFileDate = stringToTime(*(this->fItems[4]));

	//file content
	if (*(this->fItems[5]) == EMPTY)
		throw new EMessageException("The file content field cannot be empty");
	this->fFileContent = this->fItems[5];

	//verify file correctness
	string_ptr myChecksum = opensslB64Checksum(this->fFileContent);
	if (*(myChecksum) != *(this->fChecksum))
		throw new EMessageException("The received file could be corrupted; checksums do not match");
}
#pragma endregion


////////////////////////////////////////
//        TRemoveFileMessage          //
////////////////////////////////////////
#pragma region "TRemoveFileMessage"
TRemoveFileMessage::TRemoveFileMessage(TBaseMessage& aBase){
	this->fID = aBase.getID();
	this->fItems = aBase.getTokens();
	this->fEncodedMsg = aBase.getMsg();

	this->decodeMessage();
}

TRemoveFileMessage::TRemoveFileMessage(const string aToken, string aFilePath){
	this->fID = REMOVE_FILE_ID;
	this->fToken = make_shared<string>(aToken);
	this->fFilePath = make_shared<string>(aFilePath);
}

const string_ptr TRemoveFileMessage::encodeMessage(){
	this->fItems.push_back(make_shared<string>(getMessageName(this->fID)));
	this->fItems.push_back(this->fToken);
	this->fItems.push_back(this->fFilePath);

	return TBaseMessage::encodeMessage();
}

void TRemoveFileMessage::decodeMessage(){
	/*
	* item[0] -> msg name
	* item[1] -> token
	* item[2] -> file path
	*/
	TBaseMessage::decodeMessage();

	int size = (int)this->fItems.size();
	if (size != REMOVE_FILE_TOK_NUM)
		throw new EMessageException("REMOVE_FILE message contains wrong number of tokens(" + to_string(size) + " instead of " + to_string(REMOVE_FILE_TOK_NUM) + ")");

	for (int i = 0; i < size; i++)
		if (this->fItems[i] == NULL)
			throw new EMessageException("item " + to_string(i) + " is NULL");

	//message id
	if (this->fID != REMOVE_FILE_ID)
		throw new EMessageException("The given message is not a REMOVE_FILE message");

	//user token
	if (*(this->fItems[1]) == EMPTY)
		throw new EMessageException("The token field cannot be empty");
	this->fToken = this->fItems[1]; //TODO: implement -> checkToken(this->fItems[2]);

	//file path
	if (*(this->fItems[2]) == EMPTY)
		throw new EMessageException("The token field cannot be empty");
	if (*(this->fItems[2]) == EMPTY)
		throw new EMessageException("The file path field cannot be empty");
	//TODO: check se il path è valido
	//if (!isValidPath(this->fFilePath))
	//	throw new EMessageException("The file path field must contain a valid path");
	this->fFilePath = this->fItems[2];
}
#pragma endregion


///////////////////////////////////
//        TFileAckMessage        //
///////////////////////////////////
#pragma region "TFileAckMessage"
TFileAckMessage::TFileAckMessage(TBaseMessage& aBase){
	this->fID = aBase.getID();
	this->fItems = aBase.getTokens();
	this->fEncodedMsg = aBase.getMsg();

	this->decodeMessage();
}

TFileAckMessage::TFileAckMessage(const bool aResp, const string aFilePath){
	this->fID = FILE_ACK_ID;
	this->fResp = aResp;
	this->fFilePath = make_shared<string>(aFilePath);
}

const string_ptr TFileAckMessage::encodeMessage(){
	string resp = "";
	if (this->fResp)
		resp = TRUE_STR;
	else
		resp = FALSE_STR;

	this->fItems.push_back(make_shared<string>(getMessageName(this->fID)));
	this->fItems.push_back(make_shared<string>(resp));
	this->fItems.push_back(this->fFilePath);

	return TBaseMessage::encodeMessage();
}

void TFileAckMessage::decodeMessage(){
	/*
	* item[0] -> msg name
	* item[1] -> response
	* item[2] -> file path
	*/
	TBaseMessage::decodeMessage();

	int size = (int)this->fItems.size();
	if (size != FILE_ACK_TOK_NUM)
		throw new EMessageException("FILE_ACK message contains wrong number of tokens(" + to_string(size) + " instead of " + to_string(FILE_ACK_TOK_NUM) + ")");

	for (int i = 0; i < size; i++)
		if (this->fItems[i] == NULL)
			throw new EMessageException("item " + to_string(i) + " is NULL");

	if (this->fID != FILE_ACK_ID)
		throw new EMessageException("The given message is not a FILE_ACK message");

	//response
	if (*(this->fItems[1]) == EMPTY)
		throw new EMessageException("The response field cannot be empty");
	if (*(this->fItems[1]) != TRUE_STR && *(this->fItems[1]) != FALSE_STR)
		throw new EMessageException("The response field could be only true or false");

	if (*(this->fItems[1]) == TRUE_STR)
		this->fResp = true;
	else
		this->fResp = false;

	//file path
	if (*(this->fItems[2]) == EMPTY)
		throw new EMessageException("The token field cannot be empty");
	//TODO: check se il path è valido
	//if (!isValidPath(this->fFilePath))
	//	throw new EMessageException("The file path field must contain a valid path");
	this->fFilePath = this->fItems[2];
}
#pragma endregion


////////////////////////////////////////
//       TUpdateStopReqMessage        //
////////////////////////////////////////
#pragma region "TUpdateStopReqMessage"
TUpdateStopReqMessage::TUpdateStopReqMessage(TBaseMessage& aBase){
	this->fID = aBase.getID();
	this->fItems = aBase.getTokens();
	this->fEncodedMsg = aBase.getMsg();

	this->decodeMessage();
}

TUpdateStopReqMessage::TUpdateStopReqMessage(const string aToken){
	this->fID = UPDATE_STOP_REQ_ID;
	this->fToken = make_shared<string>(aToken);
}

const string_ptr TUpdateStopReqMessage::encodeMessage(){
	this->fItems.push_back(make_shared<string>(getMessageName(this->fID)));
	this->fItems.push_back(this->fToken);

	return TBaseMessage::encodeMessage();
}

void TUpdateStopReqMessage::decodeMessage(){
	/*
	* item[0] -> msg name
	* item[1] -> user token
	*/
	TBaseMessage::decodeMessage();

	int size = (int)this->fItems.size();
	if (size != UPDATE_STOP_REQ_TOK_NUM)
		throw new EMessageException("UPDATE_STOP_REQ message contains wrong number of tokens(" + to_string(size) + " instead of " + to_string(UPDATE_STOP_REQ_TOK_NUM) + ")");

	for (int i = 0; i < size; i++)
		if (this->fItems[i] == NULL)
			throw new EMessageException("item " + to_string(i) + " is NULL");

	if (this->fID != UPDATE_STOP_REQ_ID)
		throw new EMessageException("The given message is not a UPDATE_STOP_REQ message");

	//user token
	if (*(this->fItems[1]) == EMPTY)
		throw new EMessageException("The token field cannot be empty");
	this->fToken = this->fItems[1]; //TODO: implement -> checkToken(this->fItems[2]);
}
#pragma endregion


////////////////////////////////////////
//      TUpdateStopReplyMessage       //
////////////////////////////////////////
#pragma region "TUpdateStopReplyMessage"
TUpdateStopReplyMessage::TUpdateStopReplyMessage(TBaseMessage& aBase){
	this->fID = aBase.getID();
	this->fItems = aBase.getTokens();
	this->fEncodedMsg = aBase.getMsg();

	this->decodeMessage();
}

TUpdateStopReplyMessage::TUpdateStopReplyMessage(const bool aResp, unsigned int aVersion, time_t aTime){
	this->fID = UPDATE_STOP_REPLY_ID;
	this->fResp = aResp;
	this->fVersion = aVersion;
	this->fTime = aTime;
}

const string_ptr TUpdateStopReplyMessage::encodeMessage(){
	string resp = "";
	if (this->fResp)
		resp = TRUE_STR;
	else
		resp = FALSE_STR;

	this->fItems.push_back(make_shared<string>(getMessageName(this->fID)));
	this->fItems.push_back(make_shared<string>(resp));
	this->fItems.push_back(make_shared<string>(to_string(this->fVersion)));
	this->fItems.push_back(make_shared<string>(timeToString(this->fTime)));

	return TBaseMessage::encodeMessage();
}

void TUpdateStopReplyMessage::decodeMessage(){
	/*
	* item[0] -> msg name
	* item[1] -> response
	* item[2] -> version number
	* item[3] -> version timestamp
	*/
	TBaseMessage::decodeMessage();

	int size = (int)this->fItems.size();
	if (size != UPDATE_STOP_REPLY_TOK_NUM)
		throw new EMessageException("UPDATE_STOP_REPLY message contains wrong number of tokens(" + to_string(size) + " instead of " + to_string(UPDATE_STOP_REPLY_TOK_NUM) + ")");

	for (int i = 0; i < size; i++)
		if (this->fItems[i] == NULL)
			throw new EMessageException("item " + to_string(i) + " is NULL");

	if (this->fID != UPDATE_STOP_REPLY_ID)
		throw new EMessageException("The given message is not a UPDATE_STOP_REPLY message");

	//response
	if (*(this->fItems[1]) == EMPTY)
		throw new EMessageException("The response field cannot be empty");
	if (*(this->fItems[1]) != TRUE_STR && *(this->fItems[1]) != FALSE_STR)
		throw new EMessageException("The response field could be only true or false");

	if (*(this->fItems[1]) == TRUE_STR)
		this->fResp = true;
	else
		this->fResp = false;

	//version number
	if (*(this->fItems[2]) == EMPTY)
		throw new EMessageException("The version field cannot be empty");
	//TODO: check if this->fItems[2] contains a number
	this->fVersion = stoi(*(this->fItems[2]));

	//version timestamp
	if (*(this->fItems[3]) == EMPTY)
		throw new EMessageException("The version timestamp field cannot be empty");
	this->fTime = stringToTime(*(this->fItems[3]));
}
#pragma endregion


////////////////////////////////////////
//      TGetVersionsReqMessage        //
////////////////////////////////////////
#pragma region "TGetVersionsReqMessage"
TGetVersionsReqMessage::TGetVersionsReqMessage(TBaseMessage& aBase){
	this->fID = aBase.getID();
	this->fItems = aBase.getTokens();
	this->fEncodedMsg = aBase.getMsg();

	this->decodeMessage();
}

TGetVersionsReqMessage::TGetVersionsReqMessage(const string aUser, const string aPass){
	this->fID = GET_VERSIONS_REQ_ID;
	this->fUser = make_shared<string>(aUser);
	this->fPass = make_shared<string>(aPass);
}

const string_ptr TGetVersionsReqMessage::encodeMessage(){
	this->fItems.push_back(make_shared<string>(getMessageName(this->fID)));
	this->fItems.push_back(this->fUser);
	this->fItems.push_back(this->fPass);

	return TBaseMessage::encodeMessage();
}

void TGetVersionsReqMessage::decodeMessage(){
	/*
	* item[0] -> msg name
	* item[1] -> username
	* item[2] -> password
	*/
	TBaseMessage::decodeMessage();
	
	int size = (int)this->fItems.size();
	if (size != GET_VERSIONS_REQ_TOK_NUM)
		throw new EMessageException("GET_VERSIONS_REQ message contains wrong number of tokens(" + to_string(size) + " instead of " + to_string(GET_VERSIONS_REQ_TOK_NUM) + ")");

	for (int i = 0; i < size; i++)
		if (this->fItems[i] == NULL)
			throw new EMessageException("item " + to_string(i) + " is NULL");

	if (this->fID != GET_VERSIONS_REQ_ID)
		throw new EMessageException("The given message is not a GET_VERSIONS_REQ message");

	//username
	if (*(this->fItems[1]) == EMPTY)
		throw new EMessageException("The user name field cannot be empty");
	this->fUser = this->fItems[1];

	//password
	if (*(this->fItems[2]) == EMPTY)
		throw new EMessageException("The password field cannot be empty");
	this->fPass = this->fItems[2];
}
#pragma endregion


////////////////////////////////////////
//      TGetVersionsReplyMessage      //
////////////////////////////////////////
#pragma region "TGetVersionsReplyMessage"
TGetVersionsReplyMessage::TGetVersionsReplyMessage(TBaseMessage& aBase){
	this->fID = aBase.getID();
	this->fItems = aBase.getTokens();
	this->fEncodedMsg = aBase.getMsg();

	this->decodeMessage();
}

TGetVersionsReplyMessage::TGetVersionsReplyMessage(const unsigned int aTotVersions, const unsigned int aOldestVersion, const unsigned int aLastVersion, map<unsigned int, time_t> aVersions){
	this->fID = GET_VERSIONS_REPLY_ID;
	this->fTotVersions = aTotVersions;
	this->fOldestVersion = aOldestVersion;
	this->fLastVersion = aLastVersion;
	this->fVersions = aVersions;
}

const string_ptr TGetVersionsReplyMessage::encodeMessage(){
	this->fItems.push_back(make_shared<string>(getMessageName(this->fID)));
	this->fItems.push_back(make_shared<string>(to_string(this->fTotVersions)));
	this->fItems.push_back(make_shared<string>(to_string(this->fOldestVersion)));
	this->fItems.push_back(make_shared<string>(to_string(this->fLastVersion)));

	int size = (int)this->fVersions.size();
	for (int i = GET_VERSIONS_REPLY_MIN_TOK_NUM + 1; i < size; i++){
		this->fItems.push_back(make_shared<string>(timeToString(this->fVersions.at(i))));
	}

	return TBaseMessage::encodeMessage();
}

void TGetVersionsReplyMessage::decodeMessage(){
	/*
	* item[0] -> msg name
	* item[1] -> number of versions
	* item[2] -> oldest version on server
	* item[3] -> last version
	* item[4..n] -> versions timestamps
	*/
	TBaseMessage::decodeMessage();

	int size = (int)this->fItems.size();
	if (size < GET_VERSIONS_REPLY_MIN_TOK_NUM)
		throw new EMessageException("GET_VERSIONS_REPLY message does not contain the minimum number of tokens(" + to_string(GET_VERSIONS_REPLY_MIN_TOK_NUM) + ")");

	for (int i = 0; i < size; i++)
		if (this->fItems[i] == NULL)
			throw new EMessageException("item " + to_string(i) + " is NULL");

	if (this->fID != GET_VERSIONS_REPLY_ID)
		throw new EMessageException("The given message is not a GET_VERSIONS_REPLY message");

	//tot number of versions
	if (*(this->fItems[1]) == EMPTY)
		throw new EMessageException("The total versions number field cannot be empty");
	//TODO: check if this->fItems[1] contains a number
	this->fTotVersions = stoi(*(this->fItems[1]));

	//oldest version
	if (*(this->fItems[2]) == EMPTY)
		throw new EMessageException("The oldest version number field cannot be empty");
	//TODO: check if this->fItems[2] contains a number
	this->fOldestVersion = stoi(*(this->fItems[2]));

	//last version
	if (*(this->fItems[3]) == EMPTY)
		throw new EMessageException("The last version number field cannot be empty");
	//TODO: check if this->fItems[3] contains a number
	this->fLastVersion = stoi(*(this->fItems[3]));

	int totSize = GET_VERSIONS_REPLY_MIN_TOK_NUM + (this->fLastVersion - this->fOldestVersion);
	if (size != totSize)
		throw new EMessageException("GET_VERSIONS_REPLY message contains wrong number of tokens(" + to_string(size) + " instead of " + to_string(totSize) + ")");

	int i, j;
	for (i = GET_VERSIONS_REPLY_MIN_TOK_NUM + 1, j = this->fOldestVersion; i < totSize; i++, j++){
		this->fVersions.emplace(j, stringToTime(*(this->fItems[i])));
	}
}
#pragma endregion


////////////////////////////////////////
//       TRestoreVerReqMessage        //
////////////////////////////////////////
#pragma region "TRestoreVerReqMessage"
TRestoreVerReqMessage::TRestoreVerReqMessage(TBaseMessage& aBase){
	this->fID = aBase.getID();
	this->fItems = aBase.getTokens();
	this->fEncodedMsg = aBase.getMsg();

	this->decodeMessage();
}

TRestoreVerReqMessage::TRestoreVerReqMessage(const string aUser, const string aPass, const unsigned int aVersion){
	this->fID = RESTORE_VER_REQ_ID;
	this->fUser = make_shared<string>(aUser);
	this->fPass = make_shared<string>(aPass);
	this->fVersion = aVersion;
}

const string_ptr TRestoreVerReqMessage::encodeMessage(){
	this->fItems.push_back(make_shared<string>(getMessageName(this->fID)));
	this->fItems.push_back(this->fUser);
	this->fItems.push_back(this->fPass);
	this->fItems.push_back(make_shared<string>(to_string(this->fVersion)));

	return TBaseMessage::encodeMessage();
}

void TRestoreVerReqMessage::decodeMessage(){
	/*
	* item[0] -> msg name
	* item[1] -> username
	* item[2] -> password
	* item[3] -> version
	*/
	TBaseMessage::decodeMessage();
	
	int size = (int)this->fItems.size();
	if (size != RESTORE_VER_REQ_TOK_NUM)
		throw new EMessageException("RESTORE_VER_REQ message contains wrong number of tokens(" + to_string(size) + " instead of " + to_string(RESTORE_VER_REQ_TOK_NUM) + ")");

	for (int i = 0; i < size; i++)
		if (this->fItems[i] == NULL)
			throw new EMessageException("item " + to_string(i) + " is NULL");

	if (this->fID != RESTORE_VER_REQ_ID)
		throw new EMessageException("The given message is not a RESTORE_VER_REQ message");

	//username
	if (*(this->fItems[1]) == EMPTY)
		throw new EMessageException("The user name field cannot be empty");
	this->fUser = this->fItems[1];

	//password
	if (*(this->fItems[2]) == EMPTY)
		throw new EMessageException("The password field cannot be empty");
	this->fPass = this->fItems[2];


	//version
	if (*(this->fItems[3]) == EMPTY)
		throw new EMessageException("The version number field cannot be empty");
	//TODO: check if this->fItems[3] contains a number
	this->fVersion = stoi(*(this->fItems[3]));
}
#pragma endregion


////////////////////////////////////////
//      TRestoreVerReplyMessage       //
////////////////////////////////////////
#pragma region "TRestoreVerReplyMessage"
TRestoreVerReplyMessage::TRestoreVerReplyMessage(TBaseMessage& aBase){
	this->fID = aBase.getID();
	this->fItems = aBase.getTokens();
	this->fEncodedMsg = aBase.getMsg();

	this->decodeMessage();
}

TRestoreVerReplyMessage::TRestoreVerReplyMessage(const bool aResp, const string aToken){
	this->fID = RESTORE_VER_REPLY_ID;
	this->fResp = aResp;
	this->fToken = make_shared<string>(aToken);
}

const string_ptr TRestoreVerReplyMessage::encodeMessage(){
	string resp = "";
	if (this->fResp)
		resp = TRUE_STR;
	else
		resp = FALSE_STR;

	this->fItems.push_back(make_shared<string>(getMessageName(this->fID)));
	this->fItems.push_back(make_shared<string>(resp));
	this->fItems.push_back(this->fToken);

	return TBaseMessage::encodeMessage();
}

void TRestoreVerReplyMessage::decodeMessage(){
	/*
	* item[0] -> msg name
	* item[1] -> response
	* item[2] -> user token
	*/
	TBaseMessage::decodeMessage();

	int size = (int)this->fItems.size();
	if (size != RESTORE_VER_REPLY_TOK_NUM)
		throw new EMessageException("RESTORE_VER_REPLY message contains wrong number of tokens(" + to_string(size) + " instead of " + to_string(RESTORE_VER_REPLY_TOK_NUM) + ")");

	for (int i = 0; i < size; i++)
		if (this->fItems[i] == NULL)
			throw new EMessageException("item " + to_string(i) + " is NULL");

	if (this->fID != RESTORE_VER_REPLY_ID)
		throw new EMessageException("The given message is not a UPDATE_START_REPLY message");

	//response
	if (*(this->fItems[1]) == EMPTY)
		throw new EMessageException("The response field cannot be empty");
	if (*(this->fItems[1]) != TRUE_STR && *(this->fItems[1]) != FALSE_STR)
		throw new EMessageException("The response field could be only true or false");

	if (*(this->fItems[1]) == TRUE_STR)
		this->fResp = true;
	else
		this->fResp = false;

	//user token
	if (*(this->fItems[2]) == EMPTY)
		throw new EMessageException("The token field cannot be empty");
	this->fToken = this->fItems[2]; //TODO: implement -> checkToken(this->fItems[2]);
}
#pragma endregion


////////////////////////////////////////
//        TRestoreFileMessage         //
////////////////////////////////////////
#pragma region "TRestoreFileMessage"
TRestoreFileMessage::TRestoreFileMessage(TBaseMessage& aBase){
	this->fID = aBase.getID();
	this->fItems = aBase.getTokens();
	this->fEncodedMsg = aBase.getMsg();

	this->decodeMessage();
}

TRestoreFileMessage::TRestoreFileMessage(string aFilePath){
	this->fID = RESTORE_FILE_ID;
	this->fFilePath = make_shared<string>(aFilePath);
	this->fFileContent = NULL;
	this->fChecksum = NULL;
	this->fFileDate = time(NULL);
}

const string_ptr TRestoreFileMessage::encodeMessage(){
	this->fItems.push_back(make_shared<string>(getMessageName(this->fID)));
	this->fItems.push_back(this->fFilePath);

	//read file, encode it and calculate checksum
	this->fFileContent = opensslB64EncodeFile(*(this->fFilePath));
	this->fChecksum = opensslB64Checksum(this->fFileContent);

	this->fItems.push_back(this->fChecksum);
	this->fItems.push_back(make_shared<string>(timeToString(this->fFileDate)));
	this->fItems.push_back(this->fFileContent);

	return TBaseMessage::encodeMessage();
}

void TRestoreFileMessage::decodeMessage(){
	/*
	* item[0] -> msg name
	* item[1] -> file path
	* item[2] -> file checksum
	* item[3] -> file date
	* item[4] -> file content
	*/
	TBaseMessage::decodeMessage();

	int size = (int)this->fItems.size();
	if (size != RESTORE_FILE_TOK_NUM)
		throw new EMessageException("RESTORE_FILE message contains wrong number of tokens(" + to_string(size) + " instead of " + to_string(RESTORE_FILE_TOK_NUM) + ")");

	for (int i = 0; i < size; i++)
		if (this->fItems[i] == NULL)
			throw new EMessageException("item " + to_string(i) + " is NULL");

	//message id
	if (this->fID != RESTORE_FILE_ID)
		throw new EMessageException("The given message is not a RESTORE_FILE message");

	//file path
	if (*(this->fItems[2]) == EMPTY)
		throw new EMessageException("The token field cannot be empty");
	if (*(this->fItems[2]) == EMPTY)
		throw new EMessageException("The file path field cannot be empty");
	//TODO: check se il path è valido
	//if (!isValidPath(this->fFilePath))
	//	throw new EMessageException("The file path field must contain a valid path");
	this->fFilePath = this->fItems[2];

	//file checksum
	if (*(this->fItems[3]) == EMPTY)
		throw new EMessageException("The file checksum field cannot be empty");
	this->fChecksum = this->fItems[3];

	//file date
	if (*(this->fItems[4]) == EMPTY)
		throw new EMessageException("The file date field cannot be empty");
	this->fFileDate = stringToTime(*(this->fItems[4]));

	//file content
	if (*(this->fItems[5]) == EMPTY)
		throw new EMessageException("The file content field cannot be empty");
	this->fFileContent = this->fItems[5];

	//verify file correctness
	string_ptr myChecksum = opensslB64Checksum(this->fFileContent);
	if (*(myChecksum) != *(this->fChecksum))
		throw new EMessageException("The received file could be corrupted; checksums do not match");
}
#pragma endregion


//////////////////////////////////////////
//        TRestoreFileAckMessage        //
//////////////////////////////////////////
#pragma region "TRestoreFileAckMessage"
TRestoreFileAckMessage::TRestoreFileAckMessage(TBaseMessage& aBase){
	this->fID = aBase.getID();
	this->fItems = aBase.getTokens();
	this->fEncodedMsg = aBase.getMsg();

	this->decodeMessage();
}

TRestoreFileAckMessage::TRestoreFileAckMessage(const string aToken, const bool aResp, const string aFilePath){
	this->fID = RESTORE_FILE_ACK_ID;
	this->fToken = make_shared<string>(aToken);
	this->fFilePath = make_shared<string>(aFilePath);
	this->fResp = aResp;
}

const string_ptr TRestoreFileAckMessage::encodeMessage(){
	string resp = "";
	if (this->fResp)
		resp = TRUE_STR;
	else
		resp = FALSE_STR;

	this->fItems.push_back(make_shared<string>(getMessageName(this->fID)));
	this->fItems.push_back(this->fToken);
	this->fItems.push_back(make_shared<string>(resp));
	this->fItems.push_back(this->fFilePath);

	return TBaseMessage::encodeMessage();
}

void TRestoreFileAckMessage::decodeMessage(){
	/*
	* item[0] -> msg name
	* item[1] -> user token
	* item[2] -> response
	* item[3] -> file path
	*/
	TBaseMessage::decodeMessage();

	int size = (int)this->fItems.size();
	if (size != RESTORE_FILE_ACK_TOK_NUM)
		throw new EMessageException("RESTORE_FILE_ACK message contains wrong number of tokens(" + to_string(size) + " instead of " + to_string(RESTORE_FILE_ACK_TOK_NUM) + ")");

	for (int i = 0; i < size; i++)
		if (this->fItems[i] == NULL)
			throw new EMessageException("item " + to_string(i) + " is NULL");

	if (this->fID != RESTORE_FILE_ACK_ID)
		throw new EMessageException("The given message is not a RESTORE_FILE_ACK message");

	//user token
	if (*(this->fItems[1]) == EMPTY)
		throw new EMessageException("The token field cannot be empty");
	this->fToken = this->fItems[1]; //TODO: implement -> checkToken(this->fItems[1]);

	//response
	if (*(this->fItems[1]) == EMPTY)
		throw new EMessageException("The response field cannot be empty");
	if (*(this->fItems[1]) != TRUE_STR && *(this->fItems[1]) != FALSE_STR)
		throw new EMessageException("The response field could be only true or false");

	if (*(this->fItems[1]) == TRUE_STR)
		this->fResp = true;
	else
		this->fResp = false;

	//file path
	if (*(this->fItems[3]) == EMPTY)
		throw new EMessageException("The token field cannot be empty");
	//TODO: check se il path è valido
	//if (!isValidPath(this->fFilePath))
	//	throw new EMessageException("The file path field must contain a valid path");
	this->fFilePath = this->fItems[3];
}
#pragma endregion


////////////////////////////////////
//      TRestoreStopMessage       //
////////////////////////////////////
#pragma region "TRestoreStopMessage"
TRestoreStopMessage::TRestoreStopMessage(TBaseMessage& aBase){
	this->fID = aBase.getID();
	this->fItems = aBase.getTokens();
	this->fEncodedMsg = aBase.getMsg();

	this->decodeMessage();
}

TRestoreStopMessage::TRestoreStopMessage(unsigned int aVersion, time_t aTime){
	this->fID = RESTORE_STOP_ID;
	this->fVersion = aVersion;
	this->fTime = aTime;
}

const string_ptr TRestoreStopMessage::encodeMessage(){
	this->fItems.push_back(make_shared<string>(getMessageName(this->fID)));
	this->fItems.push_back(make_shared<string>(to_string(this->fVersion)));
	this->fItems.push_back(make_shared<string>(timeToString(this->fTime)));

	return TBaseMessage::encodeMessage();
}

void TRestoreStopMessage::decodeMessage(){
	/*
	* item[0] -> msg name
	* item[1] -> version number
	* item[2] -> version timestamp
	*/
	TBaseMessage::decodeMessage();

	int size = (int)this->fItems.size();
	if (size != RESTORE_STOP_TOK_NUM)
		throw new EMessageException("RESTORE_STOP message contains wrong number of tokens(" + to_string(size) + " instead of " + to_string(RESTORE_STOP_TOK_NUM) + ")");

	for (int i = 0; i < size; i++)
		if (this->fItems[i] == NULL)
			throw new EMessageException("item " + to_string(i) + " is NULL");

	if (this->fID != RESTORE_STOP_ID)
		throw new EMessageException("The given message is not a RESTORE_STOP message");

	//version number
	if (*(this->fItems[1]) == EMPTY)
		throw new EMessageException("The version field cannot be empty");
	//TODO: check if this->fItems[1] contains a number
	this->fVersion = stoi(*(this->fItems[1]));

	//version timestamp
	if (*(this->fItems[2]) == EMPTY)
		throw new EMessageException("The version timestamp field cannot be empty");
	this->fTime = stringToTime(*(this->fItems[2]));
}
#pragma endregion


//////////////////////////////////////
//        TPingReqMessage           //
//////////////////////////////////////
#pragma region "TPingReqMessage"
TPingReqMessage::TPingReqMessage(TBaseMessage& aBase){
	this->fID = aBase.getID();
	this->fItems = aBase.getTokens();
	this->fEncodedMsg = aBase.getMsg();

	this->decodeMessage();
}

TPingReqMessage::TPingReqMessage(){
	this->fID = PING_REQ_ID;
	this->fTime = time(NULL);
}

const string_ptr TPingReqMessage::encodeMessage(){
	this->fItems.push_back(make_shared<string>(getMessageName(this->fID)));
	this->fItems.push_back(make_shared<string>(timeToString(this->fTime)));

	return TBaseMessage::encodeMessage();
}

void TPingReqMessage::decodeMessage(){
	/*
	* item[0] -> msg name
	* item[1] -> timestamp
	*/
	TBaseMessage::decodeMessage();
	
	int size = (int)this->fItems.size();
	if (size != PING_TOK_NUM)
		throw new EMessageException("PING_REQ message contains wrong number of tokens(" + to_string(size) + " instead of " + to_string(PING_TOK_NUM) + ")");

	for (int i = 0; i < size; i++)
		if (this->fItems[i] == NULL)
			throw new EMessageException("item " + to_string(i) + " is NULL");

	if (this->fID != PING_REQ_ID)
		throw new EMessageException("The given message is not a PING_REQ message");

	//timestamp
	if (*(this->fItems[1]) == EMPTY)
		throw new EMessageException("The time-stamp field cannot be empty");
	this->fTime = stringToTime(*(this->fItems[1]));
}
#pragma endregion


//////////////////////////////////////
//        TPingReplyMessage         //
//////////////////////////////////////
#pragma region "TPingReplyMessage"
TPingReplyMessage::TPingReplyMessage(TBaseMessage& aBase){
	this->fID = aBase.getID();
	this->fItems = aBase.getTokens();
	this->fEncodedMsg = aBase.getMsg();

	this->decodeMessage();
}

TPingReplyMessage::TPingReplyMessage(){
	this->fID = PING_REPLY_ID;
	this->fTime = time(NULL);
}

const string_ptr TPingReplyMessage::encodeMessage(){
	this->fItems.push_back(make_shared<string>(getMessageName(this->fID)));
	this->fItems.push_back(make_shared<string>(timeToString(this->fTime)));

	return TBaseMessage::encodeMessage();
}

void TPingReplyMessage::decodeMessage(){
	/*
	* item[0] -> msg name
	* item[1] -> timestamp
	*/
	TBaseMessage::decodeMessage();

	int size = (int)this->fItems.size();
	if (size != PING_TOK_NUM)
		throw new EMessageException("PING_REPLY message contains wrong number of tokens(" + to_string(size) + " instead of " + to_string(PING_TOK_NUM) + ")");

	for (int i = 0; i < size; i++)
		if (this->fItems[i] == NULL)
			throw new EMessageException("item " + to_string(i) + " is NULL");

	if (this->fID != PING_REPLY_ID)
		throw new EMessageException("The given message is not a PING_REPLY message");

	//timestamp
	if (*(this->fItems[1]) == EMPTY)
		throw new EMessageException("The time-stamp field cannot be empty");
	this->fTime = stringToTime(*(this->fItems[1]));
}
#pragma endregion
