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

#include "CssParser.h"
using namespace CSS::Parsing;

CssParser::CssParser(const GeneralTokenStreamPtr &token_stream, string file_name) :
    GeneralParser(token_stream),
    m_stylesheet(make_shared<CssBlock>(CssBlock::STYLESHEET)),
    m_file_name(move(file_name)) {}

const CssParser::StyleSheetPtr
CssParser::parse()
{
    currentToken()->isWhiteSpace() && lookAhead();

    while (!currentToken()->isEof()) {
        if (parseAtRule() || parseQualifiedRule() || parseComment()) {
            m_stylesheet->appendElement(m_tmp_result_stack.top());
            m_tmp_result_stack.pop();
        } else throwParseError("");
    }

    tokenStream()->clear();
    return m_stylesheet;
}

/*static*/ const CssParser::StyleSheetPtr
CssParser::
parse(const shared_ptr<string> &content, const string &file_name)
{
    // Tokenize stylesheet
    const auto token_stream = CssTokenizer::tokenize(content);
    // Parse token stream and return the AST
    return CssParser(token_stream, file_name).parse();
}

/*static*/ const CssParser::StyleSheetPtr
CssParser::
parse(const string &content, const string &file_name, const uint32_t begin_row, const uint32_t begin_column)
{
    // Tokenize stylesheet
    const auto token_stream = CssTokenizer::tokenize(content, begin_row, begin_column);
    // Parse token stream and return the AST
    return CssParser(token_stream, file_name).parse();
}

/*static*/ const CssParser::StyleSheetPtr
CssParser::
parseStyleAttribute(const string &content, const uint32_t begin_row, const uint32_t begin_column)
{
    // Tokenize the HTML style attribute value
    const auto token_stream = CssTokenizer::tokenize(content, begin_row, begin_column);
    // Parse token stream and return the AST
    return CssParser(token_stream).parseStyleAttribute();
}

const CssParser::StyleSheetPtr
CssParser::
parseStyleAttribute()
{
    // If style attribute value is empty, return empty stylesheet
    if (currentToken()->isEof()) return m_stylesheet;

    // Skip whitespace at the beginning
    currentToken()->isWhiteSpace() && lookAhead();

    if (parseDeclarationList()) {
        // Set AST elements
        m_stylesheet->setElements(m_tmp_list.top());
        // Remove element from the top of the temporary element stack
        m_tmp_list.pop();
    } else throwParseError("");

    // Clear token stream
    tokenStream()->clear();

    // Return AST
    return m_stylesheet;
}

bool
CssParser::
parseComment()
{
    if (currentToken()->isComment()) {
        CssComment::CommentType comment_type;

        switch (currentToken()->type()) {
        case CssToken::COMMENT:
            comment_type = CssComment::COMMENT;
            break;
        case CssToken::CDATA_START_COMMENT:
            comment_type = CssComment::CDATA_START;
            break;
        case CssToken::CDATA_END_COMMENT:
            comment_type = CssComment::CDATA_END;
            break;
        default:;
        }

        const auto comment_element = make_shared<CssComment>(comment_type, currentToken()->content());
        m_tmp_result_stack.emplace(comment_element);
        lookAhead();
        return true;
    }

    return false;
}

bool
CssParser::
parseBlock(CssBlock::BlockType block_type)
{
    const auto block = make_shared<CssBlock>(block_type);

    if (parseDeclarationList()) {
        // Set AST elements
        block->setElements(m_tmp_list.top());
        // Remove element from the top of the temporal element stack
        m_tmp_list.pop();
    }
    else if (block->blockType() == CssBlock::PAREN) {
        if (parseValue()) {
            block->appendElement(m_tmp_result_stack.top());
            m_tmp_result_stack.pop();
        }
    }

    // Emplace block onto the temporal result stack
    m_tmp_result_stack.emplace(block);
    return true;
}

bool
CssParser::
parseCurlyBlock()
{
    if (currentToken()->isPunctuator('{') && lookAhead())
        if (parseBlock(CssBlock::CURLY)) {
            if (currentToken()->isPunctuator('}') && lookAhead())
                return true;

            throwParseError("Missing '}'");
        }

    return false;
}

bool
CssParser::
parseSquareBlock()
{
    if (currentToken()->isPunctuator('[') && lookAhead())
        return parseBlock(CssBlock::SQUARE) && currentToken()->isPunctuator(']') && lookAhead();

    return false;
}

bool
CssParser::
parseParenBlock()
{
    if (currentToken()->isPunctuator('(') && lookAhead()) {
        if (currentToken()->isPunctuator('(')) {
            const auto paren_block = make_shared<CssBlock>(CssBlock::PAREN);
            while (parseParenBlock() || parseValue()) {
                paren_block->appendElement(m_tmp_result_stack.top());
                m_tmp_result_stack.pop();
            }

            m_tmp_result_stack.emplace(paren_block);

            if (currentToken()->isPunctuator(')') && lookAhead())
                return true;

            throwParseError("Missing ')'");
        }
        else
            return (parseBlock(CssBlock::PAREN)) && currentToken()->isPunctuator(')') && lookAhead();
    }

    return false;
}

bool
CssParser::
parseQualifiedRule()
{
    if (parseSelectorList() && currentToken()->isPunctuator('{')) {
        // Create an object for the qualified rule and assign it the selector list
        const auto qualified_rule = make_shared<CssQualifiedRule>(m_tmp_list.top());
        // Remove the selector list from the top of the temporal list stack
        m_tmp_list.pop();

        if (parseCurlyBlock()) {
            // Assign the block to the qualified rule object
            qualified_rule->setBlock(m_tmp_result_stack.top());
            // Remove the block from the top of the temporal result stack
            m_tmp_result_stack.pop();
            // Emplace the qualified rule object to the temporal result stack
            m_tmp_result_stack.emplace(qualified_rule);
            return true;
        }
    }

    return false;
}

