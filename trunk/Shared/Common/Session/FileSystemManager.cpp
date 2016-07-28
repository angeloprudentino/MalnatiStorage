/*
 * Author: Angelo Prudentino
 * Date: 20/07/2016
 * File: FileSystemManager.cpp
 * Description: this file contains all classes implementing
 *              filesystem read and write operations to manage
 *              files uploaded by users
 *
 */

#include "boost/filesystem/fstream.hpp"
#include "FileSystemManager.h"
#include <windows.h>

//////////////////////////////////////
//       TFileSystemManager	        //
//////////////////////////////////////
#pragma region "TFileSystemManager"
TFileSystemManager::TFileSystemManager(){

}

TFileSystemManager::~TFileSystemManager(){

}

void TFileSystemManager::storeFile(const path& aPath, string_ptr& aFileContent){
	path p = aPath.parent_path();
	boost::system::error_code ec;
	if (!exists(p))
		create_directories(p, ec);

	if (!ec){
		boost::filesystem::ofstream of(aPath.string(), ios::out | ios::binary);
		try{
			B64result ret = opensslB64Decode(*aFileContent);
			of.write(ret.data, ret.size);
		}
		catch (EOpensslException e){

		}
		of.close();
	}

	aFileContent.reset();
}
#pragma endregion