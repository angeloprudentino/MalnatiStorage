/*
 * Author: Angelo Prudentino
 * Date: 20/07/2016
 * File: FileSystemManager.h
 * Description: this file contains all classes implementing
 *              filesystem read and write operations to manage
 *              files uploaded by users
 *
 */
#pragma once

#include <boost/filesystem.hpp>
#include "Utility.h"

#define STORAGE_ROOT_PATH "C:\\StoragePoint\\"

using namespace std;
using namespace boost::filesystem;


//////////////////////////////////////
//       TFileSystemManager	        //
//////////////////////////////////////
public class TFileSystemManager{
private:

public:
	TFileSystemManager();
	~TFileSystemManager();

	void storeFile(const path& aPath, string_ptr& aFileContent);
};