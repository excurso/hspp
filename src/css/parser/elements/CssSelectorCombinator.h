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

#ifndef CSSSELECTORCOMBINATOR_H
#define CSSSELECTORCOMBINATOR_H
//#include "CssBaseElement.h"
#include "CssSelector.h"

namespace CSS {
namespace Parsing {
namespace Elements {

class CssSelectorCombinator final : public CssBaseElement
{
public:
    enum CombinatorType : uint8_t {
        UNDEFINED, DESCENDANCY, CHILD, NEXT_SIBLING, SUBSEQUENT_SIBLING
    };

    using CssBaseElement::CssBaseElement;

    constexpr explicit
    CssSelectorCombinator(),
    CssSelectorCombinator(CombinatorType type);
    CssSelectorCombinator(CombinatorType type, CssBaseElementPtr left,  CssBaseElementPtr right);

    inline void
    accept(CssVisitorInterface &visitor) override,

    setLeft(const CssBaseElementPtr &left),
    setRight(const CssBaseElementPtr &right),
    setCombinatorType(CombinatorType combinator_type);

    inline CombinatorType
    combinatorType() const;

    inline const CssBaseElementPtr
    &left() const,
    &right() const;

    static inline CombinatorType
    getCombinatorType(const char combinator);

private:
    CssBaseElementPtr m_left, m_right;
    CombinatorType m_combinator_type {};
};

constexpr
CssSelectorCombinator::CssSelectorCombinator() :
    CssBaseElement(SELECTOR_COMBINATOR),
    m_left(nullptr), m_right(nullptr),
    m_combinator_type(UNDEFINED) {}

constexpr
CssSelectorCombinator::CssSelectorCombinator(CombinatorType type) :
    CssBaseElement(SELECTOR_COMBINATOR),
    m_left(nullptr), m_right(nullptr),
    m_combinator_type(type) {}

inline void
CssSelectorCombinator::
accept(CssVisitorInterface &visitor)
{
    visitor.visit(static_pointer_cast<CssSelectorCombinator>(shared_from_this()));
}

inline void
CssSelectorCombinator::
setCombinatorType(CombinatorType combinator_type)
{
    m_combinator_type = combinator_type;
}

inline auto
CssSelectorCombinator::
combinatorType() const -> CombinatorType
{
    return m_combinator_type;
}

inline void
CssSelectorCombinator::
setLeft(const CssBaseElementPtr &left)
{
    m_left = left;
}

inline auto
CssSelectorCombinator::
left() const -> const CssBaseElementPtr &
{
    return m_left;
}

inline void
CssSelectorCombinator::
setRight(const CssBaseElementPtr &right)
{
    m_right = right;
}

inline auto
CssSelectorCombinator::
right() const -> const CssBaseElementPtr &
{
    return m_right;
}

/*static*/ inline auto
CssSelectorCombinator::
getCombinatorType(const char combinator) -> CombinatorType
{
    switch (combinator) {
    case '>':
        return CHILD;
    case '+':
        return NEXT_SIBLING;
    case '~':
        return SUBSEQUENT_SIBLING;
    default:
        return UNDEFINED;
    }
}

using CssSelectorCombinatorPtr = shared_ptr<CssSelectorCombinator>;

} // namespace Elements
} // namespace Parsing
} // namespace CSS

#endif // CSSSELECTORCOMBINATOR_H
