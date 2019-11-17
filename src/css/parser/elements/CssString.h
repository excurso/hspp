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

#ifndef CSSSTRING_H
#define CSSSTRING_H
#include "CssBaseElement.h"

namespace CSS {
namespace Parsing {
namespace Elements {

class CssString : public CssBaseElement
{
public:
    explicit
    CssString(string value, const bool unquoted = false);

    inline void
    accept(CssVisitorInterface &visitor) override,

    setValue(const string &value),
    setUnquotedFlag(const bool unquoted = true);

    inline const string &
    value() const;

    inline bool
    unquoted() const;

private:
    string m_value;
    bool m_unquoted_flag {false};
};

inline void
CssString::
accept(CssVisitorInterface &visitor)
{
    visitor.visit(static_pointer_cast<CssString>(shared_from_this()));
}

inline void
CssString::
setValue(const string &value)
{
    m_value.reserve(value.length());
    m_value = value;
}

inline void
CssString::
setUnquotedFlag(const bool unquoted)
{
    m_unquoted_flag = unquoted;
}

inline const string &
CssString::
value() const
{
    return m_value;
}

inline bool
CssString::
unquoted() const
{
    return m_unquoted_flag;
}

using CssStringPtr = shared_ptr<CssString>;

} // namespace Elements
} // namespace Parsing
} // namespace CSS

#endif // CSSSTRING_H
