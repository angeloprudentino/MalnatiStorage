#include "stdafx.h"
#include "SQlite_db.h"

using namespace std;

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
		cout << "Opened MyDb.db." << endl << endl;
		return 0;
	}

}

void SQlite_db::CloseSQlite_db(){
	sqlite3_close(db);
	cout << "Closed MyDb.db" << endl << endl;
}

int SQlite_db::ExecuteSQl(const char *sqlInsert){

	rc = sqlite3_exec(db, sqlInsert, NULL, NULL, &error);
	if (rc)
	{
		cerr << "Error executing SQLite3 statement: " << sqlite3_errmsg(db) << endl << endl;
		sqlite3_free(error);
		return 1;
	}
	else
	{
		cout << "Inserted a value into MyTable." << endl << endl;
		return 0;
	}

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
		cout << "Created MyTable." << endl << endl;
		return 0;
	}

}

void SQlite_db::DisplayTable(const char *sqlSelect){
	char **results = NULL;
	int rows, columns;
	sqlite3_get_table(db, sqlSelect, &results, &rows, &columns, &error);
	if (rc)
	{
		cerr << "Error executing SQLite3 query: " << sqlite3_errmsg(db) << endl << endl;
		sqlite3_free(error);
	}
	else
	{
		// Display Table
		for (int rowCtr = 0; rowCtr <= rows; ++rowCtr)
		{
			for (int colCtr = 0; colCtr < columns; ++colCtr)
			{
				// Determine Cell Position
				int cellPosition = (rowCtr * columns) + colCtr;

				// Display Cell Value
				cout.width(12);
				cout.setf(ios::left);
				cout << results[cellPosition] << " ";
			}

			// End Line
			cout << endl;

			// Display Separator For Header
			if (0 == rowCtr)
			{
				for (int colCtr = 0; colCtr < columns; ++colCtr)
				{
					cout.width(12);
					cout.setf(ios::left);
					cout << "~~~~~~~~~~~~ ";
				}
				cout << endl;
			}
		}
	}
	sqlite3_free_table(results);
}