bool
CssParser::
parseSelector()
{
    CssSelectorPtr recent_selector, current_selector, parental_selector;

    while (currentToken()->isPunctuator({'.', ':', '[', '*'}) ||
           currentToken()->isHashLiteral() || currentToken()->isIdentifier()) {

        parental_selector = recent_selector ?
            recent_selector : make_shared<CssSelector>(CssSelector::UNIVERSAL, "*");

        switch (currentToken()->type()) {
        case CssToken::PUNCTUATOR:
            switch (currentToken()->content()[0]) {
            case '.':
                if (!nextToken()->isIdentifier()) throwParseError("Invalid class name");
                advance();
                current_selector = make_shared<CssSelector>(CssSelector::CLASS, parental_selector, currentToken()->content());
                current_selector->setInitialPosition(currentToken()->row(), currentToken()->column() - 1);
                advance();
                break;
            case ':':
                if (parsePseudoClass() || parsePseudoElement()) {
                    current_selector = CssSelector::fromBase(m_tmp_result_stack.top());
                    current_selector->setParentalSelector(parental_selector);
                    parental_selector->setChildSelector(current_selector);
                    m_tmp_result_stack.pop();
                } else {
                    advance(); throwParseError("");
                }
                break;
            case '[':
                if (parseAttributeSelector()) {
                    current_selector = CssSelector::fromBase(m_tmp_result_stack.top());
                    m_tmp_result_stack.pop();
                }
                break;
            case '*':
                current_selector = make_shared<CssSelector>(CssSelector::UNIVERSAL, "*");
                advance();
                break;
            }
            break;
        case CssToken::HASH_LITERAL:
            if (currentToken()->content().empty()) throwParseError("Invalid id");
            current_selector = make_shared<CssSelector>(CssSelector::ID, parental_selector, currentToken()->content());
            current_selector->setInitialPosition(currentToken()->row(), currentToken()->column());
            advance();
            break;
        case CssToken::IDENTIFIER:
            current_selector = make_shared<CssSelector>(CssSelector::TYPE, currentToken()->content());
            advance();
            break;
        default:
            throwParseError("");
        }

        parental_selector->setChildSelector(current_selector);

        if (!(current_selector->isOfType(CssSelector::UNIVERSAL) &&
              parental_selector->isOfType(CssSelector::UNIVERSAL)))
            current_selector->setParentalSelector(parental_selector);

        recent_selector = current_selector;
        current_selector = nullptr;
    }

    if (recent_selector) {
        m_tmp_result_stack.emplace(recent_selector);
        return true;
    }

    return false;
}

bool
CssParser::
parseSelectorCombination()
{
    CssBaseElementPtr left, right;
    CssSelectorCombinatorPtr combinator;

    while (true) {
        if (combinator && left && right) {
            combinator->setLeft(left);
            combinator->setRight(right);

            left = combinator;
            right = nullptr;
            combinator = nullptr;
        }

        if (parseSelector()) {
            (!left && (left = m_tmp_result_stack.top())) ||
            (!right && (right = m_tmp_result_stack.top()));
            m_tmp_result_stack.pop();
            continue;
        }

        currentToken()->isWhiteSpace() &&
        nextToken()->isPunctuator({'>', '+', '~', '{', ','}) &&
        lookAhead();

        if (currentToken()->isPunctuator({'>', '+', '~'}) || currentToken()->isWhiteSpace()) {

            combinator = currentToken()->isWhiteSpace() ?
                make_shared<CssSelectorCombinator>(CssSelectorCombinator::DESCENDANCY) :
                make_shared<CssSelectorCombinator>(
                    CssSelectorCombinator::getCombinatorType(currentToken()->content().front()));

            !left && (left = make_shared<CssSelector>(CssSelector::UNIVERSAL, "*"));

            lookAhead(); continue;
        }

        break;
    }

    if (left && !right && !combinator) {
        m_tmp_result_stack.emplace(left);
        return true;
    }

    return false;
}

bool
CssParser::
parseSelectorList()
{
    DataContainer<CssSelectorCombinator> selector_list;
    m_tmp_list.emplace(DataContainer<CssBaseElementPtr>());

    do {
        currentToken()->isWhiteSpace() && lookAhead();

        if (parseSelectorCombination()) {
            m_tmp_list.top().emplace_back(m_tmp_result_stack.top());
            m_tmp_result_stack.pop();
        }
    } while (currentToken()->isPunctuator(',') && advance());

    return !m_tmp_list.top().empty();
}

