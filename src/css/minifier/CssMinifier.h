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

#ifndef CSSMINIFIER_H
#define CSSMINIFIER_H
#include "../../config/Config.h"
#include "../../general/minifier/GeneralMinifier.h"
#include "../generator/CssGenerator.h"
#include "../modifier/CssModifier.h"
#include "../parser/CssParser.h"

namespace CSS {
namespace Minification {
using namespace CSS::Parsing;
using namespace CSS::Generation;

class CssMinifier : public GeneralMinifier
{
public:
    explicit
    CssMinifier(const CssBaseElementPtr &&ast);

    using GeneralMinifier::GeneralMinifier;

    const shared_ptr<string>
    minify();

    static const shared_ptr<string>
    minify(const shared_ptr<string> &content),
    minify(const string &content);

private:
    CssBaseElementPtr m_parse_tree;
};

} // namespace Minification
} // namespace CSS

#endif // CSSMINIFIER_H
