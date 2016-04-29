// SQLclient.cpp : definisce il punto di ingresso dell'applicazione console.
//
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

////void insert(SqlConnection^ cn);
//SqlConnection^ CreateConnection();
//void CloseConnection(SqlConnection^ cn);
//void InsertNewUser(SqlConnection^ cn, String^ user, String^ pass, String^ salt);
//int VerifyCredentials(SqlConnection^ cn, String^ user, String^ pass);
//void ShowStudentiTable(SqlConnection^ cn);
//void insertFile(SqlConnection^ cn);



//int _tmain(int argc, _TCHAR* argv[])
//{
//	SqlConnection^ cn = CreateConnection();
//	int res;
//
//
//	//3 inserimento di un utente ->Funziona con passaggio di parametri
//	System::String^ user = "Ut3";
//	System::String^ pass = "pass3";
//	System::String^ salt = "asbfaksd";
//	//era per provare
//	InsertNewUser(cn,user,pass,salt);
//
//	ShowStudentiTable(cn);
//	//NON FUNZIONA IL CONFRONTO
//	res=VerifyCredentials(cn, user, pass);
//	
//
//	CloseConnection(cn);
//	//per non farlo chiudere
//	Console::ReadLine();
//	return 0;
//}

void addVersion();

SqlConnection^ CreateConnection(){
	SqlConnection^ cn = gcnew SqlConnection();

	//1 connessione al db SQL server
	cn->ConnectionString = "Server=localhost;Database=DBApplicazione;Trusted_Connection=True;";
	cout << "provo a connettere" << endl;
	cn->Open();
	cout << "connesso al DB:" << endl;
	Console::WriteLine(cn->Database);

	return cn;
}

void CloseConnection(SqlConnection^ cn){
	Console::WriteLine("chiusura connessione con il database:"+cn->Database);
	cn->Close();
}

void ShowStudentiTable(SqlConnection^ cn){
	SqlCommand^ cmd2;
	Console::WriteLine("contenuto del DB, tabella Utenti");
	cmd2 = gcnew SqlCommand("SELECT * FROM Utenti");
	cmd2->CommandType = CommandType::Text;
	cmd2->Connection = cn;

	SqlDataReader^ reader;
	reader = cmd2->ExecuteReader();

	Console::Write("UTENTE             ");
	Console::Write("PASSWORD             ");
	Console::WriteLine("SALE   ");


	try{
		if (reader->HasRows){
			while (reader->Read()){
				Console::Write(reader->GetSqlString(0));
				Console::Write(reader->GetSqlString(1));
				Console::WriteLine(reader->GetSqlString(2));
			}
		}
	}
	catch (Exception^ ex) {

		Console::WriteLine(ex->Message);
	}
	//chiude la reader
	reader->Close();

}

void InsertNewUser(SqlConnection^ cn,String^ user,String^ pass,String^ salt){
	SqlCommand^ cmd2;
	//prima transazione, una insert -> FUNZIONA
	cmd2 = gcnew SqlCommand("INSERT INTO Utenti(Utente,Password,Sale) VALUES (@id,@pass,@salt);");
	cmd2->CommandType = CommandType::Text;
	cmd2->Connection = cn; //imposto la connessione nelle proprietà
	cmd2->Parameters->Add("@id", SqlDbType::NChar, 20)->Value = user;
	cmd2->Parameters->Add("@pass", SqlDbType::NChar, 20)->Value = pass;
	cmd2->Parameters->Add("@salt", SqlDbType::NChar, 20)->Value = salt;
	Console::WriteLine("testo della query");
	Console::WriteLine(cmd2->CommandText);
	try{
		cmd2->ExecuteNonQuery(); //viene eseguita la query
	}
	catch (Exception^ ex) {
		Console::WriteLine("rilevata eccezione");
		Console::WriteLine(ex->Message);
	}
}

int VerifyCredentials(SqlConnection^ cn, String^ user, String^ pass){
	SqlCommand^ cmd;
	Console::WriteLine("verifica delle credenziali dell' utente: " + user);
	cmd = gcnew SqlCommand("SELECT Password FROM Utenti WHERE Utente=@id");
	cmd->CommandType = CommandType::Text;
	cmd->Connection = cn;
	cmd->Parameters->Add("@id", SqlDbType::NChar, 20)->Value = user;

	SqlDataReader^ reader;
	try{
		reader = cmd->ExecuteReader();
	}
	catch (Exception^ ex) {
		Console::WriteLine("rilevata eccezione");
		Console::WriteLine(ex->Message);
	}

	//leggo il risultato della query
	System::Data::SqlTypes::SqlString read_pass;
	try{
		if (reader->HasRows){
			while (reader->Read()){
				Console::Write(reader->GetSqlString(0));
				read_pass=reader->GetSqlString(0);
				//Console::Write(reader->GetSqlString(1));
				//read_pass = reader->GetSqlString(1);
				//read_pass=reader->GetSqlString(0);
			}
		}
	}
	catch (Exception^ ex) {

		Console::WriteLine(ex->Message);
	}

	Console::WriteLine("password nel DB: ");
	Console::WriteLine(read_pass);

	//confronto tra le pass;
	String^ str=read_pass.ToString();
	Console::WriteLine("str: "+str);
	Console::WriteLine("pass: "+pass);

	//NON FUNZIONA IL CONFRONTO
	if (str->Equals(pass)){
		Console::WriteLine("password corretta");
		return 0;
	}
	else{
		Console::WriteLine("password non corretta");
		return 1;
	}
}

//void insertFile(SqlConnection^ cn){
//	System::IO::FileStream^ fs = gcnew System::IO::FileStream("progetto.txt", System::IO::FileMode::Open);
//	long long size = fs->Length;
//	//array<byte>^ bytes = gcnew array<byte>(size);
//	array<unsigned char>^ bytes = gcnew array<unsigned char>(size);
//	fs->Read(bytes, 0, sizeof(bytes));
//	fs->Close();
//	time_t t = time(0);
//	SqlCommand^ cmd2;
//
//	cmd2 = gcnew SqlCommand("INSERT INTO Utenti(NomeFile,Orario,Oggetto) VALUES (@nomefile,@orario,@file);");
//	cmd2->CommandType = CommandType::Text;
//	cmd2->Connection = cn; //imposto la connessione nelle proprietà
//	cmd2->Parameters->Add("@nomefile", SqlDbType::NChar, 10)->Value = fs->Name;;
//	cmd2->Parameters->Add("@orario", SqlDbType::Timestamp,sizeof(time_t))->Value = t;
//	//array<Byte>^byteArray = BitConverter::GetBytes(bytes);
//	cout << 
//	//cmd2->Parameters->Add("@file", SqlDbType::VarBinary, size)->Value = NULL; //->DA CONVERTIRE IN byte[]
//	cmd2->ExecuteNonQuery(); //viene eseguita la query
//	};