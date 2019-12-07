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

#ifndef CSSRESTRUCTURING_H
#define CSSRESTRUCTURING_H
#include "../../../config/Config.h"
#include "../../../HashTable.h"
#include "../../parser/elements/CssAtRule.h"
#include "../../parser/elements/CssQualifiedRule.h"
#include "../../parser/elements/CssSelector.h"
#include "../../parser/elements/CssDeclaration.h"
#include "../../parser/elements/CssDimension.h"
#include "../../parser/elements/CssPercentage.h"

namespace CSS {
namespace Minification {
using namespace CSS::Parsing::Elements;

class CssRestructuring
{
    using StyleSheetPtr = CssBlockPtr;

public:
    CssRestructuring() = default;

    inline void
    setStyleSheet(const StyleSheetPtr &stylesheet),
    appendAtRuleMedia(const CssAtRulePtr &at_rule_media);

    void
    restructure();

private:
    void
    mergeMediaRules();

    DataContainer<CssAtRulePtr>
    m_media_rules;

    StyleSheetPtr m_stylesheet;
};

inline void
CssRestructuring::
setStyleSheet(const StyleSheetPtr &stylesheet)
{
    m_stylesheet = stylesheet;
}

inline void
CssRestructuring::
appendAtRuleMedia(const CssAtRulePtr &at_rule_media)
{
    m_media_rules.emplace_back(at_rule_media);
}

} // namespace Minification
} // namespace CSS

#endif // CSSRESTRUCTURING_H
