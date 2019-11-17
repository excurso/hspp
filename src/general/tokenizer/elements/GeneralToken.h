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

#ifndef GENERALTOKEN_H
#define GENERALTOKEN_H
#include <memory>

#ifdef EOF
#undef EOF
#endif

namespace General {
namespace Tokenization {
namespace Tokens {

using namespace std;

class GeneralToken
{
public:
    GeneralToken(GeneralToken &) = delete;
    GeneralToken(const GeneralToken &) = delete;
    GeneralToken(GeneralToken &&) = delete;
    GeneralToken(const GeneralToken &&) = delete;

    GeneralToken &operator=(GeneralToken &) = delete;
    GeneralToken &operator=(const GeneralToken &) = delete;
    GeneralToken &operator=(GeneralToken &&) = delete;
    GeneralToken &operator=(const GeneralToken &&) = delete;

    explicit
    GeneralToken() = default;

    explicit
    GeneralToken(const char c),
    GeneralToken(string content);
    virtual ~GeneralToken() = default;

    inline void
    setContent(const string &content),
    setRow(const uint64_t row),
    setColumn(const uint64_t column);

    inline const string &
    content() const;

	inline bool
    hasContent(const string &content) const,
    hasContent(const initializer_list<string> content_list) const;

    inline uint64_t
    row() const, column() const;

private:
    uint64_t m_row {1}, m_column {1};
    string m_content;
};

inline void
GeneralToken::
setContent(const string &content)
{
	m_content.reserve(content.length());
    m_content = content;
}

inline const string &
GeneralToken::
content() const
{
    return m_content;
}

inline bool
GeneralToken::
hasContent(const string &content) const
{
	return m_content == content;
}

inline bool
GeneralToken::
hasContent(const initializer_list<string> content_list) const
{
	for (const auto &content : content_list)
		if (m_content == content) return true;

	return false;
}

inline void
GeneralToken::
setRow(const uint64_t row)
{
    m_row = row;
}

inline uint64_t
GeneralToken::
row() const
{
    return m_row;
}

inline void
GeneralToken::
setColumn(const uint64_t column)
{
    m_column = column;
}

inline uint64_t
GeneralToken::
column() const
{
    return m_column;
}

using GeneralTokenPtr = shared_ptr<GeneralToken>;

} // namespace Tokens
} // namespace Tokenization
} // namespace General

#endif // GENERALTOKEN_H
