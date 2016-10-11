/*
 * Authors: Angelo Prudentino & Daniele Testa
 * Date: 06/03/2016
 * File: Message.cpp
 * Description: this file contains all classes implementing
 *              messages exchanged between client and server
 *
 */

#include <boost/algorithm/string/replace.hpp>
#include <boost/filesystem.hpp>
#include <sstream>
#include <vector>
#include <time.h>
#include "Message.h"

using namespace std;
using namespace boost::filesystem;

#define MSG_SEP '$'
#define MSG_SEP_ESC "&#36"
#define MSG_INVALID "invalid"

//Message tokens
#define USER_REG_REQ_TOK_NUM 5
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
#define GET_LAST_VERSION_REQ_TOK_NUM 4
#define GET_LAST_VERSION_REPLY_TOK_NUM 4
#define RESTORE_VER_REQ_TOK_NUM 5
#define RESTORE_VER_REPLY_TOK_NUM 4
#define RESTORE_FILE_TOK_NUM 6
#define RESTORE_FILE_ACK_TOK_NUM 5
#define RESTORE_STOP_TOK_NUM 4
#define PING_TOK_NUM 4
#define VERIFY_CRED_REQ_TOK_NUM 4
#define VERIFY_CRED_REPLY_TOK_NUM 4
#define SYSTEM_ERR_TOK_NUM 3

//Message names
#define MSG_NUM 24
static string messageNames[] = {
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
	/* 12*/ "GET_LAST_VERSION_REQ",
	/* 13*/ "GET_LAST_VERSION_REPLY",
	/* 14*/ "RESTORE_VER_REQ",
	/* 15*/ "RESTORE_VER_REPLY",
	/* 16*/ "RESTORE_FILE",
	/* 17*/ "RESTORE_FILE_ACK",
	/* 18*/ "RESTORE_STOP",
	/* 19*/ "PING_REQ",
	/* 20*/ "PING_REPLY",
	/* 21*/ "VERIFY_CREDANTIALS_REQ",
	/* 22*/ "VERIFY_CREDANTIALS_REPLY",
	/* 23*/ "SYSTEM_ERROR"
};

#pragma region "Message Utility"
const bool isValidMessage(const string& aName){
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
		const string from = to_string(MSG_SEP);
		const string to = MSG_SEP_ESC;
		boost::replace_all(*aMsg, from, to);
	}
}

