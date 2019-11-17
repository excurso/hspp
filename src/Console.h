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
*******************************************************************************/

#ifndef CONSOLE_H
#define CONSOLE_H
#include <iostream>
#include <sstream>
#include "defs.h"
#include "String.h"

using namespace std;

class Console
{
public:
    Console();

    static void
    write(const string &message),
    writeLine(const string &message, const string &line_prefix = string()),
    writeFileSize(const int64_t file_size),
    writeFileSizeDifference(const int64_t input_size, const int64_t output_size, const string &line_prefix = string());

    static const string
    fileSizeFormat(const int64_t &file_size);
};

#endif // CONSOLE_H
