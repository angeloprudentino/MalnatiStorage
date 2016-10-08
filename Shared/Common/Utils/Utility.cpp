/*
 * Author: Angelo Prudentino
 * Date: 30/09/2015
 * File: Utility.cpp
 * Description: utility functions for both client and server
 *
 */
#pragma once

#include <iostream>
#include <sstream>
#include <stdio.h>
#include <time.h>
#include <fstream>
#include <boost/algorithm/string/replace.hpp>
#include <boost/filesystem/fstream.hpp>
#include "openssl/rand.h"
#include "openssl/evp.h"
#include "openssl/sha.h"
#include "openssl/buffer.h"
#include "openssl/bio.h" // BIO objects for I/O
#include "openssl/ssl.h" // SSL and SSL_CTX for SSL connections
#include "openssl/err.h" // Error reporting
#include "Utility.h"

#define RAND_LEN 20
#define SEP '$'
#define SEP_ESC "&#36"
#ifdef _DEBUG
#define LOG_PATH "Log"
#define STORAGE_ROOT_PATH "StoragePoint"
#else
#define LOG_PATH "C:\\StorageServer\\Log"
#define STORAGE_ROOT_PATH "C:\\StorageServer\\StoragePoint"
#endif

using namespace System;
using namespace std;
using namespace boost::filesystem;

string_ptr opensslB64Encode(char* aContent, int aLen); //throws EOpensslException
B64result opensslB64Decode(const string& aString); //throws EOpensslException

#pragma region "DateTime Utilities"
// Get current date/time, format is [YYYY-MM-DD HH:mm:ss]
const string currentDateTime() {
	time_t now = time(0);
	struct tm tstruct;
	char buf[23];
	tstruct = *localtime(&now);

	strftime(buf, sizeof(buf), "[%Y-%m-%d %X]", &tstruct);
	return buf;
}

//Convert time_t to std::string and vice versa
const string timeToString(const time_t& t) {
	try{
		std::ostringstream oss;
		if (oss){
			oss << t;
			return oss.str();
		}
		else
			return "";
	}
	catch (...) {
		return "";
	}
}

const time_t stringToTime(const string& s) {
	time_t t;
	try{
		istringstream stream(s);
		if (stream){
			stream >> t;
		}
	}
	catch (...){}

	return t;
}

const string formatFileDate(const time_t& t){
	try{
		struct tm tstruct;
		char buf[25];
		tstruct = *localtime(&t);

		strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);
		return buf;
	}
	catch (...) {
		return "";
	}
}

const string formatLogFileDate(const time_t& t){
	try{
		struct tm tstruct;
		char buf[9];
		tstruct = *localtime(&t);

		strftime(buf, sizeof(buf), "%Y%m%d", &tstruct);
		return buf;
	}
	catch (...) {
		return "";
	}
}
#pragma endregion

#pragma region "String conversion"
const string marshalString(String^ aStr) {
	using namespace Runtime::InteropServices;
	const char* chars = (const char*)(Marshal::StringToHGlobalAnsi(aStr)).ToPointer();
	string res = chars;
	Marshal::FreeHGlobal(IntPtr((void*)chars));

	return res;
}

String^ unmarshalString(const string& aStr){
	return gcnew String(aStr.c_str());
}
#pragma endregion

string escape(const string& aUser){
	string res = aUser;
	if (!aUser.empty()){
		string from = to_string(SEP);
		string to = SEP_ESC;
		boost::replace_all(res, from, to);
	}

	return res;
}

string unescape(const string& aUser){
	string res = aUser;
	if (!aUser.empty()){
		string from = SEP_ESC;
		string to = to_string(SEP);
		boost::replace_all(res, from, to);
	}

	return res;
}

string_ptr getUniqueToken(const string& aUser){
	string_ptr rand = opensslB64RandomToken();
	if (rand != nullptr){
		string t = timeToString(time(NULL));
		string token(*rand + SEP + escape(aUser) + SEP + t + SEP + *rand);
		return opensslB64Encode((char*)token.c_str(), (int)token.size());
	}
	else
		return nullptr;
}

