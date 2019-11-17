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

#include "String.h"

String::
String(const string &s) : string(s) {}

String::
String(const string *&s) : string(*s) {}

String::
String(const char chr) : string(&chr) {}

bool
String::
contains(const DataContainer<string> &string_list, const bool case_insensitive)
{
    if (case_insensitive)
        for (const auto &term : string_list) {
            if (search(begin(), end(), term.begin(), term.end(),
            [](const char a, const char b) {
                  return tolower(a) == tolower(b);
            }) != end()) return true;
        }
    else
        for (const auto &term : string_list)
            if (find(term) != npos)
                return true;

    return false;
}

bool
String::
contains(const initializer_list<string> string_list) const
{
    for (const auto &term : string_list)
        if (find(term) != npos) return true;

    return false;
}

bool
String::
contains(const string &s) const
{
    return find(s) != npos;
}

bool
String::
contains(const string &s, uint64_t *first_pos, uint64_t *last_pos) const
{
    uint64_t pos = 0;
    pos = find(s);

    if (pos != npos) {
        if (first_pos != nullptr) {
            *first_pos = pos;
        }

        if (last_pos != nullptr) {
            pos = find_last_of(s);
            *last_pos = pos;
        }
    }

    return pos != npos;
}

/*static*/ bool
String::
contains(const string &content, const char c)
{
    return content.find(c) != npos;
}

bool
String::
containsOneOfChars(const string &allowed_chars) const
{
    return std::find_first_of(begin(), end(), allowed_chars.begin(),
                              allowed_chars.end()) != end();
}

String
String::
left(const uint64_t count) const
{
    return String(substr(0, count));
}

String
String::
mid(const uint64_t start_pos, const uint64_t length) const
{
    return String(substr(start_pos, length));
}

String
String::
right(const uint64_t count) const
{
    return String(substr(length() - count, count));
}

bool
String::
startsWith(const string &s) const
{
    if (length() >= s.length())
        return left(s.length()) == s;

    return false;
}

bool
String::
posStartsWith(uint64_t index, const String &s, const bool case_insensitive) const
{
    if (case_insensitive)
        return s.length() < length() - s.length() && mid(index, s.length()).toLower() == s.toLower();

    return s.length() < length() - s.length() && substr(index, s.length()) == s;
}

bool
String::
endsWith(const string &s) const
{
    if (length() >= s.length())
        return right(s.length()) == s;

    return false;
}

/*static*/ bool
String::
startsWith(const string &content, const initializer_list<string> candidates)
{
    for (const auto &candidate : candidates)
        if (content.length() >= candidate.length())
            if (content.substr(0, candidate.length()) == candidate)
                return true;

    return false;
}

/*static*/ bool
String::
endsWith(const string &content, const initializer_list<string> candidates)
{
    for (const auto &candidate : candidates)
        if (content.length() >= candidate.length())
            if (content.substr(content.length() - candidate.length()) == candidate)
                return true;

    return false;
}

/*static*/ DataContainer<string>
String::
split(const string &content, const char delimiter)
{
    if (content.empty()) return DataContainer<string>();

    DataContainer<string> str_list;

    auto itr = content.begin();
    while (bool(isspace(*itr))) ++itr;

    for (auto begin = itr; begin != content.end(); ++itr) {
        if (*itr == delimiter) {
            str_list.emplace_back(string(begin, itr));
            do ++itr; while (bool(isspace(*itr)));
            begin = itr;
            continue;
        }

        if (itr == content.end()) {
            str_list.emplace_back(string(begin, itr));
            break;
        }
    }

    return str_list;
}

/*static*/ DataContainer<string>
String::
splitByWhiteSpace(const string &content)
{
    if (content.empty() || containsWhitepaceOnly(content)) return DataContainer<string>();

    DataContainer<string> str_list;

    auto itr = content.begin();
    while (bool(isspace(*itr))) ++itr;

    for (auto begin = itr; begin != content.end(); ++itr) {
        if (bool(isspace(*itr))) {
            str_list.emplace_back(string(begin, itr));
            do ++itr; while (bool(isspace(*itr)));
            begin = itr;
            continue;
        }

        if (itr == content.end()) {
            str_list.emplace_back(string(begin, itr));
            break;
        }
    }

    return str_list;
}

DataContainer<string>
String::
split(const string &separator)
{
    if (empty() || containsWhitepaceOnly()) return DataContainer<string>();

    DataContainer<string> str_list;
    iterator begin = this->begin(), end;

    do {
        end = std::find(begin, this->end(), *separator.data());
        str_list.emplace_back(string(begin, end));
        begin = end+1;
    } while (end != this->end());

    return str_list;
}

void
String::
ltrim()
{
    while (bool(isspace(front()))) erase(begin());
}

void
String::
rtrim()
{
    while (bool(isspace(back()))) pop_back();
}

void
String::
trim()
{
    ltrim(); rtrim();
}

