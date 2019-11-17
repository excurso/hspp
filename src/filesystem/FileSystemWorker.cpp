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

#include "FileSystemWorker.h"
#include "../config/Config.h"

using namespace CSS::Minification;

static DataContainer<string> s_input_files, s_output_files;

/*static*/ void
FileSystemWorker::
process()
{
    FileSystemWorker::process(cfg.inputPath());
}

/*static*/ void
FileSystemWorker::
process(const string &input_path)
{
    int64_t input_size = 0, output_size = 0;

    !FileSystem::exists(input_path) &&
        RETURN("The specified input file does not exist.");

    !FileSystem::isFile(input_path) &&
        RETURN("The specified input is not a file.");

    FileSystem::getParentPath(cfg.inputPath()) == cfg.outputPath() &&
        RETURN("Input and output path must differ.");

    const string file_name = FileSystem::getBaseName(input_path);

    string output_file_path, relative_path;

    if (!cfg.inputWorkingDirectory().empty()) {
        relative_path = FileSystem::getRelativePath(input_path, cfg.inputWorkingDirectory());
        output_file_path = cfg.outputPath() + DIR_SEP + relative_path;
    } else {
        output_file_path = cfg.outputPath() + DIR_SEP + file_name;
    }

    string file_content;

    if (!cfg.isEnabled(Config::GENERAL__OUTPUT_TO_STDO)) {
        Console::writeLine("Processing input file '" + file_name + "'");
    }

    !processFile(input_path, output_file_path) &&
        RETURN("Unknown input file extension.");

    input_size += FileSystem::getFileSize(input_path);
    output_size += FileSystem::getFileSize(output_file_path);

    if (!cfg.isEnabled(Config::GENERAL__OUTPUT_TO_STDO)) {
        Console::writeLine("[Done] Processing input file '" + file_name + "'");

        if (!cfg.isEnabled(Config::CSS__INCLUDE_EXTERNAL_STYLESHEETS))
            Console::writeFileSizeDifference(input_size, output_size);
    }

    // Sum all input file sizes to get total input size
    for (const auto &input_file : s_input_files)
        input_size += FileSystem::getFileSize(input_file);

    // Sum all output file sizes to get total output size
    for (const auto &output_file : s_output_files)
        output_size += FileSystem::getFileSize(output_file);

    if (!cfg.isEnabled(Config::GENERAL__OUTPUT_TO_STDO)) {
        Console::writeLine(NEWLINE "All done!" DBLNEWLINE "Summary:");
        Console::writeFileSizeDifference(input_size, output_size);
    }
}

bool
FileSystemWorker::
hasFileExtensionOf(const string &file_path, const DataContainer<string> &extension_list)
{
    for (const auto &extension : extension_list)
        if (String(file_path).endsWith(extension))
            return true;

    return false;
}

bool
FileSystemWorker::
processFile(const string &input_path, const string &output_path)
{
    if (!cfg.isEnabled(Config::GENERAL__OUTPUT_TO_STDO)) {
        if (!FileSystem::exists(output_path)) {
            const auto path = FileSystem::getParentPath(output_path);
            createPath(path);
        }
    }

    // Create buffer for file contents
    auto file_content = make_shared<string>();

    if (hasFileExtensionOf(input_path, cfg.cssFileExtensions())) {
        if (FileSystem::readFile(input_path, *file_content)) {
            const auto input_size = file_content->length();
            // Set buffer to minified version of file content
            file_content = CssMinifier::minify(file_content);

            // If output gets longer than the input, remove header comment
            if (file_content->length() > input_size)
                *file_content = file_content->substr(sizeof(OUTPUT_FILE_HEADER));
        }
    } else return false;

    if (cfg.isEnabled(Config::GENERAL__OUTPUT_TO_STDO))
        Console::writeLine(*file_content);
    else
        writeFile(output_path, *file_content);

    return true;
}

/*static*/ void
FileSystemWorker::
addInputFile(const string &input_file_path)
{
    s_input_files.emplace_back(input_file_path);
}

/*static*/ void
FileSystemWorker::
addOutputFile(const string &output_file_path)
{
    s_output_files.emplace_back(output_file_path);
}

/*static*/ void
FileSystemWorker::
createPath(const string &path)
{
    if (!FileSystem::exists(path)) {
        string fail_path;
        Console::writeLine(NEWLINE "Creating path " NEWLINE + path + NEWLINE);
        FileSystem::createPath(path, fail_path);

        !fail_path.empty() &&
            RETURN(NEWLINE "Could not create path" NEWLINE + path + DBLNEWLINE
                   "The path" NEWLINE + fail_path + NEWLINE "is not writable." DBLNEWLINE
                   "Check permissions.");
    } else !FileSystem::isDir(path) &&
            RETURN(NEWLINE "Could not create path" NEWLINE + path + DBLNEWLINE
                   "'" + FileSystem::getBaseName(path) +
                   "' already exists but is not a directory.");
}

/*static*/ void
FileSystemWorker::
readFile(const string &path, string &content)
{
    !FileSystem::exists(path) &&
        RETURN("File" NEWLINE + path + NEWLINE "does not exist.");

    !FileSystem::isReadable(path) &&
        RETURN("File" NEWLINE + path + NEWLINE "is not readable." DBLNEWLINE "Check permissions.");

    !FileSystem::readFile(path, content) &&
        RETURN("Could not read file" NEWLINE + path);
}

/*static*/ void
FileSystemWorker::
writeFile(const string &path, const string &content, ofstream::openmode mode)
{
    !FileSystem::writeFile(path, content, mode) &&
        RETURN("Could not write file" NEWLINE + path + DBLNEWLINE "Check permissions.");
}
