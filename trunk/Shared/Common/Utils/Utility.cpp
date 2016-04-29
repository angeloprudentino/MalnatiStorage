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
#include "openssl/rand.h"
#include "openssl/evp.h"
#include "openssl/sha.h"
#include "openssl/buffer.h"
#include "openssl/bio.h" // BIO objects for I/O
#include "openssl/ssl.h" // SSL and SSL_CTX for SSL connections
#include "openssl/err.h" // Error reporting

#include "Utility.h"

using namespace std;

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
	std::ostringstream oss;
	if (oss){
		oss << t;
		return oss.str();
	}

	return "";
}

const time_t stringToTime(const string& s) {
	istringstream stream(s);
	time_t t;
	if (stream){
		stream >> t;
	}

	return t;
}
#pragma endregion

#pragma region "Crypto Utilities"
// openssl crypto system init
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

	try_again = false;
	do {
		ret = BIO_write(aBio, (void*)aContent, aLen);
		if (ret == -2) {
			//not implemented for that bio
			string err = "BIO_write returns -2 (not implemented for that bio): ";
			throw EOpensslException(err + ERR_error_string(ERR_get_error(), NULL));
		}
		else if (ret == 0 || ret == -1){
			//maybe have something more to write
			if (BIO_should_retry(aBio)) {
				//should retry write
				try_again = true;
			}
			else{
				string err = "BIO_should_retry signals an error: ";
				throw EOpensslException(err + ERR_error_string(ERR_get_error(), NULL));
			}
		}
		else
			try_again = false;
	} while (try_again);

	if (BIO_flush(aBio) != 1){
		string err = "BIO_flush returns -1: ";
		throw EOpensslException(err + ERR_error_string(ERR_get_error(), NULL));
	}

	return aBio;
}

string_ptr opensslB64Encode(char* aContent, int aLen){
	BIO *bio = NULL;
	BIO *b64 = NULL;
	char* ret_data = NULL;
	BUF_MEM *bufferPtr = NULL;

	if (aContent == NULL) {
		return NULL;
	}

	try {
		/* setup input BIO chain */
		bio = BIO_new(BIO_s_mem());
		if (bio == NULL){
			string err = "BIO_new(BIO_s_mem()) returns NULL: ";
			throw EOpensslException(err + ERR_error_string(ERR_get_error(), NULL));
		}

		b64 = BIO_new(BIO_f_base64());
		if (b64 == NULL){
			string err = "BIO_new(BIO_f_base64()) returns NULL: ";
			throw EOpensslException(err + ERR_error_string(ERR_get_error(), NULL));
		}

		bio = BIO_push(b64, bio);
		BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL); //Ignore newlines - write everything in one line

		bio = fillOpensslBIO(bio, aContent, aLen);

		BIO_get_mem_ptr(bio, &bufferPtr);
		BIO_set_close(bio, BIO_NOCLOSE);

		BIO_free_all(bio);

		ret_data = (*bufferPtr).data;
		ret_data[(*bufferPtr).length] = '\0';
		return make_shared<string>(ret_data);
	}
	catch (const EOpensslException& e) {
		if (bio != NULL)
			BIO_free_all(bio);
		throw e;
	}
}

B64result opensslB64Decode(const string& aString){
	BIO *bio_in = NULL;
	BIO *bio_out = NULL;
	BIO *b64 = NULL;
	int ret;
	char buffer[1024];
	BUF_MEM *bufferPtr = NULL;
	B64result res;

	if (aString.empty()) {
		res.data = (char*)aString.c_str();
		res.size = (int)aString.length();
		return res;
	}

	try {
		/* setup input BIO chain */
		bio_in = BIO_new(BIO_s_mem());
		if (bio_in == NULL){
			string err = "bio_in = BIO_new(BIO_s_mem()) returns NULL: ";
			throw EOpensslException(err + ERR_error_string(ERR_get_error(), NULL));
		}

		bio_in = fillOpensslBIO(bio_in, (char*)aString.c_str(), (int)aString.length());
		BIO_set_close(bio_in, BIO_NOCLOSE);

		b64 = BIO_new(BIO_f_base64());
		if (b64 == NULL){
			string err = "BIO_new(BIO_f_base64()) returns NULL: ";
			throw EOpensslException(err + ERR_error_string(ERR_get_error(), NULL));
		}

		bio_in = BIO_push(b64, bio_in);
		BIO_set_flags(bio_in, BIO_FLAGS_BASE64_NO_NL); //Ignore newlines - write everything in one line

		bio_out = BIO_new(BIO_s_mem());
		if (bio_out == NULL) {
			string err = "bio_out = BIO_new(BIO_s_mem()) returns NULL: ";
			throw EOpensslException(err + ERR_error_string(ERR_get_error(), NULL));
		}

		while ((ret = BIO_read(b64, (void*)buffer, sizeof buffer)) > 0) {
			BIO_write(bio_out, buffer, ret);
		}
		if (BIO_flush(bio_out) != 1){
			string err = "BIO_flush(bio_out) returns -1: ";
			throw EOpensslException(err + ERR_error_string(ERR_get_error(), NULL));
		}

		BIO_get_mem_ptr(bio_out, &bufferPtr);
		BIO_set_close(bio_out, BIO_NOCLOSE);

		BIO_free_all(bio_in);
		BIO_free_all(bio_out);

		res.data = (*bufferPtr).data;
		res.data[(*bufferPtr).length] = '\0';
		res.size = (int)((*bufferPtr).length);

		return res;
	}
	catch (const EOpensslException& e) {
		BIO_free_all(bio_in);
		BIO_free_all(bio_out);
		throw e;
	}
}

