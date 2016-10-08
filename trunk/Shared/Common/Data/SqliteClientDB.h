/*
 * Author: Angelo Prudentino
 * Date: 29/09/2016
 * File: SqliteClientDB.h
 * Description: this file contains all classes implementing
 *              client-side DB management
 *
 */
#pragma once

#include "sqlite/sqlite3.h"
#include "Utility.h"

#define SQLITE_DEFAULT_DB_NAME "userInfo.sqlite"


//////////////////////////////////////
//       EUserFileException	        //
//////////////////////////////////////
class EUserFileException : public EBaseException {
public:
	EUserFileException(const string& aMsg) : EBaseException(aMsg){}
};


//////////////////////////////////////
//       ESqliteDBException	        //
//////////////////////////////////////
class ESqliteDBException : public EBaseException {
public:
	ESqliteDBException(const string& aMsg) : EBaseException(aMsg){}
};


//////////////////////////////////
//         TUserFile	        //
//////////////////////////////////
class TUserFile{
private:
	string_ptr fUser = nullptr;
	int fVersion = 0;
	string_ptr fFilePath = nullptr;
	string_ptr fFileContent = nullptr;
	string_ptr fChecksum = nullptr;
	time_t fFileDate;
	
public:
	TUserFile(const string& aUser, const int aVersion, const string& aFilePath); //throws EUserFileException
	TUserFile(const string& aUser, const int aVersion, const string& aFilePath, const string& aChecksum, const time_t& aFileDate);
	TUserFile(const TUserFile&) = delete;            // disable copying
	TUserFile& operator=(const TUserFile&) = delete; // disable assignment
	~TUserFile();

	const bool verifyChecksum(); //throws EUserFileException

	//getters
	const string getUser() { return this->fUser->c_str(); }
	const string getFilePath() { return this->fFilePath->c_str(); }
	const string getFileChecksum() { return this->fChecksum->c_str(); }
	const int getVersion() { return this->fVersion; }
	const time_t getFileDate() { return this->fFileDate; }
};
typedef std::unique_ptr<TUserFile> TUserFile_ptr;
typedef std::list<TUserFile_ptr> TUserFileList;
typedef std::unique_ptr<TUserFileList> TUserFileList_ptr;
#define new_TUserFile_ptr(aUser, aVersion, aFilePath) std::make_unique<TUserFile>(aUser, aVersion, aFilePath)
#define copy_TUserFile_ptr(aUser, aVersion, aFilePath, aChecksum, aFileDate) std::make_unique<TUserFile>(aUser, aVersion, aFilePath, aChecksum, aFileDate)
#define make_TUserFile_ptr(ptr) std::make_unique<TUserFile>(ptr)
#define move_TUserFile_ptr(ptr) std::move(ptr)
#define new_TUserFileList_ptr() std::make_unique<TUserFileList>()


//////////////////////////////////
//         TSqliteDB	        //
//////////////////////////////////
class TSqliteDB{
private:
	sqlite3* fDB = nullptr;
	string* fName = nullptr;
	bool fIsOpen = false;
	sqlite3_stmt* fInsertFileList_stmt = nullptr;
	sqlite3_stmt* fRemoveFileList_stmt = nullptr;
	sqlite3_stmt* fGetLastVersion_stmt = nullptr;
	sqlite3_stmt* fGetFileList_stmt = nullptr;

	void checkError(const int aCode, const string& aSender);

	void initDB(); //throws ESqliteDBException
	void checkIntegrity(); //throws ESqliteDBException

public:
	TSqliteDB(const string& aDBName); //throws ESqliteDBException
	TSqliteDB(const TSqliteDB&) = delete;            // disable copying
	TSqliteDB& operator=(const TSqliteDB&) = delete; // disable assignment
	~TSqliteDB(); //throws ESqliteDBException

	void insertFileList(TUserFileList_ptr& aFileList); //throws ESqliteDBException
	void removeFileList(const string& aUser); //throws ESqliteDBException
	const int getLastVersion(const string& aUser); //throws ESqliteDBException
	TUserFileList_ptr getFileList(const string& aUser, const int aVersion); //throws ESqliteDBException
};