bool
CssParser::
parseAttributeSelector()
{
    if (currentToken()->isPunctuator('[') && lookAhead()) {
        const auto row = currentToken()->row(), column = currentToken()->column();
        const auto attribute_selector = make_shared<CssSelectorAttribute>();

        if (!currentToken()->isIdentifier()) throwParseError("");

        attribute_selector->setAttributeName(currentToken()->content());
        lookAhead();

        if (currentToken()->isPunctuator({'=', '~', '|', '^', '$', '*'})) {
            if (currentToken()->isPunctuator('=') && lookAhead()) {
                attribute_selector->setOperation(CssSelectorAttribute::EQUAL);
            }
            else if (currentToken()->isPunctuator({'~', '|', '^', '$', '*'}) &&
                     nextToken()->isPunctuator('=')) {
                switch (currentToken()->content()[0]) {
                case '~':
                    attribute_selector->setOperation(CssSelectorAttribute::TILDE_EQUAL);
                    break;
                case '|':
                    attribute_selector->setOperation(CssSelectorAttribute::PIPE_EQUAL);
                    break;
                case '^':
                    attribute_selector->setOperation(CssSelectorAttribute::ROOF_EQUAL);
                    break;
                case '$':
                    attribute_selector->setOperation(CssSelectorAttribute::DOLLAR_EQUAL);
                    break;
                case '*':
                    attribute_selector->setOperation(CssSelectorAttribute::ASTERISK_EQUAL);
                    break;
                }

                advance() && lookAhead();
            } else throwParseError("");

            if (currentToken()->isIdentifier() || currentToken()->isStringLiteral()) {
                attribute_selector->setAttributeValue(currentToken()->content());
                lookAhead();
            }
        }
        else if (!currentToken()->isPunctuator(']'))
            throwParseError("");

        if (currentToken()->isIdentifier("i") && prevToken()->isWhiteSpace()) {
            attribute_selector->setCaseInsensitive();
            lookAhead();
        }

        if (currentToken()->isPunctuator(']') && advance()) {
            attribute_selector->setInitialPosition(row, column);

            m_tmp_result_stack.emplace(attribute_selector);

            return true;
        }

        throwParseError("");
    }

    return false;
}

bool
CssParser::
parsePseudoClass()
{
    if (currentToken()->isPunctuator(':') && nextToken()->isIdentifier() && advance()) {
        const auto pseudo_class = make_shared<CssSelector>(CssSelector::PSEUDO_CLASS, currentToken()->content());

        if (currentToken()->hasContent({"is", "not", "where", "has", "host", "host-context"})) {
            if (nextToken()->isPunctuator('(') && advance() && lookAhead()) {
                do {
                    if (parseSelectorCombination()) {
                        pseudo_class->appendSubSelector(CssSelector::fromBase(m_tmp_result_stack.top()));
                        m_tmp_result_stack.pop();
                    } else throwParseError("");
                } while (currentToken()->isPunctuator(',') && lookAhead());

                if (currentToken()->isPunctuator(')') && advance()) {
                    m_tmp_result_stack.emplace(pseudo_class);
                    return true;
                }

                throwParseError("");
            }
        } else if (currentToken()->hasContent({"nth-child", "nth-last-child", "nth-of-type",
                                               "nth-last-of-type", "nth-col", "nth-last-col"})) {
            if (nextToken()->isPunctuator('(') && advance() && lookAhead()) {
                string an_plus_b;

                if (currentToken()->isIdentifier("even") ||
                    currentToken()->isIdentifier("odd")) {
                    an_plus_b = currentToken()->content();
                    lookAhead();
                } else if ((an_plus_b = parseSelectorAnPlusB()).empty()) {
                    throwParseError("");
                }

                if (!an_plus_b.empty()) {
                    const auto an_plus_b_selector = make_shared<CssSelector>(CssSelector::AN_PLUS_B, an_plus_b);
                    pseudo_class->appendSubSelector(an_plus_b_selector);

                    if (currentToken()->isIdentifier("of") && lookAhead()) {
                        if (parseSelectorCombination()) {
                            an_plus_b_selector->appendSubSelector(CssSelector::fromBase(m_tmp_result_stack.top()));
                            m_tmp_result_stack.pop();
                        }
                    }
                }

                if (currentToken()->isPunctuator(')') && advance()) {
                    m_tmp_result_stack.emplace(pseudo_class);
                    return true;
                }

                throwParseError("");
            }
        }
        else if (currentToken()->hasContent({"lang", "-ms-lang"}) && nextToken()->isPunctuator('(') && advance() && lookAhead()) {
            if (currentToken()->isIdentifier()) {
                const auto lang_identifier = make_shared<CssSelector>(CssSelector::NONE, currentToken()->content());
                pseudo_class->appendSubSelector(lang_identifier);
                lookAhead();

                if (currentToken()->isPunctuator(')') && advance()) {
                    m_tmp_result_stack.emplace(pseudo_class);
                    return true;
                }

                throwParseError("");
            }
        }

        m_tmp_result_stack.emplace(pseudo_class);
        advance();
        return true;
    }

    return false;
}

const string
CssParser::
parseSelectorAnPlusB()
{
    string selector_content;

    if (currentToken()->isPunctuator('+')) {
        advance();
    }
    else if (currentToken()->isPunctuator('-')) {
        selector_content += '-';
        advance();
    }

    if (currentToken()->isNumericLiteral()) {
        selector_content += currentToken()->content();
        advance();
    }

    if (currentToken()->isUnit() || currentToken()->isIdentifier()) {
        if (currentToken()->hasContent({"n", "-n"})) {
            selector_content += currentToken()->content();
            advance();

            if (currentToken()->isPunctuator({'+', '-'})) {
                selector_content += currentToken()->content();
                advance();

                if (currentToken()->isNumericLiteral()) {
                    selector_content += currentToken()->content();
                    lookAhead();
                }
            }
        }
    }

    if (!selector_content.empty())
        return selector_content;

    return string();
}

bool
CssParser::
parsePseudoElement()
{
    if (currentToken()->isPunctuator(':') && nextToken()->isPunctuator(':') &&
        currentToken(+2)->isIdentifier() && advance(+2)) {
        const auto pseudo_element = make_shared<CssSelector>(CssSelector::PSEUDO_ELEMENT, currentToken()->content());

        m_tmp_result_stack.emplace(pseudo_element);
        advance();
        return true;
    }

    return false;
}

