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

#ifndef CSSFUNCTION_H
#define CSSFUNCTION_H
#include "CssIdentifier.h"
#include "../../../DataContainer.h"

namespace CSS {
namespace Parsing {
namespace Elements {

class CssFunction final : public CssBaseElement
{
public:
    explicit
    CssFunction(const string &name);

    inline void
    accept(CssVisitorInterface &visitor) override,
    setName(const string &name),
    setName(const shared_ptr<string> &name),
    appendParameter(const DataContainer<CssBaseElementPtr> &element);

    inline const string &
    name() const;

    inline bool
    name(const string &name) const,
    name(const initializer_list<string> candidates) const;

    inline DataContainer<DataContainer<CssBaseElementPtr> > &
    parameters();

private:
    CssIdentifier m_name;
    DataContainer<DataContainer<CssBaseElementPtr> > m_parameters;
};

inline void
CssFunction::
accept(CssVisitorInterface &visitor)
{
    visitor.visit(static_pointer_cast<CssFunction>(shared_from_this()));
}

inline void
CssFunction::
setName(const string &name)
{
    m_name.setValue(name);
}

inline void
CssFunction::
setName(const shared_ptr<string> &name)
{
    m_name.setValue(name);
}

inline const string &
CssFunction::
name() const
{
    return m_name.value();
}

inline bool
CssFunction::
name(const string &name) const
{
    return m_name.value() == name;
}

inline bool
CssFunction::
name(const initializer_list<string> candidates) const
{
    for (const auto &candidate : candidates)
        if (candidate == m_name.value())
            return true;

    return false;
}

inline void
CssFunction::
appendParameter(const DataContainer<CssBaseElementPtr> &element)
{
    m_parameters.emplace_back(element);
}

inline DataContainer<DataContainer<CssBaseElementPtr> > &
CssFunction::
parameters()
{
    return m_parameters;
}

using CssFunctionPtr = shared_ptr<CssFunction>;

} // namespace Elements
} // namespace Parsing
} // namespace CSS

#endif // CSSFUNCTION_H
