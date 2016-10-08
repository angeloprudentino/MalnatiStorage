/*
 * Author: Angelo Prudentino
 * Date: 29/09/2016
 * File: SqliteClientDB.cpp
 * Description: this file contains all classes implementing
 *              client-side DB management
 *
 */

#include "SqliteClientDB.h"


#define SQLITE_ERR_NUM 26
static string const SQLiteErrMsg[SQLITE_ERR_NUM + 1] = {
	/*  0 SQLITE_OK         */ "",
	/*  1 SQLITE_ERROR      */ "SQL error or missing database",
	/*  2 SQLITE_INTERNAL   */ "Internal logic error in SQLite",
	/*  3 SQLITE_PERM       */ "Access permission denied",
	/*  4 SQLITE_ABORT      */ "Callback routine requested an abort",
	/*  5 SQLITE_BUSY       */ "The database file is locked",
	/*  6 SQLITE_LOCKED     */ "A table in the database is locked",
	/*  7 SQLITE_NOMEM      */ "A malloc() failed",
	/*  8 SQLITE_READONLY   */ "Attempt to write a readonly database",
	/*  9 SQLITE_INTERRUPT  */ "Operation terminated by sqlite3_interrupt()",
	/* 10 SQLITE_IOERR      */ "Some kind of disk I/O error occurred",
	/* 11 SQLITE_CORRUPT    */ "The database disk image is malformed",
	/* 12 SQLITE_NOTFOUND   */ "NOT USED. Table or record not found",
	/* 13 SQLITE_FULL       */ "Insertion failed because database is full",
	/* 14 SQLITE_CANTOPEN   */ "Unable to open the database file",
	/* 15 SQLITE_PROTOCOL   */ "Database lock protocol error",
	/* 16 SQLITE_EMPTY      */ "Database is empty",
	/* 17 SQLITE_SCHEMA     */ "The database schema changed",
	/* 18 SQLITE_TOOBIG     */ "String or BLOB exceeds size limit",
	/* 19 SQLITE_CONSTRAINT */ "Abort due to constraint violation",
	/* 20 SQLITE_MISMATCH   */ "Data type mismatch",
	/* 21 SQLITE_MISUSE     */ "Library used incorrectly",
	/* 22 SQLITE_NOLFS      */ "Uses OS features not supported on host",
	/* 23 SQLITE_AUTH       */ "Authorization denied",
	/* 24 SQLITE_FORMAT     */ "Auxiliary database format error",
	/* 25 SQLITE_RANGE      */ "2nd parameter to sqlite3_bind out of range",
	/* 26 SQLITE_NOTADB     */ "File opened that is not a database file"
};


//////////////////////////////////
//         TUserFile	        //
//////////////////////////////////
#pragma region "TUserFile"
TUserFile::TUserFile(const string& aUser, const int aVersion, const string& aFilePath){
	this->fUser = make_string_ptr(aUser);
	this->fFilePath = make_string_ptr(aFilePath);
	this->fVersion = aVersion;

	//read file and calculate checksum
	try{
		this->fFileContent = readFile(path(this->fFilePath->c_str()));
		this->fChecksum = opensslB64FileChecksum(*(this->fFileContent));
	}
	catch (EOpensslException e){
		throw EUserFileException("TUserFile: " + e.getMessage());
	}
	path p(*(this->fFilePath));
	boost::system::error_code ec;
	this->fFileDate = last_write_time(p, ec);
	if (ec)
		this->fFileDate = time(nullptr);
}

TUserFile::TUserFile(const string& aUser, const int aVersion, const string& aFilePath, const string& aChecksum, const time_t& aFileDate){
	this->fUser = make_string_ptr(aUser);
	this->fFilePath = make_string_ptr(aFilePath);
	this->fVersion = aVersion;
	this->fChecksum = make_string_ptr(aChecksum);
	this->fFileDate = aFileDate;
}

