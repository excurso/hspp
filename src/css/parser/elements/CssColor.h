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

#ifndef CSSCOLOR_H
#define CSSCOLOR_H
#include "CssBaseElement.h"

namespace CSS {
namespace Parsing {
namespace Elements {

class CssColor final : public CssBaseElement
{
public:
    enum ColorType { PREDEFINED_NAME, HEX_LITERAL };

    explicit
    CssColor(const ColorType color_type, string value);

    inline void
    accept(CssVisitorInterface &visitor) override,

    setColorType(const ColorType color_type),
    setValue(string value);

    ColorType
    colorType() const;

    const string &
    value() const;

private:
    ColorType m_color_type;
    string m_value;
};

inline void
CssColor::
accept(CssVisitorInterface &visitor)
{
    visitor.visit(static_pointer_cast<CssColor>(shared_from_this()));
}

inline void
CssColor::
setColorType(const ColorType color_type)
{
    m_color_type = color_type;
}

inline auto
CssColor::
colorType() const -> ColorType
{
    return m_color_type;
}

inline void
CssColor::
setValue(string value)
{
    m_value.reserve(value.length());
    m_value = move(value);
}

inline const string &
CssColor::
value() const
{
    return m_value;
}

using CssColorPtr = shared_ptr<CssColor>;

} // namespace Elements
} // namespace Parsing
} // namespace CSS

#endif // CSSCOLOR_H