bool
CssParser::
parseAtRule()
{
    if (currentToken()->isAtKeyword())
        for (const auto &vendor_prefix : m_vendor.prefixes)
            for (const auto &at_keyword : m_at_keyword_list)
                if (currentToken()->content() == vendor_prefix + at_keyword)
                    if (parseAtRuleMedia() ||
                        parseAtRuleKeyframes() ||
                        parseAtRuleSupports() ||
                        parseAtRuleFontface() ||
                        parseAtRuleImport() ||
                        parseAtRuleCounterStyle() ||
                        parseAtRuleDocument() ||
                        parseAtRuleCharset() ||
                        parseAtRuleNamespace() ||
                        parseAtRulePage() ||
                        parseAtRuleViewport())
                        return true;

    return false;
}

bool
CssParser::
parseAtRuleCharset()
{
    for (const auto &vendor_prefix : m_vendor.prefixes) {
        const auto at_keyword_charset = vendor_prefix + "charset";
        if (currentToken()->isAtKeyword(at_keyword_charset) && lookAhead()) {
            if (currentToken()->isStringLiteral()) {
                const auto at_rule_charset = make_shared<CssAtRule>(at_keyword_charset);
                const auto css_string = make_shared<CssString>(currentToken()->content());

                at_rule_charset->appendExpression(css_string);
                lookAhead() && currentToken()->isPunctuator(';') && lookAhead();
                m_tmp_result_stack.emplace(at_rule_charset);

                return true;
            }
        }
    }

    return false;
}

bool
CssParser::
parseAtRuleDocument()
{
    for (const auto &vendor_prefix : m_vendor.prefixes) {
        const auto at_keyword_document = vendor_prefix + "document";
        if (currentToken()->isAtKeyword(at_keyword_document) && lookAhead()) {
            const auto at_rule_document = make_shared<CssAtRule>(at_keyword_document);
            const auto at_rule_block = make_shared<CssBlock>(CssBlock::CURLY);

            if (currentToken()->isIdentifier() && nextToken()->isPunctuator('(')) {
                if (parseFunction()) {
                    at_rule_document->appendExpression(m_tmp_result_stack.top());
                    m_tmp_result_stack.pop();
                }

                if (currentToken()->isPunctuator('{') && lookAhead()) {
                    while (parseAtRuleMedia() || parseAtRuleSupports() ||
                           parseAtRuleFontface() || parseQualifiedRule()) {
                        at_rule_block->appendElement(m_tmp_result_stack.top());
                        m_tmp_result_stack.pop();
                    }

                    if (currentToken()->isPunctuator('}') && lookAhead()) {
                        at_rule_document->setBlock(at_rule_block);
                        m_tmp_result_stack.emplace(at_rule_document);

                        return true;
                    }

                    throwParseError("Missing '}'");
                } else throwParseError("Expected '{'");
            }
        }
    }

    return false;
}

bool
CssParser::
parseAtRuleImport()
{
    for (const auto &vendor_prefix : m_vendor.prefixes) {
        const auto at_keyword_import = vendor_prefix + "import";

        if (currentToken()->isAtKeyword(at_keyword_import) && lookAhead()) {
            const auto at_rule_import = make_shared<CssAtRule>(at_keyword_import);

            while (parseFunctionSupports() || parseValue() || parseParenBlock()) {
                at_rule_import->appendExpression(m_tmp_result_stack.top());
                m_tmp_result_stack.pop();

                if (currentToken()->isPunctuator(',')) {
                    at_rule_import->createList(); lookAhead();
                }
            }

            if (!at_rule_import->expressions().front().empty()) {
                currentToken()->isPunctuator(';') && lookAhead();
                m_tmp_result_stack.emplace(at_rule_import);
                return true;
            }

            throwParseError("");
        }
    }

    return false;
}

bool
CssParser::
parseAtRuleNamespace()
{
    for (const auto &vendor_prefix : m_vendor.prefixes) {
        const auto at_keyword_namespace = vendor_prefix + "namespace";

        if (currentToken()->isAtKeyword(at_keyword_namespace) && lookAhead()) {
            const auto at_rule_namespace = make_shared<CssAtRule>(at_keyword_namespace);

            while (parseFunction() || parseValue()) {
                at_rule_namespace->appendExpression(m_tmp_result_stack.top());
                m_tmp_result_stack.pop();
            }

            currentToken()->isPunctuator(';') && lookAhead();
            m_tmp_result_stack.emplace(at_rule_namespace);
            return true;
        }
    }

    return false;
}

bool
CssParser::
parseAtRuleFontface()
{
    for (const auto &vendor_prefix : m_vendor.prefixes) {
        const auto at_keyword_fontface = vendor_prefix + "font-face";
        if (currentToken()->isAtKeyword(at_keyword_fontface) && lookAhead() &&
            currentToken()->isPunctuator('{') && lookAhead()) {
            const auto at_rule_font_face = make_shared<CssAtRule>(at_keyword_fontface);
            const auto at_rule_block = make_shared<CssBlock>(CssBlock::CURLY);

            if (parseDeclaration()) {
                do {
                    at_rule_block->appendElement(m_tmp_result_stack.top());
                    m_tmp_result_stack.pop();
                } while (currentToken()->isPunctuator(';') && lookAhead() && parseDeclaration());
            } else throwParseError("");

            if (currentToken()->isPunctuator('}') && lookAhead()) {
                at_rule_font_face->setBlock(at_rule_block);
                m_tmp_result_stack.emplace(at_rule_font_face);
                return true;
            }
        }
    }

    return false;
}

