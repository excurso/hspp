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

#ifndef CSSPARSER_H
#define CSSPARSER_H
#include "../../general/parser/GeneralParser.h"
#include "../CssVendorPrefixes.h"
#include "../modifier/CssColorTable.h"
#include "../tokenizer/CssTokenizer.h"
#include "../tokenizer/elements/CssToken.h"
#include "includes.h"
#include <functional>
#include <stack>

namespace CSS {
namespace Parsing {
using namespace General::Parsing;
using namespace CSS::Tokenization;
using namespace CSS::Parsing::Elements;

class CssParser final : GeneralParser
{
public:
    using StyleSheetPtr = CssBlockPtr;

    explicit
    CssParser(const GeneralTokenStreamPtr &token_stream, string file_name = string());

    static const StyleSheetPtr
    parse(const shared_ptr<string> &content, const string &file_name = string()),
    parse(const string &content, const string &file_name = string(), const uint32_t begin_row = 1, const uint32_t begin_column = 1),
    parseStyleAttribute(const string &content, const uint32_t begin_row = 1, const uint32_t begin_column = 1);

    [[noreturn]] void
    throwParseError(const string &message) const override;

private:
    inline const CssTokenPtr
    prevToken() const,
    currentToken(int64_t count = 0) const,
    nextToken() const;

    inline bool
    lookAhead();

    const StyleSheetPtr
    parse();

    const StyleSheetPtr
    parseStyleAttribute();

    const string
    parseSelectorAnPlusB();

    bool
    parseComment(),

    parseNumber(),
    parseQualifiedRule(),

    parseAtRule(),
    parseAtRuleCharset(),
    parseAtRuleImport(),
    parseAtRuleNamespace(),
    parseAtRuleDocument(),
    parseAtRuleFontface(),
    parseAtRuleKeyframes(),
    parseAtRuleMedia(),
    parseAtRulePage(),
    parseAtRuleSupports(),
    parseAtRuleCounterStyle(),
    parseAtRuleViewport(),

    parseBlock(CssBlock::BlockType block_type),
    parseCurlyBlock(),
    parseSquareBlock(),
    parseParenBlock(),

    parseDeclaration(),
    parseDeclarationList(),
    parseFunction(),
    parseValue(),

    parseMathFunction(),
    parseFunctionSupports(),
    parseFunctionAlphaIE(),

    parseSelector(),
    parseSelectorCombination(),
    parseSelectorList(),
    parseAttributeSelector(),
    parsePseudoClass(),
    parsePseudoElement(),

    isPredefinedColor(const string &identifier),
    isValidHexColor(const string &hex_color_literal) const noexcept;

    StyleSheetPtr m_stylesheet;
    stack<CssBaseElementPtr> m_tmp_result_stack;
    stack<DataContainer<CssBaseElementPtr> > m_tmp_list;

    // Stores the file name of a stylesheet file
    const string m_file_name;

    const DataContainer<string> m_at_keyword_list {
        "charset", "font-face", "import", "keyframes", "media", "supports",
        "counter-style", "document", "namespace", "page", "viewport"
    };

    const Vendor m_vendor;
};

inline const CssTokenPtr
CssParser::
prevToken() const
{
    return static_pointer_cast<CssToken>(GeneralParser::prevToken());
}

inline const CssTokenPtr
CssParser::
currentToken(int64_t count) const
{
    return static_pointer_cast<CssToken>(GeneralParser::currentToken(count));
}

inline const CssTokenPtr
CssParser::
nextToken() const
{
    return static_pointer_cast<CssToken>(GeneralParser::nextToken());
}

inline bool
CssParser::
lookAhead()
{
    while (advance() && currentToken()->isWhiteSpace());
    return true;
}

} // namespace Parsing
} // namespace CSS

#endif // CSSPARSER_H