TUserFile::~TUserFile(){
	if (this->fUser != nullptr)
		this->fUser.reset();

	if (this->fFilePath != nullptr)
		this->fFilePath.reset();

	if (this->fFileContent != nullptr)
		this->fFileContent.reset();

	if (this->fChecksum != nullptr)
		this->fChecksum.reset();

	this->fUser = nullptr;
	this->fFilePath = nullptr;
	this->fFileContent = nullptr;
	this->fChecksum = nullptr;
}

const bool TUserFile::verifyChecksum(){
	bool checksumMatches = false;
	string_ptr myChecksum = nullptr;
	try{
		myChecksum = opensslB64FileChecksum(*(this->fFileContent));
		checksumMatches = (*(myChecksum) == *(this->fChecksum));
	}
	catch (EOpensslException e){
		throw EUserFileException("verifyChecksum failed: " + e.getMessage());
	}
	myChecksum.reset();

	return checksumMatches;
}
#pragma endregion


//////////////////////////////////
//         TSqliteDB	        //
//////////////////////////////////
#pragma region "TSqliteDB"
void TSqliteDB::checkError(const int aCode, const string& aSender) {
	if (aCode > SQLITE_OK && aCode <= SQLITE_NOTADB) {
		throw ESqliteDBException(aSender + "Error Code: " + SQLiteErrMsg[aCode] + " SQLite Error: " + sqlite3_errmsg(this->fDB));
	}
}

void TSqliteDB::initDB(){
	if (this->fIsOpen) {
		string sql = "CREATE TABLE t_files (user TEXT NOT NULL DEFAULT '', version INTEGER NOT NULL DEFAULT 0, path TEXT NOT NULL DEFAULT '', ";
		sql = sql + "checksum TEXT NOT NULL DEFAULT '', lastmod INTEGER NOT NULL DEFAULT 0, PRIMARY KEY(user, version, path)); ";
		sql = sql + "CREATE INDEX IX_t_files_user_version on t_files (user, version);";

		this->checkError(sqlite3_exec(this->fDB, sql.c_str(), nullptr, nullptr, nullptr), "TSqliteDB.initDB.EXEC -> ");
	}
	else
		throw ESqliteDBException(string("The database ") + this->fName->c_str() + "is not open");
}

void TSqliteDB::checkIntegrity(){
	if (this->fIsOpen) {
		// check DB integrity
		string sql = "SELECT COUNT(*) FROM t_files;";
		int err = sqlite3_exec(this->fDB, sql.c_str(), NULL, NULL, NULL);
		if (err == SQLITE_NOTADB || err == SQLITE_CORRUPT) {
			//re-create DB from scratch
			boost::system::error_code ec;
			remove(path(this->fName->c_str()), ec);
			if (ec)
				throw ESqliteDBException(string("Error removing DB file: ") + this->fName->c_str() + " -> " + ec.message());
			this->initDB();
		}
	}
	else
		throw ESqliteDBException(string("The database ") + this->fName->c_str() + "is not open");
}

TSqliteDB::TSqliteDB(const string& aDBName){
	this->fName = new string(aDBName);

	if (!this->fIsOpen) {
		this->checkError(sqlite3_initialize(), "TSqliteDB.sqlite3_initialize -> ");
		bool toCreate = !exists(path(aDBName));

		this->checkError(sqlite3_open(aDBName.c_str(), &this->fDB), "TSqliteDB.sqlite3_open -> ");
		this->fIsOpen = true;

		this->checkError(sqlite3_exec(this->fDB, "PRAGMA foreign_keys = ON;", nullptr, nullptr, nullptr), "TSqliteDB.PRAGMA foreign_keys -> ");

		if (toCreate)
			this->initDB();

		this->checkIntegrity();
	}
	else
		throw ESqliteDBException("the database \"" + aDBName + "\" is already open");
}

