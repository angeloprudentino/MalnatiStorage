/*
 * Author: Angelo Prudentino
 * Date: 30/09/2015
 * File: Utility.cpp
 * Description: utility functions for both client and server
 *
 */

#include <iostream>
#include <sstream>
#include <string>
#include <stdio.h>
#include <time.h>

#include "Utility.h"

using namespace std;

// Get current date/time, format is [YYYY-MM-DD HH:mm:ss]
const string currentDateTime() {
	time_t now = time(0);
	struct tm tstruct;
	char buf[80];
	tstruct = *localtime(&now);

	strftime(buf, sizeof(buf), "[%Y-%m-%d %X]", &tstruct);
	return buf;
}

//Convert time_t to std::string and vice versa
const string timeToString(const time_t& t) {
	std::ostringstream oss;
	oss << t;
	return oss.str();
}

const time_t stringToTime(const string& s) {
	istringstream stream(s);
	time_t t;
	stream >> t;
	return t;
}
