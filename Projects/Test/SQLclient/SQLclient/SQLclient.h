#include "stdafx.h"
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

using namespace std;
using namespace System;
using namespace System::Data;
using namespace System::Data::SqlClient;
using namespace System::IO;

//void insert(SqlConnection^ cn);

#pragma once
class MYSQLclient{
		
	public:
		MYSQLclient(){};
		SqlConnection^ CreateConnection();
		void CloseConnection(SqlConnection^ cn);
		void InsertNewUser(SqlConnection^ cn, String^ user, String^ pass, String^ salt);
		int VerifyCredentials(SqlConnection^ cn, String^ user, String^ pass);
		void ShowStudentiTable(SqlConnection^ cn);
};
