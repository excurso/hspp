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

#ifndef FILESYSTEMWORKER_H
#define FILESYSTEMWORKER_H
#include "../Console.h"
#include "../css/minifier/CssMinifier.h"
#include <iomanip>

class FileSystemWorker
{
public:
    static bool
    hasFileExtensionOf(const string &file_path, const DataContainer<string> &extension_list),
    processFile(const string &input_path, const string &output_path);

    static void
    process(),
    process(const string &input_path),

    addInputFile(const string &input_file_path),
    addOutputFile(const string &output_file_path),

    createPath(const string &path),
    readFile(const string &path, string &content),
    writeFile(const string &path, const string &content,
              ofstream::openmode mode = ios::out | ios::trunc);
};

#endif // FILESYSTEMWORKER_H
