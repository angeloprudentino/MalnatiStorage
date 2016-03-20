/*
 * Author: Angelo Prudentino
 * Date: 06/03/2016
 * File: Message.cpp
 * Description: this file contains all classes implemented
 *              messages exchanged between client and server
 *
 */

#include <sstream>
#include <vector>
#include <time.h>
#include <boost/algorithm/string/replace.hpp>

#include "Message.h"
#include "Utility.h"

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

const string getMessageName(const int aIndex){
	if (aIndex >= 0 && aIndex < MSG_NUM)
		return messageNames[aIndex];
	else
		return MSG_INVALID;
}

// Utility functions to escape/unescape messages
string escape(string aMsg){
	string from = to_string(MSG_SEP);
	string to = MSG_SEP_ESC;
	boost::replace_all(aMsg, from, to);
	return aMsg;
}

string unescape(string aMsg){
	string from = MSG_SEP_ESC;
	string to = to_string(MSG_SEP);
	boost::replace_all(aMsg, from, to);
	return aMsg;
}
#pragma endregion


//////////////////////////////////////
//         TBaseMessage	            //
//////////////////////////////////////
#pragma region "TBaseMessage"
TBaseMessage::TBaseMessage(){
	this->fID = NO_ID;
}

TBaseMessage::TBaseMessage(const string aMsg){
	this->fID = NO_ID;
	this->decodeMessage(aMsg);
}

const string TBaseMessage::getName(){
	return getMessageName(this->fID); 
};

const string TBaseMessage::encodeMessage(){
	int len = this->fItems.size();
	std::string msg = "";
	int i = 0;
	for (i = 0; i < len; i++){
		msg += escape(this->fItems[i]) + MSG_SEP;
	}

	msg +=  MSG_END;
	return msg;
}

void TBaseMessage::decodeMessage(string aMsg){
	// Turn the string into a stream.
	stringstream ss(aMsg);
	string tok;

	while (getline(ss, tok, MSG_SEP)) {
		tok = unescape(tok);
		this->fItems.push_back(tok);
	}

	string name = this->fItems[0];
	int len = this->fItems.size();
	for (int i = 0; i < len; i++){
		if (name == getMessageName(i)){
			this->fID = i;
			break;
		}
	}

	if (this->fItems[len-1] != MSG_END)
		throw new EMessageException("The given " + name + " message is not properly terminated");
}

void TBaseMessage::processMessage(TServerSockController* aController){
	//nothing to do with base messages
}
#pragma endregion


////////////////////////////////////////
//      TUserRegistrReqMessage        //
////////////////////////////////////////
#pragma region "TUserRegistrReqMessage"
void TUserRegistrReqMessage::initFields(){
	int size = this->fItems.size();
	if (size != USER_REG_REQ_TOK_NUM)
		throw new EMessageException("USER_REG_REQ message contains wrong number of tokens(" + to_string(size) + " instead of " + to_string(USER_REG_REQ_TOK_NUM) + ")");

	if (this->fID != USER_REG_REQ_ID)
		throw new EMessageException("The given message is not a USER_REG_REQ message");

	//username
	if (this->fItems[1] == EMPTY)
		throw new EMessageException("The user name field cannot be empty");
	this->fUser = this->fItems[1];
	
	//password
	if (this->fItems[2] == EMPTY)
		throw new EMessageException("The password field cannot be empty");
	this->fPass = this->fItems[2];
}

TUserRegistrReqMessage::TUserRegistrReqMessage(TBaseMessage& aBase){
	this->fID = aBase.getID();
	this->fItems = aBase.getTokens();

	this->initFields();
}

TUserRegistrReqMessage::TUserRegistrReqMessage(const string aUser, const string aPass){
	this->fID = USER_REG_REQ_ID;
	this->fUser = aUser;
	this->fPass = aPass;
}

const string TUserRegistrReqMessage::encodeMessage(){
	this->fItems[0] = getMessageName(this->fID);
	this->fItems[1] = this->fUser;
	this->fItems[2] = this->fPass;

	return TBaseMessage::encodeMessage();
}

void TUserRegistrReqMessage::decodeMessage(string aMsg){
	/*
	* item[0] -> msg name
	* item[1] -> username
	* item[2] -> password
	*/
	TBaseMessage::decodeMessage(aMsg);
	this->initFields();
}

void TUserRegistrReqMessage::processMessage(TServerSockController* aController){

}
#pragma endregion


////////////////////////////////////////
//      TUserRegistrReplyMessage      //
////////////////////////////////////////
#pragma region "TUserRegistrReplyMessage"
void TUserRegistrReplyMessage::initFields(){
	int size = this->fItems.size();
	if (size != USER_REG_REPLY_TOK_NUM)
		throw new EMessageException("USER_REG_REPLY message contains wrong number of tokens(" + to_string(size) + " instead of " + to_string(USER_REG_REPLY_TOK_NUM) + ")");

	if (this->fID != USER_REG_REPLY_ID)
		throw new EMessageException("The given message is not a USER_REG_REPLY message");

	//response
	if (this->fItems[1] == EMPTY)
		throw new EMessageException("The response field cannot be empty");
	if (this->fItems[1] != TRUE_STR && this->fItems[1] != FALSE_STR)
		throw new EMessageException("The response field could be only true or false");

	if (this->fItems[1] == TRUE_STR)
		this->fResp = true;
	else
		this->fResp = false;
}

TUserRegistrReplyMessage::TUserRegistrReplyMessage(TBaseMessage& aBase){
	this->fID = aBase.getID();
	this->fItems = aBase.getTokens();

	this->initFields();
}

TUserRegistrReplyMessage::TUserRegistrReplyMessage(const bool aResp){
	this->fID = USER_REG_REPLY_ID;
	this->fResp = aResp;
}

