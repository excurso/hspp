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

#ifndef CSSCOMMENT_H
#define CSSCOMMENT_H
#include "CssBaseElement.h"

namespace CSS {
namespace Parsing {
namespace Elements {

class CssComment : public CssBaseElement
{
public:
    enum CommentType { COMMENT, CDATA_START, CDATA_END };

    explicit
    CssComment(string value),
    CssComment(const CommentType comment_type, string value);

    inline void
    accept(CssVisitorInterface &visitor) override;

    inline CommentType
    commentType() const;

    inline const string &
    value() const;

private:
    const CommentType m_comment_type;
    const string m_value;
};

inline void
CssComment::
accept(CssVisitorInterface &visitor)
{
    visitor.visit(static_pointer_cast<CssComment>(shared_from_this()));
}

inline auto
CssComment::
commentType() const -> CommentType
{
    return m_comment_type;
}

inline const string &
CssComment::
value() const
{
    return m_value;
}

using CssCommentPtr = shared_ptr<CssComment>;

} // namespace Elements
} // namespace Parsing
} // namespace CSS

#endif // CSSCOMMENT_H
