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

#ifndef CSSSUPPORTSCONDITION_H
#define CSSSUPPORTSCONDITION_H
#include "CssBlock.h"
#include "CssDeclaration.h"

namespace CSS {
namespace Parsing {
namespace Elements {

class CssSupportsCondition : public CssBaseElement
{
public:
    CssSupportsCondition();

    inline void
    accept(CssVisitorInterface &visitor) override,
    appendCondition(const CssBaseElementPtr &declaration);

    inline const CssBaseElementPtr
    conditionBlock() const;

private:
    CssBlockPtr m_condition_block;
};

inline void
CssSupportsCondition::
accept(CssVisitorInterface &visitor)
{
    visitor.visit(static_pointer_cast<CssSupportsCondition>(shared_from_this()));
}

inline void
CssSupportsCondition::
appendCondition(const CssBaseElementPtr &declaration)
{
    m_condition_block->appendElement(declaration);
}

inline const CssBaseElementPtr
CssSupportsCondition::
conditionBlock() const
{
    return m_condition_block;
}

using CssSupportsConditionPtr = shared_ptr<CssSupportsCondition>;

} // namespace Elements
} // namespace Parsing
} // namespace CSS

#endif // CSSSUPPORTSCONDITION_H
