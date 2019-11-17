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

#include "CssTokenizer.h"
using namespace CSS::Tokenization;

CssTokenizer::CssTokenizer(const shared_ptr<string> &content) :
    GeneralTokenizer(content), m_cdata_flag(false) {}

CssTokenizer::CssTokenizer(const string &content, const uint32_t begin_row, const uint32_t begin_column) :
    GeneralTokenizer(content, begin_row, begin_column), m_cdata_flag(false) {}

const GeneralTokenStreamPtr
CssTokenizer::
tokenize()
{
    while (!isEof()) {
        if (isWhiteSpace()) continue;
        if (isComment()) continue;
        if (isUnicodeRange()) continue;
        if (isIdentifier()) continue;
        if (isNumericLiteral()) continue;
        if (isPunctuator()) continue;
        if (isHashLiteral()) continue;
        if (isAtKeyword()) continue;
        if (isStringLiteral()) continue;
        if (isEscapeSequence()) continue;
        if (isNonAsciiIdentifier()) continue;

        // syntax error
    }

    appendToken(make_shared<CssToken>(CssToken::EOF));
    byteStream()->clear();

    return tokenStream();
}

const GeneralTokenStreamPtr
CssTokenizer::
tokenize(const shared_ptr<string> &content)
{
    return CssTokenizer(content).tokenize();
}

const GeneralTokenStreamPtr
CssTokenizer::
tokenize(const string &content, const uint32_t begin_row, const uint32_t begin_column)
{
    return CssTokenizer(content, begin_row, begin_column).tokenize();
}

inline bool
CssTokenizer::
isWhiteSpace()
{
    if (isSpaceChar()) {
        const uint64_t start_column = currentColumn();
        do advance(); while (isSpaceChar());

        // Check if recent token is not already a whitespace token.
        // Maybe there was a comment between whitespace which has been removed...
        if (!tokenStream()->empty() && !lastStreamToken()->isWhiteSpace()) {
            const auto whitespace_token = make_shared<CssToken>(CssToken::WHITESPACE);
            appendToken(whitespace_token, currentRow(), start_column);
        }

        return true;
    }

    return false;
}

inline bool
CssTokenizer::
isPunctuator()
{
    if (bool(ispunct(currentChar())) && !currentChar({'"', '\'', '#', '@', '\\'})) {
        const auto punctuator_token = make_shared<CssToken>(CssToken::PUNCTUATOR, string(1, currentChar()));
        appendToken(punctuator_token);
        advance();

        return true;
    }

    return false;
}

bool
CssTokenizer::
isIdentifier()
{
    if (bool(isalpha(currentChar())) || currentChar('_') || (currentChar('-') &&
        (nextChar('-') || bool(isalpha(nextChar())))) || currentChar('\\')) {
        uint64_t start_column = currentColumn();
        auto begin = getIterator();
        while (((bool(isalnum(currentChar())) || currentChar({'-', '_'})) && advance()) || (currentChar('\\') && isEscapeSequence()));

        const auto identifier_token = make_shared<CssToken>(CssToken::IDENTIFIER, string(begin, getIterator()));
        appendToken(identifier_token, currentRow(), start_column);

        if (currentChar('(')) {
            if (identifier_token->hasContent("url") && advance()) {
                const auto punctuator = make_shared<CssToken>(CssToken::PUNCTUATOR, '(');
                appendToken(punctuator);
                begin = getIterator();

                while (isSpaceChar()) advance();

                if (!currentChar({'"', '\''})) {
                    while (advance() && !(isEof() || currentChar(')')));

                    if (currentChar(')')) {
                        auto content = string(begin, getIterator());
                        content = String::trim(content);
                        const auto string_token = make_shared<CssToken>(CssToken::STRING_LITERAL, content);
                        appendToken(string_token);
                        const auto punctuator = make_shared<CssToken>(CssToken::PUNCTUATOR, ')');
                        appendToken(punctuator);

                        advance();
                    }
                } else if (!isStringLiteral()) return false;
            }
        }

        return true;
    }

    return false;
}

inline bool
CssTokenizer::
isAtKeyword()
{
    if (currentChar('@')) {
        const uint64_t start_column = currentColumn();
        advance();

        if (isIdentifier()) {
            const auto at_keyword_token = make_shared<CssToken>(CssToken::AT_KEYWORD, tokenStream()->back()->content());
            tokenStream()->pop_back();
            appendToken(at_keyword_token, currentRow(), start_column);

            return true;
        }
    }

    return false;
}

bool
CssTokenizer::
isStringLiteral()
{
    if (currentChar({'"', '\''})) {
        const uint64_t start_column = currentColumn();
        string str;

        if (GeneralTokenizer::isString(str)) {
            const auto string_token = make_shared<CssToken>(CssToken::STRING_LITERAL, string(str.begin()+1, str.end()-1));
            appendToken(string_token, currentRow(), start_column);

            return true;
        }
    }

    return false;
}

