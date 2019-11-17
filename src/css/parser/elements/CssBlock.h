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

#ifndef CSSBLOCK_H
#define CSSBLOCK_H
#include "CssBaseElement.h"
#include "../../../DataContainer.h"

namespace CSS {
namespace Parsing {
namespace Elements {

class CssBlock final : public CssBaseElement
{
public:
    enum BlockType : uint8_t { DEFAULT, STYLESHEET, SQUARE, PAREN, CURLY };

    explicit
    CssBlock(const BlockType block_type);

    inline void
    accept(CssVisitorInterface &visitor) override,

    setBlockType(const BlockType block_type),
    setElements(const DataContainer<CssBaseElementPtr> &element_list),
    prependElement(const CssBaseElementPtr &element),
    appendElement(const CssBaseElementPtr &element),
    removeElement(const CssBaseElementPtr &element);

    inline DataContainer<CssBaseElementPtr> &
    elements();

    inline BlockType
    blockType() const;

private:
    BlockType m_type;
    DataContainer<CssBaseElementPtr> m_elements;
};

inline void
CssBlock::
accept(CssVisitorInterface &visitor)
{
    visitor.visit(static_pointer_cast<CssBlock>(shared_from_this()));
}

inline void
CssBlock::
setElements(const DataContainer<CssBaseElementPtr> &element_list)
{
    m_elements.reserve(element_list.size());
    m_elements = element_list;
}

inline void
CssBlock::
prependElement(const CssBaseElementPtr &element)
{
    m_elements.emplace(m_elements.begin(), element);
}

inline void
CssBlock::
appendElement(const CssBaseElementPtr &element)
{
    m_elements.emplace_back(element);
}

inline void
CssBlock::
removeElement(const CssBaseElementPtr &element)
{
    m_elements.removeElement(element);
}

inline DataContainer<CssBaseElementPtr> &
CssBlock::
elements()
{
    return m_elements;
}

inline void
CssBlock::
setBlockType(const BlockType block_type)
{
    m_type = block_type;
}

inline CssBlock::BlockType
CssBlock::
blockType() const
{
    return m_type;
}

using CssBlockPtr = shared_ptr<CssBlock>;

} // namespace Elements
} // namespace Parsing
} // namespace CSS

#endif // CSSBLOCK_H