const string TUserRegistrReplyMessage::encodeMessage(){
	string resp = "";
	if (this->fResp)
		resp = TRUE_STR;
	else
		resp = FALSE_STR;

	this->fItems[0] = getMessageName(this->fID);
	this->fItems[1] = resp;

	return TBaseMessage::encodeMessage();
}

void TUserRegistrReplyMessage::decodeMessage(string aMsg){
	/*
	* item[0] -> msg name
	* item[1] -> response
	*/
	TBaseMessage::decodeMessage(aMsg);
	this->initFields();
}

void TUserRegistrReplyMessage::processMessage(TServerSockController* aController){

}
#pragma endregion


////////////////////////////////////////
//      TUpdateStartReqMessage        //
////////////////////////////////////////
#pragma region "TUpdateStartReqMessage"
void TUpdateStartReqMessage::initFields(){
	int size = this->fItems.size();
	if (size != UPDATE_START_REQ_TOK_NUM)
		throw new EMessageException("UPDATE_START_REQ message contains wrong number of tokens(" + to_string(size) + " instead of " + to_string(UPDATE_START_REQ_TOK_NUM) + ")");

	if (this->fID != UPDATE_START_REQ_ID)
		throw new EMessageException("The given message is not a UPDATE_START_REQ message");

	//username
	if (this->fItems[1] == EMPTY)
		throw new EMessageException("The user name field cannot be empty");
	this->fUser = this->fItems[1];

	//password
	if (this->fItems[2] == EMPTY)
		throw new EMessageException("The password field cannot be empty");
	this->fPass = this->fItems[2];
}

TUpdateStartReqMessage::TUpdateStartReqMessage(TBaseMessage& aBase){
	this->fID = aBase.getID();
	this->fItems = aBase.getTokens();

	this->initFields();
}

TUpdateStartReqMessage::TUpdateStartReqMessage(const string aUser, const string aPass){
	this->fID = UPDATE_START_REQ_ID;
	this->fUser = aUser;
	this->fPass = aPass;
}

const string TUpdateStartReqMessage::encodeMessage(){
	this->fItems[0] = getMessageName(this->fID);
	this->fItems[1] = this->fUser;
	this->fItems[2] = this->fPass;

	return TBaseMessage::encodeMessage();
}

void TUpdateStartReqMessage::decodeMessage(string aMsg){
	/*
	* item[0] -> msg name
	* item[1] -> username
	* item[2] -> password
	*/
	TBaseMessage::decodeMessage(aMsg);
	this->initFields();
}

void TUpdateStartReqMessage::processMessage(TServerSockController* aController){

}
#pragma endregion


////////////////////////////////////////
//      TUpdateStartReplyMessage      //
////////////////////////////////////////
#pragma region "TUpdateStartReplyMessage"
void TUpdateStartReplyMessage::initFields(){
	int size = this->fItems.size();
	if (size != UPDATE_START_REPLY_TOK_NUM)
		throw new EMessageException("UPDATE_START_REPLY message contains wrong number of tokens(" + to_string(size) + " instead of " + to_string(UPDATE_START_REPLY_TOK_NUM) + ")");

	if (this->fID != UPDATE_START_REPLY_ID)
		throw new EMessageException("The given message is not a UPDATE_START_REPLY message");

	//response
	if (this->fItems[1] == EMPTY)
		throw new EMessageException("The response field cannot be empty");
	if (this->fItems[1] != TRUE_STR && this->fItems[1] != FALSE_STR)
		throw new EMessageException("The response field could be only true or false");

	if (this->fItems[1] == TRUE_STR)
		this->fResp = true;
	else
		this->fResp = false;

	//user token
	if (this->fItems[2] == EMPTY)
		throw new EMessageException("The token field cannot be empty");
	this->fToken = this->fItems[2]; //TODO: implement -> checkToken(this->fItems[2]);
}

TUpdateStartReplyMessage::TUpdateStartReplyMessage(TBaseMessage& aBase){
	this->fID = aBase.getID();
	this->fItems = aBase.getTokens();

	this->initFields();
}

TUpdateStartReplyMessage::TUpdateStartReplyMessage(const bool aResp, const bool aToken){
	this->fID = UPDATE_START_REPLY_ID;
	this->fResp = aResp;
	this->fToken = aToken;
}

const string TUpdateStartReplyMessage::encodeMessage(){
	string resp = "";
	if (this->fResp)
		resp = TRUE_STR;
	else
		resp = FALSE_STR;

	this->fItems[0] = getMessageName(this->fID);
	this->fItems[1] = resp;
	this->fItems[2] = this->fToken;

	return TBaseMessage::encodeMessage();
}

void TUpdateStartReplyMessage::decodeMessage(string aMsg){
	/*
	* item[0] -> msg name
	* item[1] -> response
	* item[2] -> user token
	*/
	TBaseMessage::decodeMessage(aMsg);
	this->initFields();
}

void TUpdateStartReplyMessage::processMessage(TServerSockController* aController){

}
#pragma endregion


////////////////////////////////////////
//        TAddNewFileMessage          //
////////////////////////////////////////
#pragma region "TAddNewFileMessage"
void TAddNewFileMessage::initFields(){
	int size = this->fItems.size();
	if (size != ADD_NEW_FILE_TOK_NUM)
		throw new EMessageException("ADD_NEW_FILE message contains wrong number of tokens(" + to_string(size) + " instead of " + to_string(ADD_NEW_FILE_TOK_NUM) + ")");
	
	//message id
	if (this->fID != ADD_NEW_FILE_ID)
		throw new EMessageException("The given message is not a ADD_NEW_FILE message");

	//user token
	if (this->fItems[1] == EMPTY)
		throw new EMessageException("The token field cannot be empty");
	this->fToken = this->fItems[1]; //TODO: implement -> checkToken(this->fItems[2]);

	//file path
	if (this->fItems[2] == EMPTY)
		throw new EMessageException("The token field cannot be empty");
	if (this->fItems[2] == EMPTY)
		throw new EMessageException("The file path field cannot be empty");
	//TODO: check se il path è valido
	//if (!isValidPath(this->fFilePath))
	//	throw new EMessageException("The file path field must contain a valid path");
	this->fFilePath = this->fItems[2];

	//file checksum
	if (this->fItems[3] == EMPTY)
		throw new EMessageException("The file checksum field cannot be empty");
	this->fChecksum = this->fItems[3];

	//file date
	if (this->fItems[4] == EMPTY)
		throw new EMessageException("The file date field cannot be empty");
	this->fFileDate = stringToTime(this->fItems[4]);

	//file content
	//TODO: aggiungere il file content
}

