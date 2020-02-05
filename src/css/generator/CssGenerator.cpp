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
*******************************************************************************/

#include "CssGenerator.h"
using namespace CSS::Generation;

CssGenerator::CssGenerator(string &minified_content) :
    m_output_buffer_ptr(make_shared<string>(minified_content)) {}

CssGenerator::CssGenerator(shared_ptr<string> minified_content) :
    m_output_buffer_ptr(move(minified_content)) {}

void
CssGenerator::
visit(const CssAtRulePtr &at_rule)
{
    if (at_rule->replacementElement()) {
        at_rule->replacementElement()->accept(*this);
        return;
    }

    m_output_buffer += '@';
    m_output_buffer += at_rule->keyword();

    pushContext(AT_RULE_EXPRESSION_LIST);

    if (at_rule->expressions()) {
        for (const auto &list : *at_rule->expressions()) {
            for (const auto &element : *list) {
                if (m_beautify ||
                    &element != &list->front() ||
                    !element->isOfType(CssBaseElement::BLOCK))
                        m_output_buffer += ' ';

                element->accept(*this);
            }

            if (&list != &at_rule->expressions()->back()) {
                m_output_buffer += ',';

                if (m_beautify)
                    m_output_buffer += "\n      ";
            }
        }
    }

    popContext();

    if (at_rule->block()) {
        if (m_beautify)
            m_output_buffer += ' ';

        m_output_buffer += '{';

        if (m_beautify) {
            m_output_buffer += '\n';
            if (at_rule->expressions()->size() > 1)
                m_output_buffer += '\n';

            ++s_indent_width;
        }

        for (const auto &element : at_rule->block()->elements()) {
            if (m_beautify)
                m_output_buffer += String::repeatChar('\t', s_indent_width);

            element->accept(*this);

            if (!element->isQualifiedRule() &&
                !element->isAtRule() &&
                !element->isComment() &&
                &element != &at_rule->block()->elements().back())
                m_output_buffer += ';';

            if (m_beautify && m_output_buffer.back() != '\n') {
                m_output_buffer += '\n';
            }
        }

        if (m_beautify) {
            --s_indent_width;
            m_output_buffer += String::repeatChar('\t', s_indent_width);
        }

        if (m_output_buffer.back() == '\n' && *(m_output_buffer.end()-2) == '\n')
            m_output_buffer.pop_back();

        m_output_buffer += '}';
    }
    else
        m_output_buffer += ';';

    if (m_beautify)
        m_output_buffer += "\n\n";
}

void
CssGenerator::
visit(const CssBlockPtr &block)
{
    if (block->blockType() == CssBlock::STYLESHEET)
        if (!m_beautify)
            m_output_buffer += OUTPUT_FILE_HEADER "\n";

    switch (block->blockType()) {
    case CssBlock::CURLY:
        if (m_beautify)
            m_output_buffer += ' ';

        m_output_buffer += '{';

        if (m_beautify) {
            m_output_buffer += '\n';
            ++s_indent_width;
        }

        break;
    case CssBlock::SQUARE:
        m_output_buffer += '[';
        break;
    case CssBlock::PAREN:
        m_output_buffer += '(';
        break;
    default:;
    }

    for (const auto &element : block->elements()) {
        if (m_beautify)
            m_output_buffer += String::repeatChar('\t', s_indent_width);

        if (element->isIdentifier() && block->elements().size() != 1 && &element != &block->elements().front())
            m_output_buffer += ' ';

        element->accept(*this);

        if (&element != &block->elements().back() &&
            element->isDeclaration()) {
            m_output_buffer += ';';
        }

        if (m_beautify && block->blockType() == CssBlock::CURLY) {
            m_output_buffer += '\n';

            if (&element == &block->elements().back()) {
                --s_indent_width;
                m_output_buffer += String::repeatChar('\t', s_indent_width);
            }
        }

        if (element->isIdentifier() && block->elements().size() != 1)
            m_output_buffer += ' ';
    }

    switch (block->blockType()) {
    case CssBlock::STYLESHEET:
        while (m_output_buffer.back() == '\n')
            m_output_buffer.pop_back();

        break;
    case CssBlock::CURLY:
        m_output_buffer += '}';

        if (m_beautify)
            m_output_buffer += "\n\n";

        break;
    case CssBlock::SQUARE:
        m_output_buffer += ']';
        break;
    case CssBlock::PAREN:
        m_output_buffer += ')';
        break;
    default:;
    }
}