void unescape(string_ptr& aMsg){
	if (aMsg != nullptr){
		const string from = MSG_SEP_ESC;
		const string to = to_string(MSG_SEP);
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
	this->fItems = new_string_vector_ptr();
	this->decodeMessageID();
}

TBaseMessage::~TBaseMessage() {
	if (this->fItems != nullptr){
		//for (string_vector::iterator it = this->fItems->begin(); it != this->fItems->end(); it++)
		//	it->reset();
		this->fItems->clear();
		this->fItems.reset();
		this->fItems = nullptr;
	}
}

void TBaseMessage::decodeMessageID(){
	if (this->fEncodedMsg == nullptr)
		return;

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
			throw EMessageException("item " + to_string(i) + " is nullptr");

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

TUserRegistrReqMessage::TUserRegistrReqMessage(const string& aUser, const string& aPass, const string& aPath){
	this->fID = USER_REG_REQ_ID;
	this->fUser = make_string_ptr(aUser);
	try{
		this->fPass = opensslB64Checksum(aPass, true);
	}
	catch (EOpensslException& e){
		throw EMessageException("Error encrypting password: " + e.getMessage());
	}
	this->fPath = make_string_ptr(aPath);
}

TUserRegistrReqMessage::~TUserRegistrReqMessage(){
	if (this->fUser != nullptr)
		this->fUser.reset();

	if (this->fPass != nullptr)
		this->fPass.reset();

	if (this->fPath != nullptr)
		this->fPath.reset();

	this->fUser = nullptr;
	this->fPass = nullptr;
	this->fPath = nullptr;
}

string_ptr TUserRegistrReqMessage::encodeMessage(){
	this->fItems->push_back(make_string_ptr(getMessageName(this->fID)));
	this->fItems->push_back(move_string_ptr(this->fUser));
	this->fItems->push_back(move_string_ptr(this->fPass));
	this->fItems->push_back(move_string_ptr(this->fPath));

	return TBaseMessage::encodeMessage();
}

void TUserRegistrReqMessage::decodeMessage(){
   /*
	* item[0] -> msg name
	* item[1] -> username
	* item[2] -> password
	* item[3] -> root path
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

	//path
	if (*(this->fItems->at(3)) == EMPTY)
		throw EMessageException("The path field cannot be empty");
	this->fPath = move_string_ptr(this->fItems->at(3));
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

TUpdateStartReqMessage::TUpdateStartReqMessage(const string& aUser, const string& aPass){
	this->fID = UPDATE_START_REQ_ID;
	this->fUser = make_string_ptr(aUser);
	try{
		this->fPass = opensslB64Checksum(aPass, true);
	}
	catch (EOpensslException& e){
		throw EMessageException("Error encrypting password: " + e.getMessage());
	}
}

TUpdateStartReqMessage::~TUpdateStartReqMessage(){
	if (this->fUser != nullptr)
		this->fUser.reset();

	if (this->fPass != nullptr)
		this->fPass.reset();

	this->fUser = nullptr;
	this->fPass = nullptr;
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

TUpdateStartReplyMessage::TUpdateStartReplyMessage(const bool aResp, const string& aToken){
	this->fID = UPDATE_START_REPLY_ID;
	this->fResp = aResp;
	this->fToken = make_string_ptr(aToken);
}

TUpdateStartReplyMessage::~TUpdateStartReplyMessage(){
	if (this->fToken != nullptr)
		this->fToken.reset();
	this->fToken = nullptr;
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
	if (this->fResp){
		if (*(this->fItems->at(2)) == EMPTY)
			throw EMessageException("The token field cannot be empty");
	}
	this->fToken = move_string_ptr(this->fItems->at(2));
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

TAddNewFileMessage::TAddNewFileMessage(const string& aToken, const string& aFilePath){
	this->fID = ADD_NEW_FILE_ID;
	this->fToken = make_string_ptr(aToken);
	this->fFilePath = make_string_ptr(aFilePath);

	//read file, encode it and calculate checksum
	try{
		this->fFileContent = readFile(path(this->fFilePath->c_str()));
	}
	catch (EFilesystemException& e){
		throw EMessageException("TAddNewFileMessage: " + e.getMessage());
	}
	if (this->fFileContent == nullptr) this->fFileContent = new_string_ptr();

	try{
		this->fChecksum = opensslB64FileChecksum(*(this->fFileContent));
	}
	catch (EOpensslException e){
		throw EMessageException("TAddNewFileMessage: " + e.getMessage());
	}
	if (this->fChecksum == nullptr) this->fChecksum = new_string_ptr();

	path p(*(this->fFilePath));
	boost::system::error_code ec;
	this->fFileDate = last_write_time(p, ec);
	if (ec)
		this->fFileDate = time(nullptr);
}

TAddNewFileMessage::~TAddNewFileMessage(){
	if (this->fToken != nullptr)
		this->fToken.reset();
	
	if (this->fFilePath != nullptr)
		this->fFilePath.reset();
	
	if (this->fFileContent != nullptr)
		this->fFileContent.reset();
	
	if (this->fChecksum != nullptr)
		this->fChecksum.reset();

	this->fToken = nullptr;
	this->fFilePath = nullptr;
	this->fFileContent = nullptr;
	this->fChecksum = nullptr;
}

string_ptr TAddNewFileMessage::encodeMessage(){
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
	this->fToken = move_string_ptr(this->fItems->at(1));

	//file path
	if (*(this->fItems->at(2)) == EMPTY)
		throw EMessageException("The file path field cannot be empty");
	this->fFilePath = move_string_ptr(this->fItems->at(2));

	//file checksum
	//if (*(this->fItems->at(3)) == EMPTY)
	//	throw EMessageException("The file checksum field cannot be empty");
	this->fChecksum = move_string_ptr(this->fItems->at(3));

	//file date
	if (*(this->fItems->at(4)) == EMPTY)
		throw EMessageException("The file date field cannot be empty");
	this->fFileDate = stringToTime(*(this->fItems->at(4)));

	//file content
	//if (*(this->fItems->at(5)) == EMPTY)
	//	throw EMessageException("The file content field cannot be empty");
	this->fFileContent = move_string_ptr(this->fItems->at(5));
}

const bool TAddNewFileMessage::verifyChecksum(){
	bool checksumMatches = false;
	string_ptr myChecksum = nullptr;
	try{
		if (this->fFileContent != nullptr)
			myChecksum = opensslB64FileChecksum(*(this->fFileContent));
		
		if (myChecksum == nullptr)
			myChecksum = new_string_ptr();
		
		if (myChecksum != nullptr && this->fChecksum != nullptr)
			checksumMatches = (*(myChecksum) == *(this->fChecksum));
	}
	catch (EOpensslException e){
		throw EMessageException("verifyChecksum failed: " + e.getMessage());
	}
	myChecksum.reset();

	return checksumMatches;
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

TUpdateFileMessage::TUpdateFileMessage(const string& aToken, const string& aFilePath){
	this->fID = UPDATE_FILE_ID;
	this->fToken = make_string_ptr(aToken);
	this->fFilePath = make_string_ptr(aFilePath);
	
	//read file, encode it and calculate checksum
	try{
		this->fFileContent = readFile(path(this->fFilePath->c_str()));
	}
	catch (EFilesystemException& e){
		throw EMessageException("TUpdateFileMessage: " + e.getMessage());
	}
	if (this->fFileContent == nullptr) this->fFileContent = new_string_ptr();

	try{
		this->fChecksum = opensslB64FileChecksum(*(this->fFileContent));
	}
	catch (EOpensslException e){
		throw EMessageException("TUpdateFileMessage: " + e.getMessage());
	}
	if (this->fChecksum == nullptr) this->fChecksum = new_string_ptr();

	path p(*(this->fFilePath));
	boost::system::error_code ec;
	this->fFileDate = last_write_time(p, ec);
	if (ec)
		this->fFileDate = time(nullptr);
}

TUpdateFileMessage::~TUpdateFileMessage(){
	if (this->fToken != nullptr)
		this->fToken.reset();

	if (this->fFilePath != nullptr)
		this->fFilePath.reset();

	if (this->fFileContent != nullptr)
		this->fFileContent.reset();

	if (this->fChecksum != nullptr)
		this->fChecksum.reset();

	this->fToken = nullptr;
	this->fFilePath = nullptr;
	this->fFileContent = nullptr;
	this->fChecksum = nullptr;
}

string_ptr TUpdateFileMessage::encodeMessage(){
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
	this->fToken = move_string_ptr(this->fItems->at(1));

	//file path
	if (*(this->fItems->at(2)) == EMPTY)
		throw EMessageException("The file path field cannot be empty");
	this->fFilePath = move_string_ptr(this->fItems->at(2));

	//file checksum
	//if (*(this->fItems->at(3)) == EMPTY)
	//	throw EMessageException("The file checksum field cannot be empty");
	this->fChecksum = move_string_ptr(this->fItems->at(3));

	//file date
	if (*(this->fItems->at(4)) == EMPTY)
		throw EMessageException("The file date field cannot be empty");
	this->fFileDate = stringToTime(*(this->fItems->at(4)));

	//file content
	//if (*(this->fItems->at(5)) == EMPTY)
	//	throw EMessageException("The file content field cannot be empty");
	this->fFileContent = move_string_ptr(this->fItems->at(5));
}

const bool TUpdateFileMessage::verifyChecksum(){
	bool checksumMatches = false;
	string_ptr myChecksum = nullptr;
	try{
		if (this->fFileContent != nullptr)
			myChecksum = opensslB64FileChecksum(*(this->fFileContent));

		if (myChecksum == nullptr)
			myChecksum = new_string_ptr();

		if (myChecksum != nullptr && this->fChecksum != nullptr)
			checksumMatches = (*(myChecksum) == *(this->fChecksum));
	}
	catch (EOpensslException e){
		throw EMessageException("verifyChecksum failed: " + e.getMessage());
	}
	myChecksum.reset();

	return checksumMatches;
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

TRemoveFileMessage::TRemoveFileMessage(const string& aToken, const string& aFilePath){
	this->fID = REMOVE_FILE_ID;
	this->fToken = make_string_ptr(aToken);
	this->fFilePath = make_string_ptr(aFilePath);
}

TRemoveFileMessage::~TRemoveFileMessage(){
	if (this->fToken != nullptr)
		this->fToken.reset();

	if (this->fFilePath != nullptr)
		this->fFilePath.reset();

	this->fToken = nullptr;
	this->fFilePath = nullptr;
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
	this->fToken = move_string_ptr(this->fItems->at(1));

	//file path
	if (*(this->fItems->at(2)) == EMPTY)
		throw EMessageException("The file path field cannot be empty");
	this->fFilePath = move_string_ptr(this->fItems->at(2));
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

TFileAckMessage::TFileAckMessage(const bool aResp, const string& aFilePath){
	this->fID = FILE_ACK_ID;
	this->fResp = aResp;
	this->fFilePath = make_string_ptr(aFilePath);
}

TFileAckMessage::~TFileAckMessage(){
	if (this->fFilePath != nullptr)
		this->fFilePath.reset();

	this->fFilePath = nullptr;
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
		throw EMessageException("The file path field cannot be empty");
	this->fFilePath = move_string_ptr(this->fItems->at(2));
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

TUpdateStopReqMessage::TUpdateStopReqMessage(const string& aToken){
	this->fID = UPDATE_STOP_REQ_ID;
	this->fToken = make_string_ptr(aToken);
}

TUpdateStopReqMessage::~TUpdateStopReqMessage(){
	if (this->fToken != nullptr)
		this->fToken.reset();

	this->fToken = nullptr;
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
	this->fToken = move_string_ptr(this->fItems->at(1));
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

TUpdateStopReplyMessage::TUpdateStopReplyMessage(const bool aResp, const int aVersion, const time_t aTime){
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
	try{
		this->fVersion = stoi(*(this->fItems->at(2)));
	}
	catch (...){
		throw EMessageException("The version field cannot be converted into an int value");
	}

	//version timestamp
	if (*(this->fItems->at(3)) == EMPTY)
		throw EMessageException("The version timestamp field cannot be empty");
	this->fTime = stringToTime(*(this->fItems->at(3)));
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

TGetVersionsReqMessage::TGetVersionsReqMessage(const string& aUser, const string& aPass){
	this->fID = GET_VERSIONS_REQ_ID;
	this->fUser = make_string_ptr(aUser);
	try{
		this->fPass = opensslB64Checksum(aPass, true);
	}
	catch (EOpensslException& e){
		throw EMessageException("Error encrypting password: " + e.getMessage());
	}
}

TGetVersionsReqMessage::~TGetVersionsReqMessage(){
	if (this->fUser != nullptr)
		this->fUser.reset();

	if (this->fPass != nullptr)
		this->fPass.reset();

	this->fUser = nullptr;
	this->fPass = nullptr;
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

TGetVersionsReplyMessage::TGetVersionsReplyMessage(const unsigned int aTotVersions, const unsigned int aOldestVersion, const unsigned int aLastVersion, TVersionList_ptr& aVersions){
	this->fID = GET_VERSIONS_REPLY_ID;
	this->fTotVersions = aTotVersions;
	this->fOldestVersion = aOldestVersion;
	this->fLastVersion = aLastVersion;
	this->fVersions = move_TVersionList_ptr(aVersions);
}

TGetVersionsReplyMessage::~TGetVersionsReplyMessage(){
	if (this->fVersions != nullptr){
		//for (TVersionList::iterator it = this->fVersions->begin(); it != this->fVersions->end(); it++)
		//	it->reset();
		this->fVersions->clear();
		this->fVersions.reset();
		this->fVersions = nullptr;
	}
}

string_ptr TGetVersionsReplyMessage::encodeMessage(){
	this->fItems->push_back(make_string_ptr(getMessageName(this->fID)));
	this->fItems->push_back(make_string_ptr(to_string(this->fTotVersions)));
	this->fItems->push_back(make_string_ptr(to_string(this->fOldestVersion)));
	this->fItems->push_back(make_string_ptr(to_string(this->fLastVersion)));

	if (this->fVersions != nullptr){
		int size = (int)this->fVersions->size();
		for (int i = 0; i < size; i++){
			time_t t = this->fVersions->at(i)->getDate();
			this->fItems->push_back(make_string_ptr(timeToString(t)));
		}
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
	
	try{
		this->fTotVersions = stoi(*(this->fItems->at(1)));
	}
	catch (...){
		throw EMessageException("The total versions number field cannot be converted into an int value");
	}	

	//oldest version
	if (*(this->fItems->at(2)) == EMPTY)
		throw EMessageException("The oldest version number field cannot be empty");
	
	try{
		this->fOldestVersion = stoi(*(this->fItems->at(2)));
	}
	catch (...){
		throw EMessageException("The oldest version number field cannot be converted into an int value");
	}

	//last version
	if (*(this->fItems->at(3)) == EMPTY)
		throw EMessageException("The last version number field cannot be empty");

	try{
		this->fLastVersion = stoi(*(this->fItems->at(3)));
	}
	catch (...){
		throw EMessageException("The last version number field cannot be converted into an int value");
	}

	if (this->fTotVersions > 0){
		int totSize = GET_VERSIONS_REPLY_MIN_TOK_NUM + (this->fLastVersion - this->fOldestVersion) + 1;
		if (size != totSize)
			throw EMessageException("GET_VERSIONS_REPLY message contains wrong number of tokens(" + to_string(size) + " instead of " + to_string(totSize) + ")");

		int i, j;
		for (i = GET_VERSIONS_REPLY_MIN_TOK_NUM-1, j = this->fOldestVersion; i < totSize-1; i++, j++){
			if (this->fVersions == nullptr)
				this->fVersions = new_TVersionList_ptr();
			this->fVersions->push_back(new_TVersion_ptr(j, stringToTime(*(this->fItems->at(i)))));
		}
	}
}
#pragma endregion


////////////////////////////////////////
//       TGetLastVerReqMessage        //
////////////////////////////////////////
#pragma region "TGetLastVerReqMessage"
TGetLastVerReqMessage::TGetLastVerReqMessage(TBaseMessage_ptr& aBase){
	this->fID = aBase->getID();
	this->fItems = aBase->getTokens();
	this->fEncodedMsg = aBase->getMsg();

	this->decodeMessage();
}

TGetLastVerReqMessage::TGetLastVerReqMessage(const string& aUser, const string& aPass){
	this->fID = GET_LAST_VERSION_REQ_ID;
	this->fUser = make_string_ptr(aUser);
	try{
		this->fPass = opensslB64Checksum(aPass, true);
	}
	catch (EOpensslException& e){
		throw EMessageException("Error encrypting password: " + e.getMessage());
	}
}

TGetLastVerReqMessage::~TGetLastVerReqMessage(){
	if (this->fUser != nullptr)
		this->fUser.reset();

	if (this->fPass != nullptr)
		this->fPass.reset();

	this->fUser = nullptr;
	this->fPass = nullptr;
}

string_ptr TGetLastVerReqMessage::encodeMessage(){
	this->fItems->push_back(make_string_ptr(getMessageName(this->fID)));
	this->fItems->push_back(move_string_ptr(this->fUser));
	this->fItems->push_back(move_string_ptr(this->fPass));

	return TBaseMessage::encodeMessage();
}

void TGetLastVerReqMessage::decodeMessage(){
   /*
	* item[0] -> msg name
	* item[1] -> username
	* item[2] -> password
	*/
	TBaseMessage::decodeMessage();

	int size = (int)this->fItems->size();
	if (size != GET_LAST_VERSION_REQ_TOK_NUM)
		throw EMessageException("GET_LAST_VERSION_REQ message contains wrong number of tokens(" + to_string(size) + " instead of " + to_string(GET_LAST_VERSION_REQ_TOK_NUM) + ")");

	for (int i = 0; i < size; i++)
		if (this->fItems->at(i) == nullptr)
			throw EMessageException("item " + to_string(i) + " is nullptr");

	if (this->fID != GET_LAST_VERSION_REQ_ID)
		throw EMessageException("The given message is not a GET_LAST_VERSION_REQ message");

	//username
	if (*(this->fItems->at(1)) == EMPTY)
		throw EMessageException("The user name field cannot be empty");
	this->fUser = move_string_ptr(this->fItems->at(1));

	//password
	if (*(this->fItems->at(2)) == EMPTY)
		throw EMessageException("The password field cannot be empty");
	this->fPass = move_string_ptr(this->fItems->at(2));
}
#pragma endregion


////////////////////////////////////////
//      TGetLastVerReplyMessage       //
////////////////////////////////////////
#pragma region "TGetLastVerReplyMessage"
TGetLastVerReplyMessage::TGetLastVerReplyMessage(TBaseMessage_ptr& aBase){
	this->fID = aBase->getID();
	this->fItems = aBase->getTokens();
	this->fEncodedMsg = aBase->getMsg();

	this->decodeMessage();
}

TGetLastVerReplyMessage::TGetLastVerReplyMessage(int aVersion, time_t aVersionDate){
	this->fID = GET_LAST_VERSION_REPLY_ID;
	this->fVersion = aVersion;
	this->fVersionDate = aVersionDate;
}

string_ptr TGetLastVerReplyMessage::encodeMessage(){
	this->fItems->push_back(make_string_ptr(getMessageName(this->fID)));
	this->fItems->push_back(make_string_ptr(to_string(this->fVersion)));
	this->fItems->push_back(make_string_ptr(timeToString(this->fVersionDate)));

	return TBaseMessage::encodeMessage();
}

void TGetLastVerReplyMessage::decodeMessage(){
   /*
	* item[0] -> msg name
	* item[1] -> version number
	* item[2] -> version timestamp
	*/
	TBaseMessage::decodeMessage();

	int size = (int)this->fItems->size();
	if (size != GET_LAST_VERSION_REPLY_TOK_NUM)
		throw EMessageException("GET_LAST_VERSION_REPLY message contains wrong number of tokens(" + to_string(size) + " instead of " + to_string(GET_LAST_VERSION_REPLY_TOK_NUM) + ")");

	for (int i = 0; i < size; i++)
		if (this->fItems->at(i) == nullptr)
			throw EMessageException("item " + to_string(i) + " is nullptr");

	if (this->fID != GET_LAST_VERSION_REPLY_ID)
		throw EMessageException("The given message is not a GET_LAST_VERSION_REPLY message");

	//version number
	if (*(this->fItems->at(1)) == EMPTY)
		throw EMessageException("The version field cannot be empty");

	try{
		this->fVersion = stoi(*(this->fItems->at(1)));
	}
	catch (...){
		throw EMessageException("The version number field cannot be converted into an int value");
	}

	//version timestamp
	if (*(this->fItems->at(2)) == EMPTY)
		throw EMessageException("The version timestamp field cannot be empty");
	this->fVersionDate = stringToTime(*(this->fItems->at(2)));
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

TRestoreVerReqMessage::TRestoreVerReqMessage(const string& aUser, const string& aPass, const unsigned int aVersion){
	this->fID = RESTORE_VER_REQ_ID;
	this->fUser = make_string_ptr(aUser);
	try{
		this->fPass = opensslB64Checksum(aPass, true);
	}
	catch (EOpensslException& e){
		throw EMessageException("Error encrypting password: " + e.getMessage());
	}
	this->fVersion = aVersion;
}

TRestoreVerReqMessage::~TRestoreVerReqMessage(){
	if (this->fUser != nullptr)
		this->fUser.reset();

	if (this->fPass != nullptr)
		this->fPass.reset();

	this->fUser = nullptr;
	this->fPass = nullptr;
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

	try{
		this->fVersion = stoi(*(this->fItems->at(3)));
	}
	catch (...){
		throw EMessageException("The version number field cannot be converted into an int value");
	}
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

TRestoreVerReplyMessage::TRestoreVerReplyMessage(const bool aResp, const string& aToken){
	this->fID = RESTORE_VER_REPLY_ID;
	this->fResp = aResp;
	this->fToken = make_string_ptr(aToken);
}

TRestoreVerReplyMessage::~TRestoreVerReplyMessage(){
	if (this->fToken != nullptr)
		this->fToken.reset();

	this->fToken = nullptr;
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
	this->fToken = move_string_ptr(this->fItems->at(2)); 
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

TRestoreFileMessage::TRestoreFileMessage(const string& aBasePath, const string& aFilePath, const time_t aFileDate){
	this->fID = RESTORE_FILE_ID;
	this->fFilePath = make_string_ptr(aFilePath);

	//read file, encode it and calculate checksum
	try{
		this->fFileContent = readFile(path(aBasePath + "//" + aFilePath));
	}
	catch (EFilesystemException& e){
		throw EMessageException("TRestoreFileMessage: " + e.getMessage());
	}
	if (this->fFileContent == nullptr) this->fFileContent = new_string_ptr();

	try{
		this->fChecksum = opensslB64FileChecksum(*(this->fFileContent));
	}
	catch (EOpensslException e){
		throw EMessageException("TRestoreFileMessage: " + e.getMessage());
	}
	if (this->fChecksum == nullptr) this->fChecksum = new_string_ptr();

	this->fFileDate = aFileDate;
}

TRestoreFileMessage::~TRestoreFileMessage(){
	if (this->fFilePath != nullptr)
		this->fFilePath.reset();

	if (this->fFileContent != nullptr)
		this->fFileContent.reset();

	if (this->fChecksum != nullptr)
		this->fChecksum.reset();

	this->fFilePath = nullptr;
	this->fFileContent = nullptr;
	this->fChecksum = nullptr;
}

string_ptr TRestoreFileMessage::encodeMessage(){
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
		throw EMessageException("The file path field cannot be empty");
	this->fFilePath = move_string_ptr(this->fItems->at(1));

	//file checksum
	//if (*(this->fItems->at(2)) == EMPTY)
	//	throw EMessageException("The file checksum field cannot be empty");
	this->fChecksum = move_string_ptr(this->fItems->at(2));

	//file date
	if (*(this->fItems->at(3)) == EMPTY)
		throw EMessageException("The file date field cannot be empty");
	this->fFileDate = stringToTime(*(this->fItems->at(3)));

	//file content
	//if (*(this->fItems->at(4)) == EMPTY)
	//	throw EMessageException("The file content field cannot be empty");
	this->fFileContent = move_string_ptr(this->fItems->at(4));
}

const bool TRestoreFileMessage::verifyChecksum(){
	bool checksumMatches = false;
	string_ptr myChecksum = nullptr;
	try{
		if (this->fFileContent != nullptr)
			myChecksum = opensslB64FileChecksum(*(this->fFileContent));

		if (myChecksum == nullptr)
			myChecksum = new_string_ptr();

		if (myChecksum != nullptr && this->fChecksum != nullptr)
			checksumMatches = (*(myChecksum) == *(this->fChecksum));
	}
	catch (EOpensslException e){
		throw EMessageException("verifyChecksum failed: " + e.getMessage());
	}
	myChecksum.reset();

	return checksumMatches;
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

TRestoreFileAckMessage::TRestoreFileAckMessage(const bool aResp, const string& aToken, const string& aFilePath){
	this->fID = RESTORE_FILE_ACK_ID;
	this->fToken = make_string_ptr(aToken);
	this->fFilePath = make_string_ptr(aFilePath);
	this->fResp = aResp;
}

TRestoreFileAckMessage::~TRestoreFileAckMessage(){
	if (this->fToken != nullptr)
		this->fToken.reset();

	if (this->fFilePath != nullptr)
		this->fFilePath.reset();

	this->fToken = nullptr;
	this->fFilePath = nullptr;
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
	this->fToken = move_string_ptr(this->fItems->at(1));

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
		throw EMessageException("The file path field cannot be empty");
	this->fFilePath = move_string_ptr(this->fItems->at(3));
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

TRestoreStopMessage::TRestoreStopMessage(int aVersion, time_t aVersionDate){
	this->fID = RESTORE_STOP_ID;
	this->fVersion = aVersion;
	this->fVersionDate = aVersionDate;
}

string_ptr TRestoreStopMessage::encodeMessage(){
	this->fItems->push_back(make_string_ptr(getMessageName(this->fID)));
	this->fItems->push_back(make_string_ptr(to_string(this->fVersion)));
	this->fItems->push_back(make_string_ptr(timeToString(this->fVersionDate)));

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

	try{
		this->fVersion = stoi(*(this->fItems->at(1)));
	}
	catch (...){
		throw EMessageException("The version number field cannot be converted into an int value");
	}

	//version timestamp
	if (*(this->fItems->at(2)) == EMPTY)
		throw EMessageException("The version timestamp field cannot be empty");
	this->fVersionDate = stringToTime(*(this->fItems->at(2)));
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

TPingReqMessage::TPingReqMessage(const string& aToken){
	this->fID = PING_REQ_ID;
	this->fTime = time(nullptr);
	this->fToken = make_string_ptr(aToken);
}

string_ptr TPingReqMessage::encodeMessage(){
	this->fItems->push_back(make_string_ptr(getMessageName(this->fID)));
	this->fItems->push_back(make_string_ptr(timeToString(this->fTime)));
	this->fItems->push_back(move_string_ptr(this->fToken));

	return TBaseMessage::encodeMessage();
}

void TPingReqMessage::decodeMessage(){
   /*
	* item[0] -> msg name
	* item[1] -> timestamp
	* item[2] -> user token
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

	//user token
	if (*(this->fItems->at(2)) == EMPTY)
		throw EMessageException("The token field cannot be empty");
	this->fToken = move_string_ptr(this->fItems->at(2)); 

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

TPingReplyMessage::TPingReplyMessage(const string& aToken){
	this->fID = PING_REPLY_ID;
	this->fTime = time(nullptr);
	this->fToken = make_string_ptr(aToken);
}

string_ptr TPingReplyMessage::encodeMessage(){
	this->fItems->push_back(make_string_ptr(getMessageName(this->fID)));
	this->fItems->push_back(make_string_ptr(timeToString(this->fTime)));
	this->fItems->push_back(move_string_ptr(this->fToken));

	return TBaseMessage::encodeMessage();
}

void TPingReplyMessage::decodeMessage(){
	/*
	* item[0] -> msg name
	* item[1] -> timestamp
	* item[2] -> user token
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

	//user token
	if (*(this->fItems->at(2)) == EMPTY)
		throw EMessageException("The token field cannot be empty");
	this->fToken = move_string_ptr(this->fItems->at(2));
}
#pragma endregion


////////////////////////////////////////
//       TVerifyCredReqMessage        //
////////////////////////////////////////
#pragma region "TVerifyCredReqMessage"
TVerifyCredReqMessage::TVerifyCredReqMessage(TBaseMessage_ptr& aBase){
	this->fID = aBase->getID();
	this->fItems = aBase->getTokens();
	this->fEncodedMsg = aBase->getMsg();

	this->decodeMessage();
}

TVerifyCredReqMessage::TVerifyCredReqMessage(const string& aUser, const string& aPass){
	this->fID = VERIFY_CRED_REQ_ID;
	this->fUser = make_string_ptr(aUser);
	try{
		this->fPass = opensslB64Checksum(aPass, true);
	}
	catch (EOpensslException& e){
		throw EMessageException("Error encrypting password: " + e.getMessage());
	}
}

TVerifyCredReqMessage::~TVerifyCredReqMessage(){
	if (this->fUser != nullptr)
		this->fUser.reset();

	if (this->fPass != nullptr)
		this->fPass.reset();

	this->fUser = nullptr;
	this->fPass = nullptr;
}

string_ptr TVerifyCredReqMessage::encodeMessage(){
	this->fItems->push_back(make_string_ptr(getMessageName(this->fID)));
	this->fItems->push_back(move_string_ptr(this->fUser));
	this->fItems->push_back(move_string_ptr(this->fPass));

	return TBaseMessage::encodeMessage();
}

void TVerifyCredReqMessage::decodeMessage(){
	/*
	* item[0] -> msg name
	* item[1] -> username
	* item[2] -> password
	*/
	TBaseMessage::decodeMessage();

	int size = (int)this->fItems->size();
	if (size != VERIFY_CRED_REQ_TOK_NUM)
		throw EMessageException("VERIFY_CRED_REQ message contains wrong number of tokens(" + to_string(size) + " instead of " + to_string(VERIFY_CRED_REQ_TOK_NUM) + ")");

	for (int i = 0; i < size; i++)
		if (this->fItems->at(i) == nullptr)
			throw EMessageException("item " + to_string(i) + " is nullptr");

	if (this->fID != VERIFY_CRED_REQ_ID)
		throw EMessageException("The given message is not a USER_REG_REQ message");

	//username
	if (*(this->fItems->at(1)) == EMPTY)
		throw EMessageException("The user name field cannot be empty");
	this->fUser = move_string_ptr(this->fItems->at(1));

	//password
	if (*(this->fItems->at(2)) == EMPTY)
		throw EMessageException("The password field cannot be empty");
	this->fPass = move_string_ptr(this->fItems->at(2));
}
#pragma endregion


///////////////////////////////////////
//      TVerifyCredReplyMessage      //
///////////////////////////////////////
#pragma region "TVerifyCredReplyMessage"
TVerifyCredReplyMessage::TVerifyCredReplyMessage(TBaseMessage_ptr& aBase){
	this->fID = aBase->getID();
	this->fItems = aBase->getTokens();
	this->fEncodedMsg = aBase->getMsg();

	this->decodeMessage();
}

TVerifyCredReplyMessage::TVerifyCredReplyMessage(const bool aResp, const string& aPath){
	this->fID = VERIFY_CRED_REPLY_ID;
	this->fResp = aResp;
	this->fPath = make_string_ptr(aPath);
}

string_ptr TVerifyCredReplyMessage::encodeMessage(){
	string resp = "";
	if (this->fResp)
		resp = TRUE_STR;
	else
		resp = FALSE_STR;

	this->fItems->push_back(make_string_ptr(getMessageName(this->fID)));
	this->fItems->push_back(make_string_ptr(resp));
	this->fItems->push_back(move_string_ptr(this->fPath));

	return TBaseMessage::encodeMessage();
}

void TVerifyCredReplyMessage::decodeMessage(){
	/*
	* item[0] -> msg name
	* item[1] -> response
	* item[2] -> root path
	*/
	TBaseMessage::decodeMessage();

	int size = (int)this->fItems->size();
	if (size != VERIFY_CRED_REPLY_TOK_NUM)
		throw EMessageException("VERIFY_CRED_REPLY message contains wrong number of tokens(" + to_string(size) + " instead of " + to_string(VERIFY_CRED_REPLY_TOK_NUM) + ")");

	for (int i = 0; i < size; i++)
		if (this->fItems->at(i) == nullptr)
			throw EMessageException("item " + to_string(i) + " is nullptr");

	if (this->fID != VERIFY_CRED_REPLY_ID)
		throw EMessageException("The given message is not a VERIFY_CRED_REPLY message");

	//response
	if (*(this->fItems->at(1)) == EMPTY)
		throw EMessageException("The response field cannot be empty");
	if (*(this->fItems->at(1)) != TRUE_STR && *(this->fItems->at(1)) != FALSE_STR)
		throw EMessageException("The response field could be only true or false");

	if (*(this->fItems->at(1)) == TRUE_STR)
		this->fResp = true;
	else
		this->fResp = false;

	//path
	if (this->fResp == true){
		if (*(this->fItems->at(2)) == EMPTY)
			throw EMessageException("The path field cannot be empty");
	}
	this->fPath = move_string_ptr(this->fItems->at(2));
}
#pragma endregion


//////////////////////////////////////
//       TSystemErrorMessage        //
//////////////////////////////////////
#pragma region "TSystemErrorMessage"
TSystemErrorMessage::TSystemErrorMessage(TBaseMessage_ptr& aBase){
	this->fID = aBase->getID();
	this->fItems = aBase->getTokens();
	this->fEncodedMsg = aBase->getMsg();

	this->decodeMessage();
}

TSystemErrorMessage::TSystemErrorMessage(const string& aDetail){
	this->fID = SYSTEM_ERR_ID;
	this->fDetail = make_string_ptr(aDetail);
}

string_ptr TSystemErrorMessage::encodeMessage(){
	this->fItems->push_back(make_string_ptr(getMessageName(this->fID)));
	this->fItems->push_back(move_string_ptr(this->fDetail));

	return TBaseMessage::encodeMessage();
}

void TSystemErrorMessage::decodeMessage(){
	/*
	* item[0] -> msg name
	* item[1] -> error detail
	*/
	TBaseMessage::decodeMessage();

	int size = (int)this->fItems->size();
	if (size != SYSTEM_ERR_TOK_NUM)
		throw EMessageException("SYSTEM_ERROR message contains wrong number of tokens(" + to_string(size) + " instead of " + to_string(SYSTEM_ERR_TOK_NUM) + ")");

	for (int i = 0; i < size; i++)
		if (this->fItems->at(i) == nullptr)
			throw EMessageException("item " + to_string(i) + " is nullptr");

	if (this->fID != SYSTEM_ERR_ID)
		throw EMessageException("The given message is not a SYSTEM_ERROR message");

	//detail
	if (*(this->fItems->at(1)) == EMPTY)
		throw EMessageException("The detail field cannot be empty");
	this->fDetail = move_string_ptr(this->fItems->at(1));
}
#pragma endregion