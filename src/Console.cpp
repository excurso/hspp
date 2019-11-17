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

#include "Console.h"

Console::Console() {}

/*static*/ void
Console::
write(const string &message)
{
    cout << message;
}

/*static*/ void
Console::
writeLine(const string &message, const string &line_prefix)
{
    cout << line_prefix << message << endl;
}

/*static*/ void
Console::
writeFileSize(const int64_t file_size)
{
    Console::writeLine("Size: " + fileSizeFormat(file_size) + " (" + to_string(file_size) + " bytes)");
}

/*static*/ void
Console::
writeFileSizeDifference(const int64_t input_size, const int64_t output_size, const string &line_prefix)
{
    const string
    input_size_1 = fileSizeFormat(input_size),
    output_size_1 = fileSizeFormat(output_size),
    input_size_2 = " (" + to_string(input_size) + " bytes)",
    output_size_2 = " (" + to_string(output_size) + " bytes)";

    cout << line_prefix
         << "Input  size: "
         << input_size_1
         << (input_size > 1e3 ? input_size_2 : "") << NEWLINE
         << line_prefix
         << "Output size: "
         << output_size_1
         << (output_size > 1e3 ? output_size_2 : "")
         << " [-" << to_string(input_size - output_size) << " bytes]"
         << " [-" << String::numberFormat((1 - double(output_size) / input_size) * 1e2, 2) << "%]"
         << NEWLINE << endl;
}

/*static*/ const string
Console::
fileSizeFormat(const int64_t &file_size)
{
    string unit = " bytes";
    double fs = file_size;

    if (file_size > 1e12) {
        unit = "TB";
        fs /= 1e12;
    }
    else if (file_size > 1e9) {
        unit = "GB";
        fs /= 1e9;
    }
    else if (file_size > 1e6) {
        unit = "MB";
        fs /= 1e6;
    }
    else if (file_size > 1e3) {
        unit = "kB";
        fs /= 1e3;
    }

    return stringstream(String::numberFormat(fs, 2) + unit).str();
}