TAddNewFileMessage::TAddNewFileMessage(TBaseMessage& aBase){
	this->fID = aBase.getID();
	this->fItems = aBase.getTokens();

	this->initFields();
}

TAddNewFileMessage::TAddNewFileMessage(const bool aToken, string aFilePath){
	this->fID = ADD_NEW_FILE_ID;
	this->fToken = aToken;
	this->fFilePath = aFilePath;
	this->fChecksum = "";
	this->fFileDate = time(NULL);
}

const string TAddNewFileMessage::encodeMessage(){
	this->fItems[0] = getMessageName(this->fID);
	this->fItems[1] = this->fToken;
	this->fItems[2] = this->fFilePath;
	this->fItems[3] = this->fChecksum;
	this->fItems[4] = timeToString(this->fFileDate);
	//this->fItems[5] = file content;

	return TBaseMessage::encodeMessage();
}

void TAddNewFileMessage::decodeMessage(const string aMsg){
	/*
	* item[0] -> msg name
	* item[1] -> token
	* item[2] -> file path
	* item[3] -> file checksum
	* item[4] -> file date
	* item[5] -> file content
	*/
	TBaseMessage::decodeMessage(aMsg);
	this->initFields();
}

void TAddNewFileMessage::processMessage(TServerSockController* aController){

}
#pragma endregion


////////////////////////////////////////
//        TUpdateFileMessage          //
////////////////////////////////////////
#pragma region "TUpdateFileMessage"
void TUpdateFileMessage::initFields(){
	int size = this->fItems.size();
	if (size != UPDATE_FILE_TOK_NUM)
		throw new EMessageException("UPDATE_FILE message contains wrong number of tokens(" + to_string(size) + " instead of " + to_string(UPDATE_FILE_TOK_NUM) + ")");

	//message id
	if (this->fID != UPDATE_FILE_ID)
		throw new EMessageException("The given message is not a UPDATE_FILE message");

	//user token
	if (this->fItems[1] == EMPTY)
		throw new EMessageException("The token field cannot be empty");
	this->fToken = this->fItems[1]; //TODO: implement -> checkToken(this->fItems[2]);

	//file path
	if (this->fItems[2] == EMPTY)
		throw new EMessageException("The token field cannot be empty");
	if (this->fItems[2] == EMPTY)
		throw new EMessageException("The file path field cannot be empty");
	//TODO: check se il path è valido
	//if (!isValidPath(this->fFilePath))
	//	throw new EMessageException("The file path field must contain a valid path");
	this->fFilePath = this->fItems[2];

	//file checksum
	if (this->fItems[3] == EMPTY)
		throw new EMessageException("The file checksum field cannot be empty");
	this->fChecksum = this->fItems[3];

	//file date
	if (this->fItems[4] == EMPTY)
		throw new EMessageException("The file date field cannot be empty");
	this->fFileDate = stringToTime(this->fItems[4]);

	//file content
	//TODO: aggiungere il file content
}

TUpdateFileMessage::TUpdateFileMessage(TBaseMessage& aBase){
	this->fID = aBase.getID();
	this->fItems = aBase.getTokens();

	this->initFields();
}

TUpdateFileMessage::TUpdateFileMessage(const bool aToken, string aFilePath){
	this->fID = UPDATE_FILE_ID;
	this->fToken = aToken;
	this->fFilePath = aFilePath;
	this->fChecksum = "";
	this->fFileDate = time(NULL);
}

const string TUpdateFileMessage::encodeMessage(){
	this->fItems[0] = getMessageName(this->fID);
	this->fItems[1] = this->fToken;
	this->fItems[2] = this->fFilePath;
	this->fItems[3] = this->fChecksum;
	this->fItems[4] = timeToString(this->fFileDate);
	//this->fItems[5] = file content;

	return TBaseMessage::encodeMessage();
}

void TUpdateFileMessage::decodeMessage(const string aMsg){
	/*
	* item[0] -> msg name
	* item[1] -> token
	* item[2] -> file path
	* item[3] -> file checksum
	* item[4] -> file date
	* item[5] -> file content
	*/
	TBaseMessage::decodeMessage(aMsg);
	this->initFields();
}

void TUpdateFileMessage::processMessage(TServerSockController* aController){

}
#pragma endregion


////////////////////////////////////////
//        TRemoveFileMessage          //
////////////////////////////////////////
#pragma region "TRemoveFileMessage"
void TRemoveFileMessage::initFields(){
	int size = this->fItems.size();
	if (size != REMOVE_FILE_TOK_NUM)
		throw new EMessageException("REMOVE_FILE message contains wrong number of tokens(" + to_string(size) + " instead of " + to_string(REMOVE_FILE_TOK_NUM) + ")");

	//message id
	if (this->fID != REMOVE_FILE_ID)
		throw new EMessageException("The given message is not a REMOVE_FILE message");

	//user token
	if (this->fItems[1] == EMPTY)
		throw new EMessageException("The token field cannot be empty");
	this->fToken = this->fItems[1]; //TODO: implement -> checkToken(this->fItems[2]);

	//file path
	if (this->fItems[2] == EMPTY)
		throw new EMessageException("The token field cannot be empty");
	if (this->fItems[2] == EMPTY)
		throw new EMessageException("The file path field cannot be empty");
	//TODO: check se il path è valido
	//if (!isValidPath(this->fFilePath))
	//	throw new EMessageException("The file path field must contain a valid path");
	this->fFilePath = this->fItems[2];
}

