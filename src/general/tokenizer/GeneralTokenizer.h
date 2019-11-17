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

#ifndef GENERALTOKENIZER_H
#define GENERALTOKENIZER_H
#include "../../String.h"
#include "../../config/Config.h"
#include "elements/GeneralToken.h"
#include <memory>

namespace General {
namespace Tokenization {
using namespace General::Tokenization::Tokens;
using GeneralTokenStream    = DataContainer<GeneralTokenPtr>;
using GeneralTokenStreamPtr = shared_ptr<GeneralTokenStream>;

class GeneralTokenizer
{
public:
    GeneralTokenizer(GeneralTokenizer &) = delete;
    GeneralTokenizer(const GeneralTokenizer &) = delete;
    GeneralTokenizer(GeneralTokenizer &&) = delete;
    GeneralTokenizer(const GeneralTokenizer &&) = delete;

    explicit
    GeneralTokenizer(shared_ptr<string> content),
    GeneralTokenizer(const string &content, const uint64_t begin_row = 1, const uint64_t begin_column = 1);

    ~GeneralTokenizer() = default;

    GeneralTokenizer &operator=(GeneralTokenizer &) = delete;
    GeneralTokenizer &operator=(const GeneralTokenizer &) = delete;
    GeneralTokenizer &operator=(GeneralTokenizer &&) = delete;
    GeneralTokenizer &operator=(const GeneralTokenizer &&) = delete;

protected:
    inline bool
    isOneOfChars(const string &allowed) const,
    isSpaceChar() const noexcept;

    void
    skipSpace() const noexcept;

    inline bool
    isLineTerminator() const,
    isTab() const;

    bool
    isComment               (const string &comment_start_identifier,
                             const string &comment_end_identifier,
                             string &comment) const,

    isString                (string &str) const,
    isTerm                  (string *str = nullptr) const,

    isCharOfRange           (char from_char, char to_char) const,

    posStartsWith           (const string &s,
                             const bool case_insensitive = false) const,
    posStartsWith           (const DataContainer<string> &string_list,
                             const bool case_insensitive = false) const,
    isOneOfChars            (const string::iterator iter, const string &allowed) const;

    String
    readCharSequence        (const string &not_allowed_chars) const;

    inline void
    appendToken(const GeneralTokenPtr &token),
    appendToken(const GeneralTokenPtr &token, const uint64_t row, const uint64_t column),
    setIterator(const string::iterator iterator) const;

    inline char
    currentChar() const,
    nextChar() const,
    prevChar() const;

	inline const string::iterator
    getIterator(int64_t count = 0) const;

    inline bool
    advance(int64_t count = 1) const,
    currentChar(const char c) const,
    currentChar(const initializer_list<char> chars) const,
    nextChar(const char c) const,
    nextChar(const initializer_list<char> chars) const,
    prevChar(const char c) const,
    isEof(const uint64_t count = 0) const;

    inline uint64_t
    currentRow() const,
    currentColumn() const;

    inline const shared_ptr<string>
    byteStream() const;

    inline const GeneralTokenStreamPtr
    tokenStream() const;

    [[noreturn]] void
    throwSyntaxError(const string &message = "");

private:
	GeneralTokenStreamPtr       m_token_stream;
	shared_ptr<string>			m_content;

    mutable uint64_t			m_row, m_column;
    mutable string::iterator	m_iterator, m_row_begin;
};

inline void
GeneralTokenizer::
appendToken(const GeneralTokenPtr &token)
{
    token->setRow(m_row);
    token->setColumn(m_column - token->content().length() + 1);
    tokenStream()->emplace_back(token);
}

inline void
GeneralTokenizer::
appendToken(const GeneralTokenPtr &token, const uint64_t row, const uint64_t column)
{
    token->setRow(row);
    token->setColumn(column);
    tokenStream()->emplace_back(token);
}

inline bool
GeneralTokenizer::
advance(int64_t count) const
{
    const auto isUtf8MultibyteChar = [&]() -> bool {
        uint8_t char_count = 0;

        if ((currentChar() & 0x80) == 0x80) {
            // 4 byte char
            if ((currentChar() & 0xf0) == 0xf0) {
                char_count = 4;
            }
            // 3 byte char
            else if ((currentChar() & 0xe0) == 0xe0 && (currentChar() & 0x10) != 0x10) {
                char_count = 3;
            }
            // 2 byte char
            else if ((currentChar() & 0xc0) == 0xc0 && (currentChar() & 0x20) != 0x20) {
                char_count = 2;
            }
        }

        if (bool(char_count)) {
            const auto end = getIterator()+char_count;
            while (!isEof() && getIterator() != end) ++m_iterator;
            ++m_column;
            return true;
        }

        return false;
    };

    if ((count < 0 && getIterator(+count) < byteStream()->begin()) || getIterator(+count) > byteStream()->end()) return false;

    if (count > 0) {
        auto end = getIterator(+count);

        while (getIterator() < end) {
            if (isUtf8MultibyteChar())
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

inline char
GeneralTokenizer::
currentChar() const
{
    return *getIterator();
}

inline char
GeneralTokenizer::
nextChar() const
{
    return *getIterator(+1);
}

inline char
GeneralTokenizer::
prevChar() const
{
    return *getIterator(-1);
}

inline bool
GeneralTokenizer::
currentChar(const char c) const
{
    return *getIterator() == c;
}

inline bool
GeneralTokenizer::
currentChar(const initializer_list<char> chars) const
{
    for (const auto &c : chars) if (currentChar(c)) return true;

    return false;
}

inline bool
GeneralTokenizer::
nextChar(const char c) const
{
    return *getIterator(+1) == c;
}

inline bool
GeneralTokenizer::
nextChar(const initializer_list<char> chars) const
{
    for (const auto &c : chars)
        if (nextChar(c)) return true;

    return false;
}

inline bool
GeneralTokenizer::
prevChar(const char c) const
{
    return *getIterator(-1) == c;
}

inline void
GeneralTokenizer::
setIterator(const string::iterator iterator) const
{
    m_iterator = iterator;
}

inline const string::iterator
GeneralTokenizer::
getIterator(int64_t count) const
{
    return m_iterator+count;
}

inline bool
GeneralTokenizer::
isOneOfChars(const string &allowed) const
{
    return std::find(allowed.begin(), allowed.end(), currentChar()) != allowed.end();
}

inline bool
GeneralTokenizer::
isSpaceChar() const noexcept
{
    return bool(isspace(currentChar()));
}

inline bool
GeneralTokenizer::
isLineTerminator() const
{
    if (currentChar('\n')) {
        ++m_row; m_column = 1;
        return true;
    }
    return false;
}

inline bool
GeneralTokenizer::
isTab() const
{
    return currentChar('\t') && bool(m_column += cfg.tabWidth());
}

inline bool
GeneralTokenizer::
isEof(const uint64_t count) const
{
    return getIterator(+int64_t(count)) == byteStream()->end();
}

inline uint64_t
GeneralTokenizer::
currentRow() const
{
    return m_row;
}

inline uint64_t
GeneralTokenizer::
currentColumn() const
{
    return m_column;
}

inline const shared_ptr<string>
GeneralTokenizer::
byteStream() const
{
    return m_content;
}

inline const GeneralTokenStreamPtr
GeneralTokenizer::
tokenStream() const
{
    return m_token_stream;
}

} // namespace Tokenization
} // namespace General

#endif // GENERALTOKENIZER_H
