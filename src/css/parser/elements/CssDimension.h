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

#ifndef CSSDIMENSION_H
#define CSSDIMENSION_H
#include "CssNumber.h"

namespace CSS {
namespace Parsing {
namespace Elements {

class CssDimension final : public CssNumber
{
public:
    explicit
    CssDimension(string number, string unit);

    inline void
    accept(CssVisitorInterface &visitor) override,
    setUnit(const string &unit);

    inline const string &
    unit() const;

    inline bool
	unit(const string &unit_value) const,
    unit(const initializer_list<string> candidates) const;

private:
    string m_unit;
};

inline void
CssDimension::
accept(CssVisitorInterface &visitor)
{
    visitor.visit(static_pointer_cast<CssDimension>(shared_from_this()));
}

inline void
CssDimension::
setUnit(const string &unit)
{
    m_unit.reserve(unit.length());
    m_unit = unit;
}

inline const string &
CssDimension::
unit() const
{
    return m_unit;
}

inline bool
CssDimension::
unit(const string &unit) const
{
	return m_unit == unit;
}

inline bool
CssDimension::
unit(const initializer_list<string> candidates) const
{
    for (const auto &candidate : candidates)
        if (m_unit == candidate)
            return true;

    return false;
}

using CssDimensionPtr = shared_ptr<CssDimension>;

} // namespace Elements
} // namespace Parsing
} // namespace CSS

#endif // CSSDIMENSION_H
