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
        for (auto itr1 = m_media_rules.begin(); itr1 != m_media_rules.end()-1; ++itr1) {
            for (auto itr2 = itr1+1; itr2 != m_media_rules.end(); ++itr2) {
                if (&*(*itr1)->expressions() == &*(*itr2)->expressions()) {
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
