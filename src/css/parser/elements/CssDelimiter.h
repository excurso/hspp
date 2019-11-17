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

#ifndef CSSDELIMITER_H
#define CSSDELIMITER_H
#include "CssBaseElement.h"

namespace CSS {
namespace Parsing {
namespace Elements {

class CssDelimiter : public CssBaseElement
{
public:
    explicit
    CssDelimiter(string value);

    inline void
    accept(CssVisitorInterface &visitor) override,

    setValue(const string &value);

    inline const string &
    value() const;

private:
    string m_delimiter_value;
};

inline void
CssDelimiter::
accept(CssVisitorInterface &visitor)
{
    visitor.visit(static_pointer_cast<CssDelimiter>(shared_from_this()));
}

inline void
CssDelimiter::
setValue(const string &value)
{
    m_delimiter_value.reserve(value.length());
    m_delimiter_value = value;
}

inline const string &
CssDelimiter::
value() const
{
    return m_delimiter_value;
}

using CssDelimiterPtr = shared_ptr<CssDelimiter>;

} // namespace Elements
} // namespace Parsing
} // namespace CSS

#endif // CSSDELIMITER_H