const string getUserFromToken(const string& aToken){
	B64result ret = opensslB64Decode(aToken);

	if (ret.data != nullptr && ret.size > 0){
		// Turn into a stream.
		stringstream ss(ret.data);
		string tok;

		getline(ss, tok, SEP); //skip first rand
		getline(ss, tok, SEP);

		return unescape(tok);
	}
	else
		return "";
}

#pragma region "Crypto Utilities"
void initCrypto(){
	CRYPTO_malloc_init();           // Initialize malloc, free, etc for OpenSSL's use
	SSL_library_init();             // Initialize OpenSSL's SSL libraries
	SSL_load_error_strings();       // Load SSL error strings
	ERR_load_BIO_strings();         // Load BIO error strings
	OpenSSL_add_all_algorithms();   // Load all available encryption algorithms
}

BIO* fillOpensslBIO(BIO* aBio, char* aContent, int aLen){
	bool try_again = false;
	int ret;

	if (aBio == nullptr)
		throw EOpensslException("error in fillOpensslBIO(): aBio is NULL!");

	if (aContent == nullptr)
		throw EOpensslException("error in fillOpensslBIO(): aContent is NULL!");

	if (aLen == 0)
		throw EOpensslException("error in fillOpensslBIO(): aContent is empty!");

	try_again = false;
	do {
		ret = BIO_write(aBio, (void*)aContent, aLen);
		if (ret == -2) {
			//not implemented for that bio
			string err = "error in fillOpensslBIO(): BIO_write returns -2 (not implemented for that bio): ";
			throw EOpensslException(err + ERR_error_string(ERR_get_error(), nullptr));
		}
		else if (ret == 0 || ret == -1){
			//maybe have something more to write
			if (BIO_should_retry(aBio)) {
				//should retry write
				try_again = true;
			}
			else{
				string err = "error in fillOpensslBIO(): BIO_should_retry signals an error: ";
				throw EOpensslException(err + ERR_error_string(ERR_get_error(), nullptr));
			}
		}
		else
			try_again = false;
	} while (try_again);

	if (BIO_flush(aBio) != 1){
		string err = "error in fillOpensslBIO(): BIO_flush returns -1: ";
		throw EOpensslException(err + ERR_error_string(ERR_get_error(), nullptr));
	}

	return aBio;
}

string_ptr opensslB64Encode(char* aContent, int aLen){
	BIO *bio = nullptr;
	BIO *b64 = nullptr;
	char* ret_data = nullptr;
	BUF_MEM *bufferPtr = nullptr;

	if (aContent == nullptr) 
		throw EOpensslException("error in opensslB64Encode(): aContent is NULL!");
	
	if (aLen == 0)
		throw EOpensslException("error in opensslB64Encode(): aContent is empty!");

	try {
		/* setup input BIO chain */
		bio = BIO_new(BIO_s_mem());
		if (bio == nullptr){
			string err = "error in opensslB64Encode(): BIO_new(BIO_s_mem()) returns nullptr: ";
			throw EOpensslException(err + ERR_error_string(ERR_get_error(), nullptr));
		}

		b64 = BIO_new(BIO_f_base64());
		if (b64 == nullptr){
			string err = "error in opensslB64Encode(): BIO_new(BIO_f_base64()) returns nullptr: ";
			throw EOpensslException(err + ERR_error_string(ERR_get_error(), nullptr));
		}

		bio = BIO_push(b64, bio);
		BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL); //Ignore newlines - write everything in one line

		bio = fillOpensslBIO(bio, aContent, aLen);

		BIO_get_mem_ptr(bio, &bufferPtr);
		BIO_set_close(bio, BIO_NOCLOSE);

		BIO_free_all(bio);
		
		if (bufferPtr != nullptr){
			ret_data = (*bufferPtr).data;
			if (ret_data != nullptr)
				ret_data[(*bufferPtr).length] = '\0';
			return make_string_ptr(ret_data);
		}
		else
			return nullptr;
	}
	catch (const EOpensslException& e) {
		if (bio != nullptr)
			BIO_free_all(bio);
		throw e;
	}
}

