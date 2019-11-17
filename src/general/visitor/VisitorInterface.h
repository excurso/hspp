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

#ifndef VISITORINTERFACE_H
#define VISITORINTERFACE_H

namespace General {
namespace Visitor {

template<class ...VisitableTypes>
class VisitorInterface;

template<class VisitableType>
class VisitorInterface<VisitableType> {
public:
    virtual void visit(const VisitableType &visitable) = 0;
};

template<class VisitableType, class ...VisitableTypes>
class VisitorInterface<VisitableType, VisitableTypes...> : public VisitorInterface<VisitableTypes...>
{
public:
    VisitorInterface() = default;
    VisitorInterface(VisitorInterface &) = delete;
    VisitorInterface(const VisitorInterface &) = delete;
    VisitorInterface(VisitorInterface &&) noexcept = default;
    virtual ~VisitorInterface() = default;

    VisitorInterface &operator=(VisitorInterface &) noexcept = delete;
    VisitorInterface &operator=(const VisitorInterface &) noexcept = delete;
    VisitorInterface &operator=(VisitorInterface &&) noexcept = default;

    using VisitorInterface<VisitableTypes...>::visit;
    virtual void visit(const VisitableType &visitable) = 0;
};

} // namespace Visitor
} // namespace General

#endif // VISITORINTERFACE_H
