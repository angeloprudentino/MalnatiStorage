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


#pragma once
ref class Versione
{
private:
	float VersionNum;
	String^ Utente;
	DateTime^ VersionDate;
	String^ VersionID;

public:
	Versione(float VersionNum,String^ Utente,DateTime^ VersionDate,String^ VersionID);
	~Versione();
	float GetVersionNum();
	String^ GetUtente();
	DateTime^ GetVersionDate();
	String^ GetVersionID();
};

