/*
 * Author: Angelo Prudentino
 * Date: 30/09/2015
 * File: Utility.cpp
 * Description: utility functions for both client and server
 *
 */

#include <iostream>
#include <string>
#include <stdio.h>
#include <time.h>

#include "Utility.h"


// Get current date/time, format is [YYYY-MM-DD HH:mm:ss]
std::string currentDateTime() {
	time_t now = time(0);
	struct tm tstruct;
	char buf[80];
	tstruct = *localtime(&now);

	strftime(buf, sizeof(buf), "[%Y-%m-%d %X]", &tstruct);
	return buf;
}
