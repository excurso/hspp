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

#ifndef HELP_H
#define HELP_H
#include "defs.h"
#include "String.h"

#ifdef WIN
#define PARTITION "X:"
#else
#define PARTITION
#endif

#define APP_USAGE \
    "    " APP_NAME " -i " PARTITION DIR_SEP "path" DIR_SEP \
    "to" DIR_SEP "input" DIR_SEP "file -o " PARTITION DIR_SEP \
    "path" DIR_SEP "to" DIR_SEP "output" DIR_SEP "directory"

#define APP_USAGE_QUOTED \
    "    " APP_NAME " -i \"" PARTITION DIR_SEP "path to input" \
    DIR_SEP "file\" -o \"" PARTITION DIR_SEP "path to output" DIR_SEP "directory\""

#define APP_HEADER\
    NEWLINE LINE\
    APP_FULL_NAME " v" APP_VERSION "   (c) " APP_RELEASE_YEAR " " APP_AUTHOR NEWLINE\
    LINE NEWLINE

#define APP_LICENSE_HEADER\
    "This is free software, and you are welcome to redistribute" NEWLINE\
    "it under certain conditions. This application is published" NEWLINE\
    "under the " APP_LICENSE " license." NEWLINE\
    "See https://www.gnu.org/licenses/gpl-3.0 for license information." NEWLINE\
    "This program comes with ABSOLUTELY NO WARRANTY." DBLNEWLINE

#define INFO_ON_NO_ARGUMENTS\
    "Use '--help' to get usage information."

#define HELP_PAGE_CONTENT \
    "Usage:" NEWLINE\
    APP_USAGE DBLNEWLINE\
    "or if the paths contain whitespace:" NEWLINE\
    APP_USAGE_QUOTED DBLNEWLINE\
    "Options:" NEWLINE\
    "    --help                    Show this help" NEWLINE\
    "    --create-config-file      Create configuration file with" NEWLINE\
    "                              default settings" NEWLINE\
    "    --config-file             Set the configuration file path" NEWLINE\
    "    --config-info             Show current configuration" NEWLINE\
    "    --stdo                    Use standard output" NEWLINE\
    "                              instead of a file" DBLNEWLINE\
    "    -i                        Input file path" NEWLINE\
    "    -o                        Output directory path" DBLNEWLINE\
    "The input and output paths must differ." NEWLINE\
    "The full output path will be created, if it does not already exist." NEWLINE\
    "If the output file already exists, it will be overwritten." DBLNEWLINE\
    "IMPORTANT NOTE:" NEWLINE\
    "Make sure to perform a backup of your data, before performing" NEWLINE\
    "any processing, to prevent data loss." NEWLINE

#endif // HELP_H
