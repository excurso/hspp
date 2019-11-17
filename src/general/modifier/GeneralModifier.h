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

#ifndef GENERALMODIFIER_H
#define GENERALMODIFIER_H
#include "../../config/Config.h"
#include "../../Console.h"

namespace General {
namespace Minification {
using namespace std;

class GeneralModifier
{
public:
    GeneralModifier(GeneralModifier &) = delete;
    GeneralModifier(const GeneralModifier &) = delete;
    GeneralModifier(GeneralModifier &&) = delete;
    GeneralModifier(const GeneralModifier &&) = delete;

    GeneralModifier &operator=(GeneralModifier &) = delete;
    GeneralModifier &operator=(const GeneralModifier &) = delete;
    GeneralModifier &operator=(GeneralModifier &&) = delete;
    GeneralModifier &operator=(const GeneralModifier &&) = delete;

    explicit
    GeneralModifier();
    ~GeneralModifier() = default;

protected:
    uint8_t
    requestAction(const string &message, const uint8_t number_of_choices);

    const string &
    getShortId(string &counter);

    inline void
    setUseUtf8BomFlag(const bool = false);

    inline bool
    useUtf8Bom();

private:
    /// This flag is used to have the ability to skip writing UTF8 BOM
    /// for a single file if another encoding has been detected.
    /// Initially set to the value from the configuration file or
    /// default setting...
    bool m_use_utf8_bom_flag;
};

inline void
GeneralModifier::
setUseUtf8BomFlag(const bool use_utf8_bom)
{
    m_use_utf8_bom_flag = use_utf8_bom;
}

inline bool
GeneralModifier::
useUtf8Bom()
{
    return m_use_utf8_bom_flag;
}

} // namespace Minification
} // namespace General

#endif // GENERALMODIFIER_H
