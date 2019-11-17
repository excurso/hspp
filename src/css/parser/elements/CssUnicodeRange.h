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

#ifndef CSSUNICODERANGE_H
#define CSSUNICODERANGE_H
#include "CssBaseElement.h"

namespace CSS {
namespace Parsing {
namespace Elements {

class CssUnicodeRange : public CssBaseElement
{
public:
    explicit
    CssUnicodeRange(string value);

    inline void
    accept(CssVisitorInterface &visitor) override;

    inline const string &
    value() const;

private:
    string m_value;
};

inline void
CssUnicodeRange::
accept(CssVisitorInterface &visitor)
{
    visitor.visit(static_pointer_cast<CssUnicodeRange>(shared_from_this()));
}

inline const string &
CssUnicodeRange::
value() const
{
    return m_value;
}

using CssUnicodeRangePtr = shared_ptr<CssUnicodeRange>;

} // namespace Elements
} // namespace Parsing
} // namespace CSS

#endif // CSSUNICODERANGE_H
