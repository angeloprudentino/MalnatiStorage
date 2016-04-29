
#include "stdafx.h"
#include "SQLclient.h"
#include "Versione.h"
#include "MYSQLClient.h"
#include <stdio.h>
#include <iostream>
#include <string>
#include <string.h>
#include <time.h>
#include <ctime>
#using <mscorlib.dll>
#using <System.dll>
#using <System.Data.dll>
#using <System.Xml.dll>

//using byte = unsigned char;


using namespace System;
using namespace System::Data;
using namespace System::Data::SqlClient;
using namespace System::IO;
using namespace std;


int _tmain(int argc, _TCHAR* argv[])
{
	MYSQLClient^ db;
	db = gcnew MYSQLClient();
	
	SqlConnection^ cn = db->CreateConnection();
	int res;


	//3 inserimento di un utente ->Funziona con passaggio di parametri
	System::String^ user = "Ut4";
	System::String^ pass = "pass4";
	System::String^ salt = "asbfaksd";
	////era per provare
	db->InsertNewUser(cn, user, pass, salt);

	db->ShowUtentiTable(cn);
	////NON FUNZIONA IL CONFRONTO
	res = db->VerifyCredentials(cn, user, pass);
	//inserimento di una nuova versione
	//void InsertNewVersion(SqlConnection^ cn, int vers_num, String^ user, String^  date, String^ version_id);
	float vers_num = 2;
	user = "mario";
	//std::string date = "2008/12/1";
	//DateTime dt(2008,12,1,7,32,22);
	System::DateTime^ dt;
	dt= gcnew System::DateTime(2015, 12, 1, 7, 32, 22);
	String^ str = dt->ToString();
	str->Trim();
	Console::WriteLine("data di prova "+str);
	String^ version_id = "id1";

	Versione^ v1 =gcnew Versione(vers_num,user,dt,version_id);


	db->InsertNewVersion(cn,v1);

	db->ShowVersioniTable(cn);

	db->ShowVersioniUtente(cn, user);

	db->LastVersion(cn, user);

	db->CloseConnection(cn);
	//per non farlo chiudere
	Console::ReadLine();
	return 0;
}