TSqliteDB::~TSqliteDB(){
	if (this->fName != nullptr)
		delete this->fName;

	this->fName = nullptr;

	if (this->fInsertFileList_stmt != nullptr)
		this->checkError(sqlite3_finalize(this->fInsertFileList_stmt), "~TSqliteDB.sqlite3_finalize(fInsertFileList_stmt) -> ");
	this->fInsertFileList_stmt = nullptr;

	if (this->fRemoveFileList_stmt != nullptr)
		this->checkError(sqlite3_finalize(this->fRemoveFileList_stmt), "~TSqliteDB.sqlite3_finalize(fRemoveFileList_stmt) -> ");
	this->fRemoveFileList_stmt = nullptr;

	if (this->fGetLastVersion_stmt != nullptr)
		this->checkError(sqlite3_finalize(this->fGetLastVersion_stmt), "~TSqliteDB.sqlite3_finalize(fGetLastVersion_stmt) -> ");
	this->fGetLastVersion_stmt = nullptr;

	if (this->fGetFileList_stmt != nullptr)
		this->checkError(sqlite3_finalize(this->fGetFileList_stmt), "~TSqliteDB.sqlite3_finalize(fGetFileList_stmt) -> ");
	this->fGetFileList_stmt = nullptr;
	
	if (this->fIsOpen) {
		this->checkError(sqlite3_close(this->fDB), "~TSqliteDB.sqlite3_close -> ");
		this->fDB = nullptr;
	}
	
	this->checkError(sqlite3_shutdown(), "~TSqliteDB.sqlite3_shutdown -> ");
}

void TSqliteDB::insertFileList(TUserFileList_ptr& aFileList){
	if (this->fIsOpen) {
		for (TUserFileList::iterator it = aFileList->begin(); it != aFileList->end(); it++){
			try {
				if (fInsertFileList_stmt == nullptr) {
					std::string sql = "INSERT INTO t_files(user, version, path, checksum, lastmod)";
					sql = sql + "VALUES(:user, :version, :path, :checksum, :lastmod);";
					this->checkError(sqlite3_prepare_v2(this->fDB, sql.c_str(), -1, &fInsertFileList_stmt, nullptr), "TSqliteDB.insertFileList.sqlite3_prepare_v2 -> ");
				}

				string user = (*it)->getUser();
				this->checkError(sqlite3_bind_text(fInsertFileList_stmt, sqlite3_bind_parameter_index(fInsertFileList_stmt, ":user"), user.c_str(), (int)user.length(), nullptr), "TSqliteDB.insertFileList.sqlite3_bind_text -> ");
				string path = (*it)->getFilePath();
				this->checkError(sqlite3_bind_text(fInsertFileList_stmt, sqlite3_bind_parameter_index(fInsertFileList_stmt, ":path"), path.c_str(), (int)path.length(), nullptr), "TSqliteDB.insertFileList.sqlite3_bind_text -> ");
				string checksum = (*it)->getFileChecksum();
				this->checkError(sqlite3_bind_text(fInsertFileList_stmt, sqlite3_bind_parameter_index(fInsertFileList_stmt, ":user"), checksum.c_str(), (int)checksum.length(), nullptr), "TSqliteDB.insertFileList.sqlite3_bind_text -> ");
				int version = (*it)->getVersion();
				this->checkError(sqlite3_bind_int(fInsertFileList_stmt, sqlite3_bind_parameter_index(fInsertFileList_stmt, ":version"), version), "TSqliteDB.insertFileList.sqlite3_bind_int -> ");
				time_t lastmod = (*it)->getFileDate();
				this->checkError(sqlite3_bind_int(fInsertFileList_stmt, sqlite3_bind_parameter_index(fInsertFileList_stmt, ":lastmod"), (int)lastmod), "TSqliteDB.insertFileList.sqlite3_bind_int -> ");

				int res = -1;
				while (res != SQLITE_DONE) {
					res = sqlite3_step(fInsertFileList_stmt);
					if (res != SQLITE_DONE)
						this->checkError(res, "TSqliteDB.insertFileList.sqlite3_step -> ");
				}
			}
			catch (ESqliteDBException e) {
				if (fInsertFileList_stmt != nullptr) {
					this->checkError(sqlite3_reset(fInsertFileList_stmt), "TSqliteDB.insertFileList.sqlite3_reset -> ");
					this->checkError(sqlite3_clear_bindings(fInsertFileList_stmt), "TSqliteDB.insertFileList.sqlite3_clear_bindings -> ");
				}
				throw e;
			}
			if (fInsertFileList_stmt != nullptr) {
				this->checkError(sqlite3_reset(fInsertFileList_stmt), "TSqliteDB.insertFileList.sqlite3_reset -> ");
				this->checkError(sqlite3_clear_bindings(fInsertFileList_stmt), "TSqliteDB.insertFileList.sqlite3_clear_bindings -> ");
			}
		}
	}
	else
		throw ESqliteDBException(string("The database ") + this->fName->c_str() + "is not open");
}

