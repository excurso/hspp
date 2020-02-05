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

#ifndef CSSSELECTOR_H
#define CSSSELECTOR_H
#include "CssIdentifier.h"
#include "../../../DataContainer.h"

namespace CSS {
namespace Parsing {
namespace Elements {

class CssSelector : public CssBaseElement
{
public:
    using CssSelectorPtr = shared_ptr<CssSelector>;

    enum SelectorType : uint8_t { NONE, UNIVERSAL, TYPE, ID, CLASS, ATTRIBUTE, PSEUDO_CLASS,
                                  AN_PLUS_B, PSEUDO_ELEMENT, KEYFRAMES };

    explicit
    CssSelector(const SelectorType selector_type),
    CssSelector(const SelectorType selector_type, string name),
    CssSelector(const SelectorType selector_type, CssSelectorPtr parental_selector, string name);

    static inline CssSelectorPtr
    /// Cast CssBaseElementPtr to CssSelectorPtr
    fromBase(const CssBaseElementPtr &element);

    inline void
    accept(CssVisitorInterface &visitor) override,

    setSelectorType(const SelectorType selector_type),
    setSubselectorList(const shared_ptr<DataContainer<CssSelectorPtr> > &subselector_list),
    setName(const string &name),
    setName(string *name),
    setName(const shared_ptr<string> &name),

    setParentalSelector(const CssSelectorPtr &parental_selector),
    appendSubSelector(const CssSelectorPtr &sub_selector),
    setChildSelector(const CssSelectorPtr &child_selector);

    inline SelectorType
    selectorType() const;

    inline bool
    isOfType(const SelectorType selector_type) const,
    isOfType(const initializer_list<SelectorType> candidates) const;

    inline const string &
    name() const;

    inline const shared_ptr<string> &
    namePtr() const;

    inline const CssSelectorPtr
    &parentalSelector() const,
    &childSelector() const;

    inline const shared_ptr<DataContainer<CssSelectorPtr> > &
    subSelectors() const;

protected:
    SelectorType m_selector_type;
    shared_ptr<string> m_name;

    CssSelectorPtr
    // Example: .parental_selector.current_selector { ... }
    m_parental_selector,
    // Example: .current_selector.child_selector { ... }
    m_child_selector;

    // Used in pseudo-class selectors
    shared_ptr<DataContainer<CssSelectorPtr> > m_sub_selectors;
};

using CssSelectorPtr = shared_ptr<CssSelector>;

inline void
CssSelector::
accept(CssVisitorInterface &visitor)
{
    visitor.visit(static_pointer_cast<CssSelector>(shared_from_this()));
}

/*static*/ inline CssSelectorPtr
CssSelector::
fromBase(const CssBaseElementPtr &element)
{
    return static_pointer_cast<CssSelector>(element);
}

inline void
CssSelector::
setSelectorType(const SelectorType selector_type)
{
    m_selector_type = selector_type;
}

inline auto
CssSelector::
selectorType() const -> SelectorType
{
    return m_selector_type;
}

inline bool
CssSelector::
isOfType(const SelectorType selector_type) const
{
    return m_selector_type == selector_type;
}

inline bool
CssSelector::
isOfType(const initializer_list<SelectorType> candidates) const
{
    for (const auto &candidate : candidates)
        if (m_selector_type == candidate)
            return true;

    return false;
}

inline void
CssSelector::
setName(const shared_ptr<string> &name)
{
    m_name->reserve(name->length());
    m_name = name;
}

inline void
CssSelector::
setName(const string &name)
{
    m_name->reserve(name.length());
    *m_name = name;
}

inline void
CssSelector::
setName(string *name)
{
    m_name.reset(name);
}

inline const string &
CssSelector::
name() const
{
    return *m_name;
}

inline const shared_ptr<string> &
CssSelector::
namePtr() const
{
    return m_name;
}

inline void
CssSelector::
setParentalSelector(const CssSelectorPtr &parental_selector)
{
    m_parental_selector = parental_selector;
}

inline const CssSelectorPtr &
CssSelector::
parentalSelector() const
{
    return m_parental_selector;
}

inline void
CssSelector::
setChildSelector(const CssSelectorPtr &child_selector)
{
    m_child_selector = child_selector;
}

inline const CssSelectorPtr &
CssSelector::
childSelector() const
{
    return m_child_selector;
}

inline void
CssSelector::
appendSubSelector(const CssSelectorPtr &sub_selector)
{
    if (!m_sub_selectors) m_sub_selectors = make_shared<DataContainer<CssSelectorPtr> >();

    m_sub_selectors->emplace_back(sub_selector);
}

inline void
CssSelector::
setSubselectorList(const shared_ptr<DataContainer<CssSelectorPtr> > &subselector_list)
{
    if (!m_sub_selectors) m_sub_selectors = make_shared<DataContainer<CssSelectorPtr> >();
    m_sub_selectors->reserve(subselector_list->size());
    m_sub_selectors = subselector_list;
}

inline const shared_ptr<DataContainer<CssSelectorPtr> > &
CssSelector::
subSelectors() const
{
    return m_sub_selectors;
}

} // namespace Elements
} // namespace Parsing
} // namespace CSS

#endif // CSSSELECTOR_H
