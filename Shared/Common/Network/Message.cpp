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
#define USER_REG_REQ_TOK_NUM 4
#define USER_REG_REPLY_TOK_NUM 3
#define UPDATE_START_REQ_TOK_NUM 4
#define UPDATE_START_REPLY_TOK_NUM 4
#define ADD_NEW_FILE_TOK_NUM 7
#define UPDATE_FILE_TOK_NUM 7
#define REMOVE_FILE_TOK_NUM 4
#define FILE_ACK_TOK_NUM 4
#define UPDATE_STOP_REQ_TOK_NUM 3
#define UPDATE_STOP_REPLY_TOK_NUM 5
#define GET_VERSIONS_REQ_TOK_NUM 4
#define GET_VERSIONS_REPLY_MIN_TOK_NUM 5
#define RESTORE_VER_REQ_TOK_NUM 5
#define RESTORE_VER_REPLY_TOK_NUM 4
#define RESTORE_FILE_TOK_NUM 6
#define RESTORE_FILE_ACK_TOK_NUM 5
#define RESTORE_STOP_TOK_NUM 4
#define PING_TOK_NUM 3

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
void escape(string_ptr& aMsg){
	if (aMsg != nullptr){
		string from = to_string(MSG_SEP);
		string to = MSG_SEP_ESC;
		boost::replace_all(*aMsg, from, to);
	}
}