TRemoveFileMessage::TRemoveFileMessage(TBaseMessage& aBase){
	this->fID = aBase.getID();
	this->fItems = aBase.getTokens();

	this->initFields();
}

TRemoveFileMessage::TRemoveFileMessage(const string aToken, string aFilePath){
	this->fID = REMOVE_FILE_ID;
	this->fToken = aToken;
	this->fFilePath = aFilePath;
}

const string TRemoveFileMessage::encodeMessage(){
	this->fItems[0] = getMessageName(this->fID);
	this->fItems[1] = this->fToken;
	this->fItems[2] = this->fFilePath;

	return TBaseMessage::encodeMessage();
}

void TRemoveFileMessage::decodeMessage(const string aMsg){
	/*
	* item[0] -> msg name
	* item[1] -> token
	* item[2] -> file path
	*/
	TBaseMessage::decodeMessage(aMsg);
	this->initFields();
}

void TRemoveFileMessage::processMessage(TServerSockController* aController){

}
#pragma endregion


///////////////////////////////////
//        TFileAckMessage        //
///////////////////////////////////
#pragma region "TFileAckMessage"
void TFileAckMessage::initFields(){
	int size = this->fItems.size();
	if (size != FILE_ACK_TOK_NUM)
		throw new EMessageException("FILE_ACK message contains wrong number of tokens(" + to_string(size) + " instead of " + to_string(FILE_ACK_TOK_NUM) + ")");

	if (this->fID != FILE_ACK_ID)
		throw new EMessageException("The given message is not a FILE_ACK message");

	//response
	if (this->fItems[1] == EMPTY)
		throw new EMessageException("The response field cannot be empty");
	if (this->fItems[1] != TRUE_STR && this->fItems[1] != FALSE_STR)
		throw new EMessageException("The response field could be only true or false");

	if (this->fItems[1] == TRUE_STR)
		this->fResp = true;
	else
		this->fResp = false;

	//file path
	if (this->fItems[2] == EMPTY)
		throw new EMessageException("The token field cannot be empty");
	//TODO: check se il path è valido
	//if (!isValidPath(this->fFilePath))
	//	throw new EMessageException("The file path field must contain a valid path");
	this->fFilePath = this->fItems[2];
}

TFileAckMessage::TFileAckMessage(TBaseMessage& aBase){
	this->fID = aBase.getID();
	this->fItems = aBase.getTokens();

	this->initFields();
}

TFileAckMessage::TFileAckMessage(const bool aResp, const bool aFilePath){
	this->fID = FILE_ACK_ID;
	this->fResp = aResp;
	this->fFilePath = aFilePath;
}

const string TFileAckMessage::encodeMessage(){
	string resp = "";
	if (this->fResp)
		resp = TRUE_STR;
	else
		resp = FALSE_STR;

	this->fItems[0] = getMessageName(this->fID);
	this->fItems[1] = resp;
	this->fItems[2] = this->fFilePath;

	return TBaseMessage::encodeMessage();
}

void TFileAckMessage::decodeMessage(string aMsg){
	/*
	* item[0] -> msg name
	* item[1] -> response
	* item[2] -> file path
	*/
	TBaseMessage::decodeMessage(aMsg);
	this->initFields();
}

void TFileAckMessage::processMessage(TServerSockController* aController){

}
#pragma endregion


////////////////////////////////////////
//       TUpdateStopReqMessage        //
////////////////////////////////////////
#pragma region "TUpdateStopReqMessage"
void TUpdateStopReqMessage::initFields(){
	int size = this->fItems.size();
	if (size != UPDATE_STOP_REQ_TOK_NUM)
		throw new EMessageException("UPDATE_STOP_REQ message contains wrong number of tokens(" + to_string(size) + " instead of " + to_string(UPDATE_STOP_REQ_TOK_NUM) + ")");

	if (this->fID != UPDATE_STOP_REQ_ID)
		throw new EMessageException("The given message is not a UPDATE_STOP_REQ message");

	//user token
	if (this->fItems[1] == EMPTY)
		throw new EMessageException("The token field cannot be empty");
	this->fToken = this->fItems[1]; //TODO: implement -> checkToken(this->fItems[2]);
}

TUpdateStopReqMessage::TUpdateStopReqMessage(TBaseMessage& aBase){
	this->fID = aBase.getID();
	this->fItems = aBase.getTokens();

	this->initFields();
}

TUpdateStopReqMessage::TUpdateStopReqMessage(const string aToken){
	this->fID = UPDATE_STOP_REQ_ID;
	this->fToken = aToken;
}

const string TUpdateStopReqMessage::encodeMessage(){
	this->fItems[0] = getMessageName(this->fID);
	this->fItems[1] = this->fToken;

	return TBaseMessage::encodeMessage();
}

void TUpdateStopReqMessage::decodeMessage(string aMsg){
	/*
	* item[0] -> msg name
	* item[1] -> user token
	*/
	TBaseMessage::decodeMessage(aMsg);
	this->initFields();
}

void TUpdateStopReqMessage::processMessage(TServerSockController* aController){

}
#pragma endregion


