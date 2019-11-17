#ifndef DATACONTAINER_H
#define DATACONTAINER_H
#include <vector>
#include <functional>
#include <algorithm>
using namespace std;

/// Container which is derived from vector
template<class T>
class DataContainer : public vector<T>
{
public:
    using vector<T>::vector;

    inline void
    appendElement(const T &element),
    removeElement(const T &element),

    iterateAll(const function<void(const T &element)> &) const;

    inline bool
    hasElement(const T &element);

    void
    removeElement(const function<bool(const T &element)> &),
    removeElements(const function<bool(const T &element)> &),
    iterateAllConditional(const function<bool(const T &element)> &) const,
    iterateAllReverse(const function<void(const T &element)> &) const;

    typename DataContainer<T>::iterator
    find(const T &element);
};

template<class T>
typename DataContainer<T>::iterator
DataContainer<T>::
find(const T &element)
{
    return std::find(this->begin(), this->end(), element);
}

template<class T>
inline void
DataContainer<T>::
appendElement(const T &element)
{
    this->emplace_back(element);
}

template<class T>
inline void
DataContainer<T>::
removeElement(const T &element)
{
    this->erase(this->find(element));
}

template<class T>
void
DataContainer<T>::
removeElement(const function<bool(const T &element)> &function)
{
    // The callback function has to return true to remove the element and
    // stop iteration

    iterateAllConditional([&](const T &element) -> bool {
        if (function(element)) {
            removeElement(element);

            // Stop iteration by returning false
            return false;
        }

        // Iterate through elements by returning true
        return true;
    });
}

template<class T>
void
DataContainer<T>::
removeElements(const function<bool(const T &element)> &function)
{
    // The callback function has to return true to remove the element.
    // Iteration does't stop until the end.

    iterateAll([&](const T &element) -> void {
        if (function(element))
            removeElement(element);
    });
}

template<class T>
inline bool
DataContainer<T>::
hasElement(const T &element)
{
    return this->find(element) != this->end();
}

template<class T>
inline void
DataContainer<T>::
iterateAll(const function<void(const T &element)> &function) const
{
    iterateAllConditional([&function](const T &element) -> bool {
        function(element); return true;
    });
}

template<class T>
void
DataContainer<T>::
iterateAllConditional(const function<bool(const T &element)> &function) const
{
    // The callback function has to return true to continue iteration.
    // If false is returned, iteration will be stopped.

    if (!this->empty())
        for (uint64_t i = 0; i < this->size(); ++i)
            // If the callback function returns false, stop iteration.
            if (!function(this->at(i))) return;
}

template<class T>
void
DataContainer<T>::
iterateAllReverse(const function<void(const T &element)> &function) const
{
    if (!this->empty())
        for (auto itr = this->rbegin(); itr != this->rend(); ++itr)
            function(*itr);
}

#endif // DATACONTAINER_H
