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

#ifndef CSSQUALIFIEDRULE_H
#define CSSQUALIFIEDRULE_H
#include "CssBlock.h"
#include "../../../DataContainer.h"
#include "CssSelectorCombinator.h"

namespace CSS {
namespace Parsing {
namespace Elements {

class CssQualifiedRule final : public CssBaseElement
{
public:
    explicit
    CssQualifiedRule(),
    CssQualifiedRule(DataContainer<CssBaseElementPtr> selector_list);

    inline void
    accept(CssVisitorInterface &visitor) override,

    setSelectors(const DataContainer<CssBaseElementPtr> &selector_list),
    appendSelector(const CssBaseElementPtr &selector_combinator),
    setBlock(const CssBaseElementPtr &block);

    inline DataContainer<CssBaseElementPtr> &
    selectors();

    const CssBlockPtr &
    block() const;

private:
    DataContainer<CssBaseElementPtr> m_selector_list;
    CssBlockPtr m_block;
};

inline void
CssQualifiedRule::
accept(CssVisitorInterface &visitor)
{
    visitor.visit(static_pointer_cast<CssQualifiedRule>(shared_from_this()));
}

inline void
CssQualifiedRule::
setBlock(const CssBaseElementPtr &block)
{
    m_block = static_pointer_cast<CssBlock>(block);
}

inline const CssBlockPtr &
CssQualifiedRule::
block() const
{
    return m_block;
}

inline void
CssQualifiedRule::
setSelectors(const DataContainer<CssBaseElementPtr> &selector_list)
{
    m_selector_list.reserve(selector_list.size());
    m_selector_list = selector_list;
}

inline void
CssQualifiedRule::
appendSelector(const CssBaseElementPtr &selector_combinator)
{
    m_selector_list.emplace_back(selector_combinator);
}

inline DataContainer<CssBaseElementPtr> &
CssQualifiedRule::
selectors()
{
    return m_selector_list;
}

using CssQualifiedRulePtr = shared_ptr<CssQualifiedRule>;

} // namespace Elements
} // namespace Parsing
} // namespace CSS

#endif // CSSQUALIFIEDRULE_H