bool
CssParser::
parseAtRuleMedia()
{
    for (const auto &vendor_prefix : m_vendor.prefixes) {
        const auto at_keyword_media = vendor_prefix + "media";

        if (currentToken()->isAtKeyword(at_keyword_media) && lookAhead()) {
            const auto at_rule_media = make_shared<CssAtRule>(at_keyword_media);
            const auto at_rule_block = make_shared<CssBlock>(CssBlock::CURLY);

            while (parseValue() || parseParenBlock() || parseComment()) {
                at_rule_media->appendExpression(m_tmp_result_stack.top());
                m_tmp_result_stack.pop();

                if (currentToken()->isPunctuator(',') && lookAhead())
                    at_rule_media->createList();
            }

            if (currentToken()->isPunctuator('{') && lookAhead()) {
                while (parseQualifiedRule() || parseAtRule() || parseComment()) {
                    at_rule_block->appendElement(m_tmp_result_stack.top());
                    m_tmp_result_stack.pop();
                }

                if (currentToken()->isPunctuator('}') && lookAhead()) {
                    at_rule_media->setBlock(at_rule_block);
                    m_tmp_result_stack.emplace(at_rule_media);

                    return true;
                }

                throwParseError("Missing '}'");
            } else throwParseError("");
        }
    }

    return false;
}

bool
CssParser::
parseAtRulePage()
{
    for (const auto &vendor_prefix : m_vendor.prefixes) {
        const auto at_keyword_page = vendor_prefix + "page";

        if (currentToken()->isAtKeyword(at_keyword_page) && lookAhead()) {
            const auto at_rule_page = make_shared<CssAtRule>(at_keyword_page);

            if (currentToken()->isPunctuator('{')) {
                if (parseCurlyBlock()) {
                    at_rule_page->setBlock(m_tmp_result_stack.top());
                    m_tmp_result_stack.pop();
                    m_tmp_result_stack.emplace(at_rule_page);

                    return true;
                }

                lookAhead();

                const auto at_rule_block = make_shared<CssBlock>(CssBlock::CURLY);

                while (parseAtRule() || parseQualifiedRule()) {
                    at_rule_block->appendElement(m_tmp_result_stack.top());
                    m_tmp_result_stack.pop();
                }

                if (currentToken()->isPunctuator('}') && lookAhead()) {
                    at_rule_page->setBlock(at_rule_block);
                    m_tmp_result_stack.emplace(at_rule_page);

                    return true;
                }

                throwParseError("Missing '}'");
            } else throwParseError("");
        }
    }

    return false;
}

bool
CssParser::
parseAtRuleSupports()
{
    for (const auto &vendor_prefix : m_vendor.prefixes) {
        const auto at_keyword_supports = vendor_prefix + "supports";

        if (currentToken()->isAtKeyword(at_keyword_supports) && lookAhead()) {
            const auto at_rule_supports = make_shared<CssAtRule>(at_keyword_supports);
            const auto at_rule_block = make_shared<CssBlock>(CssBlock::CURLY);

            while (parseValue() || parseParenBlock()) {
                at_rule_supports->appendExpression(m_tmp_result_stack.top());
                m_tmp_result_stack.pop();

                if (currentToken()->isPunctuator(',')) {
                    at_rule_supports->createList(); lookAhead();
                }
            }

            if (currentToken()->isPunctuator('{') && lookAhead()) {
                while (parseAtRuleMedia() || parseAtRuleSupports() ||
                       parseAtRuleFontface() || parseQualifiedRule()) {
                    at_rule_block->appendElement(m_tmp_result_stack.top());
                    m_tmp_result_stack.pop();
                }

                if (currentToken()->isPunctuator('}') && lookAhead()) {
                    at_rule_supports->setBlock(at_rule_block);
                    m_tmp_result_stack.emplace(at_rule_supports);

                    return true;
                }

                throwParseError("Missing '}'");
            } else throwParseError("");
        }
    }

    return false;
}

bool
CssParser::
parseAtRuleCounterStyle()
{
    if (currentToken()->isAtKeyword()) {
        for (const auto &vendor_prefix : m_vendor.prefixes) {
            const auto at_keyword_counter_style = vendor_prefix + "counter-style";

            if (currentToken()->isAtKeyword(at_keyword_counter_style) && lookAhead()) {
                const auto at_rule_counter_style = make_shared<CssAtRule>(at_keyword_counter_style);
                const auto at_rule_block = make_shared<CssBlock>(CssBlock::CURLY);

                while (parseValue() || parseParenBlock()) {
                    at_rule_counter_style->appendExpression(m_tmp_result_stack.top());
                    m_tmp_result_stack.pop();

                    if (currentToken()->isPunctuator(',')) {
                        at_rule_counter_style->createList(); lookAhead();
                    }
                }

                if (currentToken()->isPunctuator('{') && lookAhead()) {
                    if (parseDeclarationList()) {
                        at_rule_block->setElements(m_tmp_list.top());
                        m_tmp_list.pop();
                    }

                    if (currentToken()->isPunctuator('}') && lookAhead()) {
                        at_rule_counter_style->setBlock(at_rule_block);
                        m_tmp_result_stack.emplace(at_rule_counter_style);

                        return true;
                    }

                    throwParseError("Missing '}'");
                } else throwParseError("");
            }
        }
    }

    return false;
}