////////////////////////////////////////
//      TUpdateStopReplyMessage       //
////////////////////////////////////////
#pragma region "TUpdateStopReplyMessage"
void TUpdateStopReplyMessage::initFields(){
	int size = this->fItems.size();
	if (size != UPDATE_STOP_REPLY_TOK_NUM)
		throw new EMessageException("UPDATE_STOP_REPLY message contains wrong number of tokens(" + to_string(size) + " instead of " + to_string(UPDATE_STOP_REPLY_TOK_NUM) + ")");

	if (this->fID != UPDATE_STOP_REPLY_ID)
		throw new EMessageException("The given message is not a UPDATE_STOP_REPLY message");

	//response
	if (this->fItems[1] == EMPTY)
		throw new EMessageException("The response field cannot be empty");
	if (this->fItems[1] != TRUE_STR && this->fItems[1] != FALSE_STR)
		throw new EMessageException("The response field could be only true or false");

	if (this->fItems[1] == TRUE_STR)
		this->fResp = true;
	else
		this->fResp = false;

	//version number
	if (this->fItems[2] == EMPTY)
		throw new EMessageException("The version field cannot be empty");
	//TODO: check if this->fItems[2] contains a number
	this->fVersion = stoi(this->fItems[2]);

	//version timestamp
	if (this->fItems[3] == EMPTY)
		throw new EMessageException("The version timestamp field cannot be empty");
	this->fTime = stringToTime(this->fItems[3]);
}

TUpdateStopReplyMessage::TUpdateStopReplyMessage(TBaseMessage& aBase){
	this->fID = aBase.getID();
	this->fItems = aBase.getTokens();

	this->initFields();
}

TUpdateStopReplyMessage::TUpdateStopReplyMessage(const bool aResp, unsigned int aVersion, time_t aTime){
	this->fID = UPDATE_STOP_REPLY_ID;
	this->fResp = aResp;
	this->fVersion = aVersion;
	this->fTime = aTime;
}

const string TUpdateStopReplyMessage::encodeMessage(){
	string resp = "";
	if (this->fResp)
		resp = TRUE_STR;
	else
		resp = FALSE_STR;

	this->fItems[0] = getMessageName(this->fID);
	this->fItems[1] = resp;
	this->fItems[2] = to_string(this->fVersion);
	this->fItems[3] = timeToString(this->fTime);

	return TBaseMessage::encodeMessage();
}

void TUpdateStopReplyMessage::decodeMessage(string aMsg){
	/*
	* item[0] -> msg name
	* item[1] -> response
	* item[2] -> version number
	* item[3] -> version timestamp
	*/
	TBaseMessage::decodeMessage(aMsg);
	this->initFields();
}

void TUpdateStopReplyMessage::processMessage(TServerSockController* aController){

}
#pragma endregion


////////////////////////////////////////
//      TGetVersionsReqMessage        //
////////////////////////////////////////
#pragma region "TGetVersionsReqMessage"
void TGetVersionsReqMessage::initFields(){
	int size = this->fItems.size();
	if (size != GET_VERSIONS_REQ_TOK_NUM)
		throw new EMessageException("GET_VERSIONS_REQ message contains wrong number of tokens(" + to_string(size) + " instead of " + to_string(GET_VERSIONS_REQ_TOK_NUM) + ")");

	if (this->fID != GET_VERSIONS_REQ_ID)
		throw new EMessageException("The given message is not a GET_VERSIONS_REQ message");

	//username
	if (this->fItems[1] == EMPTY)
		throw new EMessageException("The user name field cannot be empty");
	this->fUser = this->fItems[1];

	//password
	if (this->fItems[2] == EMPTY)
		throw new EMessageException("The password field cannot be empty");
	this->fPass = this->fItems[2];
}

TGetVersionsReqMessage::TGetVersionsReqMessage(TBaseMessage& aBase){
	this->fID = aBase.getID();
	this->fItems = aBase.getTokens();

	this->initFields();
}

TGetVersionsReqMessage::TGetVersionsReqMessage(const string aUser, const string aPass){
	this->fID = GET_VERSIONS_REQ_ID;
	this->fUser = aUser;
	this->fPass = aPass;
}

const string TGetVersionsReqMessage::encodeMessage(){
	this->fItems[0] = getMessageName(this->fID);
	this->fItems[1] = this->fUser;
	this->fItems[2] = this->fPass;

	return TBaseMessage::encodeMessage();
}

void TGetVersionsReqMessage::decodeMessage(string aMsg){
	/*
	* item[0] -> msg name
	* item[1] -> username
	* item[2] -> password
	*/
	TBaseMessage::decodeMessage(aMsg);
	this->initFields();
}

void TGetVersionsReqMessage::processMessage(TServerSockController* aController){

}
#pragma endregion


////////////////////////////////////////
//      TGetVersionsReplyMessage      //
////////////////////////////////////////
#pragma region "TGetVersionsReplyMessage"
void TGetVersionsReplyMessage::initFields(){
	int size = this->fItems.size();
	if (size < GET_VERSIONS_REPLY_MIN_TOK_NUM)
		throw new EMessageException("GET_VERSIONS_REPLY message does not contain the minimum number of tokens(" + to_string(GET_VERSIONS_REPLY_MIN_TOK_NUM) + ")");

	if (this->fID != GET_VERSIONS_REPLY_ID)
		throw new EMessageException("The given message is not a GET_VERSIONS_REPLY message");

	//tot number of versions
	if (this->fItems[1] == EMPTY)
		throw new EMessageException("The total versions number field cannot be empty");
	//TODO: check if this->fItems[1] contains a number
	this->fTotVersions = stoi(this->fItems[1]);

	//oldest version
	if (this->fItems[2] == EMPTY)
		throw new EMessageException("The oldest version number field cannot be empty");
	//TODO: check if this->fItems[2] contains a number
	this->fOldestVersion = stoi(this->fItems[2]);

	//last version
	if (this->fItems[3] == EMPTY)
		throw new EMessageException("The last version number field cannot be empty");
	//TODO: check if this->fItems[3] contains a number
	this->fLastVersion = stoi(this->fItems[3]);

	int totSize = GET_VERSIONS_REPLY_MIN_TOK_NUM + (this->fLastVersion - this->fOldestVersion);
	if (size != totSize)
		throw new EMessageException("GET_VERSIONS_REPLY message contains wrong number of tokens(" + to_string(size) + " instead of " + to_string(totSize) + ")");

	int i, j;
	for (i = GET_VERSIONS_REPLY_MIN_TOK_NUM + 1, j = this->fOldestVersion; i < totSize; i++, j++){
		this->fVersions.emplace(j, stringToTime(this->fItems[i]));
	}
}