B64result opensslB64Decode(const string& aString){
	BIO *bio_in = nullptr;
	BIO *bio_out = nullptr;
	BIO *b64 = nullptr;
	int ret;
	char buffer[1024];
	BUF_MEM *bufferPtr = nullptr;
	B64result res;

	if (aString.empty()) {
		res.data = (char*)aString.c_str();
		res.size = (int)aString.length();
		return res;
	}

	try {
		/* setup input BIO chain */
		bio_in = BIO_new(BIO_s_mem());
		if (bio_in == nullptr){
			string err = "error in opensslB64Decode(): BIO_new(BIO_s_mem()) returns nullptr: ";
			throw EOpensslException(err + ERR_error_string(ERR_get_error(), nullptr));
		}

		bio_in = fillOpensslBIO(bio_in, (char*)aString.c_str(), (int)aString.length());
		BIO_set_close(bio_in, BIO_NOCLOSE);

		b64 = BIO_new(BIO_f_base64());
		if (b64 == nullptr){
			string err = "error in opensslB64Decode(): BIO_new(BIO_f_base64()) returns nullptr: ";
			throw EOpensslException(err + ERR_error_string(ERR_get_error(), nullptr));
		}

		bio_in = BIO_push(b64, bio_in);
		BIO_set_flags(bio_in, BIO_FLAGS_BASE64_NO_NL); //Ignore newlines - write everything in one line

		bio_out = BIO_new(BIO_s_mem());
		if (bio_out == nullptr) {
			string err = "error in opensslB64Decode(): BIO_new(BIO_s_mem()) returns nullptr: ";
			throw EOpensslException(err + ERR_error_string(ERR_get_error(), nullptr));
		}

		while ((ret = BIO_read(b64, (void*)buffer, sizeof buffer)) > 0) {
			BIO_write(bio_out, buffer, ret);
		}
		if (BIO_flush(bio_out) != 1){
			string err = "error in opensslB64Decode(): BIO_flush(bio_out) returns -1: ";
			throw EOpensslException(err + ERR_error_string(ERR_get_error(), nullptr));
		}

		BIO_get_mem_ptr(bio_out, &bufferPtr);
		BIO_set_close(bio_out, BIO_NOCLOSE);

		BIO_free_all(bio_in);
		BIO_free_all(bio_out);

		if (bufferPtr != nullptr){
			res.data = (*bufferPtr).data;
			if (res.data != nullptr)
				res.data[(*bufferPtr).length] = '\0';
			res.size = (int)((*bufferPtr).length);
		}
		
		return res;
	}
	catch (const EOpensslException& e) {
		if (bio_in != nullptr)
			BIO_free_all(bio_in);
		
		if (bio_out != nullptr)
			BIO_free_all(bio_out);
		
		throw e;
	}
}

string_ptr opensslCoreChecksum(BIO* aInputBio, const bool aStrongAlg){
	BIO *md;
	const EVP_MD* alg;
	bool try_again = false;
	char buf[1024];
	char mdbuf[EVP_MAX_MD_SIZE];
	int mdlen;
	
	if (aInputBio == nullptr) {
		throw EOpensslException("error in opensslCoreChecksum(): aInputBio is NULL!");
	}

	md = BIO_new(BIO_f_md());
	if (md == nullptr){
		string err = "error in opensslCoreChecksum(): BIO_new(BIO_f_md()) returns nullptr: ";
		throw EOpensslException(err + ERR_error_string(ERR_get_error(), nullptr));
	}

	if (aStrongAlg)
		alg = EVP_sha256();
	else
		alg = EVP_md5();

	if (BIO_set_md(md, alg) == 0){
		string err = "error in opensslCoreChecksum(): BIO_set_md() returns an error: ";
		throw EOpensslException(err + ERR_error_string(ERR_get_error(), nullptr));
	}

	aInputBio = BIO_push(md, aInputBio);

	/* reading through the MD BIO calculates the digest */
	while (BIO_read(aInputBio, buf, sizeof buf) > 0);

	try_again = false;
	do {
		/* retrieve the message digest */
		mdlen = BIO_gets(md, mdbuf, sizeof mdbuf);
		if (mdlen == -2) {
			//not implemented for that bio
			string err = "error in opensslCoreChecksum(): BIO_gets returns -2 (not implemented for that bio): ";
			throw EOpensslException(err + ERR_error_string(ERR_get_error(), nullptr));
		}
		else if (mdlen == 0 || mdlen == -1){
			//maybe have something more to read
			if (BIO_should_retry(md)) {
				//should retry read
				try_again = true;
			}
			else{
				string err = "error in opensslCoreChecksum(): BIO_should_retry signals an error: ";
				throw EOpensslException(err + ERR_error_string(ERR_get_error(), nullptr));
			}
		}
		else
			try_again = false;
	} while (try_again);

	return opensslB64Encode(mdbuf, mdlen);
}

