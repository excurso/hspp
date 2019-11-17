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

#ifndef CSSIDENTIFIER_H
#define CSSIDENTIFIER_H
#include "CssBaseElement.h"

namespace CSS {
namespace Parsing {
namespace Elements {

class CssIdentifier : public CssBaseElement
{
public:
    using CssBaseElement::CssBaseElement;

    explicit
    CssIdentifier(const string &value),
    CssIdentifier(shared_ptr<string> value),
    CssIdentifier(const ElementType type, const string &value),
    CssIdentifier(const ElementType type, shared_ptr<string> value);

    inline void
    accept(CssVisitorInterface &visitor) override,
    setValue(const string &value),
    setValue(const shared_ptr<string> &value);

    inline const string &
    value() const;

    inline bool
    value(const string &value) const,
    value(const initializer_list<string> candidates) const;

	inline const shared_ptr<string> &
	valuePtr();

private:
    shared_ptr<string> m_value;
};

inline void
CssIdentifier::
accept(CssVisitorInterface &visitor)
{
    visitor.visit(static_pointer_cast<CssIdentifier>(shared_from_this()));
}

inline void
CssIdentifier::
setValue(const string &value)
{
    m_value->reserve(value.length());
    *m_value = value;
}

inline void
CssIdentifier::
setValue(const shared_ptr<string> &value)
{
    m_value = value;
}

inline const string &
CssIdentifier::
value() const
{
    return *m_value;
}

inline bool
CssIdentifier::
value(const string &value) const
{
    return *m_value == value;
}

inline bool
CssIdentifier::
value(const initializer_list<string> candidates) const
{
    for (const auto &candidate: candidates)
        if (*m_value == candidate)
            return true;

    return false;
}

inline const shared_ptr<string> &
CssIdentifier::
valuePtr()
{
	return m_value;
}

using CssIdentifierPtr = shared_ptr<CssIdentifier>;

} // namespace Elements
} // namespace Parsing
} // namespace CSS

#endif // CSSIDENTIFIER_H
