#include "stdafx.h"
#include "Versione.h"
#include <stdio.h>
#include <iostream>
#include <string>
#include <string.h>
#include <time.h>
#include <ctime>
#include <vector>
#include <array>
#using <mscorlib.dll>
#using <System.dll>
#using <System.Data.dll>
#using <System.Xml.dll>

//using byte = unsigned char;

using namespace std;
using namespace System;
using namespace System::Data;
using namespace System::Data::SqlClient;
using namespace System::IO;

#pragma once
ref class MYSQLClient
{private:
	//SqlConnection^ cn;    unsed
public:
	MYSQLClient();
	~MYSQLClient();
	SqlConnection^ CreateConnection();
	void CloseConnection(SqlConnection^ cn);
	void InsertNewUser(SqlConnection^ cn, String^ user, String^ pass, String^ salt);
	void InsertNewVersion(SqlConnection^ cn,Versione^ v);
	//NON FUNZIONA IL CONFRONTO
	int VerifyCredentials(SqlConnection^ cn, String^ user, String^ pass);
	void ShowUtentiTable(SqlConnection^ cn);
	void ShowVersioniTable(SqlConnection^ cn);
	void ShowVersioniUtente(SqlConnection^ cn, String^ user); 
	Versione^ LastVersion(SqlConnection^ cn, String^ user);
};