void
CssGenerator::
visit(const CssDeclarationPtr &declaration)
{
    pushContext(DECLARATION);

    declaration->namePtr()->accept(*this);

    m_output_buffer += ':';

    if (m_beautify) m_output_buffer += ' ';

    for (const auto &list : declaration->values()) {
        for (const auto &value : list) {
            value->accept(*this);

            if (&value != &list.back() && !value->isDelimiter())
                m_output_buffer += ' ';
        }

        if (&list != &declaration->values().back())
            m_output_buffer += ',';
    }

    if (declaration->isImportant()) {
        if (m_beautify) m_output_buffer += ' ';
        m_output_buffer += "!important";
    }
    else if (!declaration->importantHack().empty()) {
        if (m_beautify) m_output_buffer += ' ';
        m_output_buffer += "!" + declaration->importantHack();
    }

    popContext();
}

void
CssGenerator::
visit(const CssPercentagePtr &percentage)
{
    if (percentage->replacementElement()) {
        percentage->replacementElement()->accept(*this);
        return;
    }

    visit(static_pointer_cast<CssNumber>(percentage));

    m_output_buffer += '%';
}

void
CssGenerator::
visit(const CssDimensionPtr &dimension)
{
    if (dimension->replacementElement()) {
        dimension->replacementElement()->accept(*this);
        return;
    }

    visit(static_pointer_cast<CssNumber>(dimension));

    m_output_buffer += dimension->unit();
}

void
CssGenerator::
visit(const CssFunctionPtr &function)
{
    if (function->replacementElement()) {
        function->replacementElement()->accept(*this);
        return;
    }

    m_output_buffer += function->name();
    m_output_buffer += '(';

    if (function->name() == "calc" ||
        function->name() == "min" ||
        function->name() == "max" ||
        function->name() == "clamp" ||
        function->name() == "alpha") {

        for (const auto &list : function->parameters()) {
            for (const auto &element : list) {
                element->accept(*this);
            }

            if (&list != &function->parameters().back())
                m_output_buffer += ',';
        }

        m_output_buffer += ')';

        return;
    }

    for (const auto &list : function->parameters()) {
        for (const auto &element : list) {
            element->accept(*this);

            if (&element != &list.back() && !element->isDelimiter())
                m_output_buffer += ' ';
        }

        if (&list != &function->parameters().back())
            m_output_buffer += ',';
    }

    m_output_buffer += ')';
}

void
CssGenerator::
visit(const CssIdentifierPtr &identifier)
{
#ifdef DEBUG
    if (identifier->value().empty())
        cerr << "CssGenerator::visit(const CssIdentifierPtr &identifier):" NEWLINE
             << "Empty identifier" << endl;
#endif

    if (identifier->replacementElement()) {
        identifier->replacementElement()->accept(*this);
        return;
    }

    m_output_buffer += identifier->value();
}

void
CssGenerator::
visit(const CssCustomPropertyPtr &custom_property)
{
    if (custom_property->replacementElement()) {
        custom_property->replacementElement()->accept(*this);
        return;
    }

    m_output_buffer += "--";
    m_output_buffer += custom_property->value();
}

void
CssGenerator::
visit(const CssNumberPtr &number)
{
    if (number->replacementElement()) {
        number->replacementElement()->accept(*this);
        return;
    }

    if (number->isNegative()) m_output_buffer += '-';
    m_output_buffer += number->value();
    m_output_buffer += number->scientificPostfix();
}

