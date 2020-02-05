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

#ifndef CSSDECLARATION_H
#define CSSDECLARATION_H
#include "CssCustomProperty.h"
#include "../../../DataContainer.h"

namespace CSS {
namespace Parsing {
namespace Elements {

class CssDeclaration final : public CssBaseElement
{
public:
    explicit
	CssDeclaration(const string &name),
    CssDeclaration(const shared_ptr<string> &name),
    CssDeclaration(CssIdentifierPtr name);

    inline void
    accept(CssVisitorInterface &visitor) override,

    setName(const string &name),
    setName(const shared_ptr<string> &name),
    setName(const CssIdentifierPtr &name),
    appendValue(const CssBaseElementPtr &value),
    setImportantHack(const string &important_hack),
    createList(),
    setImportantFlag();

    inline const string &
    name() const;

    inline const CssIdentifierPtr &
    namePtr();

    inline const string
    &importantHack() const;

    inline DataContainer<DataContainer<CssBaseElementPtr> > &
    values();

    inline bool
    name(initializer_list<string> candidates) const,
    isImportant() const;

private:
    // For IE hacks
    string m_important_hack;
	CssIdentifierPtr m_name;
	DataContainer<DataContainer<CssBaseElementPtr> > m_values;
    bool m_important_flag {false};
};

inline void
CssDeclaration::
accept(CssVisitorInterface &visitor)
{
    visitor.visit(static_pointer_cast<CssDeclaration>(shared_from_this()));
}

inline void
CssDeclaration::
setName(const string &name)
{
    m_name->setValue(name);
}

inline void
CssDeclaration::
setName(const shared_ptr<string> &name)
{
    m_name->setValue(name);
}

inline void
CssDeclaration::
setName(const CssIdentifierPtr &name) {
    m_name = name;
}

inline const string &
CssDeclaration::
name() const
{
    return m_name->value();
}

inline bool
CssDeclaration::
name(const initializer_list<string> candidates) const
{
    return m_name->value(candidates);
}

inline const CssIdentifierPtr &
CssDeclaration::
namePtr()
{
    return m_name;
}

inline void
CssDeclaration::
createList()
{
    m_values.emplace_back(DataContainer<CssBaseElementPtr>());
}

inline void
CssDeclaration::
appendValue(const CssBaseElementPtr &value)
{
    m_values.back().emplace_back(value);
}

inline DataContainer<DataContainer<CssBaseElementPtr> > &
CssDeclaration::
values()
{
    return m_values;
}

inline void
CssDeclaration::
setImportantFlag()
{
    m_important_flag = true;
}

inline bool
CssDeclaration::
isImportant() const
{
    return m_important_flag;
}

inline void
CssDeclaration::
setImportantHack(const string &important_hack)
{
    m_important_hack = important_hack;
}

inline const string &
CssDeclaration::
importantHack() const
{
    return m_important_hack;
}

using CssDeclarationPtr = shared_ptr<CssDeclaration>;

} // namespace Elements
} // namespace Parsing
} // namespace CSS

#endif // CSSDECLARATION_H