void unescape(string_ptr& aMsg){
	if (aMsg != nullptr){
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
	this->fEncodedMsg = nullptr;
	this->fItems = new_string_vector_ptr();
}

TBaseMessage::TBaseMessage(string_ptr& aMsg){
	this->fID = NO_ID;
	this->fEncodedMsg = move_string_ptr(aMsg);
	this->decodeMessageID();
}

TBaseMessage::~TBaseMessage() {
	if (this->fItems != nullptr){
		this->fItems.reset();
		this->fItems = nullptr;
	}
}

void TBaseMessage::decodeMessageID(){
	if (this->fEncodedMsg == nullptr)
		throw EMessageException("fEncodedMsg field is nullptr");

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

string_ptr TBaseMessage::encodeMessage(){
	int len = (int)this->fItems->size();
	string_ptr msg(new string());
	int i = 0;
	for (i = 0; i < len; i++){
		if (this->fItems->at(i) == nullptr)
			throw new EMessageException("item " + to_string(i) + " is nullptr");

		escape(this->fItems->at(i));
		*msg += *(this->fItems->at(i)) + MSG_SEP;
	}

	*msg += END_MSG;
	return move_string_ptr(msg);
}

void TBaseMessage::decodeMessage(){
	if (this->fEncodedMsg == nullptr)
		throw EMessageException("fEncodedMsg field is nullptr");

	// Turn the string into a stream.
	stringstream ss(*(this->fEncodedMsg));
	string tok;

	while (getline(ss, tok, MSG_SEP)) {
		string_ptr tok_ptr = make_string_ptr(tok);
		unescape(tok_ptr);
		this->fItems->push_back(move_string_ptr(tok_ptr));
	}

	int len = (int)this->fItems->size();
	if (*(this->fItems->at(len - 1)) != END_MSG)
		throw EMessageException("The given " + *(this->fItems->at(0)) + " message is not properly terminated");
}
#pragma endregion


////////////////////////////////////////
//      TUserRegistrReqMessage        //
////////////////////////////////////////
#pragma region "TUserRegistrReqMessage"
TUserRegistrReqMessage::TUserRegistrReqMessage(TBaseMessage_ptr& aBase){
	this->fID = aBase->getID();
	this->fItems = aBase->getTokens();
	this->fEncodedMsg = aBase->getMsg();

	this->decodeMessage();
}

TUserRegistrReqMessage::TUserRegistrReqMessage(const string aUser, const string aPass){
	this->fID = USER_REG_REQ_ID;
	this->fUser = make_string_ptr(aUser);
	this->fPass = make_string_ptr(aPass);
}

string_ptr TUserRegistrReqMessage::encodeMessage(){
	this->fItems->push_back(make_string_ptr(getMessageName(this->fID)));
	this->fItems->push_back(move_string_ptr(this->fUser));
	this->fItems->push_back(move_string_ptr(this->fPass));

	return TBaseMessage::encodeMessage();
}

void TUserRegistrReqMessage::decodeMessage(){
	/*
	* item[0] -> msg name
	* item[1] -> username
	* item[2] -> password
	*/
	TBaseMessage::decodeMessage();

	int size = (int)this->fItems->size();
	if (size != USER_REG_REQ_TOK_NUM)
		throw EMessageException("USER_REG_REQ message contains wrong number of tokens(" + to_string(size) + " instead of " + to_string(USER_REG_REQ_TOK_NUM) + ")");

	for (int i = 0; i < size; i++)
		if (this->fItems->at(i) == nullptr)
			throw EMessageException("item " + to_string(i) + " is nullptr");

	if (this->fID != USER_REG_REQ_ID)
		throw EMessageException("The given message is not a USER_REG_REQ message");

	//username
	if (*(this->fItems->at(1)) == EMPTY)
		throw EMessageException("The user name field cannot be empty");
	this->fUser = move_string_ptr(this->fItems->at(1));

	//password
	if (*(this->fItems->at(2)) == EMPTY)
		throw EMessageException("The password field cannot be empty");
	this->fPass = move_string_ptr(this->fItems->at(2));

	this->fItems->erase(this->fItems->begin(), this->fItems->end());
}
#pragma endregion


////////////////////////////////////////
//      TUserRegistrReplyMessage      //
////////////////////////////////////////
#pragma region "TUserRegistrReplyMessage"
TUserRegistrReplyMessage::TUserRegistrReplyMessage(TBaseMessage_ptr& aBase){
	this->fID = aBase->getID();
	this->fItems = aBase->getTokens();
	this->fEncodedMsg = aBase->getMsg();

	this->decodeMessage();
}

TUserRegistrReplyMessage::TUserRegistrReplyMessage(const bool aResp){
	this->fID = USER_REG_REPLY_ID;
	this->fResp = aResp;
}

string_ptr TUserRegistrReplyMessage::encodeMessage(){
	string resp = "";
	if (this->fResp)
		resp = TRUE_STR;
	else
		resp = FALSE_STR;

	this->fItems->push_back(make_string_ptr(getMessageName(this->fID)));
	this->fItems->push_back(make_string_ptr(resp));

	return TBaseMessage::encodeMessage();
}

void TUserRegistrReplyMessage::decodeMessage(){
	/*
	* item[0] -> msg name
	* item[1] -> response
	*/
	TBaseMessage::decodeMessage();

	int size = (int)this->fItems->size();
	if (size != USER_REG_REPLY_TOK_NUM)
		throw EMessageException("USER_REG_REPLY message contains wrong number of tokens(" + to_string(size) + " instead of " + to_string(USER_REG_REPLY_TOK_NUM) + ")");

	for (int i = 0; i < size; i++)
		if (this->fItems->at(i) == nullptr)
			throw EMessageException("item " + to_string(i) + " is nullptr");

	if (this->fID != USER_REG_REPLY_ID)
		throw EMessageException("The given message is not a USER_REG_REPLY message");

	//response
	if (*(this->fItems->at(1)) == EMPTY)
		throw EMessageException("The response field cannot be empty");
	if (*(this->fItems->at(1)) != TRUE_STR && *(this->fItems->at(1)) != FALSE_STR)
		throw EMessageException("The response field could be only true or false");

	if (*(this->fItems->at(1)) == TRUE_STR)
		this->fResp = true;
	else
		this->fResp = false;

	this->fItems->erase(this->fItems->begin(), this->fItems->end());
}
#pragma endregion


////////////////////////////////////////
//      TUpdateStartReqMessage        //
////////////////////////////////////////
#pragma region "TUpdateStartReqMessage"
TUpdateStartReqMessage::TUpdateStartReqMessage(TBaseMessage_ptr& aBase){
	this->fID = aBase->getID();
	this->fItems = aBase->getTokens();
	this->fEncodedMsg = aBase->getMsg();

	this->decodeMessage();
}

TUpdateStartReqMessage::TUpdateStartReqMessage(const string aUser, const string aPass){
	this->fID = UPDATE_START_REQ_ID;
	this->fUser = make_string_ptr(aUser);
	this->fPass = make_string_ptr(aPass);
}

string_ptr TUpdateStartReqMessage::encodeMessage(){
	this->fItems->push_back(make_string_ptr(getMessageName(this->fID)));
	this->fItems->push_back(move_string_ptr(this->fUser));
	this->fItems->push_back(move_string_ptr(this->fPass));

	return TBaseMessage::encodeMessage();
}

void TUpdateStartReqMessage::decodeMessage(){
	/*
	* item[0] -> msg name
	* item[1] -> username
	* item[2] -> password
	*/
	TBaseMessage::decodeMessage();

	int size = (int)this->fItems->size();
	if (size != UPDATE_START_REQ_TOK_NUM)
		throw EMessageException("UPDATE_START_REQ message contains wrong number of tokens(" + to_string(size) + " instead of " + to_string(UPDATE_START_REQ_TOK_NUM) + ")");

	for (int i = 0; i < size; i++)
		if (this->fItems->at(i) == nullptr)
			throw EMessageException("item " + to_string(i) + " is nullptr");

	if (this->fID != UPDATE_START_REQ_ID)
		throw EMessageException("The given message is not a UPDATE_START_REQ message");

	//username
	if (*(this->fItems->at(1)) == EMPTY)
		throw EMessageException("The user name field cannot be empty");
	this->fUser = move_string_ptr(this->fItems->at(1));

	//password
	if (*(this->fItems->at(2)) == EMPTY)
		throw EMessageException("The password field cannot be empty");
	this->fPass = move_string_ptr(this->fItems->at(2));

	this->fItems->erase(this->fItems->begin(), this->fItems->end());
}
#pragma endregion


////////////////////////////////////////
//      TUpdateStartReplyMessage      //
////////////////////////////////////////
#pragma region "TUpdateStartReplyMessage"
TUpdateStartReplyMessage::TUpdateStartReplyMessage(TBaseMessage_ptr& aBase){
	this->fID = aBase->getID();
	this->fItems = aBase->getTokens();
	this->fEncodedMsg = aBase->getMsg();

	this->decodeMessage();
}

TUpdateStartReplyMessage::TUpdateStartReplyMessage(const bool aResp, const string aToken){
	this->fID = UPDATE_START_REPLY_ID;
	this->fResp = aResp;
	this->fToken = make_string_ptr(aToken);
}

string_ptr TUpdateStartReplyMessage::encodeMessage(){
	string resp = "";
	if (this->fResp)
		resp = TRUE_STR;
	else
		resp = FALSE_STR;

	this->fItems->push_back(make_string_ptr(getMessageName(this->fID)));
	this->fItems->push_back(make_string_ptr(resp));
	this->fItems->push_back(move_string_ptr(this->fToken));

	return TBaseMessage::encodeMessage();
}

void TUpdateStartReplyMessage::decodeMessage(){
	/*
	* item[0] -> msg name
	* item[1] -> response
	* item[2] -> user token
	*/
	TBaseMessage::decodeMessage();

	int size = (int)this->fItems->size();
	if (size != UPDATE_START_REPLY_TOK_NUM)
		throw EMessageException("UPDATE_START_REPLY message contains wrong number of tokens(" + to_string(size) + " instead of " + to_string(UPDATE_START_REPLY_TOK_NUM) + ")");

	for (int i = 0; i < size; i++)
		if (this->fItems->at(i) == nullptr)
			throw EMessageException("item " + to_string(i) + " is nullptr");

	if (this->fID != UPDATE_START_REPLY_ID)
		throw EMessageException("The given message is not a UPDATE_START_REPLY message");

	//response
	if (*(this->fItems->at(1)) == EMPTY)
		throw EMessageException("The response field cannot be empty");
	if (*(this->fItems->at(1)) != TRUE_STR && *(this->fItems->at(1)) != FALSE_STR)
		throw EMessageException("The response field could be only true or false");

	if (*(this->fItems->at(1)) == TRUE_STR)
		this->fResp = true;
	else
		this->fResp = false;

	//user token
	if (*(this->fItems->at(2)) == EMPTY)
		throw EMessageException("The token field cannot be empty");
	this->fToken = move_string_ptr(this->fItems->at(2)); //TODO: implement -> checkToken(this->fItems->at(2));

	this->fItems->erase(this->fItems->begin(), this->fItems->end());
}
#pragma endregion


////////////////////////////////////////
//        TAddNewFileMessage          //
////////////////////////////////////////
#pragma region "TAddNewFileMessage"
TAddNewFileMessage::TAddNewFileMessage(TBaseMessage_ptr& aBase){
	this->fID = aBase->getID();
	this->fItems = aBase->getTokens();
	this->fEncodedMsg = aBase->getMsg();

	this->decodeMessage();
}

TAddNewFileMessage::TAddNewFileMessage(const string aToken, string aFilePath){
	this->fID = ADD_NEW_FILE_ID;
	this->fToken = make_string_ptr(aToken);
	this->fFilePath = make_string_ptr(aFilePath);
	this->fFileContent = nullptr;
	this->fChecksum = nullptr;
	this->fFileDate = time(nullptr);
}

string_ptr TAddNewFileMessage::encodeMessage(){
	//read file, encode it and calculate checksum
	this->fFileContent = opensslB64EncodeFile(*(this->fFilePath));
	this->fChecksum = opensslB64Checksum(*(this->fFileContent));

	this->fItems->push_back(make_string_ptr(getMessageName(this->fID)));
	this->fItems->push_back(move_string_ptr(this->fToken));
	this->fItems->push_back(move_string_ptr(this->fFilePath));
	this->fItems->push_back(move_string_ptr(this->fChecksum));
	this->fItems->push_back(make_string_ptr(timeToString(this->fFileDate)));
	this->fItems->push_back(move_string_ptr(this->fFileContent));

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

	int size = (int)this->fItems->size();
	if (size != ADD_NEW_FILE_TOK_NUM)
		throw EMessageException("ADD_NEW_FILE message contains wrong number of tokens(" + to_string(size) + " instead of " + to_string(ADD_NEW_FILE_TOK_NUM) + ")");

	for (int i = 0; i < size; i++)
		if (this->fItems->at(i) == nullptr)
			throw EMessageException("item " + to_string(i) + " is nullptr");

	//message id
	if (this->fID != ADD_NEW_FILE_ID)
		throw EMessageException("The given message is not a ADD_NEW_FILE message");

	//user token
	if (*(this->fItems->at(1)) == EMPTY)
		throw EMessageException("The token field cannot be empty");
	this->fToken = move_string_ptr(this->fItems->at(1)); //TODO: implement -> checkToken(this->fItems->at(2));

	//file path
	if (*(this->fItems->at(2)) == EMPTY)
		throw EMessageException("The token field cannot be empty");
	if (*(this->fItems->at(2)) == EMPTY)
		throw EMessageException("The file path field cannot be empty");
	//TODO: check se il path è valido
	//if (!isValidPath(this->fFilePath))
	//	throw EMessageException("The file path field must contain a valid path");
	this->fFilePath = move_string_ptr(this->fItems->at(2));

	//file checksum
	if (*(this->fItems->at(3)) == EMPTY)
		throw EMessageException("The file checksum field cannot be empty");
	this->fChecksum = move_string_ptr(this->fItems->at(3));

	//file date
	if (*(this->fItems->at(4)) == EMPTY)
		throw EMessageException("The file date field cannot be empty");
	this->fFileDate = stringToTime(*(this->fItems->at(4)));

	//file content
	if (*(this->fItems->at(5)) == EMPTY)
		throw EMessageException("The file content field cannot be empty");
	this->fFileContent = move_string_ptr(this->fItems->at(5));

	this->fItems->erase(this->fItems->begin(), this->fItems->end());

	//verify file correctness
	string_ptr myChecksum = nullptr;
	try{
		myChecksum = opensslB64Checksum(*(this->fFileContent));
	}
	catch (EOpensslException& e){
		throw EMessageException("error calculating file checksum: " + e.getMessage());
	}

	if (*(myChecksum) != *(this->fChecksum))
		throw EMessageException("The received file could be corrupted; checksums do not match");
}
#pragma endregion


////////////////////////////////////////
//        TUpdateFileMessage          //
////////////////////////////////////////
#pragma region "TUpdateFileMessage"
TUpdateFileMessage::TUpdateFileMessage(TBaseMessage_ptr& aBase){
	this->fID = aBase->getID();
	this->fItems = aBase->getTokens();
	this->fEncodedMsg = aBase->getMsg();

	this->decodeMessage();
}

TUpdateFileMessage::TUpdateFileMessage(const string aToken, string aFilePath){
	this->fID = UPDATE_FILE_ID;
	this->fToken = make_string_ptr(aToken);
	this->fFilePath = make_string_ptr(aFilePath);
	this->fFileContent = nullptr;
	this->fChecksum = nullptr;
	this->fFileDate = time(nullptr);
}

string_ptr TUpdateFileMessage::encodeMessage(){
	//read file, encode it and calculate checksum
	this->fFileContent = opensslB64EncodeFile(*(this->fFilePath));
	this->fChecksum = opensslB64Checksum(*(this->fFileContent));

	this->fItems->push_back(make_string_ptr(getMessageName(this->fID)));
	this->fItems->push_back(move_string_ptr(this->fToken));
	this->fItems->push_back(move_string_ptr(this->fFilePath));
	this->fItems->push_back(move_string_ptr(this->fChecksum));
	this->fItems->push_back(make_string_ptr(timeToString(this->fFileDate)));
	this->fItems->push_back(move_string_ptr(this->fFileContent));

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

	int size = (int)this->fItems->size();
	if (size != UPDATE_FILE_TOK_NUM)
		throw EMessageException("UPDATE_FILE message contains wrong number of tokens(" + to_string(size) + " instead of " + to_string(UPDATE_FILE_TOK_NUM) + ")");

	for (int i = 0; i < size; i++)
		if (this->fItems->at(i) == nullptr)
			throw EMessageException("item " + to_string(i) + " is nullptr");

	//message id
	if (this->fID != UPDATE_FILE_ID)
		throw EMessageException("The given message is not a UPDATE_FILE message");

	//user token
	if (*(this->fItems->at(1)) == EMPTY)
		throw EMessageException("The token field cannot be empty");
	this->fToken = move_string_ptr(this->fItems->at(1)); //TODO: implement -> checkToken(this->fItems->at(2));

	//file path
	if (*(this->fItems->at(2)) == EMPTY)
		throw EMessageException("The token field cannot be empty");
	if (*(this->fItems->at(2)) == EMPTY)
		throw EMessageException("The file path field cannot be empty");
	//TODO: check se il path è valido
	//if (!isValidPath(this->fFilePath))
	//	throw EMessageException("The file path field must contain a valid path");
	this->fFilePath = move_string_ptr(this->fItems->at(2));

	//file checksum
	if (*(this->fItems->at(3)) == EMPTY)
		throw EMessageException("The file checksum field cannot be empty");
	this->fChecksum = move_string_ptr(this->fItems->at(3));

	//file date
	if (*(this->fItems->at(4)) == EMPTY)
		throw EMessageException("The file date field cannot be empty");
	this->fFileDate = stringToTime(*(this->fItems->at(4)));

	//file content
	if (*(this->fItems->at(5)) == EMPTY)
		throw EMessageException("The file content field cannot be empty");
	this->fFileContent = move_string_ptr(this->fItems->at(5));

	this->fItems->erase(this->fItems->begin(), this->fItems->end());

	//verify file correctness
	string_ptr myChecksum = nullptr;
	try{
		myChecksum = opensslB64Checksum(*(this->fFileContent));
	}
	catch (EOpensslException& e){
		throw EMessageException("error calculating file checksum: " + e.getMessage());
	}

	if (*(myChecksum) != *(this->fChecksum))
		throw EMessageException("The received file could be corrupted; checksums do not match");
}
#pragma endregion


////////////////////////////////////////
//        TRemoveFileMessage          //
////////////////////////////////////////
#pragma region "TRemoveFileMessage"
TRemoveFileMessage::TRemoveFileMessage(TBaseMessage_ptr& aBase){
	this->fID = aBase->getID();
	this->fItems = aBase->getTokens();
	this->fEncodedMsg = aBase->getMsg();

	this->decodeMessage();
}

TRemoveFileMessage::TRemoveFileMessage(const string aToken, string aFilePath){
	this->fID = REMOVE_FILE_ID;
	this->fToken = make_string_ptr(aToken);
	this->fFilePath = make_string_ptr(aFilePath);
}

string_ptr TRemoveFileMessage::encodeMessage(){
	this->fItems->push_back(make_string_ptr(getMessageName(this->fID)));
	this->fItems->push_back(move_string_ptr(this->fToken));
	this->fItems->push_back(move_string_ptr(this->fFilePath));

	return TBaseMessage::encodeMessage();
}

void TRemoveFileMessage::decodeMessage(){
	/*
	* item[0] -> msg name
	* item[1] -> token
	* item[2] -> file path
	*/
	TBaseMessage::decodeMessage();

	int size = (int)this->fItems->size();
	if (size != REMOVE_FILE_TOK_NUM)
		throw EMessageException("REMOVE_FILE message contains wrong number of tokens(" + to_string(size) + " instead of " + to_string(REMOVE_FILE_TOK_NUM) + ")");

	for (int i = 0; i < size; i++)
		if (this->fItems->at(i) == nullptr)
			throw EMessageException("item " + to_string(i) + " is nullptr");

	//message id
	if (this->fID != REMOVE_FILE_ID)
		throw EMessageException("The given message is not a REMOVE_FILE message");

	//user token
	if (*(this->fItems->at(1)) == EMPTY)
		throw EMessageException("The token field cannot be empty");
	this->fToken = move_string_ptr(this->fItems->at(1)); //TODO: implement -> checkToken(this->fItems->at(2));

	//file path
	if (*(this->fItems->at(2)) == EMPTY)
		throw EMessageException("The token field cannot be empty");
	if (*(this->fItems->at(2)) == EMPTY)
		throw EMessageException("The file path field cannot be empty");
	//TODO: check se il path è valido
	//if (!isValidPath(this->fFilePath))
	//	throw EMessageException("The file path field must contain a valid path");
	this->fFilePath = move_string_ptr(this->fItems->at(2));

	this->fItems->erase(this->fItems->begin(), this->fItems->end());
}
#pragma endregion


///////////////////////////////////
//        TFileAckMessage        //
///////////////////////////////////
#pragma region "TFileAckMessage"
TFileAckMessage::TFileAckMessage(TBaseMessage_ptr& aBase){
	this->fID = aBase->getID();
	this->fItems = aBase->getTokens();
	this->fEncodedMsg = aBase->getMsg();

	this->decodeMessage();
}

TFileAckMessage::TFileAckMessage(const bool aResp, const string aFilePath){
	this->fID = FILE_ACK_ID;
	this->fResp = aResp;
	this->fFilePath = make_string_ptr(aFilePath);
}

string_ptr TFileAckMessage::encodeMessage(){
	string resp = "";
	if (this->fResp)
		resp = TRUE_STR;
	else
		resp = FALSE_STR;

	this->fItems->push_back(make_string_ptr(getMessageName(this->fID)));
	this->fItems->push_back(make_string_ptr(resp));
	this->fItems->push_back(move_string_ptr(this->fFilePath));

	return TBaseMessage::encodeMessage();
}

void TFileAckMessage::decodeMessage(){
	/*
	* item[0] -> msg name
	* item[1] -> response
	* item[2] -> file path
	*/
	TBaseMessage::decodeMessage();

	int size = (int)this->fItems->size();
	if (size != FILE_ACK_TOK_NUM)
		throw EMessageException("FILE_ACK message contains wrong number of tokens(" + to_string(size) + " instead of " + to_string(FILE_ACK_TOK_NUM) + ")");

	for (int i = 0; i < size; i++)
		if (this->fItems->at(i) == nullptr)
			throw EMessageException("item " + to_string(i) + " is nullptr");

	if (this->fID != FILE_ACK_ID)
		throw EMessageException("The given message is not a FILE_ACK message");

	//response
	if (*(this->fItems->at(1)) == EMPTY)
		throw EMessageException("The response field cannot be empty");
	if (*(this->fItems->at(1)) != TRUE_STR && *(this->fItems->at(1)) != FALSE_STR)
		throw EMessageException("The response field could be only true or false");

	if (*(this->fItems->at(1)) == TRUE_STR)
		this->fResp = true;
	else
		this->fResp = false;

	//file path
	if (*(this->fItems->at(2)) == EMPTY)
		throw EMessageException("The token field cannot be empty");
	//TODO: check se il path è valido
	//if (!isValidPath(this->fFilePath))
	//	throw EMessageException("The file path field must contain a valid path");
	this->fFilePath = move_string_ptr(this->fItems->at(2));

	this->fItems->erase(this->fItems->begin(), this->fItems->end());
}
#pragma endregion


////////////////////////////////////////
//       TUpdateStopReqMessage        //
////////////////////////////////////////
#pragma region "TUpdateStopReqMessage"
TUpdateStopReqMessage::TUpdateStopReqMessage(TBaseMessage_ptr& aBase){
	this->fID = aBase->getID();
	this->fItems = aBase->getTokens();
	this->fEncodedMsg = aBase->getMsg();

	this->decodeMessage();
}

TUpdateStopReqMessage::TUpdateStopReqMessage(const string aToken){
	this->fID = UPDATE_STOP_REQ_ID;
	this->fToken = make_string_ptr(aToken);
}

string_ptr TUpdateStopReqMessage::encodeMessage(){
	this->fItems->push_back(make_string_ptr(getMessageName(this->fID)));
	this->fItems->push_back(move_string_ptr(this->fToken));

	return TBaseMessage::encodeMessage();
}

void TUpdateStopReqMessage::decodeMessage(){
	/*
	* item[0] -> msg name
	* item[1] -> user token
	*/
	TBaseMessage::decodeMessage();

	int size = (int)this->fItems->size();
	if (size != UPDATE_STOP_REQ_TOK_NUM)
		throw EMessageException("UPDATE_STOP_REQ message contains wrong number of tokens(" + to_string(size) + " instead of " + to_string(UPDATE_STOP_REQ_TOK_NUM) + ")");

	for (int i = 0; i < size; i++)
		if (this->fItems->at(i) == nullptr)
			throw EMessageException("item " + to_string(i) + " is nullptr");

	if (this->fID != UPDATE_STOP_REQ_ID)
		throw EMessageException("The given message is not a UPDATE_STOP_REQ message");

	//user token
	if (*(this->fItems->at(1)) == EMPTY)
		throw EMessageException("The token field cannot be empty");
	this->fToken = move_string_ptr(this->fItems->at(1)); //TODO: implement -> checkToken(this->fItems->at(2));

	this->fItems->erase(this->fItems->begin(), this->fItems->end());
}
#pragma endregion


////////////////////////////////////////
//      TUpdateStopReplyMessage       //
////////////////////////////////////////
#pragma region "TUpdateStopReplyMessage"
TUpdateStopReplyMessage::TUpdateStopReplyMessage(TBaseMessage_ptr& aBase){
	this->fID = aBase->getID();
	this->fItems = aBase->getTokens();
	this->fEncodedMsg = aBase->getMsg();

	this->decodeMessage();
}

TUpdateStopReplyMessage::TUpdateStopReplyMessage(const bool aResp, unsigned int aVersion, time_t aTime){
	this->fID = UPDATE_STOP_REPLY_ID;
	this->fResp = aResp;
	this->fVersion = aVersion;
	this->fTime = aTime;
}

string_ptr TUpdateStopReplyMessage::encodeMessage(){
	string resp = "";
	if (this->fResp)
		resp = TRUE_STR;
	else
		resp = FALSE_STR;

	this->fItems->push_back(make_string_ptr(getMessageName(this->fID)));
	this->fItems->push_back(make_string_ptr(resp));
	this->fItems->push_back(make_string_ptr(to_string(this->fVersion)));
	this->fItems->push_back(make_string_ptr(timeToString(this->fTime)));

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

	int size = (int)this->fItems->size();
	if (size != UPDATE_STOP_REPLY_TOK_NUM)
		throw EMessageException("UPDATE_STOP_REPLY message contains wrong number of tokens(" + to_string(size) + " instead of " + to_string(UPDATE_STOP_REPLY_TOK_NUM) + ")");

	for (int i = 0; i < size; i++)
		if (this->fItems->at(i) == nullptr)
			throw EMessageException("item " + to_string(i) + " is nullptr");

	if (this->fID != UPDATE_STOP_REPLY_ID)
		throw EMessageException("The given message is not a UPDATE_STOP_REPLY message");

	//response
	if (*(this->fItems->at(1)) == EMPTY)
		throw EMessageException("The response field cannot be empty");
	if (*(this->fItems->at(1)) != TRUE_STR && *(this->fItems->at(1)) != FALSE_STR)
		throw EMessageException("The response field could be only true or false");

	if (*(this->fItems->at(1)) == TRUE_STR)
		this->fResp = true;
	else
		this->fResp = false;

	//version number
	if (*(this->fItems->at(2)) == EMPTY)
		throw EMessageException("The version field cannot be empty");
	//TODO: check if this->fItems->at(2) contains a number
	this->fVersion = stoi(*(this->fItems->at(2)));

	//version timestamp
	if (*(this->fItems->at(3)) == EMPTY)
		throw EMessageException("The version timestamp field cannot be empty");
	this->fTime = stringToTime(*(this->fItems->at(3)));

	this->fItems->erase(this->fItems->begin(), this->fItems->end());
}
#pragma endregion


////////////////////////////////////////
//      TGetVersionsReqMessage        //
////////////////////////////////////////
#pragma region "TGetVersionsReqMessage"
TGetVersionsReqMessage::TGetVersionsReqMessage(TBaseMessage_ptr& aBase){
	this->fID = aBase->getID();
	this->fItems = aBase->getTokens();
	this->fEncodedMsg = aBase->getMsg();

	this->decodeMessage();
}

TGetVersionsReqMessage::TGetVersionsReqMessage(const string aUser, const string aPass){
	this->fID = GET_VERSIONS_REQ_ID;
	this->fUser = make_string_ptr(aUser);
	this->fPass = make_string_ptr(aPass);
}

string_ptr TGetVersionsReqMessage::encodeMessage(){
	this->fItems->push_back(make_string_ptr(getMessageName(this->fID)));
	this->fItems->push_back(move_string_ptr(this->fUser));
	this->fItems->push_back(move_string_ptr(this->fPass));

	return TBaseMessage::encodeMessage();
}

void TGetVersionsReqMessage::decodeMessage(){
	/*
	* item[0] -> msg name
	* item[1] -> username
	* item[2] -> password
	*/
	TBaseMessage::decodeMessage();
	
	int size = (int)this->fItems->size();
	if (size != GET_VERSIONS_REQ_TOK_NUM)
		throw EMessageException("GET_VERSIONS_REQ message contains wrong number of tokens(" + to_string(size) + " instead of " + to_string(GET_VERSIONS_REQ_TOK_NUM) + ")");

	for (int i = 0; i < size; i++)
		if (this->fItems->at(i) == nullptr)
			throw EMessageException("item " + to_string(i) + " is nullptr");

	if (this->fID != GET_VERSIONS_REQ_ID)
		throw EMessageException("The given message is not a GET_VERSIONS_REQ message");

	//username
	if (*(this->fItems->at(1)) == EMPTY)
		throw EMessageException("The user name field cannot be empty");
	this->fUser = move_string_ptr(this->fItems->at(1));

	//password
	if (*(this->fItems->at(2)) == EMPTY)
		throw EMessageException("The password field cannot be empty");
	this->fPass = move_string_ptr(this->fItems->at(2));

	this->fItems->erase(this->fItems->begin(), this->fItems->end());
}
#pragma endregion


////////////////////////////////////////
//      TGetVersionsReplyMessage      //
////////////////////////////////////////
#pragma region "TGetVersionsReplyMessage"
TGetVersionsReplyMessage::TGetVersionsReplyMessage(TBaseMessage_ptr& aBase){
	this->fID = aBase->getID();
	this->fItems = aBase->getTokens();
	this->fEncodedMsg = aBase->getMsg();

	this->decodeMessage();
}

TGetVersionsReplyMessage::TGetVersionsReplyMessage(const unsigned int aTotVersions, const unsigned int aOldestVersion, const unsigned int aLastVersion, map<unsigned int, time_t> aVersions){
	this->fID = GET_VERSIONS_REPLY_ID;
	this->fTotVersions = aTotVersions;
	this->fOldestVersion = aOldestVersion;
	this->fLastVersion = aLastVersion;
	this->fVersions = aVersions;
}

string_ptr TGetVersionsReplyMessage::encodeMessage(){
	this->fItems->push_back(make_string_ptr(getMessageName(this->fID)));
	this->fItems->push_back(make_string_ptr(to_string(this->fTotVersions)));
	this->fItems->push_back(make_string_ptr(to_string(this->fOldestVersion)));
	this->fItems->push_back(make_string_ptr(to_string(this->fLastVersion)));

	int size = (int)this->fVersions.size();
	for (int i = GET_VERSIONS_REPLY_MIN_TOK_NUM + 1; i < size; i++){
		this->fItems->push_back(make_string_ptr(timeToString(this->fVersions.at(i))));
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

	int size = (int)this->fItems->size();
	if (size < GET_VERSIONS_REPLY_MIN_TOK_NUM)
		throw EMessageException("GET_VERSIONS_REPLY message does not contain the minimum number of tokens(" + to_string(GET_VERSIONS_REPLY_MIN_TOK_NUM) + ")");

	for (int i = 0; i < size; i++)
		if (this->fItems->at(i) == nullptr)
			throw EMessageException("item " + to_string(i) + " is nullptr");

	if (this->fID != GET_VERSIONS_REPLY_ID)
		throw EMessageException("The given message is not a GET_VERSIONS_REPLY message");

	//tot number of versions
	if (*(this->fItems->at(1)) == EMPTY)
		throw EMessageException("The total versions number field cannot be empty");
	//TODO: check if this->fItems->at(1) contains a number
	this->fTotVersions = stoi(*(this->fItems->at(1)));

	//oldest version
	if (*(this->fItems->at(2)) == EMPTY)
		throw EMessageException("The oldest version number field cannot be empty");
	//TODO: check if this->fItems->at(2) contains a number
	this->fOldestVersion = stoi(*(this->fItems->at(2)));

	//last version
	if (*(this->fItems->at(3)) == EMPTY)
		throw EMessageException("The last version number field cannot be empty");
	//TODO: check if this->fItems->at(3) contains a number
	this->fLastVersion = stoi(*(this->fItems->at(3)));

	int totSize = GET_VERSIONS_REPLY_MIN_TOK_NUM + (this->fLastVersion - this->fOldestVersion);
	if (size != totSize)
		throw EMessageException("GET_VERSIONS_REPLY message contains wrong number of tokens(" + to_string(size) + " instead of " + to_string(totSize) + ")");

	int i, j;
	for (i = GET_VERSIONS_REPLY_MIN_TOK_NUM + 1, j = this->fOldestVersion; i < totSize; i++, j++){
		this->fVersions.emplace(j, stringToTime(*(this->fItems->at(i))));
	}

	this->fItems->erase(this->fItems->begin(), this->fItems->end());
}
#pragma endregion


////////////////////////////////////////
//       TRestoreVerReqMessage        //
////////////////////////////////////////
#pragma region "TRestoreVerReqMessage"
TRestoreVerReqMessage::TRestoreVerReqMessage(TBaseMessage_ptr& aBase){
	this->fID = aBase->getID();
	this->fItems = aBase->getTokens();
	this->fEncodedMsg = aBase->getMsg();

	this->decodeMessage();
}

TRestoreVerReqMessage::TRestoreVerReqMessage(const string aUser, const string aPass, const unsigned int aVersion){
	this->fID = RESTORE_VER_REQ_ID;
	this->fUser = make_string_ptr(aUser);
	this->fPass = make_string_ptr(aPass);
	this->fVersion = aVersion;
}

string_ptr TRestoreVerReqMessage::encodeMessage(){
	this->fItems->push_back(make_string_ptr(getMessageName(this->fID)));
	this->fItems->push_back(move_string_ptr(this->fUser));
	this->fItems->push_back(move_string_ptr(this->fPass));
	this->fItems->push_back(make_string_ptr(to_string(this->fVersion)));

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
	
	int size = (int)this->fItems->size();
	if (size != RESTORE_VER_REQ_TOK_NUM)
		throw EMessageException("RESTORE_VER_REQ message contains wrong number of tokens(" + to_string(size) + " instead of " + to_string(RESTORE_VER_REQ_TOK_NUM) + ")");

	for (int i = 0; i < size; i++)
		if (this->fItems->at(i) == nullptr)
			throw EMessageException("item " + to_string(i) + " is nullptr");

	if (this->fID != RESTORE_VER_REQ_ID)
		throw EMessageException("The given message is not a RESTORE_VER_REQ message");

	//username
	if (*(this->fItems->at(1)) == EMPTY)
		throw EMessageException("The user name field cannot be empty");
	this->fUser = move_string_ptr(this->fItems->at(1));

	//password
	if (*(this->fItems->at(2)) == EMPTY)
		throw EMessageException("The password field cannot be empty");
	this->fPass = move_string_ptr(this->fItems->at(2));


	//version
	if (*(this->fItems->at(3)) == EMPTY)
		throw EMessageException("The version number field cannot be empty");
	//TODO: check if this->fItems->at(3) contains a number
	this->fVersion = stoi(*(this->fItems->at(3)));

	this->fItems->erase(this->fItems->begin(), this->fItems->end());
}
#pragma endregion


////////////////////////////////////////
//      TRestoreVerReplyMessage       //
////////////////////////////////////////
#pragma region "TRestoreVerReplyMessage"
TRestoreVerReplyMessage::TRestoreVerReplyMessage(TBaseMessage_ptr& aBase){
	this->fID = aBase->getID();
	this->fItems = aBase->getTokens();
	this->fEncodedMsg = aBase->getMsg();

	this->decodeMessage();
}

TRestoreVerReplyMessage::TRestoreVerReplyMessage(const bool aResp, const string aToken){
	this->fID = RESTORE_VER_REPLY_ID;
	this->fResp = aResp;
	this->fToken = make_string_ptr(aToken);
}

string_ptr TRestoreVerReplyMessage::encodeMessage(){
	string resp = "";
	if (this->fResp)
		resp = TRUE_STR;
	else
		resp = FALSE_STR;

	this->fItems->push_back(make_string_ptr(getMessageName(this->fID)));
	this->fItems->push_back(make_string_ptr(resp));
	this->fItems->push_back(move_string_ptr(this->fToken));

	return TBaseMessage::encodeMessage();
}

void TRestoreVerReplyMessage::decodeMessage(){
	/*
	* item[0] -> msg name
	* item[1] -> response
	* item[2] -> user token
	*/
	TBaseMessage::decodeMessage();

	int size = (int)this->fItems->size();
	if (size != RESTORE_VER_REPLY_TOK_NUM)
		throw EMessageException("RESTORE_VER_REPLY message contains wrong number of tokens(" + to_string(size) + " instead of " + to_string(RESTORE_VER_REPLY_TOK_NUM) + ")");

	for (int i = 0; i < size; i++)
		if (this->fItems->at(i) == nullptr)
			throw EMessageException("item " + to_string(i) + " is nullptr");

	if (this->fID != RESTORE_VER_REPLY_ID)
		throw EMessageException("The given message is not a UPDATE_START_REPLY message");

	//response
	if (*(this->fItems->at(1)) == EMPTY)
		throw EMessageException("The response field cannot be empty");
	if (*(this->fItems->at(1)) != TRUE_STR && *(this->fItems->at(1)) != FALSE_STR)
		throw EMessageException("The response field could be only true or false");

	if (*(this->fItems->at(1)) == TRUE_STR)
		this->fResp = true;
	else
		this->fResp = false;

	//user token
	if (*(this->fItems->at(2)) == EMPTY)
		throw EMessageException("The token field cannot be empty");
	this->fToken = move_string_ptr(this->fItems->at(2)); //TODO: implement -> checkToken(this->fItems->at(2));

	this->fItems->erase(this->fItems->begin(), this->fItems->end());
}
#pragma endregion


////////////////////////////////////////
//        TRestoreFileMessage         //
////////////////////////////////////////
#pragma region "TRestoreFileMessage"
TRestoreFileMessage::TRestoreFileMessage(TBaseMessage_ptr& aBase){
	this->fID = aBase->getID();
	this->fItems = aBase->getTokens();
	this->fEncodedMsg = aBase->getMsg();

	this->decodeMessage();
}

TRestoreFileMessage::TRestoreFileMessage(string aFilePath){
	this->fID = RESTORE_FILE_ID;
	this->fFilePath = make_string_ptr(aFilePath);
	this->fFileContent = nullptr;
	this->fChecksum = nullptr;
	this->fFileDate = time(nullptr);
}

string_ptr TRestoreFileMessage::encodeMessage(){
	//read file, encode it and calculate checksum
	this->fFileContent = opensslB64EncodeFile(*(this->fFilePath));
	this->fChecksum = opensslB64Checksum(*(this->fFileContent));

	this->fItems->push_back(make_string_ptr(getMessageName(this->fID)));
	this->fItems->push_back(move_string_ptr(this->fFilePath));
	this->fItems->push_back(move_string_ptr(this->fChecksum));
	this->fItems->push_back(make_string_ptr(timeToString(this->fFileDate)));
	this->fItems->push_back(move_string_ptr(this->fFileContent));

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

	int size = (int)this->fItems->size();
	if (size != RESTORE_FILE_TOK_NUM)
		throw EMessageException("RESTORE_FILE message contains wrong number of tokens(" + to_string(size) + " instead of " + to_string(RESTORE_FILE_TOK_NUM) + ")");

	for (int i = 0; i < size; i++)
		if (this->fItems->at(i) == nullptr)
			throw EMessageException("item " + to_string(i) + " is nullptr");

	//message id
	if (this->fID != RESTORE_FILE_ID)
		throw EMessageException("The given message is not a RESTORE_FILE message");

	//file path
	if (*(this->fItems->at(1)) == EMPTY)
		throw EMessageException("The token field cannot be empty");
	if (*(this->fItems->at(1)) == EMPTY)
		throw EMessageException("The file path field cannot be empty");
	//TODO: check se il path è valido
	//if (!isValidPath(this->fFilePath))
	//	throw EMessageException("The file path field must contain a valid path");
	this->fFilePath = move_string_ptr(this->fItems->at(1));

	//file checksum
	if (*(this->fItems->at(2)) == EMPTY)
		throw EMessageException("The file checksum field cannot be empty");
	this->fChecksum = move_string_ptr(this->fItems->at(2));

	//file date
	if (*(this->fItems->at(3)) == EMPTY)
		throw EMessageException("The file date field cannot be empty");
	this->fFileDate = stringToTime(*(this->fItems->at(3)));

	//file content
	if (*(this->fItems->at(4)) == EMPTY)
		throw EMessageException("The file content field cannot be empty");
	this->fFileContent = move_string_ptr(this->fItems->at(4));

	this->fItems->erase(this->fItems->begin(), this->fItems->end());

	//verify file correctness
	string_ptr myChecksum = nullptr;
	try{
		myChecksum = opensslB64Checksum(*(this->fFileContent));
	}
	catch (EOpensslException& e){
		throw EMessageException("error calculating file checksum: " + e.getMessage());
	}

	if (*(myChecksum) != *(this->fChecksum))
		throw EMessageException("The received file could be corrupted; checksums do not match");
}
#pragma endregion


//////////////////////////////////////////
//        TRestoreFileAckMessage        //
//////////////////////////////////////////
#pragma region "TRestoreFileAckMessage"
TRestoreFileAckMessage::TRestoreFileAckMessage(TBaseMessage_ptr& aBase){
	this->fID = aBase->getID();
	this->fItems = aBase->getTokens();
	this->fEncodedMsg = aBase->getMsg();

	this->decodeMessage();
}

TRestoreFileAckMessage::TRestoreFileAckMessage(const string aToken, const bool aResp, const string aFilePath){
	this->fID = RESTORE_FILE_ACK_ID;
	this->fToken = make_string_ptr(aToken);
	this->fFilePath = make_string_ptr(aFilePath);
	this->fResp = aResp;
}

string_ptr TRestoreFileAckMessage::encodeMessage(){
	string resp = "";
	if (this->fResp)
		resp = TRUE_STR;
	else
		resp = FALSE_STR;

	this->fItems->push_back(make_string_ptr(getMessageName(this->fID)));
	this->fItems->push_back(move_string_ptr(this->fToken));
	this->fItems->push_back(make_string_ptr(resp));
	this->fItems->push_back(move_string_ptr(this->fFilePath));

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

	int size = (int)this->fItems->size();
	if (size != RESTORE_FILE_ACK_TOK_NUM)
		throw EMessageException("RESTORE_FILE_ACK message contains wrong number of tokens(" + to_string(size) + " instead of " + to_string(RESTORE_FILE_ACK_TOK_NUM) + ")");

	for (int i = 0; i < size; i++)
		if (this->fItems->at(i) == nullptr)
			throw EMessageException("item " + to_string(i) + " is nullptr");

	if (this->fID != RESTORE_FILE_ACK_ID)
		throw EMessageException("The given message is not a RESTORE_FILE_ACK message");

	//user token
	if (*(this->fItems->at(1)) == EMPTY)
		throw EMessageException("The token field cannot be empty");
	this->fToken = move_string_ptr(this->fItems->at(1)); //TODO: implement -> checkToken(this->fItems->at(1));

	//response
	if (*(this->fItems->at(2)) == EMPTY)
		throw EMessageException("The response field cannot be empty");
	if (*(this->fItems->at(2)) != TRUE_STR && *(this->fItems->at(2)) != FALSE_STR)
		throw EMessageException("The response field could be only true or false");

	if (*(this->fItems->at(2)) == TRUE_STR)
		this->fResp = true;
	else
		this->fResp = false;

	//file path
	if (*(this->fItems->at(3)) == EMPTY)
		throw EMessageException("The token field cannot be empty");
	//TODO: check se il path è valido
	//if (!isValidPath(this->fFilePath))
	//	throw EMessageException("The file path field must contain a valid path");
	this->fFilePath = move_string_ptr(this->fItems->at(3));

	this->fItems->erase(this->fItems->begin(), this->fItems->end());
}
#pragma endregion


////////////////////////////////////
//      TRestoreStopMessage       //
////////////////////////////////////
#pragma region "TRestoreStopMessage"
TRestoreStopMessage::TRestoreStopMessage(TBaseMessage_ptr& aBase){
	this->fID = aBase->getID();
	this->fItems = aBase->getTokens();
	this->fEncodedMsg = aBase->getMsg();

	this->decodeMessage();
}

TRestoreStopMessage::TRestoreStopMessage(unsigned int aVersion, time_t aTime){
	this->fID = RESTORE_STOP_ID;
	this->fVersion = aVersion;
	this->fTime = aTime;
}

string_ptr TRestoreStopMessage::encodeMessage(){
	this->fItems->push_back(make_string_ptr(getMessageName(this->fID)));
	this->fItems->push_back(make_string_ptr(to_string(this->fVersion)));
	this->fItems->push_back(make_string_ptr(timeToString(this->fTime)));

	return TBaseMessage::encodeMessage();
}

void TRestoreStopMessage::decodeMessage(){
	/*
	* item[0] -> msg name
	* item[1] -> version number
	* item[2] -> version timestamp
	*/
	TBaseMessage::decodeMessage();

	int size = (int)this->fItems->size();
	if (size != RESTORE_STOP_TOK_NUM)
		throw EMessageException("RESTORE_STOP message contains wrong number of tokens(" + to_string(size) + " instead of " + to_string(RESTORE_STOP_TOK_NUM) + ")");

	for (int i = 0; i < size; i++)
		if (this->fItems->at(i) == nullptr)
			throw EMessageException("item " + to_string(i) + " is nullptr");

	if (this->fID != RESTORE_STOP_ID)
		throw EMessageException("The given message is not a RESTORE_STOP message");

	//version number
	if (*(this->fItems->at(1)) == EMPTY)
		throw EMessageException("The version field cannot be empty");
	//TODO: check if this->fItems->at(1) contains a number
	this->fVersion = stoi(*(this->fItems->at(1)));

	//version timestamp
	if (*(this->fItems->at(2)) == EMPTY)
		throw EMessageException("The version timestamp field cannot be empty");
	this->fTime = stringToTime(*(this->fItems->at(2)));

	this->fItems->erase(this->fItems->begin(), this->fItems->end());
}
#pragma endregion


//////////////////////////////////////
//        TPingReqMessage           //
//////////////////////////////////////
#pragma region "TPingReqMessage"
TPingReqMessage::TPingReqMessage(TBaseMessage_ptr& aBase){
	this->fID = aBase->getID();
	this->fItems = aBase->getTokens();
	this->fEncodedMsg = aBase->getMsg();

	this->decodeMessage();
}

TPingReqMessage::TPingReqMessage(){
	this->fID = PING_REQ_ID;
	this->fTime = time(nullptr);
}

string_ptr TPingReqMessage::encodeMessage(){
	this->fItems->push_back(make_string_ptr(getMessageName(this->fID)));
	this->fItems->push_back(make_string_ptr(timeToString(this->fTime)));

	return TBaseMessage::encodeMessage();
}

void TPingReqMessage::decodeMessage(){
	/*
	* item[0] -> msg name
	* item[1] -> timestamp
	*/
	TBaseMessage::decodeMessage();
	
	int size = (int)this->fItems->size();
	if (size != PING_TOK_NUM)
		throw EMessageException("PING_REQ message contains wrong number of tokens(" + to_string(size) + " instead of " + to_string(PING_TOK_NUM) + ")");

	for (int i = 0; i < size; i++)
		if (this->fItems->at(i) == nullptr)
			throw EMessageException("item " + to_string(i) + " is nullptr");

	if (this->fID != PING_REQ_ID)
		throw EMessageException("The given message is not a PING_REQ message");

	//timestamp
	if (*(this->fItems->at(1)) == EMPTY)
		throw EMessageException("The time-stamp field cannot be empty");
	this->fTime = stringToTime(*(this->fItems->at(1)));

	this->fItems->erase(this->fItems->begin(), this->fItems->end());
}
#pragma endregion


//////////////////////////////////////
//        TPingReplyMessage         //
//////////////////////////////////////
#pragma region "TPingReplyMessage"
TPingReplyMessage::TPingReplyMessage(TBaseMessage_ptr& aBase){
	this->fID = aBase->getID();
	this->fItems = aBase->getTokens();
	this->fEncodedMsg = aBase->getMsg();

	this->decodeMessage();
}

TPingReplyMessage::TPingReplyMessage(){
	this->fID = PING_REPLY_ID;
	this->fTime = time(nullptr);
}

string_ptr TPingReplyMessage::encodeMessage(){
	this->fItems->push_back(make_string_ptr(getMessageName(this->fID)));
	this->fItems->push_back(make_string_ptr(timeToString(this->fTime)));

	return TBaseMessage::encodeMessage();
}

void TPingReplyMessage::decodeMessage(){
	/*
	* item[0] -> msg name
	* item[1] -> timestamp
	*/
	TBaseMessage::decodeMessage();

	int size = (int)this->fItems->size();
	if (size != PING_TOK_NUM)
		throw EMessageException("PING_REPLY message contains wrong number of tokens(" + to_string(size) + " instead of " + to_string(PING_TOK_NUM) + ")");

	for (int i = 0; i < size; i++)
		if (this->fItems->at(i) == nullptr)
			throw EMessageException("item " + to_string(i) + " is nullptr");

	if (this->fID != PING_REPLY_ID)
		throw EMessageException("The given message is not a PING_REPLY message");

	//timestamp
	if (*(this->fItems->at(1)) == EMPTY)
		throw EMessageException("The time-stamp field cannot be empty");
	this->fTime = stringToTime(*(this->fItems->at(1)));

	this->fItems->erase(this->fItems->begin(), this->fItems->end());
}
#pragma endregion
