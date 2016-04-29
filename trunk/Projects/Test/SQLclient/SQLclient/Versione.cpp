#include "stdafx.h"
#include "Versione.h"


Versione::Versione(float VersionNum, String^ Utente, DateTime^ VersionDate, String^ VersionID)
{
	this->VersionNum = VersionNum;
	this->Utente = Utente;
	this->VersionDate = VersionDate;
	this->VersionID = VersionID;
}

Versione::~Versione(){

}

String^ Versione::GetUtente(){
	return this->Utente;
}

float Versione::GetVersionNum(){
	return this->VersionNum;
}

DateTime^ Versione::GetVersionDate(){
	return this->VersionDate;
}

String^ Versione::GetVersionID(){
	return this->VersionID;
}