TGetVersionsReplyMessage::TGetVersionsReplyMessage(TBaseMessage& aBase){
	this->fID = aBase.getID();
	this->fItems = aBase.getTokens();

	this->initFields();
}

TGetVersionsReplyMessage::TGetVersionsReplyMessage(const unsigned int aTotVersions, const unsigned int aOldestVersion, const unsigned int aLastVersion, map<unsigned int, time_t> aVersions){
	this->fID = GET_VERSIONS_REPLY_ID;
	this->fTotVersions = aTotVersions;
	this->fOldestVersion = aOldestVersion;
	this->fLastVersion = aLastVersion;
	this->fVersions = aVersions;
}

const string TGetVersionsReplyMessage::encodeMessage(){
	this->fItems[0] = getMessageName(this->fID);
	this->fItems[1] = this->fTotVersions;
	this->fItems[2] = this->fOldestVersion;
	this->fItems[3] = this->fLastVersion;

	int size = this->fVersions.size();
	for (int i = GET_VERSIONS_REPLY_MIN_TOK_NUM + 1; i < size; i++){
		this->fItems[i] = this->fVersions.at(i);
	}

	return TBaseMessage::encodeMessage();
}

void TGetVersionsReplyMessage::decodeMessage(string aMsg){
	/*
	* item[0] -> msg name
	* item[1] -> number of versions
	* item[2] -> oldest version on server
	* item[3] -> last version
	* item[4..n] -> versions timestamps
	*/
	TBaseMessage::decodeMessage(aMsg);
	this->initFields();
}

void TGetVersionsReplyMessage::processMessage(TServerSockController* aController){

}
#pragma endregion


////////////////////////////////////////
//       TRestoreVerReqMessage        //
////////////////////////////////////////
#pragma region "TRestoreVerReqMessage"
void TRestoreVerReqMessage::initFields(){
	int size = this->fItems.size();
	if (size != RESTORE_VER_REQ_TOK_NUM)
		throw new EMessageException("RESTORE_VER_REQ message contains wrong number of tokens(" + to_string(size) + " instead of " + to_string(RESTORE_VER_REQ_TOK_NUM) + ")");

	if (this->fID != RESTORE_VER_REQ_ID)
		throw new EMessageException("The given message is not a RESTORE_VER_REQ message");

	//username
	if (this->fItems[1] == EMPTY)
		throw new EMessageException("The user name field cannot be empty");
	this->fUser = this->fItems[1];

	//password
	if (this->fItems[2] == EMPTY)
		throw new EMessageException("The password field cannot be empty");
	this->fPass = this->fItems[2];


	//version
	if (this->fItems[3] == EMPTY)
		throw new EMessageException("The version number field cannot be empty");
	//TODO: check if this->fItems[3] contains a number
	this->fVersion = stoi(this->fItems[3]);
}

TRestoreVerReqMessage::TRestoreVerReqMessage(TBaseMessage& aBase){
	this->fID = aBase.getID();
	this->fItems = aBase.getTokens();

	this->initFields();
}

TRestoreVerReqMessage::TRestoreVerReqMessage(const string aUser, const string aPass, const unsigned int aVersion){
	this->fID = RESTORE_VER_REQ_ID;
	this->fUser = aUser;
	this->fPass = aPass;
	this->fVersion = aVersion;
}

const string TRestoreVerReqMessage::encodeMessage(){
	this->fItems[0] = getMessageName(this->fID);
	this->fItems[1] = this->fUser;
	this->fItems[2] = this->fPass;
	this->fItems[3] = this->fVersion;

	return TBaseMessage::encodeMessage();
}

void TRestoreVerReqMessage::decodeMessage(string aMsg){
	/*
	* item[0] -> msg name
	* item[1] -> username
	* item[2] -> password
	* item[3] -> version
	*/
	TBaseMessage::decodeMessage(aMsg);
	this->initFields();
}

void TRestoreVerReqMessage::processMessage(TServerSockController* aController){

}
#pragma endregion


////////////////////////////////////////
//      TRestoreVerReplyMessage       //
////////////////////////////////////////
#pragma region "TRestoreVerReplyMessage"
void TRestoreVerReplyMessage::initFields(){
	int size = this->fItems.size();
	if (size != RESTORE_VER_REPLY_TOK_NUM)
		throw new EMessageException("RESTORE_VER_REPLY message contains wrong number of tokens(" + to_string(size) + " instead of " + to_string(RESTORE_VER_REPLY_TOK_NUM) + ")");

	if (this->fID != RESTORE_VER_REPLY_ID)
		throw new EMessageException("The given message is not a UPDATE_START_REPLY message");

	//response
	if (this->fItems[1] == EMPTY)
		throw new EMessageException("The response field cannot be empty");
	if (this->fItems[1] != TRUE_STR && this->fItems[1] != FALSE_STR)
		throw new EMessageException("The response field could be only true or false");

	if (this->fItems[1] == TRUE_STR)
		this->fResp = true;
	else
		this->fResp = false;

	//user token
	if (this->fItems[2] == EMPTY)
		throw new EMessageException("The token field cannot be empty");
	this->fToken = this->fItems[2]; //TODO: implement -> checkToken(this->fItems[2]);
}

