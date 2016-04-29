#include "stdafx.h"
#include "MYSQLClient.h"
#include "Versione.h"
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
//TO DO
//1)non funziona il confronto tra stringhe in VerifyCredentials
//2)problema nella gestione dei parametri per la lettura delle versioni


using namespace std;
using namespace System;
using namespace System::Data;
using namespace System::Data::SqlClient;
using namespace System::IO;

MYSQLClient::MYSQLClient()
{

}

MYSQLClient::~MYSQLClient(){

}
SqlConnection^ MYSQLClient::CreateConnection(){
	SqlConnection^ cn = gcnew SqlConnection();

	//1 connessione al db SQL server
	cn->ConnectionString = "Server=localhost;Database=DBApplicazione;Trusted_Connection=True;";
	cout << "provo a connettere" << endl;
	cn->Open();
	cout << "connesso al DB:" << endl;
	Console::WriteLine(cn->Database);

	return cn;
}


void MYSQLClient::CloseConnection(SqlConnection^ cn){
	Console::WriteLine("chiusura connessione con il database:" + cn->Database);
	cn->Close();
}

void MYSQLClient::ShowUtentiTable(SqlConnection^ cn){
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

void MYSQLClient::ShowVersioniTable(SqlConnection^ cn){
	SqlCommand^ cmd2;
	Console::WriteLine("contenuto del DB, tabella Versioni2");
	cmd2 = gcnew SqlCommand("SELECT * FROM Versioni2");
	cmd2->CommandType = CommandType::Text;
	cmd2->Connection = cn;

	SqlDataReader^ reader;
	reader = cmd2->ExecuteReader();

	Console::Write("VERSIONNUM          ");
	Console::Write("UTENTE              ");
	Console::Write("VERSIONDATE         ");
	Console::WriteLine("VERSIONID   ");


	try{
		if (reader->HasRows){
			while (reader->Read()){
				Console::Write(reader->GetSqlString(0));
				SqlTypes::SqlString str = reader->GetSqlString(0);
				String^ sstr = str.ToString();
				
				Console::Write(reader->GetSqlString(1));
				Console::Write(reader->GetSqlString(2));
				Console::WriteLine(reader->GetSqlString(3));
			}
		}
	}
	catch (Exception^ ex) {

		Console::WriteLine(ex->Message);
	}
	//chiude la reader
	reader->Close();

}
void MYSQLClient::InsertNewUser(SqlConnection^ cn, String^ user, String^ pass, String^ salt){
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

void MYSQLClient::InsertNewVersion(SqlConnection^ cn,Versione^ v){
	SqlCommand^ cmd2;
	//prima transazione, una insert -> FUNZIONA
	cmd2 = gcnew SqlCommand("INSERT INTO Versioni2(VersionNum,Utente,VersionDate,VersionID) VALUES (@vers_num,@utente,@date,@version_id);");
	cmd2->CommandType = CommandType::Text;
	cmd2->Connection = cn; //imposto la connessione nelle proprietà
	cmd2->Parameters->Add("@vers_num", SqlDbType::Float)->Value = v->GetVersionNum();
	cmd2->Parameters->Add("@utente", SqlDbType::NChar, 20)->Value = v->GetUtente();
	DateTime^ dt = v->GetVersionDate();
	
	cmd2->Parameters->Add("@date", SqlDbType::NChar,20)->Value = dt->ToString();
	cmd2->Parameters->Add("@version_id", SqlDbType::NChar, 20)->Value = v->GetVersionID();

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

int MYSQLClient::VerifyCredentials(SqlConnection^ cn, String^ user, String^ pass){
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
				read_pass = reader->GetSqlString(0);
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
	String^ str = read_pass.ToString();
	Console::WriteLine("str: " + str);
	Console::WriteLine("pass: " + pass);
	reader->Close();
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

void MYSQLClient::ShowVersioniUtente(SqlConnection^ cn, String^ user){
	SqlCommand^ cmd2;
	Console::WriteLine("versioni per l' utente: "+user);
	cmd2 = gcnew SqlCommand("SELECT * FROM Versioni2 WHERE Utente=@id");
	cmd2->CommandType = CommandType::Text;
	cmd2->Connection = cn;
	cmd2->Parameters->Add("@id", SqlDbType::NChar, 20)->Value = user;

	SqlDataReader^ reader;
	reader = cmd2->ExecuteReader();

	Console::Write("VERSIONNUM          ");
	Console::Write("UTENTE              ");
	Console::Write("VERSIONDATE         ");
	Console::WriteLine("VERSIONID   ");


	try{
		if (reader->HasRows){
			while (reader->Read()){
				Console::Write(reader->GetSqlString(0));
				SqlTypes::SqlString str = reader->GetSqlString(0);
				String^ sstr = str.ToString();

				Console::Write(reader->GetSqlString(1));
				Console::Write(reader->GetSqlString(2));
				Console::WriteLine(reader->GetSqlString(3));
			}
		}
	}
	catch (Exception^ ex) {

		Console::WriteLine(ex->Message);
	}
	//chiude la reader
	reader->Close();

}

Versione^ MYSQLClient::LastVersion(SqlConnection^ cn, String^ user){
	SqlCommand^ cmd2;
	Console::WriteLine("Ultima versione per l' utente: " + user);
	cmd2 = gcnew SqlCommand("SELECT * FROM Versioni2 WHERE Utente=@id AND VersionNum = (SELECT MAX(VersionNum) FROM Versioni2 WHERE Utente=@id)");
	cmd2->CommandType = CommandType::Text;
	cmd2->Connection = cn;
	cmd2->Parameters->Add("@id", SqlDbType::NChar, 20)->Value = user;

	SqlDataReader^ reader;
	reader = cmd2->ExecuteReader();

	Console::Write("VERSIONNUM          ");
	Console::Write("UTENTE              ");
	Console::Write("VERSIONDATE         ");
	Console::WriteLine("VERSIONID   ");
	float vers_num;
	DateTime^ dt = gcnew DateTime();
	String^ version_id;
	
	try{
		if (reader->HasRows){
			while (reader->Read()){
				Console::Write(reader->GetSqlString(0));
				SqlTypes::SqlString vers_num = reader->GetSqlString(0);
				String^ svers_num = vers_num.ToString();

				Console::Write(reader->GetSqlString(1));
				Console::Write(reader->GetSqlString(2));
				SqlTypes::SqlString date = reader->GetSqlString(2);
				String^ date_s = date.ToString();
				dt->Parse(date_s);
				Console::WriteLine(reader->GetSqlString(3));
				SqlTypes::SqlString vers_id = reader->GetSqlString(3);
				version_id = vers_id.ToString();
			}
		}
	}
	catch (Exception^ ex) {

		Console::WriteLine(ex->Message);
	}
	Versione^ v1 = gcnew Versione(vers_num, user, dt, version_id);
	//chiude la reader
	reader->Close();

	return v1;
}