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

#ifndef CSSNUMBER_H
#define CSSNUMBER_H
#include "CssBaseElement.h"

namespace CSS {
namespace Parsing {
namespace Elements {

class CssNumber : public CssBaseElement
{
public:
    using CssBaseElement::CssBaseElement;

    explicit
    CssNumber(string value),
    CssNumber(const ElementType type, string value),
    CssNumber(const ElementType type, const shared_ptr<CssNumber> &number);

    inline void
    accept(CssVisitorInterface &visitor) override,

    setNegativeFlag(const bool is_negative = true),
    setNumber(const string &value),
    setScientificPostfix(const string &scientific_postfix);

    inline bool
    isNegative() const;

    inline const string
    &value() const,
    &scientificPostfix() const;

	inline bool
    value(const string &value) const,
    value(const initializer_list<string> candidates) const;

private:
    bool m_is_negative {false};
    string m_value;
    string m_scientific_postfix;
};

inline void
CssNumber::
accept(CssVisitorInterface &visitor)
{
    visitor.visit(static_pointer_cast<CssNumber>(shared_from_this()));
}

inline void
CssNumber::
setNegativeFlag(const bool is_negative)
{
    m_is_negative = is_negative;
}

inline bool
CssNumber::
isNegative() const
{
    return m_is_negative;
}

inline void
CssNumber::
setNumber(const string &value)
{
    m_value.reserve(value.length());
    m_value = value;
}

inline const string &
CssNumber::
value() const
{
    return m_value;
}

inline void
CssNumber::
setScientificPostfix(const string &scientific_postfix)
{
    m_scientific_postfix.reserve(scientific_postfix.length());
    m_scientific_postfix = scientific_postfix;
}

inline const string &
CssNumber::
scientificPostfix() const
{
    return m_scientific_postfix;
}

inline bool
CssNumber::
value(const string &value) const
{
	return m_value == value;
}

inline bool
CssNumber::
value(const initializer_list<string> candidates) const
{
	for (const auto &candidate : candidates)
		if (m_value == candidate)
			return true;

	return false;
}

using CssNumberPtr = shared_ptr<CssNumber>;

} // namespace Elements
} // namespace Parsing
} // namespace CSS

#endif // CSSNUMBER_H
