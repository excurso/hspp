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
*******************************************************************************/

#ifndef HASHTABLE_H
#define HASHTABLE_H
#include <functional>
#include <unordered_map>
using namespace std;

/// Container which is derived from unordered_map
template<class T1, class T2>
class HashTable final : public unordered_map<T1, T2>
{
public:
    using unordered_map<T1, T2>::unordered_map;

    inline void
    appendElement(const T1 &key, const T2 &value),
    removeElement(const T1 &key);

    T2
    &setElement(T1 key, T2 &element);

    void
    iterateAll(function<void(T1 &)> function) const;
};

template<class T1, class T2>
inline void
HashTable<T1, T2>::
appendElement(const T1 &key, const T2 &value)
{
    this->emplace(key, value);
}

template<class T1, class T2>
inline void
HashTable<T1, T2>::
removeElement(const T1 &key)
{
    this->erase(this->find(key));
}

template<class T1, class T2>
T2 &
HashTable<T1, T2>::
setElement(const T1 key, T2 &element)
{
    // If key exists, let element point to the existing element,
    // else insert new element

    const auto &found = this->find(key);

    if (found != this->end()) {
        element = found->second;
    } else {
        this->emplace(key, element);
    }

    // Return reference to already existing or newly inserted element.
    return element;
}

#endif // HASHTABLE_H
