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

#ifndef CSSCUSTOMPROPERTY_H
#define CSSCUSTOMPROPERTY_H
#include "CssIdentifier.h"

namespace CSS {
namespace Parsing {
namespace Elements {

class CssCustomProperty final : public CssIdentifier
{
public:
    using CssIdentifier::CssIdentifier;

    explicit
    CssCustomProperty(const string &name);

    inline void
    accept(CssVisitorInterface &visitor) override;
};

inline void
CssCustomProperty::
accept(CssVisitorInterface &visitor)
{
    visitor.visit(static_pointer_cast<CssCustomProperty>(shared_from_this()));
}

using CssCustomPropertyPtr = shared_ptr<CssCustomProperty>;

} // namespace Elements
} // namespace Parsing
} // namespace CSS

#endif // CSSCUSTOMPROPERTY_H
