/******************************************************************************
This source file is part of the project
HyperSheetsPreprocessor (HSPP) - Optimizer and minifier for CSS
Copyright (C) 2019 Waldemar Zimpel <hspp@utilizer.de>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <https://www.gnu.org/licenses/>.
******************************************************************************/

#ifndef MAIN_H
#define MAIN_H
#include "Console.h"
#include "Help.h"
#include "config/Config.h"
#include "config/ConfigFile.h"
#include "config/ConfigFile.h"
#include "filesystem/FileSystemWorker.h"
#include <chrono>
#include <string>

using namespace std;

extern inline bool RETURN [[noreturn]] (const string &message);
//inline bool RETURN [[noreturn]] (const string &message);
inline bool isSet(const string &arg);
void prepare(DataContainer<pair<const string, const string> > &args);
bool parseArguments(DataContainer<string> &, DataContainer<pair<const string, const string> > &);
bool isSupportedArgument(const string &arg);

#ifdef WIN
#define TIME_UNIT "us"
#else
#define TIME_UNIT "μs"
#endif

#endif // MAIN_H
