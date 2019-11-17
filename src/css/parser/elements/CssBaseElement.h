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

#ifndef CSSBASEELEMENT_H
#define CSSBASEELEMENT_H
#include "../../../general/visitor/VisitorInterface.h"
#include <memory>

#ifndef NDEBUG
#include <iostream>
#endif

namespace CSS {
namespace Parsing {
namespace Elements {
using namespace std;
using namespace General::Visitor;
class CssBaseElement;
using CssBaseElementPtr = shared_ptr<CssBaseElement>;

class CssAtRule; class CssBlock; class CssDeclaration;
class CssDimension; class CssFunction; class CssIdentifier;
class CssNumber; class CssColor; class CssQualifiedRule; class CssString;
class CssSelector; class CssSelectorAttribute; class CssSelectorCombinator; class CssDelimiter;
class CssUnicodeRange; class CssSupportsCondition; class CssComment; class CssPercentage;
class CssCustomProperty;

using CssVisitorInterface =
    VisitorInterface<shared_ptr<CssAtRule>, shared_ptr<CssBlock>, shared_ptr<CssDeclaration>,
                     shared_ptr<CssPercentage>, shared_ptr<CssDimension>, shared_ptr<CssFunction>,
                     shared_ptr<CssIdentifier>, shared_ptr<CssCustomProperty>, shared_ptr<CssNumber>,
                     shared_ptr<CssColor>, shared_ptr<CssQualifiedRule>, shared_ptr<CssString>,
                     shared_ptr<CssSelector>, shared_ptr<CssSelectorAttribute>,
                     shared_ptr<CssSelectorCombinator>, shared_ptr<CssDelimiter>,
                     shared_ptr<CssUnicodeRange>, shared_ptr<CssSupportsCondition>,
                     shared_ptr<CssComment> >;

class CssBaseElement : public enable_shared_from_this<CssBaseElement>
{
public:
    enum ElementType : uint8_t {
        SELECTOR, SELECTOR_COMBINATOR, BLOCK, IDENTIFIER, COLOR,
        NUMBER, STRING, AT_RULE, QUALIFIED_RULE, SUPPORTS_CONDITION,
        DECLARATION, FUNCTION, PERCENTAGE, DIMENSION, DELIMITER, COMMENT,
        UNICODE_RANGE, CUSTOM_PROPERTY
    };

    constexpr explicit
    CssBaseElement(const ElementType type);

    CssBaseElement(const CssBaseElement &) = delete;
    CssBaseElement(CssBaseElement &&) = delete;

    CssBaseElement &operator=(const CssBaseElement &) = delete;
    CssBaseElement &operator=(CssBaseElement &&) = delete;

    inline virtual void
    accept(CssVisitorInterface &);

    inline void
    setType(const ElementType type),
    setInitialRow(const uint64_t initial_row),
    setInitialColumn(const uint64_t initial_column),
    setInitialPosition(const uint64_t initial_row, const uint64_t initial_column),
    setOutputColumn(const uint64_t output_column),

    setReplacementElement(const CssBaseElementPtr &element);

    inline const CssBaseElementPtr &
    replacementElement() const;

    inline ElementType
    type() const;

    inline bool
	isOfType(const ElementType element_type),
    isOfType(const initializer_list<ElementType> candidates),

    isComment(),
    isColor(),
    isPercentage(),
    isDimension(),
    isNumber(),
    isString(),
    isDelimiter(),
    isFunction(),
    isIdentifier(),
    isAtRule(),
    isQualifiedRule(),
    isDeclaration();

    inline uint64_t
    initialRow() const,
    initialColumn() const,
    outputColumn() const;

protected:
    virtual
    ~CssBaseElement() = default;

private:
    /// If m_replacement_element is set, it signals the CssGenerator,
    /// it should be written instead of current element.
    /// This is a workaround. Because of the restriction, a visitable
    /// element cannot be replaced by another type of element.
    CssBaseElementPtr m_replacement_element;

    uint64_t m_initial_row = 0, m_initial_column = 0, m_output_column = 0;

    ElementType m_type;
};

constexpr
CssBaseElement::CssBaseElement(const ElementType type) :
    m_replacement_element(nullptr), m_initial_row(1),
    m_initial_column(1), m_type(type) {}

inline auto
CssBaseElement::
type() const -> ElementType
{
    return m_type;
}

inline bool
CssBaseElement::
isOfType(const ElementType element_type)
{
	return m_type == element_type;
}

inline bool
CssBaseElement::
isOfType(const initializer_list<ElementType> candidates)
{
    for (const auto &candidate : candidates)
        if (m_type == candidate)
            return true;

    return false;
}

inline bool
CssBaseElement::
isComment()
{
    return m_type == COMMENT;
}

inline bool
CssBaseElement::
isColor()
{
    return m_type == COLOR;
}

inline bool
CssBaseElement::
isPercentage()
{
    return m_type == PERCENTAGE;
}

inline bool
CssBaseElement::
isDimension()
{
    return m_type == DIMENSION;
}

inline bool
CssBaseElement::
isNumber()
{
    return m_type == NUMBER;
}

inline bool
CssBaseElement::
isString()
{
    return m_type == STRING;
}

inline bool
CssBaseElement::
isDelimiter()
{
    return m_type == DELIMITER;
}

inline bool
CssBaseElement::
isFunction()
{
    return m_type == FUNCTION;
}

inline bool
CssBaseElement::
isIdentifier()
{
    return m_type == IDENTIFIER;
}

inline bool
CssBaseElement::
isAtRule()
{
    return m_type == AT_RULE;
}

inline bool
CssBaseElement::
isQualifiedRule()
{
    return m_type == QUALIFIED_RULE;
}

inline bool
CssBaseElement::
isDeclaration()
{
    return m_type == DECLARATION;
}

inline void
CssBaseElement::
setType(const ElementType type)
{
    m_type = type;
}

inline void
CssBaseElement::
setInitialRow(const uint64_t initial_row)
{
    m_initial_row = initial_row;
}

inline uint64_t
CssBaseElement::
initialRow() const
{
    return m_initial_row;
}

inline void
CssBaseElement::
setInitialColumn(const uint64_t initial_column)
{
    m_initial_column = initial_column;
}

inline uint64_t
CssBaseElement::
initialColumn() const
{
    return m_initial_column;
}

inline void
CssBaseElement::
setOutputColumn(const uint64_t output_column)
{
    m_output_column = output_column;
}

inline uint64_t
CssBaseElement::
outputColumn() const
{
    return m_output_column;
}

inline void
CssBaseElement::
setInitialPosition(const uint64_t initial_row, const uint64_t initial_column)
{
    m_initial_row = initial_row;
    m_initial_column = initial_column;

}

inline void
CssBaseElement::
setReplacementElement(const CssBaseElementPtr &element)
{
    m_replacement_element = element;
}

inline const CssBaseElementPtr &
CssBaseElement::
replacementElement() const
{
    return m_replacement_element;
}

inline /*virtual*/ void
CssBaseElement::
accept(CssVisitorInterface &)
{
#ifndef NDEBUG
    cerr << "Error: Using function " << __FUNCTION__ << "() of base class 'CssBaseElement'." << endl;
    exit(1);
#endif
}

} // namespace Elements
} // namespace Parsing
} // namespace CSS

#endif // CSSBASEELEMENT_H