TRestoreVerReplyMessage::TRestoreVerReplyMessage(TBaseMessage& aBase){
	this->fID = aBase.getID();
	this->fItems = aBase.getTokens();

	this->initFields();
}

TRestoreVerReplyMessage::TRestoreVerReplyMessage(const bool aResp, const bool aToken){
	this->fID = RESTORE_VER_REPLY_ID;
	this->fResp = aResp;
	this->fToken = aToken;
}

const string TRestoreVerReplyMessage::encodeMessage(){
	string resp = "";
	if (this->fResp)
		resp = TRUE_STR;
	else
		resp = FALSE_STR;

	this->fItems[0] = getMessageName(this->fID);
	this->fItems[1] = resp;
	this->fItems[2] = this->fToken;

	return TBaseMessage::encodeMessage();
}

void TRestoreVerReplyMessage::decodeMessage(string aMsg){
	/*
	* item[0] -> msg name
	* item[1] -> response
	* item[2] -> user token
	*/
	TBaseMessage::decodeMessage(aMsg);
	this->initFields();
}

void TRestoreVerReplyMessage::processMessage(TServerSockController* aController){

}
#pragma endregion


////////////////////////////////////////
//        TRestoreFileMessage         //
////////////////////////////////////////
#pragma region "TRestoreFileMessage"
void TRestoreFileMessage::initFields(){
	int size = this->fItems.size();
	if (size != RESTORE_FILE_TOK_NUM)
		throw new EMessageException("RESTORE_FILE message contains wrong number of tokens(" + to_string(size) + " instead of " + to_string(RESTORE_FILE_TOK_NUM) + ")");

	//message id
	if (this->fID != RESTORE_FILE_ID)
		throw new EMessageException("The given message is not a RESTORE_FILE message");

	//file path
	if (this->fItems[1] == EMPTY)
		throw new EMessageException("The token field cannot be empty");
	if (this->fItems[1] == EMPTY)
		throw new EMessageException("The file path field cannot be empty");
	//TODO: check se il path è valido
	//if (!isValidPath(this->fFilePath))
	//	throw new EMessageException("The file path field must contain a valid path");
	this->fFilePath = this->fItems[1];

	//file checksum
	if (this->fItems[2] == EMPTY)
		throw new EMessageException("The file checksum field cannot be empty");
	this->fChecksum = this->fItems[2];

	//file date
	if (this->fItems[3] == EMPTY)
		throw new EMessageException("The file date field cannot be empty");
	this->fFileDate = stringToTime(this->fItems[3]);

	//file content
	//TODO: aggiungere il file content
}

TRestoreFileMessage::TRestoreFileMessage(TBaseMessage& aBase){
	this->fID = aBase.getID();
	this->fItems = aBase.getTokens();

	this->initFields();
}

TRestoreFileMessage::TRestoreFileMessage(string aFilePath){
	this->fID = RESTORE_FILE_ID;
	this->fFilePath = aFilePath;
	this->fChecksum = "";
	this->fFileDate = time(NULL);
}

const string TRestoreFileMessage::encodeMessage(){
	this->fItems[0] = getMessageName(this->fID);
	this->fItems[1] = this->fFilePath;
	this->fItems[2] = this->fChecksum;
	this->fItems[3] = timeToString(this->fFileDate);
	//this->fItems[4] = file content;

	return TBaseMessage::encodeMessage();
}

void TRestoreFileMessage::decodeMessage(const string aMsg){
	/*
	* item[0] -> msg name
	* item[1] -> file path
	* item[2] -> file checksum
	* item[3] -> file date
	* item[4] -> file content
	*/
	TBaseMessage::decodeMessage(aMsg);
	this->initFields();
}

void TRestoreFileMessage::processMessage(TServerSockController* aController){

}
#pragma endregion


//////////////////////////////////////////
//        TRestoreFileAckMessage        //
//////////////////////////////////////////
#pragma region "TRestoreFileAckMessage"
void TRestoreFileAckMessage::initFields(){
	int size = this->fItems.size();
	if (size != RESTORE_FILE_ACK_TOK_NUM)
		throw new EMessageException("RESTORE_FILE_ACK message contains wrong number of tokens(" + to_string(size) + " instead of " + to_string(RESTORE_FILE_ACK_TOK_NUM) + ")");

	if (this->fID != RESTORE_FILE_ACK_ID)
		throw new EMessageException("The given message is not a RESTORE_FILE_ACK message");

	//user token
	if (this->fItems[1] == EMPTY)
		throw new EMessageException("The token field cannot be empty");
	this->fToken = this->fItems[1]; //TODO: implement -> checkToken(this->fItems[1]);

	//response
	if (this->fItems[2] == EMPTY)
		throw new EMessageException("The response field cannot be empty");
	if (this->fItems[2] != TRUE_STR && this->fItems[2] != FALSE_STR)
		throw new EMessageException("The response field could be only true or false");

	if (this->fItems[2] == TRUE_STR)
		this->fResp = true;
	else
		this->fResp = false;

	//file path
	if (this->fItems[3] == EMPTY)
		throw new EMessageException("The token field cannot be empty");
	//TODO: check se il path è valido
	//if (!isValidPath(this->fFilePath))
	//	throw new EMessageException("The file path field must contain a valid path");
	this->fFilePath = this->fItems[3];
}

TRestoreFileAckMessage::TRestoreFileAckMessage(TBaseMessage& aBase){
	this->fID = aBase.getID();
	this->fItems = aBase.getTokens();

	this->initFields();
}

TRestoreFileAckMessage::TRestoreFileAckMessage(const string aToken, const bool aResp, const bool aFilePath){
	this->fID = RESTORE_FILE_ACK_ID;
	this->fToken = aToken;
	this->fResp = aResp;
	this->fFilePath = aFilePath;
}

