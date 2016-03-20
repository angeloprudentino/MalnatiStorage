/*
 * Author: Angelo Prudentino
 * Date: 30/09/2015
 * File: Utility.h
 * Description: utility functions for both client and server
 *
 */
#pragma once

#include <string>

#include "Message.h"

using namespace std;

#define EMPTY ""
#define TRUE_STR "true"
#define FALSE_STR "false"
#define DEFAULT_TCP_PORT 4700

// Get current date/time, format is [YYYY-MM-DD HH:mm:ss]
const string currentDateTime();

//Convert time_t to std::string and vice versa
const string timeToString(const time_t& t);
const time_t stringToTime(const string& s);