void TSqliteDB::removeFileList(const string& aUser){
	if (this->fIsOpen) {
		try {
			if (fRemoveFileList_stmt == nullptr) {
				std::string sql = "DELETE FROM t_files WHERE user = :user;";
				this->checkError(sqlite3_prepare_v2(this->fDB, sql.c_str(), -1, &fRemoveFileList_stmt, nullptr), "TSqliteDB.removeFileList.sqlite3_prepare_v2 -> ");
			}

			this->checkError(sqlite3_bind_text(fRemoveFileList_stmt, sqlite3_bind_parameter_index(fRemoveFileList_stmt, ":user"), aUser.c_str(), (int)aUser.length(), nullptr), "TSqliteDB.removeFileList.sqlite3_bind_text -> ");

			int res = -1;
			while (res != SQLITE_DONE) {
				res = sqlite3_step(fRemoveFileList_stmt);
				if (res != SQLITE_DONE)
					this->checkError(res, "TSqliteDB.removeFileList.sqlite3_step -> ");
			}
		}
		catch (ESqliteDBException e) {
			if (fRemoveFileList_stmt != nullptr) {
				this->checkError(sqlite3_reset(fRemoveFileList_stmt), "TSqliteDB.removeFileList.sqlite3_reset -> ");
				this->checkError(sqlite3_clear_bindings(fRemoveFileList_stmt), "TSqliteDB.removeFileList.sqlite3_clear_bindings -> ");
			}
			throw e;
		}
		if (fRemoveFileList_stmt != nullptr) {
			this->checkError(sqlite3_reset(fRemoveFileList_stmt), "TSqliteDB.removeFileList.sqlite3_reset -> ");
			this->checkError(sqlite3_clear_bindings(fRemoveFileList_stmt), "TSqliteDB.removeFileList.sqlite3_clear_bindings -> ");
		}
	}
	else
		throw ESqliteDBException(string("The database ") + this->fName->c_str() + "is not open");
}

const int TSqliteDB::getLastVersion(const string& aUser){
	int query_res = 0;

	if (this->fIsOpen) {
		try {
			if (fGetLastVersion_stmt == nullptr) {
				std::string sql = "SELECT MAX(version) FROM t_files WHERE user = :user;";
				this->checkError(sqlite3_prepare_v2(this->fDB, sql.c_str(), -1, &fGetLastVersion_stmt, nullptr), "TSqliteDB.getLastVersion.sqlite3_prepare_v2 -> ");
			}

			this->checkError(sqlite3_bind_text(fGetLastVersion_stmt, sqlite3_bind_parameter_index(fGetLastVersion_stmt, ":user"), aUser.c_str(), (int)aUser.length(), nullptr), "TSqliteDB.getLastVersion.sqlite3_bind_text -> ");

			int res = -1;
			while (res != SQLITE_DONE) {
				res = sqlite3_step(fGetLastVersion_stmt);
				if (res == SQLITE_ROW) {
					query_res = sqlite3_column_int(fGetLastVersion_stmt, 0);
				}
				else if (res != SQLITE_DONE)
					this->checkError(res, "TSqliteDB.getLastVersion.sqlite3_step -> ");
			}

			while (res != SQLITE_DONE) {
				res = sqlite3_step(fGetLastVersion_stmt);
				if (res != SQLITE_DONE)
					this->checkError(res, "TSqliteDB.getLastVersion.sqlite3_step -> ");
			}
		}
		catch (ESqliteDBException e) {
			if (fGetLastVersion_stmt != nullptr) {
				this->checkError(sqlite3_reset(fGetLastVersion_stmt), "TSqliteDB.getLastVersion.sqlite3_reset -> ");
				this->checkError(sqlite3_clear_bindings(fGetLastVersion_stmt), "TSqliteDB.getLastVersion.sqlite3_clear_bindings -> ");
			}
			throw e;
		}
		if (fGetLastVersion_stmt != nullptr) {
			this->checkError(sqlite3_reset(fGetLastVersion_stmt), "TSqliteDB.getLastVersion.sqlite3_reset -> ");
			this->checkError(sqlite3_clear_bindings(fGetLastVersion_stmt), "TSqliteDB.getLastVersion.sqlite3_clear_bindings -> ");
		}
	}
	else
		throw ESqliteDBException(string("The database ") + this->fName->c_str() + "is not open");

	return query_res;
}

