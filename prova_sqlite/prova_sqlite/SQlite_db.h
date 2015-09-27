#include "sqlite3.h"
#include "stdafx.h"
#include <list>
#include <stdio.h>
#include <string.h>
#include <string>
#include <Windows.h>
#include <process.h>
#include <vector>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <tchar.h> 
#include <strsafe.h>
#include <fileapi.h>
#include <fstream>
#include <iostream>
using namespace std;

#pragma once
class SQlite_db
{

private:
	sqlite3 *db; // puntatore al db
	int rc; //risultato ultima operazione eseguita (0 -> corretta,1 -> sbagliata)
	char *error; //stringa per stampare errori
public:
	int OpenSQlite_db();
	void CloseSQlite_db();
	int CreateTable(const char *sqlCreateTable);
	int ExecuteSQl(const char *sqlInsert);
	void DisplayTable(const char *sqlSelect);
	SQlite_db();
	~SQlite_db();
};