void
CssGenerator::
visit(const CssColorPtr &color)
{
    m_output_buffer += color->colorType() == CssColor::HEX_LITERAL ? "#" : "";
    m_output_buffer += color->value();
}

void
CssGenerator::
visit(const CssQualifiedRulePtr &qualified_rule)
{
    pushContext(SELECTOR_LIST);

    for (const auto &selector : qualified_rule->selectors()) {
        selector->accept(*this);

        if (&selector != &qualified_rule->selectors().back()) {
            m_output_buffer += ',';

            if (m_beautify) {
                m_output_buffer += '\n';
                m_output_buffer += String::repeatChar('\t', s_indent_width);
            }
        }
    }

    popContext();

    qualified_rule->block()->accept(*this);
}

void
CssGenerator::
visit(const CssStringPtr &string)
{
    char quote = '"';

    if (String::contains(string->value(), '"')) {
        quote = '\'';
    }

    if (!string->unquoted())
        m_output_buffer += quote;

    m_output_buffer += string->value();

    if (!string->unquoted())
        m_output_buffer += quote;
}

void
CssGenerator::
visit(const CssSelectorPtr &selector)
{
    if (selector->replacementElement()) {
        selector->replacementElement()->accept(*this);
        return;
    }

    if (selector->parentalSelector())
        selector->parentalSelector()->accept(*this);

    switch (selector->selectorType()) {
    case CssSelector::ID:
        m_output_buffer += '#';
        break;
    case CssSelector::CLASS:
        m_output_buffer += '.';
        break;
    case CssSelector::PSEUDO_CLASS:
        m_output_buffer += ':';
        break;
    case CssSelector::PSEUDO_ELEMENT:
        m_output_buffer += "::";
        break;
    case CssSelector::UNIVERSAL:
        if (!selector->childSelector())
            m_output_buffer += '*';
        return;
    default:;
    }

    if (selector->selectorType() != CssSelector::UNIVERSAL)
        m_output_buffer += selector->name();

    if (selector->selectorType() == CssSelector::PSEUDO_CLASS) {
        if (selector->subSelectors() && !selector->subSelectors()->empty()) {
            m_output_buffer += '(';

            for (const auto &subselector : *selector->subSelectors()) {
                subselector->accept(*this);

                if (&subselector != &selector->subSelectors()->back())
                    m_output_buffer += ',';
            }

            m_output_buffer += ')';
        }
    }
    else if (selector->selectorType() == CssSelector::AN_PLUS_B) {
        if (selector->subSelectors() && !selector->subSelectors()->empty()) {
            m_output_buffer += " of ";
            for (const auto &subselector : *selector->subSelectors())
                subselector->accept(*this);
        }
    }
}

void
CssGenerator::
visit(const CssSelectorAttributePtr &attribute_selector)
{
    if (attribute_selector->replacementElement()) {
        attribute_selector->replacementElement()->accept(*this);
        return;
    }

    if (attribute_selector->parentalSelector())
        attribute_selector->parentalSelector()->accept(*this);

    m_output_buffer += "[";
    m_output_buffer += attribute_selector->attributeName();

    if (!attribute_selector->attributeValue().empty()) {
        switch (attribute_selector->operation()) {
        case CssSelectorAttribute::EQUAL:
            m_output_buffer += "=";
            break;
        case CssSelectorAttribute::ROOF_EQUAL:
            m_output_buffer += "^=";
            break;
        case CssSelectorAttribute::DOLLAR_EQUAL:
            m_output_buffer += "$=";
            break;
        case CssSelectorAttribute::PIPE_EQUAL:
            m_output_buffer += "|=";
            break;
        case CssSelectorAttribute::TILDE_EQUAL:
            m_output_buffer += "~=";
            break;
        case CssSelectorAttribute::ASTERISK_EQUAL:
            m_output_buffer += "*=";
            break;
        case CssSelectorAttribute::NONE:;
        }

        const auto quote = String(attribute_selector->attributeValue()).containsOneOfChars(" \"\'=<>`") ||
                           bool(isdigit(attribute_selector->attributeValue().front()));

        m_output_buffer += quote ? "\"" : "";
        m_output_buffer += attribute_selector->attributeValue();
        m_output_buffer += quote ? "\"" : "";
    }

    if (attribute_selector->caseInsensitive())
        m_output_buffer += " i";

    m_output_buffer += "]";
}