bool
CssParser::
parseAtRuleKeyframes()
{
    for (const auto &vendor_prefix : m_vendor.prefixes) {
        const auto at_keyword_keyframes = vendor_prefix + "keyframes";

        if (currentToken()->isAtKeyword(at_keyword_keyframes) && lookAhead()) {
            const auto at_rule_keyframes = make_shared<CssAtRule>(at_keyword_keyframes);
            const auto at_rule_block = make_shared<CssBlock>(CssBlock::CURLY);

            if (parseValue()) {
                at_rule_keyframes->appendExpression(m_tmp_result_stack.top());
                m_tmp_result_stack.pop();

                if (currentToken()->isPunctuator('{') && lookAhead()) {
                    while (true) {
                        const auto rule = make_shared<CssQualifiedRule>();
                        do {
                            if (currentToken()->hasContent({ "from", "to" })) {
                                const auto selector = make_shared<CssSelector>(CssSelector::KEYFRAMES, currentToken()->content());
                                rule->appendSelector(selector); lookAhead();
                            } else if (currentToken()->isNumericLiteral() &&
                                       nextToken()->hasContent("%")) {
                                const auto selector = make_shared<CssSelector>(CssSelector::KEYFRAMES, currentToken()->content() + "%");
                                rule->appendSelector(selector); advance(); lookAhead();
                            }
                        } while (currentToken()->isPunctuator(',') && lookAhead());

                        if (currentToken()->isPunctuator('{') && lookAhead()) {
                            if (parseBlock(CssBlock::CURLY)) {
                                rule->setBlock(static_pointer_cast<CssBlock>(m_tmp_result_stack.top()));
                                m_tmp_result_stack.pop();

                                if (currentToken()->isPunctuator('}') && lookAhead()) {
                                    at_rule_block->appendElement(rule);
                                    continue;
                                }
                            }
                        }

                        break;
                    }

                    if (currentToken()->isPunctuator('}') && lookAhead()) {
                        at_rule_keyframes->setBlock(at_rule_block);
                        m_tmp_result_stack.emplace(at_rule_keyframes);

                        return true;
                    }
                }
            }
        }
    }

    return false;
}

bool
CssParser::
parseAtRuleViewport()
{
    for (const auto &vendor_prefix : m_vendor.prefixes) {
        const auto at_keyword_viewport = vendor_prefix + "viewport";

        if (currentToken()->isAtKeyword(at_keyword_viewport) && lookAhead()) {
            const auto at_rule_viewport = make_shared<CssAtRule>(at_keyword_viewport);

            if (currentToken()->isPunctuator('{') && parseCurlyBlock()) {
                at_rule_viewport->setBlock(m_tmp_result_stack.top());
                m_tmp_result_stack.pop();
                m_tmp_result_stack.emplace(at_rule_viewport);

                return true;
            }
        }
    }

    return false;
}

bool
CssParser::
parseDeclaration()
{
    rememberPosition();

    // IE <= 7 hack
    auto ie_hack = false;

    if (currentToken()->isPunctuator('*') && lookAhead())
        ie_hack = true;

    if (currentToken()->isIdentifier()) {
        CssDeclarationPtr declaration;

        if (currentToken()->content().substr(0, 2) == "--") {
            const auto custom_property = make_shared<CssCustomProperty>(currentToken()->content().substr(2));
            declaration = make_shared<CssDeclaration>(custom_property);
        } else
            declaration = make_shared<CssDeclaration>((ie_hack ? "*" : "") + currentToken()->content());

        lookAhead();

        if (currentToken()->isPunctuator(':') && lookAhead()) {
            const auto begin = getIterator();

            while (!currentToken()->isEof()) {
                if (parseValue() || parseComment()) {
                    declaration->appendValue(m_tmp_result_stack.top());
                    m_tmp_result_stack.pop();
                    continue;
                }

                if (currentToken()->isPunctuator(',') && lookAhead()) {
                    declaration->createList();
                    continue;
                }

                break;
            }

            if (declaration->name() == "filter" && declaration->values().size() == 1 &&
                declaration->values()[0].size() == 1) {

                if (declaration->values()[0][0]->isIdentifier()) {
                    const auto &value = static_pointer_cast<CssIdentifier>(declaration->values()[0][0]);

                    if (value->value() == "progid") {
                        setIterator(begin);
                        string content;

                        while (!currentToken()->isEof() &&
                               !currentToken()->isPunctuator({';', '}'})) {
                            if (currentToken()->isHashLiteral())
                                content += '#';

                            content += currentToken()->content(); advance();
                        }

                        const auto css_string = make_shared<CssString>(content, true);
                        declaration->values()[0][0]->setReplacementElement(css_string);
                    }
                }
            }

            if (currentToken()->isPunctuator('!') && lookAhead()) {
                if (currentToken()->isIdentifier("important") && lookAhead())
                    declaration->setImportantFlag();
                else if (currentToken()->isIdentifier()) {
                    declaration->setImportantReplacement(currentToken()->content());
                    lookAhead();
                } else throwParseError("");
            }

            m_tmp_result_stack.emplace(declaration);

            popPosition();
            return true;
        }
    }

    resetPosition();

    return false;
}

bool
CssParser::
parseDeclarationList()
{
    m_tmp_list.emplace(DataContainer<CssBaseElementPtr>());

    while (parseComment()) {
        m_tmp_list.top().emplace_back(m_tmp_result_stack.top());
        m_tmp_result_stack.pop();
    }

    if (parseDeclaration()) {
        m_tmp_list.top().emplace_back(m_tmp_result_stack.top());
        m_tmp_result_stack.pop();

        do {
            while (parseComment()) {
                m_tmp_list.top().emplace_back(m_tmp_result_stack.top());
                m_tmp_result_stack.pop();
            }

            if (parseDeclaration()) {
                m_tmp_list.top().emplace_back(m_tmp_result_stack.top());
                m_tmp_result_stack.pop();
            }
        } while (currentToken()->isPunctuator(';') && lookAhead());

        return true;
    }

    return false;
}

