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

#ifndef STRING_H
#define STRING_H
#include "DataContainer.h"
#include "defs.h"
#include <algorithm>
#include <codecvt>
#include <ctgmath>
#include <locale>
#include <string>

#define SPACE_CHARS " \n\t\r\f\v"

//static constexpr array<char, 7> SPACE_CHARS = {" \n\t\r\f\v"};

class String : public string
{
public:
    using string::string;

	explicit
	String              (const string &s),
	String              (const string *&s),
	String              (const char chr);

    static bool
    isOneOfChars        (char c, const string &allowed),
    isOneOfChars        (const string &allowed, string::iterator itr),
    contains            (const string &content, const char c),
	startsWith          (const string &content, const initializer_list<string> candidates),
	endsWith            (const string &content, const initializer_list<string> candidates);

    static inline bool
    containsWhitepace(const string &content),
    containsWhitepaceOnly(const string &content);

    bool
    containsOneOfChars  (const string &allowed_chars) const,

    containsWhitepaceOnly() const,
    containsOnly        (const string &allowed_chars) const,
    startsWith          (const string &s) const,
    endsWith            (const string &s) const,
    isCharOfRange       (string::iterator itr, char from_char, char to_char) const,
    isOneOfChars        (string::iterator itr, const string &allowed) const,
    isSpaceChar         (string::iterator itr) const,
    contains            (const DataContainer<string> &string_list,
                         const bool case_insensitive = true),
	contains            (const initializer_list<string> string_list) const,
    contains            (const string &s) const,
    contains            (const string &s, uint64_t *first_pos = nullptr,
                         uint64_t *last_pos = nullptr) const,

    posStartsWith       (uint64_t index, const String &s,
                         const bool case_insensitive = false) const;

    static String
	repeatChar          (const char chr, const uint64_t num),
    repeat              (const string &str, const uint64_t num);

    static const String
    toLower             (const String &str),
    toUpper             (const String &str),
    toLowerUtf8         (const String &str),
    toUpperUtf8         (const String &str);

    String
	left                (const uint64_t count) const,
	mid                 (const uint64_t start_pos, const uint64_t length) const,
	right               (const uint64_t) const,
    readCharSequence    (string::iterator &itr, const string &not_allowed_chars);

    DataContainer<string>
    split               (const string &separator);

    static DataContainer<string>
	split               (const string &content, const char delimiter),
    splitByWhiteSpace   (const string &content);

    static string
    toLower             (const string &content),
    trim                (const string &content),
    collapseWhiteSpace  (const string &content);

    void
    ltrim(), rtrim(), trim(),
    skipSpace           (iterator &itr) const;

    uint64_t
    indexOf             (const string &substr, const uint64_t start_pos = 0,
                         uint64_t end_pos = npos) const,
    lastIndexOf         (const string &substr, const uint64_t start_pos = 0, uint64_t end_pos = npos) const;

    static const string
    numberFormat        (double number, const uint32_t precition);

    const String
    toLower             () const,
    toUpper             () const,
    toLowerUtf8         () const,
    toUpperUtf8         () const;
};

/*static*/ inline bool
String::
containsWhitepace(const string &content)
{
    return any_of(content.begin(), content.end(), ::isspace);
}

/*static*/ inline bool
String::
containsWhitepaceOnly(const string &content)
{
    return all_of(content.begin(), content.end(), ::isspace);
}

inline bool
String::
containsWhitepaceOnly() const
{
    return all_of(begin(), end(), ::isspace);
}

#endif // STRING_H
