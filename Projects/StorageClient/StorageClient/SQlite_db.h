#pragma once

#include <sqlite3.h>
class SQlite_db
{
private:
	sqlite3 *db; // puntatore al db
	int rc; //risultato ultima operazione eseguita (0 -> corretta,1 -> sbagliata)
	char *error; //stringa per stampare errori	
public:
	SQlite_db();
	~SQlite_db();
	int OpenSQlite_db();
	void CloseSQlite_db();
	int CreateTable(const char *sqlCreateTable);
};