bool
CssParser::
parseFunction()
{
    if (currentToken()->isIdentifier() && nextToken()->isPunctuator('(') &&
        currentToken()->content() != "supports") {

        if (parseMathFunction() || parseFunctionAlphaIE()) return true;

        const auto function = make_shared<CssFunction>(currentToken()->content());
        function->setInitialPosition(currentToken()->row(), currentToken()->column());
        advance(+2) && currentToken()->isWhiteSpace() && lookAhead();

        do {
            m_tmp_list.emplace(DataContainer<CssBaseElementPtr>());

            while (parseValue()) {
                m_tmp_list.top().emplace_back(m_tmp_result_stack.top());
                m_tmp_result_stack.pop();
            }

            function->appendParameter(m_tmp_list.top());
            m_tmp_list.pop();
        } while (currentToken()->isPunctuator(',') && lookAhead());

        m_tmp_result_stack.emplace(function);

        if (currentToken()->isPunctuator(')') && lookAhead())
            return true;

        throwParseError("");
    }

    return false;
}

bool
CssParser::
parseValue()
{
    switch (currentToken()->type()) {
    case CssToken::IDENTIFIER: {
        if (parseFunction()) return true;

        if (currentToken()->content().substr(0, 2) == "--") {
            const auto custom_property = make_shared<CssCustomProperty>(currentToken()->content().substr(2));
            custom_property->setInitialPosition(currentToken()->row(), currentToken()->column());
            m_tmp_result_stack.emplace(custom_property);
            lookAhead();
            return true;
        }

        if (currentToken()->content() == "transparent" || isPredefinedColor(currentToken()->content())) {
            const auto color = make_shared<CssColor>(CssColor::PREDEFINED_NAME, currentToken()->content());
            color->setInitialPosition(currentToken()->row(), currentToken()->column());
            m_tmp_result_stack.emplace(color);
            lookAhead();
            return true;
        }

        const auto identifier = make_shared<CssIdentifier>(currentToken()->content());
        identifier->setInitialPosition(currentToken()->row(), currentToken()->column());
        m_tmp_result_stack.emplace(identifier);
        lookAhead();
        return true;
    }
    case CssToken::NUMERIC_LITERAL:
        return parseNumber();
    case CssToken::STRING_LITERAL: {
        const auto string = make_shared<CssString>(currentToken()->content());
        string->setInitialPosition(currentToken()->row(), currentToken()->column());
        m_tmp_result_stack.emplace(string);
        lookAhead();
        return true;
    }
    case CssToken::HASH_LITERAL: {
        if (!isValidHexColor(currentToken()->content()))
            throwParseError("Invalid hex color: '#" + currentToken()->content() + "'");

        const auto hex_color = make_shared<CssColor>(CssColor::HEX_LITERAL, String::toLower(currentToken()->content()));
        hex_color->setInitialPosition(currentToken()->row(), currentToken()->column());
        m_tmp_result_stack.emplace(hex_color);
        lookAhead();
        return true;
    }
    case CssToken::UNICODE_RANGE: {
        const auto unicode_range = make_shared<CssUnicodeRange>(currentToken()->content());
        unicode_range->setInitialPosition(currentToken()->row(), currentToken()->column());
        m_tmp_result_stack.emplace(unicode_range);
        lookAhead();
        return true;
    }
    default:
        if (currentToken()->isPunctuator({'+', '-'}) && parseNumber())
            return true;

        if (currentToken()->isPunctuator({'+', '-', '*', '/'})) {
            const auto delimiter = make_shared<CssDelimiter>(currentToken()->content());
            delimiter->setInitialPosition(currentToken()->row(), currentToken()->column());
            m_tmp_result_stack.emplace(delimiter);
            lookAhead();
            return true;
        }
    }

    return false;
}

bool
CssParser::
parseNumber()
{
    bool negative_number = false;

    if (nextToken()->isNumericLiteral()) {
        if (currentToken()->isPunctuator('+'))
            advance();
        else if (currentToken()->isPunctuator('-')) {
            negative_number = true;
            advance();
        }
    }

    if (currentToken()->isNumericLiteral()) {
        const auto row = currentToken()->row(), column = currentToken()->column();

        const auto number_element = make_shared<CssNumber>(currentToken()->content());
        number_element->setInitialPosition(row, column);
        number_element->setNegativeFlag(negative_number);
        advance();

        if (currentToken()->isScientificLiteral()) {
            number_element->setScientificPostfix(currentToken()->content());
            advance();
        }

        if (currentToken()->isUnit()) {
            const auto dimension_element = make_shared<CssDimension>(number_element->value(), currentToken()->content());
            dimension_element->setNegativeFlag(negative_number);
            dimension_element->setInitialPosition(row, column);
            m_tmp_result_stack.emplace(dimension_element);
            lookAhead();
        }
        else if (currentToken()->isPunctuator('%')) {
            const auto percentage_element = make_shared<CssPercentage>(number_element->value());
            percentage_element->setNegativeFlag(negative_number);
            percentage_element->setInitialPosition(row, column);
            m_tmp_result_stack.emplace(percentage_element);
            lookAhead();
        }
        else {
            m_tmp_result_stack.emplace(number_element);
        }

        currentToken()->isWhiteSpace() && lookAhead();

        return true;
    }

    return false;
}