string_ptr opensslB64Checksum(const string& aString, const bool aStrongAlg){
	BIO *bio_in = nullptr;
	bool try_again = false;

	if (aString.empty()) {
		return nullptr;
	}

	try {
		/* setup input BIO chain */
		bio_in = BIO_new(BIO_s_mem());
		if (bio_in == nullptr){
			string err = "error in opensslB64Checksum(): BIO_new(BIO_s_mem()) returns nullptr: ";
			throw EOpensslException(err + ERR_error_string(ERR_get_error(), nullptr));
		}

		bio_in = fillOpensslBIO(bio_in, (char*)aString.c_str(), (int)aString.length());
		BIO_set_close(bio_in, BIO_NOCLOSE);

		string_ptr result = opensslCoreChecksum(bio_in, aStrongAlg);
		BIO_free_all(bio_in);
		return result;
	}
	catch (const EOpensslException& e) {
		if (bio_in != nullptr)
			BIO_free_all(bio_in);
		throw e;
	}
}

string_ptr opensslB64PathChecksum(const string& aString){
	if (aString.empty()) {
		return nullptr;
	}

	string_ptr res = opensslB64Checksum(aString, true);

	const string from = "/";
	const string to = "_";
	boost::replace_all(*res, from, to);

	return move_string_ptr(res);
}

string_ptr opensslB64RandomToken(){
	char* buff = new char[RAND_LEN];
	int res = RAND_bytes((unsigned char*)buff, 20);

	if (res == 0){
		delete[] buff;
		string err = ERR_error_string(ERR_get_error(), NULL);
		throw EOpensslException("Error in RAND_bytes(): " + err);
	}

	string_ptr result = opensslB64Encode(buff, 20);
	delete[] buff;
	return move_string_ptr(result);
}
#pragma endregion


#pragma region "Log"
void logToFile(const string& aClassName, const string& aFuncName, const string& aMsg){
	path p = LOG_PATH;
	boost::system::error_code ec;
	if (!exists(p))
		create_directory(p, ec);

	if (!ec){
		string log = formatLogFileDate(time(NULL)) + "_log.txt";
		p /= log;
		boost::filesystem::ofstream of(p, ios::app);
		std::string toLog = currentDateTime();
		toLog.append(" ").append(aClassName).append("::").append(aFuncName).append(": ").append(aMsg).append("\n");
		of.write(toLog.c_str(), toLog.size());
		of.close();
	}
}

void warningToFile(const string& aClassName, const string& aFuncName, const string& aMsg){
	logToFile(aClassName, aFuncName, "wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww");
	logToFile(aClassName, aFuncName, "w  " + aMsg);
	logToFile(aClassName, aFuncName, "wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww");
}

void errorToFile(const string& aClassName, const string& aFuncName, const string& aMsg){
	logToFile(aClassName, aFuncName, "************************************************");
	logToFile(aClassName, aFuncName, "************************************************");
	logToFile(aClassName, aFuncName, "** ");
	logToFile(aClassName, aFuncName, "**  " + aMsg);
	logToFile(aClassName, aFuncName, "** ");
	logToFile(aClassName, aFuncName, "************************************************");
	logToFile(aClassName, aFuncName, "************************************************");
}

