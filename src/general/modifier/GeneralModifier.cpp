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

#include "GeneralModifier.h"
using namespace General::Minification;

GeneralModifier::GeneralModifier() :
    m_use_utf8_bom_flag(cfg.isEnabled(Config::GENERAL__USE_UTF8_BOM)) {}

const string &
GeneralModifier::
getShortId(string &counter)
{
    const function<void(string::iterator)> increment = [&counter, &increment](const string::iterator itr) -> void {
        switch (*itr) {
        case 'z':
            *itr = 'A';
            break;
        case 'Z':
            if (counter.length() == 1 || (counter.length() > 1 && itr == counter.begin())) {
                *itr = 'a';
                counter.insert(counter.begin(), 'a');
            } else {
                counter.back() = '0';
            }

            break;
        case '9':
            *itr = 'a';
            if (itr-1 >= counter.begin())
                increment(itr-1);

            break;
        default:
            if (!counter.empty())
                ++*itr;
            else
                counter = 'a';
        }
    };

    increment(counter.end()-1);

    return counter;
}

uint8_t
GeneralModifier::
requestAction(const string &message, const uint8_t number_of_choices)
{
    string str_choice;
    uint8_t choice = 0;

    cout << message << endl;

    while (true) {
        cout << "Your choice (";

        for (uint8_t i = 0; i != number_of_choices; ++i)
            cout << to_string(i+1) << (i+1 != number_of_choices ? "/" : "");

        cout << "): " << flush;

        getline(cin, str_choice);

        if (bool(isdigit(str_choice.front())))
            choice = static_cast<uint8_t>(stoi(str_choice));

        if (choice == 0 || choice > number_of_choices) {
            cout << "Invalid input: '" << str_choice << "'" NEWLINE;
            cout << "Enter ";
            for (uint8_t i = 0; i != number_of_choices; ++i)
                cout << "'" << to_string(i+1) << "'"
                     << (i+1 != number_of_choices ? i+2 < number_of_choices ? ", " : " or " : "");

            cout << NEWLINE << endl;

            continue;
        }

        break;
    }

    return choice;
}