/*static*/ string
String::
trim(const string &content)
{
    string trimmed_content = content;

    while (bool(isspace(trimmed_content.back()))) trimmed_content.pop_back();
    while (bool(isspace(trimmed_content.front()))) trimmed_content.erase(trimmed_content.begin());

    return trimmed_content;
}

/*static*/ string
String::
collapseWhiteSpace(const string &content)
{
    string str;

    for (const auto &c : content) {
        if (bool(isspace(c)) && !bool(isspace(str.back()))) { str += ' '; continue; }
        str += c;
    }

    return str;
}

uint64_t
String::
indexOf(const string &substr, const uint64_t start_pos, uint64_t end_pos) const
{
    end_pos = end_pos == npos ? length() : end_pos - substr.length();

    for (uint64_t i = start_pos; i < end_pos; ++i)
        if (this->substr(i, substr.length()) == substr)
            return i;

    return npos;
}

uint64_t
String::
lastIndexOf(const string &substr, const uint64_t start_pos, uint64_t end_pos) const
{
    end_pos = end_pos == npos ? length() : end_pos - substr.length();

    for (uint64_t i = end_pos; i > start_pos; --i)
        if (this->substr(i, substr.length()) == substr)
            return i;

    return npos;
}

bool
String::
isCharOfRange(iterator itr, char from_char, char to_char) const
{
    return *itr >= from_char && *itr <= to_char;
}

bool
String::
isOneOfChars(char c, const string &allowed)
{
    return std::find(allowed.begin(), allowed.end(), c) != allowed.end();
}

bool
String::
isOneOfChars(string::iterator itr, const string &allowed) const
{
    return std::find(allowed.begin(), allowed.end(), *itr) != allowed.end();
}

bool
String::
isOneOfChars(const string &allowed, String::iterator itr)
{
    return std::find(allowed.begin(), allowed.end(), *itr) != allowed.end();
}

bool
String::
isSpaceChar(String::iterator itr) const
{
    return bool(isspace(*itr));
}

String
String::
readCharSequence(iterator &itr, const string &not_allowed_chars)
{
    const auto begin = itr,
    end = find_if(begin, this->end(), [&](char c) {
        return std::find(not_allowed_chars.begin(), not_allowed_chars.end(), c) != not_allowed_chars.end();
    });

    itr = end;
    return String(begin, end);
}

void
String::
skipSpace(iterator &itr) const
{
    while (itr != end() && bool(isspace(*itr))) ++itr;
}

String
String::
repeatChar(const char chr, const uint64_t num)
{
    String result;

    for (uint64_t i = 0; i < num; ++i)
        result += chr;

    return result;
}

String
String::
repeat(const string &str, const uint64_t num)
{
    String result;

    for (uint64_t i = 0; i < num; ++i)
        result += str;

    return result;
}

bool
String::
containsOnly(const string &allowed_chars) const
{
    return all_of(begin(), end(), [&allowed_chars](char c) {
        return std::find(allowed_chars.begin(), allowed_chars.end(), c) != allowed_chars.end();
    });
}

const string
String::
numberFormat(double number, const uint32_t precition)
{
    const double prec = pow(10, precition);
    number = round(number * prec) / prec;

    ostringstream ostrstr;
    ostrstr.setf(ostringstream::fixed);
    ostrstr.precision(precition);
    ostrstr << number;

    return ostrstr.str();
}

const String String::toLower() const
{
    String str = *this;

    if (!all_of(str.begin(), str.end(), ::islower))
        std::transform(str.begin(), str.end(), str.begin(), ::tolower);

    return str;
}

/*static*/ string
String::
toLower(const string &content)
{
    string lc_content = content;

    if (!all_of(lc_content.begin(), lc_content.end(), ::islower)) {
        std::transform(lc_content.begin(), lc_content.end(), lc_content.begin(), ::tolower);
    }

    return lc_content;
}

const String
String::
toLower(const String &str)
{
    return str.toLower();
}

const String
String::
toUpper() const
{
    String str = *this;

    std::transform(str.begin(), str.end(), str.begin(), ::toupper);

    return str;
}

const String
String::
toUpper(const String &str)
{
    return str.toUpper();
}

const String
String::
toLowerUtf8(const String &str)
{
    return str.toLowerUtf8();
}

const String
String::
toLowerUtf8() const
{
    const locale utf8("en_US.UTF-8");

    wstring_convert<codecvt_utf8<wchar_t> > conv;
    wstring wstr = conv.from_bytes(*this);

    for (wchar_t &wchr : wstr)
    {
        wchr = std::tolower(wchr, utf8);
    }

    return String(conv.to_bytes(wstr));
}

const String
String::
toUpperUtf8(const String &str)
{
    return str.toUpperUtf8();
}

const String
String::
toUpperUtf8() const
{
    const locale utf8("en_US.UTF-8");

    wstring_convert<codecvt_utf8<wchar_t>> conv;
    wstring wstr = conv.from_bytes(*this);

    for (wchar_t &wchr : wstr)
    {
        wchr = std::toupper(wchr, utf8);
    }

    return String(conv.to_bytes(wstr));
}
