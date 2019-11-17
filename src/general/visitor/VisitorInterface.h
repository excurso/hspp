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
