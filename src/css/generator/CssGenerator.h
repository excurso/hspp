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

#ifndef CSSGENERATOR_H
#define CSSGENERATOR_H
#include "../../config/Config.h"
#include "../../defs.h"
#include "../../general/visitor/VisitorInterface.h"
#include "../parser/includes.h"
#include <stack>

namespace CSS {
namespace Generation {
using namespace CSS::Parsing::Elements;

using CssVisitorInterface =
    VisitorInterface<CssAtRulePtr, CssBlockPtr, CssDeclarationPtr, CssPercentagePtr,
                     CssDimensionPtr, CssFunctionPtr, CssIdentifierPtr, CssCustomPropertyPtr,
                     CssNumberPtr, CssColorPtr, CssQualifiedRulePtr, CssStringPtr,
                     CssSelectorPtr, CssSelectorAttributePtr, CssSelectorCombinatorPtr,
                     CssDelimiterPtr, CssUnicodeRangePtr, CssSupportsConditionPtr, CssCommentPtr>;

class CssGenerator : public CssVisitorInterface
{
public:
    explicit
    CssGenerator(string &minified_content),
    CssGenerator(shared_ptr<string> minified_content);

    using CssVisitorInterface::CssVisitorInterface;

    void
    visit(const CssAtRulePtr &)               override,
    visit(const CssBlockPtr &)                override,
    visit(const CssDeclarationPtr &)          override,
    visit(const CssPercentagePtr &)           override,
    visit(const CssDimensionPtr &)            override,
    visit(const CssFunctionPtr &)             override,
    visit(const CssIdentifierPtr &)           override,
    visit(const CssCustomPropertyPtr &)       override,
    visit(const CssNumberPtr &)               override,
    visit(const CssColorPtr &)                override,
    visit(const CssQualifiedRulePtr &)        override,
    visit(const CssStringPtr &)               override,
    visit(const CssSelectorPtr &)             override,
    visit(const CssSelectorAttributePtr &)    override,
    visit(const CssSelectorCombinatorPtr &)   override,
    visit(const CssDelimiterPtr &)            override,
    visit(const CssUnicodeRangePtr &)         override,
    visit(const CssSupportsConditionPtr &)    override,
    visit(const CssCommentPtr &)              override;

    inline const string &
    outputBuffer() const;

    inline void
    clearOutputBuffer();

private:
    /// Reference to buffer for output content
    shared_ptr<string> m_output_buffer_ptr;
    string &m_output_buffer {*m_output_buffer_ptr};

    enum Context {STYLESHEET, DECLARATION, SELECTOR_LIST, AT_RULE_EXPRESSION_LIST};

    inline void
    pushContext(const Context context),
    popContext();

    inline bool
    context(const Context context) const,
    context(const initializer_list<Context> candidates) const;

    stack<Context> m_context_stack {{STYLESHEET}};

    bool m_beautify {cfg.isEnabled(Config::GENERAL__BEAUTIFY_OUTPUT)};
};

static uint8_t s_indent_width = 0;

inline void
CssGenerator::
pushContext(const Context context)
{
    m_context_stack.emplace(context);
}

inline void
CssGenerator::
popContext()
{
    m_context_stack.pop();
}

inline bool
CssGenerator::
context(const Context context) const
{
    return m_context_stack.top() == context;
}

inline bool
CssGenerator::
context(const initializer_list<Context> candidates) const
{
    for (const auto &candidate : candidates)
        if (m_context_stack.top() == candidate)
            return true;

    return false;
}

inline const string &
CssGenerator::
outputBuffer() const
{
    return m_output_buffer;
}

inline void
CssGenerator::
clearOutputBuffer()
{
    m_output_buffer.clear();
}

} // namespace Generation
} // namespace CSS

#endif // CSSGENERATOR_H
