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

#ifndef CSSTOKEN_H
#define CSSTOKEN_H
#include "../../../general/tokenizer/elements/GeneralToken.h"

namespace CSS {
namespace Tokenization {
namespace Tokens {
using namespace General::Tokenization::Tokens;
using namespace std;

class CssToken final : public GeneralToken
{
public:
    enum CssTokenType : uint8_t {
        WHITESPACE, PUNCTUATOR, IDENTIFIER, AT_KEYWORD, STRING_LITERAL, HASH_LITERAL,
        NUMERIC_LITERAL, SCIENTIFIC_LITERAL, UNIT, COMMENT, CDATA_START_COMMENT,
        CDATA_END_COMMENT, UNICODE_RANGE, ESCAPE, EOF
    };

    CssToken(CssTokenType type);
    CssToken(CssTokenType type, const char c);
    CssToken(CssTokenType type, const string &content);

    inline bool
    isPunctuator() const,
    isPunctuator(const char punctuator) const,
    isPunctuator(const initializer_list<char> &candidates) const,
    isIdentifier() const,
    isIdentifier(const string &content) const,
    isIdentifier(const initializer_list<string> &candidates) const,
    isStringLiteral() const,
    isHashLiteral() const,
    isNumericLiteral() const,
    isScientificLiteral() const,
    isUnit() const,
    isAtKeyword() const,
    isAtKeyword(const string &keyword) const,
    isComment() const,
    isWhiteSpace() const,
    isEof() const,

    isOfType(const initializer_list<CssTokenType> &type_list) const;

    inline void
    setType(CssTokenType type);

    inline CssTokenType
    type() const;

private:
    CssTokenType m_type;
};

inline void
CssToken::
setType(CssTokenType type)
{
    m_type = type;
}

inline auto
CssToken::
type() const -> CssTokenType
{
    return m_type;
}

inline bool
CssToken::
isPunctuator() const
{
    return m_type == PUNCTUATOR;
}

inline bool
CssToken::
isPunctuator(const char punctuator) const
{
    return m_type == PUNCTUATOR && content().front() == punctuator;
}

inline bool
CssToken::
isPunctuator(const initializer_list<char> &candidates) const
{
    if (m_type == PUNCTUATOR)
        for (const auto &punctuator : candidates)
            if (punctuator == content().front())
                return true;

	return false;
}

inline bool
CssToken::
isIdentifier() const
{
    return m_type == IDENTIFIER;
}

inline bool
CssToken::
isIdentifier(const string &content) const
{
    return m_type == IDENTIFIER && this->content() == content;
}

inline bool
CssToken::
isIdentifier(const initializer_list<string> &candidates) const
{
    if (m_type == IDENTIFIER)
        for (const auto &identifier : candidates)
            if (identifier == content())
                return true;

    return false;
}

inline bool
CssToken::
isStringLiteral() const
{
    return m_type == STRING_LITERAL;
}

inline bool
CssToken::
isHashLiteral() const
{
    return m_type == HASH_LITERAL;
}

inline bool
CssToken::
isNumericLiteral() const
{
    return m_type == NUMERIC_LITERAL;
}

inline bool
CssToken::
isScientificLiteral() const
{
    return m_type == SCIENTIFIC_LITERAL;
}

inline bool
CssToken::
isUnit() const
{
    return m_type == UNIT;
}

inline bool
CssToken::
isAtKeyword() const
{
    return m_type == AT_KEYWORD;
}

inline bool
CssToken::
isAtKeyword(const string &keyword) const
{
    return m_type == AT_KEYWORD && content() == keyword;
}

inline bool
CssToken::
isComment() const
{
    return m_type == COMMENT || m_type == CDATA_START_COMMENT || m_type == CDATA_END_COMMENT;
}

inline bool
CssToken::
isWhiteSpace() const
{
    return m_type == WHITESPACE;
}

inline bool
CssToken::
isEof() const
{
    return m_type == EOF;
}

inline bool
CssToken::
isOfType(const initializer_list<CssTokenType> &type_list) const
{
    for (const auto &type : type_list)
        if (m_type == type) return true;

    return false;
}

using CssTokenPtr = shared_ptr<CssToken>;

}
}
}

#endif // CSSTOKEN_H