void
CssGenerator::
visit(const CssSelectorCombinatorPtr &selector_combinator)
{
    if (selector_combinator->left()->isOfType(CssBaseElement::SELECTOR)) {
        const auto &selector = CssSelector::fromBase(selector_combinator->left());

        if (selector->selectorType() == CssSelector::UNIVERSAL)
            m_output_buffer += '*';
        else
            selector_combinator->left()->accept(*this);
    } else {
        selector_combinator->left()->accept(*this);
    }

    if (m_beautify && selector_combinator->combinatorType() != CssSelectorCombinator::DESCENDANCY)
        m_output_buffer += ' ';

    switch (selector_combinator->combinatorType()) {
    case CssSelectorCombinator::DESCENDANCY:
        switch (selector_combinator->left()->type()) {
        case CssBaseElement::SELECTOR_COMBINATOR:
        case CssBaseElement::SELECTOR:
            m_output_buffer += ' ';
            break;
        default:;
        }
        break;
    case CssSelectorCombinator::CHILD:
        m_output_buffer += '>';
        break;
    case CssSelectorCombinator::NEXT_SIBLING:
        m_output_buffer += '+';
        break;
    case CssSelectorCombinator::SUBSEQUENT_SIBLING:
        m_output_buffer += '~';
        break;
    default:;
    }

    if (m_beautify && selector_combinator->combinatorType() != CssSelectorCombinator::DESCENDANCY)
        m_output_buffer += ' ';

    if (selector_combinator->right()->isOfType(CssBaseElement::SELECTOR)) {
        const auto &selector = CssSelector::fromBase(selector_combinator->right());

        if (selector->selectorType() == CssSelector::UNIVERSAL) {
            m_output_buffer += '*';
            return;
        }
    }

    selector_combinator->right()->accept(*this);
}

void
CssGenerator::
visit(const CssDelimiterPtr &delimiter)
{
    if (bool(isspace(m_output_buffer.back())))
        m_output_buffer.pop_back();

    if (delimiter->value() == "+" || delimiter->value() == "-")
        m_output_buffer += ' ';

    m_output_buffer += delimiter->value();

    if (delimiter->value() == "+" || delimiter->value() == "-")
        m_output_buffer += ' ';
}

void
CssGenerator::
visit(const CssUnicodeRangePtr &unicode_range)
{
    m_output_buffer += unicode_range->value();
}

void
CssGenerator::
visit(const CssSupportsConditionPtr &supports_condition)
{
    m_output_buffer += "supports";
    supports_condition->conditionBlock()->accept(*this);
}

void
CssGenerator::
visit(const CssCommentPtr &comment)
{
    if (comment->commentType() == CssComment::COMMENT &&
        !m_output_buffer.empty() &&
        m_output_buffer.back() != '\n' &&
        !context({DECLARATION, SELECTOR_LIST, AT_RULE_EXPRESSION_LIST}))
        m_output_buffer += '\n';

    m_output_buffer += "/*";
    m_output_buffer += comment->value();
    m_output_buffer += "*/";

    if (comment->commentType() == CssComment::COMMENT &&
        !context({DECLARATION, SELECTOR_LIST, AT_RULE_EXPRESSION_LIST}))
        m_output_buffer += '\n';
}