TUserFileList_ptr TSqliteDB::getFileList(const string& aUser, const int aVersion){
	TUserFileList_ptr query_res = nullptr;

	if (this->fIsOpen) {
		try {
			if (fGetFileList_stmt == nullptr) {
				std::string sql = "SELECT user, version, path, checksum, lastmod FROM t_files WHERE user = :user AND version = :version;";
				this->checkError(sqlite3_prepare_v2(this->fDB, sql.c_str(), -1, &fGetFileList_stmt, nullptr), "TSqliteDB.getFileList.sqlite3_prepare_v2 -> ");
			}

			this->checkError(sqlite3_bind_text(fGetFileList_stmt, sqlite3_bind_parameter_index(fGetFileList_stmt, ":user"), aUser.c_str(), (int)aUser.length(), nullptr), "TSqliteDB.getFileList.sqlite3_bind_text -> ");
			this->checkError(sqlite3_bind_int(fRemoveFileList_stmt, sqlite3_bind_parameter_index(fRemoveFileList_stmt, ":version"), aVersion), "TSqliteDB.removeFileList.sqlite3_bind_int -> ");

			int res = -1;
			while (res != SQLITE_DONE) {
				res = sqlite3_step(fGetFileList_stmt);
				if (res == SQLITE_ROW) {
					if (query_res == nullptr)
						query_res = new_TUserFileList_ptr();
					
					string u = (const char*)sqlite3_column_text(fGetFileList_stmt, 0);
					int v = sqlite3_column_int(fGetFileList_stmt, 1);
					string p = (const char*)sqlite3_column_text(fGetFileList_stmt, 2);
					string c = (const char*)sqlite3_column_text(fGetFileList_stmt, 3);
					long ts = sqlite3_column_int(fGetFileList_stmt, 4);
					TUserFile_ptr ptr = copy_TUserFile_ptr(u, v, p, c, ts);
					query_res->push_back(move_TUserFile_ptr(ptr));
				}
				else if (res != SQLITE_DONE)
					this->checkError(res, "TSqliteDB.getFileList.sqlite3_step -> ");
			}

			while (res != SQLITE_DONE) {
				res = sqlite3_step(fGetFileList_stmt);
				if (res != SQLITE_DONE)
					this->checkError(res, "TSqliteDB.getFileList.sqlite3_step -> ");
			}
		}
		catch (ESqliteDBException e) {
			if (fGetFileList_stmt != nullptr) {
				this->checkError(sqlite3_reset(fGetFileList_stmt), "TSqliteDB.getFileList.sqlite3_reset -> ");
				this->checkError(sqlite3_clear_bindings(fGetFileList_stmt), "TSqliteDB.getFileList.sqlite3_clear_bindings -> ");
			}
			throw e;
		}
		if (fGetFileList_stmt != nullptr) {
			this->checkError(sqlite3_reset(fGetFileList_stmt), "TSqliteDB.getFileList.sqlite3_reset -> ");
			this->checkError(sqlite3_clear_bindings(fGetFileList_stmt), "TSqliteDB.getFileList.sqlite3_clear_bindings -> ");
		}
	}
	else
		throw ESqliteDBException(string("The database ") + this->fName->c_str() + "is not open");

	return query_res;
}
#pragma endregion