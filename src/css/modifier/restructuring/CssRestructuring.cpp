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

#include "CssRestructuring.h"
using namespace CSS::Minification;

void
CssRestructuring::
restructure()
{
    if (cfg.isEnabled(Config::CSS__MERGE_MEDIA_RULES))
        mergeMediaRules();
}

void
CssRestructuring::
mergeMediaRules()
{
    if (m_media_rules.size() > 1) {
        using ExpressionList = DataContainer<CssBaseElementPtr>;
        using ExpressionListPtr = shared_ptr<ExpressionList>;
        using ExpressionLists = DataContainer<DataContainer<CssBaseElementPtr> >;
        using ExpressionListsPtr = shared_ptr<DataContainer<shared_ptr<DataContainer<CssBaseElementPtr> > > >;

        const auto compareExpression = [](const ExpressionListPtr &expression_list_1, const ExpressionListPtr &expression_list_2) -> bool {
            ExpressionLists expressions1, expressions2;

            for (size_t i = 0, pos1 = 0, pos2 = 0, size = expression_list_1->size(); i != size; ++i) {
                if (i+1 == size) {
                    expressions1.emplace_back(ExpressionList());
                    expressions2.emplace_back(ExpressionList());

                    for (size_t x = pos1; x <= i; ++x)
                        expressions1.back().emplace_back((*expression_list_1)[x]);

                    for (size_t x = pos2; x <= i; ++x)
                        expressions2.back().emplace_back((*expression_list_2)[x]);
                }

                if ((*expression_list_1)[i]->type() == CssBaseElement::IDENTIFIER) {
                    const auto identifier = static_pointer_cast<CssIdentifier>((*expression_list_1)[i]);

                    if (identifier->value("and")) {
                        expressions1.emplace_back(ExpressionList());

                        for (size_t x = pos1; x != i; ++x)
                            expressions1.back().emplace_back((*expression_list_1)[x]);

                        pos1 = i+1;
                    }
                }

                if ((*expression_list_2)[i]->type() == CssBaseElement::IDENTIFIER) {
                    const auto identifier = static_pointer_cast<CssIdentifier>((*expression_list_2)[i]);

                    if (identifier->value("and")) {
                        expressions2.emplace_back(ExpressionList());

                        for (size_t x = pos2; x != i; ++x)
                            expressions2.back().emplace_back((*expression_list_2)[x]);

                        pos2 = i+1;
                    }
                }
            }

            const auto compareExpression = [](const ExpressionList &expr_list_1, const ExpressionList &expr_list_2) -> bool {
                for (size_t i = 0, size = expr_list_1.size(); i != size; ++i) {
                    if (&*(expr_list_1[i]) != &*(expr_list_2[i]))
                        return false;
                }

                return true;
            };

            const auto expressionExists = [&compareExpression](const ExpressionLists &expression_lists, const ExpressionList &expression_list) -> bool {
                for (const auto &_expression_list : expression_lists) {
                    if (_expression_list.size() == expression_list.size())
                        if (compareExpression(_expression_list, expression_list))
                            return true;
                }

                return false;
            };

            if (expressions1.size() != expressions2.size())
                return false;

            for (const auto &expr : expressions1)
                if (!expressionExists(expressions2, expr))
                    return false;

            return true;
        };

        const auto expressionExists = [&compareExpression](const ExpressionListsPtr &expression_lists, const ExpressionListPtr &expression_list) -> bool {
            for (const auto &_expression_list : *expression_lists) {
                if (&*_expression_list == &*expression_list)
                    return true;

                if (_expression_list->size() == expression_list->size() && _expression_list->size() > 2 && expression_list->size() > 2)
                    if (compareExpression(_expression_list, expression_list))
                        return true;
            }

            return false;
        };

        const auto compareExpressionList = [&expressionExists](const ExpressionListsPtr &expression_list_1, const ExpressionListsPtr &expression_list_2) -> bool {
            for (const auto &expression : *expression_list_1)
                if (!expressionExists(expression_list_2, expression))
                    return false;

            return true;
        };

        for (auto itr1 = m_media_rules.begin(); itr1 != m_media_rules.end()-1; ++itr1) {
            for (auto itr2 = itr1+1; itr2 != m_media_rules.end(); ++itr2) {
                if (compareExpressionList((*itr1)->expressions(), (*itr2)->expressions())) {
                    for (const auto &element : (*itr2)->block()->elements())
                        (*itr1)->block()->elements().appendElement(element);

                    m_stylesheet->removeElement(*itr2);
                    m_media_rules.erase(itr2--);
                }
            }

            if (&*itr1 == &m_media_rules.back()) break;
        }
    }
}
