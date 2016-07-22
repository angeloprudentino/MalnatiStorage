#include "pch.h"
#include "SQlite_db.h"


SQlite_db::SQlite_db()
{
}


SQlite_db::~SQlite_db()
{
}

int SQlite_db::OpenSQlite_db(){
	int rc;
	rc = sqlite3_open("MyDb.db", &db);
	if (rc)
	{
		cerr << "Error opening SQLite3 database: " << sqlite3_errmsg(db) << endl << endl;
		sqlite3_close(db);
		return 1;
	}
	else
	{
		//cout << "Opened MyDb.db." << endl << endl;
		return 0;
	}

}

void SQlite_db::CloseSQlite_db(){
	sqlite3_close(db);
	//cout << "Closed MyDb.db" << endl << endl;
}

int SQlite_db::CreateTable(const char *sqlCreateTable){
	rc = sqlite3_exec(db, sqlCreateTable, NULL, NULL, &error);
	if (rc)
	{
		cerr << "Error executing SQLite3 statement: " << sqlite3_errmsg(db) << endl << endl;
		sqlite3_free(error);
		return 1;
	}
	else
	{
		//cout << "Created MyTable." << endl << endl;
		return 0;
	}

}