void criticalErrorToFile(const string& aClassName, const string& aFuncName, const string& aMsg){
	errorToFile(aClassName, aFuncName, aMsg);
}
#pragma endregion


#pragma region "Filesystem utilities"
void storeFile(const path& aPath, string_ptr& aFileContent){
	path p = aPath.parent_path();
	boost::system::error_code ec;
	if (!exists(p))
		create_directories(p, ec);

	if (!ec){
		boost::filesystem::ofstream of(aPath, ios::out | ios::binary);
		try{
#ifdef _DEBUG
			B64result ret = opensslB64Decode(*aFileContent);
			aFileContent.reset();
			if (ret.data != nullptr && ret.size > 0)
				of.write(ret.data, ret.size);
			of.close();
		}
		catch (EOpensslException e){
			aFileContent.reset();
			of.close();
			throw EFilesystemException("Error decoding file: " + aPath.string() + " -> " + e.getMessage());
		}
#else
			if(!aFileContent->empty())
				of.write(aFileContent->c_str(), aFileContent->size());
			aFileContent.reset();
			of.close();
		}
		catch (...){
			aFileContent.reset();
			of.close();
			throw EFilesystemException("Error saving file: " + aPath.string());
		}
#endif
	}
	else{
		aFileContent.reset();
		throw EFilesystemException("Error creating directories: " + p.string() + " -> " + ec.message());
	}
}

string_ptr readFile(const path& aPath){
	string* contents = new string();
	boost::filesystem::ifstream file(aPath, ios::in | ios::binary);
	if (file){
		file.seekg(0, ios::end);
		contents->resize(file.tellg());
		file.seekg(0, ios::beg);

		file.read((char*)contents->c_str(), contents->size());
		if (!file)
			throw EFilesystemException("an error is occurred reading from file " + aPath.string());

		file.close();
		if (!file)
			throw EFilesystemException("an error is occurred closing file " + aPath.string());
	}

	string_ptr res = nullptr;
#ifdef _DEBUG
	if (contents != nullptr){
		try{
			res = opensslB64Encode((char*)contents->c_str(), (int)contents->length());
			delete contents;
		}
		catch (EOpensslException e){
			delete contents;
			throw EFilesystemException("Error encoding " + aPath.string() + ": " + e.getMessage());
		}
	}
#else
	if (contents != nullptr){
		res = make_string_ptr(contents->c_str());
		delete contents;
	}
#endif
	return move_string_ptr(res);
}

void removeDir(const path& aPath){
	boost::system::error_code ec;

	if (exists(aPath)){
		remove_all(aPath, ec);

		if (ec)
			throw EFilesystemException("Error removing path: " + aPath.string() + " -> " + ec.message());
	}
}

string buildServerPathPrefix(const string& aUser, const int aVersion){
	if (aUser.empty())
		throw EFilesystemException("error in buildServerPathPrefix(): aUser is empty!");

	if(aVersion <= 0)
		throw EFilesystemException("error in buildServerPathPrefix(): aVersion is lower than 1!");

	string res = STORAGE_ROOT_PATH + string("\\") + aUser + string("\\") + to_string(aVersion);
	return res;
}

void moveAllFiles(const path& aSrc, const path& aDst){
	boost::system::error_code ec;

	if (exists(aSrc)){
		if (is_regular_file(aSrc)){
			path parent = aDst.parent_path();
			if (!exists(parent)){
				create_directories(parent, ec);
				if (ec)
					throw EFilesystemException("Error creating directory path: " + parent.string() + " -> " + ec.message());
			}

			rename(aSrc, aDst, ec);
			if (ec)
				throw EFilesystemException("Error renaming path: " + aSrc.string() + " into:" + aDst.string() + " -> " + ec.message());
		}
		else if (is_directory(aSrc)){
			for (directory_entry& x : directory_iterator(aSrc)){
				path fName = path(x.path().string().substr(aSrc.string().length()));
				moveAllFiles(x.path(), aDst / fName);
			}
		}
	}
}
#pragma endregion