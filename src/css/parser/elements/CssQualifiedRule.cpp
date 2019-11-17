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

#include "CssQualifiedRule.h"
using namespace CSS::Parsing::Elements;

CssQualifiedRule::CssQualifiedRule() :
    CssBaseElement(QUALIFIED_RULE) {}

CssQualifiedRule::CssQualifiedRule(DataContainer<CssBaseElementPtr> selector_list) :
    CssBaseElement(QUALIFIED_RULE), m_selector_list(move(selector_list)) {}
