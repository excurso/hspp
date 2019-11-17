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

#ifndef CSSTOKENIZER_H
#define CSSTOKENIZER_H
#include "../../config/Config.h"
#include "../../general/tokenizer/GeneralTokenizer.h"
#include "elements/CssToken.h"

namespace CSS {
namespace Tokenization {
using namespace General::Tokenization;
using namespace Tokens;

class CssTokenizer : public GeneralTokenizer
{
public:
    explicit
    CssTokenizer(const shared_ptr<string> &content),
    CssTokenizer(const string &content, const uint32_t begin_row = 1, const uint32_t begin_column = 1);

    static const GeneralTokenStreamPtr
    tokenize(const shared_ptr<string> &content),
    tokenize(const string &content, const uint32_t begin_row = 1, const uint32_t begin_column = 1);

private:
    inline const GeneralTokenStreamPtr
    tokenize();

    inline bool
    isWhiteSpace(),
    isPunctuator(),
    isNumericLiteral(),
    isHashLiteral(),
    isAtKeyword(),
    isComment(),
    isUnicodeRange(),
    isNonAsciiIdentifier();

    bool
    isIdentifier(),
    isStringLiteral(),
    isEscapeSequence(),
    isHexDigit(const char c);

    inline const CssTokenPtr
    lastStreamToken();

    bool m_cdata_flag;
};

inline const CssTokenPtr
CssTokenizer::
lastStreamToken()
{
    return static_pointer_cast<CssToken>(tokenStream()->back());
}

} // namespace Tokenization
} // namespace CSS

#endif // CSSTOKENIZER_H
