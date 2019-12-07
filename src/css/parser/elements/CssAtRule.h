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

#ifndef CSSATRULE_H
#define CSSATRULE_H
#include "CssIdentifier.h"
#include "CssBlock.h"

namespace CSS {
namespace Parsing {
namespace Elements {

class CssAtRule final : public CssBaseElement
{
public:
    explicit
    CssAtRule(const string &keyword);

    inline void
    accept(CssVisitorInterface &visitor) override,

    setKeyword(const string &keyword),
    setBlock(const CssBaseElementPtr &block),
    createList(),
    setExpressions(const shared_ptr<DataContainer<DataContainer<CssBaseElementPtr> > > &expressions),
    appendExpression(const CssBaseElementPtr &component);

    inline const string &
    keyword() const;

    inline const shared_ptr<DataContainer<DataContainer<CssBaseElementPtr> > > &
    expressions() const;

    inline const CssBlockPtr &
    block() const;

private:
    CssIdentifier m_keyword;
    shared_ptr<DataContainer<DataContainer<CssBaseElementPtr> > > m_expression_lists;
    CssBlockPtr m_block;
};

inline void
CssAtRule::
accept(CssVisitorInterface &visitor)
{
    visitor.visit(static_pointer_cast<CssAtRule>(shared_from_this()));
}

inline void
CssAtRule::
setKeyword(const string &keyword)
{
    m_keyword.setValue(keyword);
}

inline const string &
CssAtRule::
keyword() const
{
    return m_keyword.value();
}

inline void
CssAtRule::
setBlock(const CssBaseElementPtr &block)
{
    m_block = static_pointer_cast<CssBlock>(block);
}

inline const CssBlockPtr &
CssAtRule::
block() const
{
    return m_block;
}

inline void
CssAtRule::
setExpressions(const shared_ptr<DataContainer<DataContainer<CssBaseElementPtr> > > &expressions)
{
    m_expression_lists = expressions;
}

inline void
CssAtRule::
appendExpression(const CssBaseElementPtr &component)
{
    if (!m_expression_lists) {
        m_expression_lists = make_shared<DataContainer<DataContainer<CssBaseElementPtr> > >();
        m_expression_lists->emplace_back(DataContainer<CssBaseElementPtr>());
    }

    m_expression_lists->back().emplace_back(component);
}

inline const shared_ptr<DataContainer<DataContainer<CssBaseElementPtr> > > &
CssAtRule::
expressions() const
{
    return m_expression_lists;
}

inline void
CssAtRule::
createList()
{
    m_expression_lists->emplace_back(DataContainer<CssBaseElementPtr>());
}

using CssAtRulePtr = shared_ptr<CssAtRule>;

} // namespace Elements
} // namespace Parsing
} // namespace CSS

#endif // CSSATRULE_H
