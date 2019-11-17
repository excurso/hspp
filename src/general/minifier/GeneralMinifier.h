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

#ifndef MINIFIER_H
#define MINIFIER_H
#include <memory>

namespace General {
namespace Minification {

using namespace std;

class GeneralMinifier
{
public:
    GeneralMinifier(GeneralMinifier &) = delete;
    GeneralMinifier(const GeneralMinifier &) = delete;
    GeneralMinifier(GeneralMinifier &&) = delete;
    GeneralMinifier(const GeneralMinifier &&) = delete;

    GeneralMinifier &operator=(GeneralMinifier &) = delete;
    GeneralMinifier &operator=(const GeneralMinifier &) = delete;
    GeneralMinifier &operator=(GeneralMinifier &&) = delete;
    GeneralMinifier &operator=(const GeneralMinifier &&) = delete;

    GeneralMinifier();
    ~GeneralMinifier() = default;

protected:
    inline shared_ptr<string> &
    outputBuffer();

private:
    shared_ptr<string> m_output_buffer;
};

inline shared_ptr<string> &
GeneralMinifier::
outputBuffer()
{
    return m_output_buffer;
}

} // namespace Minification
} // namespace General

#endif // MINIFIER_H
