// prova_sqlite.cpp : definisce il punto di ingresso dell'applicazione console.
//
//PRESO DA TUTORIAL NEI PREFERITI
//saltare la parte sulla generazione libreria,vedere punto 11



//VEDI
//il codice interagisce con un DB sqlite, fa una LS su una cartella passata da linea di comando (da proprietà del progetto)
//e inserisce, costruendo la query, ogni file nel db

//DA FARE
//convertire la data di ultima modifica in stringa per poterla inserire nel db

//Dà ERRORI IN ESECUZIONE PERCHè CERCA NELLA STESSA CARTELLA E RISULTANO CHIAVI GIà INSERITE
//basta testarlo su una nuova tabella

#include "stdafx.h"
#include "sqlite3.h"
#include "stdafx.h"
#include "SQlite_db.h"
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
#include <sstream>

using namespace std;
SQlite_db db1;
int search(wstring dir);
void DisplayErrorBox(LPTSTR lpszFunction);
char* build_insert_query(WIN32_FIND_DATA ffd, LARGE_INTEGER filesize);

int _tmain(int argc, _TCHAR* argv[])
{
	int result;
	//AGGIUNTO
	if (argc != 2){
		cout << "errato numero di parametri/n inserire il path come primo parametro" << endl;
		exit(0);
	}
	// Open Database
	cout << "Opening MyDb.db ..." << endl;

	result=db1.OpenSQlite_db();
	if (result != 0){
		cout << "errore in apertura db, chiusura in corso" << endl;
		exit(1);
	}

	// Execute SQL
	cout << "Creating MyTable ..." << endl;
	//const char *sqlCreateTable = "CREATE TABLE MyTable (id INTEGER PRIMARY KEY, value STRING);";
	const char *sqlCreateTable = "CREATE TABLE Files3 (id STRING PRIMARY KEY, value DOUBLE);";
	result=db1.CreateTable(sqlCreateTable);
	if (result != 0){
		cout << "tabella già esistente" << endl;
	}


	// Execute SQL -> eseguite nella LS
	cout << "inserimento di prova in Files3" << endl;
	//const char *sqlInsert = "INSERT INTO MyTable VALUES(NULL, 'A Value');";
	const char *sqlInsert = "INSERT INTO Files3 VALUES('file_prova2', '1678');";
	result=db1.ExecuteSQl(sqlInsert);
	if (result != 0){
		cout << "errore in esecuzione sql, chiusura in corso" << endl;
	}

	// Display MyTable
	cout << "Retrieving values in Files3 ..." << endl;
	const char *sqlSelect = "SELECT * FROM Files3;";
	db1.DisplayTable(sqlSelect);

	//INSERISCO LS
	wstring cartella;
	cartella = argv[1]; //inserire il path
	//durante la LS, ogni file sarà inserito nel DB con nome(chiave primaria), dimensione
	search(cartella);

	// Display MyTable
	cout << "Retrieving values in Files3 ..." << endl;
	const char *sqlSelect2 = "SELECT * FROM Files3;";
	db1.DisplayTable(sqlSelect2);

	// Close Database
	db1.CloseSQlite_db();

	// Wait For User To Close Program
	cout << "Please press any key to exit the program ..." << endl;
	cin.get();
	return 0;
}
//NON FUNZIONA BENE LA RICERCA ricorsiva IN SOTTOCARTELLE, 
//ho usato una cartella senza sottocartelle
int search(wstring dir){
	WIN32_FIND_DATA ffd;
	LARGE_INTEGER filesize;
	//aggiunti
	FILETIME ftLastWriteTime;
	SYSTEMTIME stUTC, stLocal;
	DWORD dwRet;
	LPTSTR  pszDest= new TCHAR[100];
	//
	TCHAR szDir[MAX_PATH];
	HANDLE hFind = INVALID_HANDLE_VALUE;
	DWORD dwError = 0;
	wstring rec_path(dir);
	
	//   _tprintf(TEXT("\nTarget directory is %s\n\n"), dir);

	StringCchCopy(szDir, MAX_PATH, dir.c_str());  //dir deve essere 
	StringCchCat(szDir, MAX_PATH, TEXT("\\*"));
	wcout << "cerco nella cartella " << dir << endl;
	hFind = FindFirstFile(szDir, &ffd);

	if (INVALID_HANDLE_VALUE == hFind)
	{
		DisplayErrorBox(TEXT("errore in FindFirstFile"));
		return (-1);
	}
	
	do
	{
		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			wstring fold(ffd.cFileName);
			if ((wcscmp(ffd.cFileName, wstring(L".").c_str()) != 0) && (wcscmp(ffd.cFileName, wstring(L"..").c_str()) != 0))
				//non considerare direttorio corrente e padre
			{
				_tprintf(TEXT("  %s   <dir>\n"), ffd.cFileName);
				//è un direttorio, ricorro
				rec_path.append(L"/");
				rec_path.append(fold);
				search(rec_path);
			}
		}

		else
		{
			//un file
			filesize.LowPart = ffd.nFileSizeLow;
			filesize.HighPart = ffd.nFileSizeHigh;
			
			//aggiunto
			ftLastWriteTime=ffd.ftLastWriteTime;
			// Convert the last-write time to local time.
			FileTimeToSystemTime(&ftLastWriteTime, &stUTC);
			SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);
			// Build a string showing the date and time.
			
			dwRet = StringCchPrintf(pszDest, STRSAFE_MAX_CCH, TEXT("%02d/%02d/%d  %02d:%02d"),
				stLocal.wMonth, stLocal.wDay, stLocal.wYear,
				stLocal.wHour, stLocal.wMinute);

		_tprintf(TEXT("  %s   %ld  bytes \n"), ffd.cFileName, filesize.QuadPart);
		//funzione che costruisce la query per l' inserimento
		char* query = build_insert_query(ffd, filesize);
		cout << query << endl;
		//inserimento nel db
		db1.ExecuteSQl(query);
		}
	} while (FindNextFile(hFind, &ffd) != 0);

	dwError = GetLastError();
	if (dwError != ERROR_NO_MORE_FILES)
	{
		DisplayErrorBox(TEXT("FindFirstFile"));
	}

	FindClose(hFind);
	return 0;
}

char* build_insert_query(WIN32_FIND_DATA ffd, LARGE_INTEGER filesize){

	//conversione ffd.cfilename in stringa
	std::wstring arr_w(ffd.cFileName);
	std::string arr_s(arr_w.begin(), arr_w.end()); //nome del file in formato string
	//conversione filesize.QuadPart in stringa da long long
	stringstream size;
	size << filesize.QuadPart;

	//costruisco la stringa per la query
	string query_p("INSERT INTO Files3 VALUES('");
	query_p += arr_s;
	query_p += "','";
	query_p += size.str();
	query_p += "');";
	//copia di string in un char* , da inserire nel DB
	char* query = new char[query_p.length() + 1];
	strcpy_s(query, query_p.length() + 1, query_p.c_str());

	return query;
}

void DisplayErrorBox(LPTSTR lpszFunction)
{
	// Retrieve the system error message for the last-error code

	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;
	DWORD dw = GetLastError();

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0, NULL);

	// Display the error message and clean up

	lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
		(lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40)*sizeof(TCHAR));
	StringCchPrintf((LPTSTR)lpDisplayBuf,
		LocalSize(lpDisplayBuf) / sizeof(TCHAR),
		TEXT("%s failed with error %d: %s"),
		lpszFunction, dw, lpMsgBuf);
	MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);
}

