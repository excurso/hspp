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

#ifndef CSSVENDORPREFIXES
#define CSSVENDORPREFIXES
#include "../DataContainer.h"
#include <string>

struct Vendor {
    Vendor() = default;

    const DataContainer<std::string> prefixes {
        "", "-webkit-", "-moz-", "-o-", "-ms-"
    };

    bool maybePrefixedKeyword(const std::string &maybe_prefixed_keyword, const std::string &unprefixed_keyword) const
    {
        for (const auto &prefix : prefixes)
            if (maybe_prefixed_keyword == prefix + unprefixed_keyword)
                return true;

        return false;
    }
};

#endif //CSSVENDORPREFIXES
