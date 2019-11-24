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

#include "GeneralTokenizer.h"
using namespace General::Tokenization;

GeneralTokenizer::GeneralTokenizer(shared_ptr<string> content) :
    m_token_stream(make_shared<GeneralTokenStream>()),
    m_content(move(content)), m_row(1), m_column(1),
    m_iterator(m_content->begin()), m_row_begin(m_iterator) {}

GeneralTokenizer::GeneralTokenizer(const string &content, const uint64_t begin_row, const uint64_t begin_column) :
    m_token_stream(make_shared<GeneralTokenStream>()),
    m_content(make_shared<string>(content)),
    m_row(begin_row), m_column(begin_column),
    m_iterator(m_content->begin()), m_row_begin(m_iterator) {}

bool
GeneralTokenizer::
isComment(const string &comment_start_identifier, const string &comment_end_identifier, string &comment) const
{
    if (posStartsWith(comment_start_identifier)) {
        advance(+int64_t(comment_start_identifier.length()));
        const auto begin = getIterator();

        while (!isEof() && !(currentChar(comment_end_identifier.front()) &&
               posStartsWith(comment_end_identifier)) && advance());

        if (posStartsWith(comment_end_identifier)) {
            comment = string(begin, getIterator());
            advance(+int64_t(comment_end_identifier.length()));
            return true;
        }
    }

    return false;
}

bool
GeneralTokenizer::
isTerm(string *str) const
{
    if (bool(isalpha(currentChar()))) {
        string term;

        term = currentChar();
        advance();

        while (!isEof()) {
            if (bool(isalpha(currentChar())) || bool(isdigit(currentChar())) || currentChar('-') || currentChar('_')) {
                term += currentChar();
                advance();
            }
            else {
                if (term.empty()) break;
                if (str != nullptr) *str = term;

                return true;
            }
        }
    }

    return false;
}

void
GeneralTokenizer::
skipSpace() const noexcept
{
    while (!isEof() && isSpaceChar() && advance());
}

bool
GeneralTokenizer::
posStartsWith(const string &s, const bool case_insensitive) const
{
    if (m_content->length() > s.length()) {
        if (case_insensitive) {
            return search(getIterator(), getIterator(+std::distance(s.begin(), s.end())),
                s.begin(), s.end(), [](char ch1, char ch2) {
                return tolower(ch1) == tolower(ch2);}) == getIterator();
        }

        return search(getIterator(), getIterator(+std::distance(s.begin(), s.end())),
            s.begin(), s.end()) == getIterator();
    }

    return false;
}

bool
GeneralTokenizer::
posStartsWith(const DataContainer<string> &string_list, const bool case_insensitive) const
{
    for (const auto &s : string_list)
        if (posStartsWith(s, case_insensitive))
            return true;

    return false;
}

bool
GeneralTokenizer::
isString(string &str) const
{
    if (currentChar({'"', '\''})) {
        const auto begin = getIterator();

        do advance(); while (!isEof() && !currentChar(*begin));

        if (currentChar(*begin) && advance()) {
            str = string(begin, getIterator());
            return true;
        }
    }

    return false;
}

bool
GeneralTokenizer::
isOneOfChars(const string::iterator iter, const string &allowed) const
{
    return find(allowed.begin(), allowed.end(), *iter) != allowed.end();
}

bool
GeneralTokenizer::
isCharOfRange(char from_char, char to_char) const
{
    return currentChar() >= from_char && currentChar() <= to_char;
}

String
GeneralTokenizer::
readCharSequence(const string &not_allowed_chars) const
{
    string char_sequence;

    while (!isEof() && !isOneOfChars(not_allowed_chars)) {
        char_sequence += currentChar();
        advance();
    }

    return String(char_sequence);
}

bool
GeneralTokenizer::
advance(int64_t count) const
{
    if ((count < 0 && getIterator(+count) < byteStream()->begin()) || getIterator(+count) > byteStream()->end()) return false;

    if (count > 0) {
        auto end = getIterator(+count);

        while (getIterator() < end) {
            if (m_encoding == UTF8 && isUtf8MultibyteChar())
                continue;

            if (!isEof() && (isTab() || isLineTerminator() || bool(++m_column)))
                ++m_iterator;
        }
    } else if (count < 0) {
        if (getIterator()+count > byteStream()->begin()) m_iterator+=count;
        else return false;
    }

    return true;
}

void
GeneralTokenizer::
throwSyntaxError(const string &message)
{
    cout << "Syntax error: Unexpected character '"
         << currentChar()
         << "' on row "
         << m_row
         << " column "
         << m_column
         << NEWLINE
         << message
         << endl;

    exit(1);
}
