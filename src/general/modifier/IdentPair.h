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

#ifndef GENERALIDENTPAIR_H
#define GENERALIDENTPAIR_H
#include <memory>

namespace General {
namespace Minification {
using namespace std;

struct IdentPair {
	explicit
	IdentPair(shared_ptr<string> original_id) :
	    original_id(move(original_id)),
        replacement_id(new string),
        count(1) {}

    shared_ptr<string> original_id, replacement_id;
    uint32_t count;
};

using IdentPairPtr = shared_ptr<IdentPair>;

} // namespace Minification
} // namespace General

#endif // GENERALIDENTPAIR_H
