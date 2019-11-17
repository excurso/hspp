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

#ifndef FILESYSTEM_H
#define FILESYSTEM_H
#include "../String.h"
#include <algorithm>
#include <cstring>
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <memory>
#include <sys/stat.h>
#include <unistd.h>

class FileSystem
{
public:
    static DataContainer<string>
    getDirectoryContents(const string &path);

    static inline bool
    exists              (const string &path),
    isReadable          (const string &path),
    isWritable          (const string &path),
    isFile              (const string &path),
    isDir               (const string &path),
    createDirectory     (const string &path),
	isEmptyDir          (const string &path),
    deleteFile			(const string &path),
    isAbsolutePath      (const string &path);

    static bool
    createPath          (string path, string &fail_path),
    copyFile            (const String &source_path, const String &target_path),
    readFile            (const string &path, string &content),
    writeFile           (const string &path, const string &content,
                         ofstream::openmode mode = ios::out | ios::trunc),
    isRemoteAddress(const string &addr);

    static inline const string
    getBaseName         (string path),
    getParentPath       (const string &path),
    getFileExtension    (string path),
    getRelativePath     (const string &path1, const string &path2);

    static const string
    getCleanPath        (string path),
    getRelativePath2    (string path1, string path2);

    static inline int64_t
    getFileSize         (const string &file_path);
};

/*static*/ inline bool
FileSystem::
isFile(const string &path)
{
#ifdef DEBUG
    errno = 0;
#endif

    struct stat path_stat {};
    stat(path.data(), &path_stat);

#ifdef DEBUG
    if (errno != 0) {
        perror("FileSystem::isFile()");
        cout << path << endl;
    }
#endif

    return S_ISREG(path_stat.st_mode) != 0;
}

/*static*/ inline bool
FileSystem::
isDir(const string &path)
{
#ifdef DEBUG
    errno = 0;
#endif

    struct stat path_stat {};
    stat(path.data(), &path_stat);

#ifdef DEBUG
    if (errno != 0) {
        perror("FileSystem::isDir()");
    }
#endif

    const int res = S_ISDIR(path_stat.st_mode);
    return res != 0;
}

/*static*/ inline bool
FileSystem::
exists(const string &path)
{
    return access(path.data(), F_OK) == 0;
}

/*static*/ inline bool
FileSystem::
isReadable(const string &path)
{
    return access(path.data(), R_OK) == 0;
}

/*static*/ inline bool
FileSystem::
isWritable(const string &path)
{
    const auto &result = access(path.data(), W_OK);
    return result == 0;
}

/*static*/ inline const string
FileSystem::
getBaseName(string path)
{
    if (path.back() == DIR_SEP[0]) path.erase(path.end()-1);
    return string(find(path.rbegin(), path.rend(), DIR_SEP[0]).base(), path.end());
}

/*static*/ inline const string
FileSystem::
getParentPath(const string &path)
{
    return string(path.begin(), find(path.rbegin(), path.rend(), DIR_SEP[0]).base()-1);
}

/*static*/ inline const string
FileSystem::
getRelativePath(const string &path1, const string &path2)
{
    for (auto itr1 = path1.begin(), itr2 = path2.begin();
         itr1 != path1.end() || itr2 != path2.end(); ++itr1, ++itr2) {
        if (*itr1 == *itr2) continue;

        if (*itr1 == DIR_SEP[0]) ++itr1;
        return string(itr1, path1.end());
    }

    return string();
}

/*static*/ inline const string
FileSystem::
getFileExtension(string path)
{
    path = getBaseName(path);

    return string(find(path.begin(), path.end(), '.'), path.end());
}

/*static*/ inline bool
FileSystem::
createDirectory(const string &path)
{
#ifdef WIN
    return mkdir(path.data()) == 0;
#else
    return mkdir(path.data(), 0755) == 0;
#endif
}

/*static*/ inline bool
FileSystem::
isEmptyDir(const string &path)
{
    char cnt = 0;
    DIR *dir = opendir(path.data());

    while (bool(readdir(dir))) {
        if (++cnt > 2) {
            closedir(dir);
            return false;
        }
    }

    return true;
}

/*static*/ inline bool
FileSystem::
deleteFile(const string &path)
{
	return remove(path.c_str()) == 0;
}

/*static*/ inline bool
FileSystem::
isAbsolutePath(const string &path)
{
#ifdef WIN
    return !path.empty() && isalpha(path.front()) && path.at(1) == ':';
#else
    return !path.empty() && path.front() == DIR_SEP[0];
#endif
}

/*static*/ inline int64_t
FileSystem::
getFileSize(const string &file_path)
{
#ifdef DEBUG
    errno = 0;
#endif

    struct stat st {};

    if (stat(file_path.data(), &st) == 0) {

#ifdef DEBUG
    if (errno != 0) {
        perror("FileSystem::getFileSize()");
    }
#endif

        return st.st_size;
    }

    return -1;
}

#endif // FILESYSTEM_H