const string TRestoreFileAckMessage::encodeMessage(){
	string resp = "";
	if (this->fResp)
		resp = TRUE_STR;
	else
		resp = FALSE_STR;

	this->fItems[0] = getMessageName(this->fID);
	this->fItems[1] = this->fToken;
	this->fItems[2] = resp;
	this->fItems[3] = this->fFilePath;

	return TBaseMessage::encodeMessage();
}

void TRestoreFileAckMessage::decodeMessage(string aMsg){
	/*
	* item[0] -> msg name
	* item[1] -> user token
	* item[2] -> response
	* item[3] -> file path
	*/
	TBaseMessage::decodeMessage(aMsg);
	this->initFields();
}

void TRestoreFileAckMessage::processMessage(TServerSockController* aController){

}
#pragma endregion


////////////////////////////////////
//      TRestoreStopMessage       //
////////////////////////////////////
#pragma region "TRestoreStopMessage"
void TRestoreStopMessage::initFields(){
	int size = this->fItems.size();
	if (size != RESTORE_STOP_TOK_NUM)
		throw new EMessageException("RESTORE_STOP message contains wrong number of tokens(" + to_string(size) + " instead of " + to_string(RESTORE_STOP_TOK_NUM) + ")");

	if (this->fID != RESTORE_STOP_ID)
		throw new EMessageException("The given message is not a RESTORE_STOP message");

	//version number
	if (this->fItems[1] == EMPTY)
		throw new EMessageException("The version field cannot be empty");
	//TODO: check if this->fItems[1] contains a number
	this->fVersion = stoi(this->fItems[1]);

	//version timestamp
	if (this->fItems[2] == EMPTY)
		throw new EMessageException("The version timestamp field cannot be empty");
	this->fTime = stringToTime(this->fItems[2]);
}

TRestoreStopMessage::TRestoreStopMessage(TBaseMessage& aBase){
	this->fID = aBase.getID();
	this->fItems = aBase.getTokens();

	this->initFields();
}

TRestoreStopMessage::TRestoreStopMessage(unsigned int aVersion, time_t aTime){
	this->fID = RESTORE_STOP_ID;
	this->fVersion = aVersion;
	this->fTime = aTime;
}

const string TRestoreStopMessage::encodeMessage(){
	this->fItems[0] = getMessageName(this->fID);
	this->fItems[1] = to_string(this->fVersion);
	this->fItems[2] = timeToString(this->fTime);

	return TBaseMessage::encodeMessage();
}

void TRestoreStopMessage::decodeMessage(string aMsg){
	/*
	* item[0] -> msg name
	* item[1] -> version number
	* item[2] -> version timestamp
	*/
	TBaseMessage::decodeMessage(aMsg);
	this->initFields();
}

void TRestoreStopMessage::processMessage(TServerSockController* aController){

}
#pragma endregion


//////////////////////////////////////
//        TPingReqMessage           //
//////////////////////////////////////
#pragma region "TPingReqMessage"
void TPingReqMessage::initFields(){
	int size = this->fItems.size();
	if (size != PING_TOK_NUM)
		throw new EMessageException("PING_REQ message contains wrong number of tokens(" + to_string(size) + " instead of " + to_string(PING_TOK_NUM) + ")");

	if (this->fID != PING_REQ_ID)
		throw new EMessageException("The given message is not a PING_REQ message");

	//timestamp
	if (this->fItems[1] == EMPTY)
		throw new EMessageException("The time-stamp field cannot be empty");
	this->fTime = stringToTime(this->fItems[1]);
}

TPingReqMessage::TPingReqMessage(TBaseMessage& aBase){
	this->fID = aBase.getID();
	this->fItems = aBase.getTokens();

	this->initFields();
}

TPingReqMessage::TPingReqMessage(){
	this->fID = PING_REQ_ID;
	this->fTime = time(NULL);
}

const string TPingReqMessage::encodeMessage(){
	this->fItems[0] = getMessageName(this->fID);
	this->fItems[1] = timeToString(this->fTime);

	return TBaseMessage::encodeMessage();
}

void TPingReqMessage::decodeMessage(string aMsg){
	/*
	* item[0] -> msg name
	* item[1] -> timestamp
	*/
	TBaseMessage::decodeMessage(aMsg);
	this->initFields();
}

void TPingReqMessage::processMessage(TServerSockController* aController){

}
#pragma endregion


//////////////////////////////////////
//        TPingReplyMessage         //
//////////////////////////////////////
#pragma region "TPingReplyMessage"
void TPingReplyMessage::initFields(){
	int size = this->fItems.size();
	if (size != PING_TOK_NUM)
		throw new EMessageException("PING_REPLY message contains wrong number of tokens(" + to_string(size) + " instead of " + to_string(PING_TOK_NUM) + ")");

	if (this->fID != PING_REPLY_ID)
		throw new EMessageException("The given message is not a PING_REPLY message");

	//timestamp
	if (this->fItems[1] == EMPTY)
		throw new EMessageException("The timestamp field cannot be empty");
	this->fTime = stringToTime(this->fItems[1]);
}

TPingReplyMessage::TPingReplyMessage(TBaseMessage& aBase){
	this->fID = aBase.getID();
	this->fItems = aBase.getTokens();

	this->initFields();
}

TPingReplyMessage::TPingReplyMessage(){
	this->fID = PING_REPLY_ID;
	this->fTime = time(NULL);
}

const string TPingReplyMessage::encodeMessage(){
	this->fItems[0] = getMessageName(this->fID);
	this->fItems[1] = timeToString(this->fTime);

	return TBaseMessage::encodeMessage();
}

void TPingReplyMessage::decodeMessage(string aMsg){
	/*
	* item[0] -> msg name
	* item[1] -> timestamp
	*/
	TBaseMessage::decodeMessage(aMsg);
	this->initFields();
}

void TPingReplyMessage::processMessage(TServerSockController* aController){

}
#pragma endregion