bool
CssParser::
parseFunctionSupports()
{
    if (currentToken()->isIdentifier() && currentToken()->content() == "supports" &&
        nextToken()->isPunctuator('(') && advance(+2)) {
        const auto supports_condition = make_shared<CssSupportsCondition>();

        if (parseDeclaration()) {
            supports_condition->appendCondition(m_tmp_result_stack.top());
            m_tmp_result_stack.pop();

            if (currentToken()->isPunctuator(')') && lookAhead()) {
                m_tmp_result_stack.emplace(supports_condition);
                return true;
            }

            throwParseError("Missing ')'");
        }
    }

    return false;
}

bool
CssParser::
parseMathFunction()
{
    if (currentToken()->isIdentifier({"calc", "min", "max", "clamp"}) &&
        nextToken()->isPunctuator('(')) {

        const auto function = make_shared<CssFunction>(currentToken()->content());
        advance() && lookAhead();
        uint8_t paren_counter = 1;

        m_tmp_list.emplace(DataContainer<CssBaseElementPtr>());

        while (true) {
            if (currentToken()->isPunctuator()) {
                if (currentToken()->content()[0] == '-' && nextToken()->isNumericLiteral()) {
                    if (parseNumber()) {
                        m_tmp_list.top().emplace_back(m_tmp_result_stack.top());
                        m_tmp_result_stack.pop();
                        continue;
                    }
                }

                switch (currentToken()->content().front()) {
                case '(':
                case ')':
                case '+':
                case '-':
                case '*':
                case '/': {
                    if (currentToken()->content().front() == '(') {
                        ++paren_counter;
                    }
                    else if (currentToken()->content().front() == ')')
                        if (!bool(--paren_counter)) break;

                    const auto punctuator = make_shared<CssDelimiter>(currentToken()->content());
                    m_tmp_list.top().emplace_back(punctuator); lookAhead();

                    continue;
                }
                case ',':
                    function->appendParameter(m_tmp_list.top()); lookAhead();
                    m_tmp_list.top().clear();
                    continue;
                }
            }
            else if (currentToken()->isNumericLiteral()) {
                if (nextToken()->isUnit() || nextToken()->isPunctuator('%')) {
                    const auto dimension = make_shared<CssDimension>(
                                currentToken()->content(), nextToken()->content());

                    advance() && lookAhead();

                    m_tmp_list.top().emplace_back(dimension);
                    continue;
                }

                const auto number = make_shared<CssNumber>(currentToken()->content());
                m_tmp_list.top().emplace_back(number);
                lookAhead();

                continue;
            }
            else if (parseFunction()) {
                m_tmp_list.top().emplace_back(m_tmp_result_stack.top());
                m_tmp_result_stack.pop();

                continue;
            }

            function->appendParameter(m_tmp_list.top());
            m_tmp_list.pop();
            break;
        }

        if (currentToken()->isPunctuator(')') && lookAhead()) {
            m_tmp_result_stack.emplace(function);
            return true;
        }
    }

    return false;
}

bool
CssParser::
parseFunctionAlphaIE()
{
    if (currentToken()->isIdentifier() &&
        String::toLower(currentToken()->content()) == "alpha" &&
        nextToken()->isPunctuator('(')) {
        advance() && lookAhead();

        string content;

        while (!currentToken()->isEof() && !currentToken()->isPunctuator(')')) {
            content += currentToken()->content(); advance();
        }

        if (currentToken()->isPunctuator(')') && lookAhead()) {
            const auto function_alpha_ie = make_shared<CssFunction>("alpha");
            const auto css_string = make_shared<CssString>(move(content), true);
            function_alpha_ie->appendParameter({css_string});

            m_tmp_result_stack.emplace(function_alpha_ie);
            return true;
        }

        throwParseError("Missing ')'");
    }

    return false;
}

bool
CssParser::
isPredefinedColor(const string &identifier)
{
    for (const auto &color_pair : s_css_color_table)
        if (color_pair.first == identifier)
            return true;

    return false;
}

bool
CssParser::
isValidHexColor(const string &hex_color_literal) const noexcept
{
    switch(hex_color_literal.length()) {
    case 3: case 4: case 6: case 8:
        break;
    default:
        return false;
    }

    for (const auto &hex_digit : hex_color_literal) {
        if (!(hex_digit >= '0' && hex_digit <= '9') &&
            !(hex_digit >= 'a' && hex_digit <= 'f') &&
            !(hex_digit >= 'A' && hex_digit <= 'F')) {
            return false;
        }
    }

    return true;
}

void
CssParser::
throwParseError(const string &message) const
{
    cout << NEWLINE "[CSS PARSER]" NEWLINE;

    if (!currentToken()->isEof()) {
        cout << "Parse error: Unexpected token '";

        // This is needed because '@' is not part of the at rule token content
        // and '#' also is not part of the hash token.
        if (currentToken()->isAtKeyword())
            cout << '@';
        else if (currentToken()->isHashLiteral())
            cout << '#';

        cout << currentToken()->content()
             << "' on row "
             << currentToken()->row()
             << " column "
             << currentToken()->column()
             << NEWLINE
             << (m_file_name.empty() ? "" : "in file '" + m_file_name + "'" NEWLINE)
             << message
             << NEWLINE
             << endl;
    } else {
        cout << "Parse error on row "
             << currentToken()->row()
             << " column "
             << currentToken()->column()
             << NEWLINE
             << (m_file_name.empty() ? "" : "in file " + m_file_name + NEWLINE)
             << message
             << NEWLINE
             << endl;
    }

    exit(1);
}
