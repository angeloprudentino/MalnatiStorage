#include "pch.h"
#include "SQlite_db.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string.h>
using namespace std;

SQlite_db::SQlite_db()
{
}


SQlite_db::~SQlite_db()
{
}

char* SQlite_db::OpenSQlite_db(const char* path){
	char w[1000]="";
	int rc;
	char path2[1000] ="";
	strcpy(path2, path);
	//rc = sqlite3_open("MyDb.db", &db);
	strcat(path2, "/MyDb.db");
	rc = sqlite3_open(path, &db);
	if (rc)
	{
		cerr << "Error opening SQLite3 database: " << sqlite3_errmsg(db) << endl << endl;
		strcat(w, "Error executing SQLite3 query: ");
		strcat(w, sqlite3_errmsg(db));
		strcat(w, "\n");
		sqlite3_close(db);
		return w;
	}
	else
	{
		//cout << "Opened MyDb.db." << endl << endl;
		strcat(w, "Db Open");
		strcat(w, " DONE");
		return w;
	}

}

void SQlite_db::CloseSQlite_db(){
	sqlite3_close(db);
	//cout << "Closed MyDb.db" << endl << endl;
}

char* SQlite_db::CreateTable(const char *sqlCreateTable){
	
	char w[1000] = ""; 
	rc = sqlite3_exec(db, sqlCreateTable, NULL, NULL, &error);
	if (rc)
	{
		cerr << "Error executing SQLite3 statement: " << sqlite3_errmsg(db) << endl << endl;
		strcat(w, "Error executing SQLite3 query: ");
		strcat(w, sqlite3_errmsg(db));
		strcat(w, "\n");
		sqlite3_free(error);
		return w;
	}
	else
	{
		//cout << "Created MyTable." << endl << endl;
		strcat(w, sqlCreateTable);
		strcat(w, " DONE");
		return w;
	}

}

char* SQlite_db::DisplayTable(char *sqlSelect){
	//wchar_t* str=L"";
	//wstring str;
	//char* w = "provo a scrivere in char*\n";
	char w[10000]="";



	char **results = NULL;
	int rows, columns;
	rc = sqlite3_get_table(db, sqlSelect, &results, &rows, &columns, &error);
	if (rc)
	{
		cerr << "Error executing SQLite3 query: " << sqlite3_errmsg(db) << endl << endl;
		strcat(w, "Error executing SQLite3 query: ");
		strcat(w,sqlite3_errmsg(db));
		strcat(w, "\n");
		sqlite3_free(error);
	}
	else
	{
		// Display Table
		strcat(w, "No errors, now display table\n");
		for (int rowCtr = 0; rowCtr <= rows; ++rowCtr)
		{
			for (int colCtr = 0; colCtr < columns; ++colCtr)
			{
				// Determine Cell Position
				int cellPosition = (rowCtr * columns) + colCtr;

				// Display Cell Value
				//cout.width(12);			
				//cout.setf(ios::left);
				//cout << results[cellPosition] << " ";
				
				char* c = results[cellPosition];
				strcat(w, c);
				strcat(w, " ");

			}

			// End Line
			//myfile << endl;
			//str.append(L"\n");
			strcat(w, "\n");

			// Display Separator For Header
			if (0 == rowCtr)
			{
				for (int colCtr = 0; colCtr < columns; ++colCtr)
				{
					//myfile.width(12);
					//myfile.setf(ios::left);
					//myfile << "~~~~~~~~~~~~ ";
					strcat(w,"~~~~~~~~~~~~ ");
				}
				//myfile << endl;
				strcat(w, "\n");
			}
		}
	}
	sqlite3_free_table(results);

	strcat(w,"End of writing on file.\n");
	return w;
}

char* SQlite_db::ExecuteSQl(char *sqlInsert){
	char w[10000] = "";
	rc = sqlite3_exec(db, sqlInsert, NULL, NULL, &error);
	if (rc)
	{
		cerr << "Error executing SQLite3 statement: " << sqlite3_errmsg(db) << endl << endl;
		strcat(w, "Error executing SQLite3 query: ");
		strcat(w, sqlite3_errmsg(db));
		strcat(w, "\n");
		sqlite3_free(error);
		return w;
	}
	else
	{
		//cout << "Inserted a value into MyTable." << endl << endl;
		strcat(w, sqlInsert);
		strcat(w, " DONE");
		return w;
	}

}