string_ptr opensslB64EncodeFile(const string aFileName){
	string* contents = new string();
	ifstream file(aFileName, ios::in | ios::binary);
	if (file){
		file.seekg(0, ios::end);
		contents->resize(file.tellg());
		file.seekg(0, ios::beg);

		file.read((char*)contents->c_str(), contents->size());
		if (!file)
			throw EOpensslException("an error is occurred reading from file " + aFileName);

		file.close();
		if (!file)
			throw EOpensslException("an error is occurred closing file " + aFileName);
	}

	string_ptr res = opensslB64Encode((char*)contents->c_str(), (int)contents->length());
	delete contents;

	return res;
}

string_ptr opensslCoreChecksum(BIO* aInputBio){
	BIO *md;
	bool try_again = false;
	char buf[1024];
	char mdbuf[EVP_MAX_MD_SIZE];
	int mdlen;
	
	md = BIO_new(BIO_f_md());
	if (md == NULL){
		string err = "BIO_new(BIO_f_md()) returns NULL: ";
		throw EOpensslException(err + ERR_error_string(ERR_get_error(), NULL));
	}

	if (BIO_set_md(md, EVP_sha224()) == 0){
		string err = "BIO_set_md() returns an error: ";
		throw EOpensslException(err + ERR_error_string(ERR_get_error(), NULL));
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
			string err = "BIO_gets returns -2 (not implemented for that bio): ";
			throw EOpensslException(err + ERR_error_string(ERR_get_error(), NULL));
		}
		else if (mdlen == 0 || mdlen == -1){
			//maybe have something more to read
			if (BIO_should_retry(md)) {
				//should retry read
				try_again = true;
			}
			else{
				string err = "BIO_should_retry signals an error: ";
				throw EOpensslException(err + ERR_error_string(ERR_get_error(), NULL));
			}
		}
		else
			try_again = false;
	} while (try_again);

	return opensslB64Encode(mdbuf, mdlen);
}

//string_ptr opensslChecksum(char* aContent, int aLen){
//	BIO *bio_in = NULL;
//	bool try_again = false;
//
//	if (aContent == NULL) {
//		return NULL;
//	}
//
//	try {
//		/* setup input BIO chain */
//		bio_in = BIO_new(BIO_s_mem());
//		if (bio_in == NULL){
//			string err = "bio_in = BIO_new(BIO_s_mem()) returns NULL: ";
//			throw EOpensslException(err + ERR_error_string(ERR_get_error(), NULL));
//		}
//
//		bio_in = fillOpensslBIO(bio_in, aContent, aLen);
//		BIO_set_close(bio_in, BIO_NOCLOSE);
//
//		string_ptr result = opensslCoreChecksum(bio_in);
//		BIO_free_all(bio_in);
//		return result;
//	}
//	catch (EOpensslException& e) {
//		BIO_free_all(bio_in);
//		throw e;
//	}
//}
//
//string_ptr opensslFileChecksum(const string aFileName){
//	BIO *bio_in;
//
//	try {
//		/* setup input BIO chain */
//		bio_in = BIO_new(BIO_s_file());
//		if (bio_in == NULL){
//			string err = "BIO_new(BIO_s_file()) returns NULL: ";
//			throw EOpensslException(err + ERR_error_string(ERR_get_error(), NULL));
//		}
//
//		if (BIO_read_filename(bio_in, aFileName.c_str()) == 0){
//			string err = "BIO_read_filename() returns an error: ";
//			throw EOpensslException(err + ERR_error_string(ERR_get_error(), NULL));
//		}
//
//		string_ptr result = opensslCoreChecksum(bio_in);
//		BIO_free_all(bio_in);
//		return result;
//	}
//	catch (EOpensslException& e) {
//		BIO_free_all(bio_in);
//		throw e;
//	}
//}

string_ptr opensslB64Checksum(const string_ptr aString){
	BIO *bio_in = NULL;
	bool try_again = false;

	if (aString->empty()) {
		return NULL;
	}

	try {
		/* setup input BIO chain */
		bio_in = BIO_new(BIO_s_mem());
		if (bio_in == NULL){
			string err = "bio_in = BIO_new(BIO_s_mem()) returns NULL: ";
			throw EOpensslException(err + ERR_error_string(ERR_get_error(), NULL));
		}

		bio_in = fillOpensslBIO(bio_in, (char*)aString->c_str(), (int)aString->length());
		BIO_set_close(bio_in, BIO_NOCLOSE);

		string_ptr result = opensslCoreChecksum(bio_in);
		BIO_free_all(bio_in);
		return result;
	}
	catch (const EOpensslException& e) {
		if (bio_in != NULL)
			BIO_free_all(bio_in);
		throw e;
	}
}
#pragma endregion