inline bool
CssTokenizer::
isNumericLiteral()
{
    bool got_dot = false;

    if (bool(isdigit(currentChar())) || (currentChar('.') && bool(isdigit(nextChar())) && (got_dot = true))) {
        auto begin = getIterator();
        while (advance() && bool(isdigit(currentChar())));

        if (currentChar('.') && !got_dot && (got_dot = true)) {
            if (bool(isdigit(nextChar())))
                while (advance() && bool(isdigit(currentChar())));
            else throwSyntaxError();
        }

        if (currentChar('.') && got_dot) throwSyntaxError();

        const auto number_token = make_shared<CssToken>(CssToken::NUMERIC_LITERAL, string(begin, getIterator()));
        appendToken(number_token);

        if (currentChar('e')) {
            begin = getIterator();
            if ((bool(isdigit(nextChar())) && advance()) ||
                (nextChar({'+', '-'}) && bool(isdigit(*getIterator(+2))) && advance(+2))) {

                while (advance() && bool(isdigit(currentChar())));

                const auto &scient_postfix_token = make_shared<CssToken>(CssToken::SCIENTIFIC_LITERAL, string(begin, getIterator()));
                appendToken(scient_postfix_token);
            }
        }

        if (bool(isalpha(currentChar()))) {
            begin = getIterator();
            while (advance() && bool(isalpha(currentChar())));

            const auto unit_token = make_shared<CssToken>(CssToken::UNIT, string(begin, getIterator()));
            appendToken(unit_token);
        }

        return true;
    }

    return false;
}

inline bool
CssTokenizer::
isHashLiteral()
{
    if (currentChar('#') && !isEof(+1)) {
        const uint64_t start_column = currentColumn();
        const auto begin = getIterator(+1);
        do advance(); while (!isEof() && (bool(isalnum(currentChar())) || currentChar({'-', '_'})));

        const auto hash_token = make_shared<CssToken>(CssToken::HASH_LITERAL, string(begin, getIterator()));
        appendToken(hash_token, currentRow(), start_column);

        return true;
    }

    return false;
}

inline bool
CssTokenizer::
isComment()
{
    if (currentChar('/') && nextChar('*')) {
        const uint64_t start_column = currentColumn();
        advance(+2);
        const auto begin = getIterator();

        while (!isEof(+1) && !(currentChar('*') && nextChar('/')) && advance());

        auto comment_content = string(begin, getIterator());
        auto comment_type = CssToken::COMMENT;

//        if (comment_content.find("<![CDATA[") != string::npos) {
//            comment_type = CssToken::CDATA_START_COMMENT;
//            m_cdata_flag = true;
//            comment_content = String::trim(comment_content);
//        } else if (comment_content.find("]]>") != string::npos) {
//            m_cdata_flag && (comment_type = CssToken::CDATA_END_COMMENT);
//            comment_content = String::trim(comment_content);
//        }

        if (currentChar('*') && nextChar('/')) {
            if (!cfg.isEnabled(Config::CSS__REMOVE_COMMENTS) ||
                (comment_type == CssToken::COMMENT &&
                 String(comment_content).contains(cfg.cssCommentTerms())) ||
                comment_type != CssToken::COMMENT) {

                const auto comment_token = make_shared<CssToken>(comment_type, comment_content);
                appendToken(comment_token, currentRow(), start_column);
            }

            advance(+2);
            return true;
        }
    }

    return false;
}

inline bool
CssTokenizer::
isUnicodeRange()
{
    // https://www.w3.org/TR/css-syntax-3/#urange

    if (currentChar({ 'u', 'U' }) && nextChar('+')) {
        const auto begin = getIterator();
        advance(+2);

        if (isHexDigit(currentChar())) {
            do advance(); while (isHexDigit(currentChar()));

            if (currentChar('?'))
                do advance(); while (currentChar('?'));
            else if (currentChar('-') && isHexDigit(nextChar()))
                do advance(); while (isHexDigit(currentChar()));

            const auto unicode_range_token = make_shared<CssToken>(CssToken::UNICODE_RANGE, String::toLower(string(begin, getIterator())));
            appendToken(unicode_range_token);

            return true;
        }

        throwSyntaxError();
    }

    return false;
}

bool
CssTokenizer::
isEscapeSequence()
{
    if (currentChar('\\')) {
        if (isHexDigit(nextChar()) && advance()) {
            const auto begin = getIterator();
            while (advance() && getIterator() != begin+6 && isHexDigit(currentChar()));
            const auto length = distance(begin, getIterator());

            length < 6 && isSpaceChar() && advance();

            return true;
        }

        advance();

        if (currentChar({ ' ', '!', '"', '#', '$', '%', '&', '\'', '(', ')', '*', '+',
                          ',', '-', '.', '/', ':', ';', '<', '=', '>', '?', '@', '[',
                          '\\',']', '^', '`', '{', '|', '}' })) {
            advance();
        } else throwSyntaxError();

        return true;
    }

    return false;
}

inline bool
CssTokenizer::
isNonAsciiIdentifier()
{
    if (currentChar() < 0) {
        const auto begin = getIterator();
        const auto non_ascii_identifier = make_shared<CssToken>(CssToken::IDENTIFIER);

        while (currentChar() < 0 && advance());

        non_ascii_identifier->setContent(string(begin, getIterator()));

        appendToken(non_ascii_identifier);
    }

    return false;
}

bool
CssTokenizer::
isHexDigit(const char c)
{
    return bool(isdigit(c)) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}
