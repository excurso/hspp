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

#ifndef GENERALPARSER_H
#define GENERALPARSER_H
#include "../tokenizer/GeneralTokenizer.h"
#include <stack>

namespace General {
namespace Parsing {
using namespace std;
using namespace General::Tokenization;
using namespace General::Tokenization::Tokens;

class GeneralParser
{
public:
	GeneralParser(GeneralParser &) = delete;
	GeneralParser(const GeneralParser &) = delete;
	GeneralParser(GeneralParser &&) = delete;
	GeneralParser(const GeneralParser &&) = delete;

	GeneralParser &operator=(GeneralParser &) = delete;
	GeneralParser &operator=(const GeneralParser &) = delete;
	GeneralParser &operator=(GeneralParser &&) = delete;
	GeneralParser &operator=(const GeneralParser &&) = delete;

	explicit
    GeneralParser(GeneralTokenStreamPtr token_stream);
    virtual ~GeneralParser() = default;

protected:
    inline const GeneralTokenStreamPtr
    tokenStream() const;

	inline bool
    advance(const int64_t count = 1) const;

    inline const GeneralTokenPtr
    prevToken() const,
    currentToken(const int64_t count = 0) const,
    nextToken() const;

    inline const GeneralTokenStream::iterator
    getIterator() const;

	inline void
    setIterator(const GeneralTokenStream::iterator iterator),
    rememberPosition(),
    resetPosition(),
    popPosition();

	[[noreturn]] virtual void
	throwParseError(const string &message) const = 0;

private:
	const GeneralTokenStreamPtr m_token_stream;
    stack<GeneralTokenStream::iterator> m_position_stack;
    mutable GeneralTokenStream::iterator m_iterator;
};

inline const GeneralTokenStreamPtr
GeneralParser::
tokenStream() const
{
    return m_token_stream;
}

inline bool
GeneralParser::
advance(const int64_t count) const
{
	m_iterator += count;
    return true;
}

inline const GeneralTokenPtr
GeneralParser::
prevToken() const
{
    return *(m_iterator-1);
}

inline const GeneralTokenPtr
GeneralParser::
currentToken(int64_t count) const
{
    return *(m_iterator+count);
}

inline const GeneralTokenPtr
GeneralParser::
nextToken() const
{
    return *(m_iterator+1);
}

inline void
GeneralParser::
setIterator(const GeneralTokenStream::iterator iterator)
{
    m_iterator = iterator;
}

inline const GeneralTokenStream::iterator
GeneralParser::
getIterator() const
{
    return m_iterator;
}

inline void
GeneralParser::
rememberPosition()
{
    m_position_stack.push(m_iterator);
}

inline void
GeneralParser::
resetPosition()
{
    m_iterator = m_position_stack.top();
    m_position_stack.pop();
}

inline void
GeneralParser::
popPosition()
{
    m_position_stack.pop();
}

} // namespace Parsing
} // namespace General

#endif // GENERALPARSER_H
